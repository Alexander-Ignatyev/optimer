#!/usr/bin/python
#
# Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.
#
# Set parameters in function main()

# [general]
# problem_path = data/smth.atsp
# container_type = [lifo, priority]
# valuation_type = [serial, parallel-lock]
# [tsp]
# branching_rule = 3
# [scheduler]
# type = [giving, requesting]
# num_threads = 4
# num_minimum_nodes = 24
# num_maximum_nodes = 40

import os
import os.path
import sys
import re
import time
import datetime
import threading
import subprocess
import yaml
from copy import copy
from tempfile import NamedTemporaryFile

sys.path.append(os.path.dirname(__file__))
from config_generator import * 

class Timer(object):
    def __init__(self):
        if sys.platform == "win32":
            self.timer = time.clock
        else:
            self.timer = time.time
        self.reset()

    def reset(self):
        self.start_time = self.timer()

    def elapsed(self):
        return self.timer() - self.start_time

def terminate_process(popen):
    popen.terminate()

class RunningResult(object):
    re_generated_sets = re.compile('\s*Generated sets:\s+(\d+)')
    re_constrained_by_record = re.compile('\s*Constrained by record:\s+(\d+)')

    def __init__(self, process, timeout=-1):
        timer = Timer()
        termTimer = None
        if (timeout > 0):
            termTimer = threading.Timer(timeout, terminate_process, [process])
            termTimer.start()
        self.stdout, self.stderr = process.communicate()
        if (termTimer != None):
            termTimer.cancel();
        self.running_time = timer.elapsed()
        self.return_code = process.returncode
        self.log_filename = ''
        self.generated_sets = -1
        self.constrained_by_record = -1

        self.parsers = []
        self.parsers.append(self._parse_generated_sets)
        self.parsers.append(self._parse_constrained_by_record)

    def get_log_filename(self):
        pattern = 'log location:'
        if self.log_filename == '':
            for line in self.stderr.split('\n'):
                pos = line.find(pattern)
                if pos != -1:
                    self.log_filename = line[pos+len(pattern)+1:]
                    return self.log_filename
        return self.log_filename

    def __str__(self):
        return 'return: {0}, total time: {1}'.format(self.return_code, self.running_time)

    def __repr__(self):
        log_filename = os.path.basename(self.get_log_filename())
        return '<<return: {0}, total time: {1}>>'.format(self.return_code, self.running_time)

    def _parse_generated_sets(self, line):
        res = RunningResult.re_generated_sets.match(line)
        if res:
            self.generated_sets = int(res.groups()[0])
            return True
        return False
    
    def _parse_constrained_by_record(self, line):
        res = RunningResult.re_constrained_by_record.match(line)
        if res:
            self.constrained_by_record = int(res.groups()[0])
            return True
        return False

    def parse_log(self):
        with open(self.get_log_filename()) as f:
            for line in f:
                for parser in self.parsers:
                    if parser(line):
                        break
        if self.generated_sets < 0:
            raise RuntimeError('incorrect generated_sets for '+self.get_log_filename())
        if self.constrained_by_record < 0:
            raise RuntimeError('incorrect constrained_by_record for '+self.get_log_filename())

class LaTeXWriter(object):
    
    @staticmethod
    def generate(all_runs):
        lines = []
        lines += LaTeXWriter._header()
        for task_name in sorted(all_runs.iterkeys()):
            lines += LaTeXWriter._line(all_runs, task_name)
        lines += LaTeXWriter._footer()
        return lines
    
    @staticmethod
    def _line(all_runs, task_name):
        runs = all_runs[task_name]
        generated_sets = 0
        constrained_by_record = 0
        running_time = 0
        num_runs = len(runs)
        for run in runs:
            run.parse_log()
            generated_sets += run.generated_sets
            constrained_by_record += run.constrained_by_record
            running_time += run.running_time
        generated_sets /= num_runs
        constrained_by_record /= num_runs
        running_time /= num_runs
        
        sep = ' & '
        end_line = ' \\\\'
        
        line = task_name + sep + '{:10.3f}'.format(running_time) + sep + str(generated_sets) + sep + str(constrained_by_record) + end_line + '\n'
        line += '\\hline\n'
        return [line]
        
    @staticmethod
    def _header():
        return ["""\\begin{table}
\\caption{TABLE CAPTION.}
\\label{tab:1}
\\begin{center}
\\begin{tabular}{|l|l|l|l|}
\\hline
"""]

    @staticmethod
    def _footer():
        return ["""\\end{tabular}
\\end{center}
\\end{table}
"""]

def run_task(args, timeout):
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return RunningResult(process, timeout)

def run_configs(module, num_runs, config_list, timeout):
    num = 0
    num_configs = len(config_list)
    all_runs = {}
    for config in config_list:
        num += 1
        task_name = config.name()
        config_filename = ''
        print '[{0: 3}/{1}] '.format(num, num_configs), 'running ', task_name, '...'
        with NamedTemporaryFile(delete=False) as file:
            config.write(file)
            config_filename = file.name
        runs = []
        for _ in xrange(num_runs):
            res = run_task([module, config_filename], timeout)
            runs.append(res)
            if res.return_code != 0:
                print 'config_file:', config_filename
                print 'stdout: ['+res.stdout+']'
                print 'stderr: ['+res.stderr+']'
        all_runs[task_name] = runs
    return all_runs
        

def prepare_results(all_runs):
    all_avg_times = {}
    for task_name, runs in all_runs.iteritems():
        avg_time = reduce(lambda sum, run: sum + run.running_time, runs, 0.0) / len(runs)
        all_avg_times[task_name] = avg_time

    lines = []
    lines.append('Average results:\n')
    for task_name in sorted(all_avg_times.iterkeys()):
        lines.append('{0} {1}\n'.format(task_name, all_avg_times[task_name]))

    lines.append('\nLaTeX table:\n')
    lines += LaTeXWriter.generate(all_runs)

    lines.append('\nResults:\n')
    for task_name in sorted(all_runs.iterkeys()):
        lines.append('{0} {1}\n'.format(task_name, all_runs[task_name]))

    lines.append('\nLogs:\n')
    for task_name in sorted(all_runs.iterkeys()):
        lines.append('{0}\n'.format(task_name))
        for run in all_runs[task_name]:
            lines.append('\t{0}\n'.format(run.get_log_filename()))
            if not run.get_log_filename():
                continue
            with open(run.get_log_filename()) as f:
                for line in f:
                    lines.append('\t\t{0}'.format(line))
            #os.unlink(run.get_log_filename())
            lines.append('\n')
    return lines

def main(config_path):
    params_data = yaml.load(open(config_path))
    module = params_data['module']
    num_runs = params_data['num_runs']
    timeout = params_data['timeout']

    config_root = read_settings(params_data)
    config_list = []
    config_root.process(Config(), config_list)
    
    all_runs = run_configs(module, num_runs, config_list, timeout)
    print 'preparing results...'
    lines = prepare_results(all_runs)

    result_filename = 'pms_result.'
    result_filename += datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
    result_filename += '.txt'
    print 'saving results to', result_filename, '...'
    with open(result_filename, 'w') as f:
        f.writelines(lines)

    print 'done'

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage:', sys.argv[0], '<settings>.yaml'
        sys.exit(1)
    main(sys.argv[1])
