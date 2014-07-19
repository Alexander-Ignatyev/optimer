#!/usr/bin/env python
#
# Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.
#

# [general]
# problem_path = data/smth.atsp
# problem_size = [integer]
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
import pms_report

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
    re_number_of_sets = re.compile('\s*# of sets per max time:\s+(\d+\.?\d*)')
    re_valuation_time = re.compile('Valuation Time:\s+(\d+\.?\d*)')
    re_thread_time_variance = re.compile('\s*Time variance:\s+(\d+\.?\d*)')

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

def run_task(args, timeout):
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return RunningResult(process, timeout)

def run_configs(module, num_runs, config_list, timeout):
    num = 0
    num_configs = len(config_list)
    logs = []
    for config in config_list:
        num += 1
        task_name = config.name()
        config_filename = ''
        print '[{0: 3}/{1}] '.format(num, num_configs), 'running ', task_name, '...'
        with NamedTemporaryFile(delete=False) as file:
            config.write(file)
            config_filename = file.name
        for _ in xrange(num_runs):
            res = run_task([module, config_filename], timeout)
            if res.return_code == 0:
                log = {}
                log['task_name'] = task_name
                log['running_time'] = res.running_time
                with open(res.get_log_filename()) as f:
                    log['output'] = f.read()
                logs.append(log)
                os.unlink(res.get_log_filename())

            else:
                print 'config_file:', config_filename
                print 'stdout: ['+res.stdout+']'
                print 'stderr: ['+res.stderr+']'
    return logs

def main(config_path):
    params_data = yaml.load(open(config_path))
    module = params_data['module']
    num_runs = params_data['num_runs']
    timeout = params_data['timeout']

    config_root = read_settings(params_data)
    config_list = []
    config_root.process(Config(), config_list)

    logs = run_configs(module, num_runs, config_list, timeout)

    result_filename = 'pms_result.'
    result_filename += datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
    result_filename += '.yaml'
    print 'saving results to', result_filename, '...'
    params_data['logs'] = logs
    with open(result_filename, 'w') as f:
        f.write(yaml.dump(params_data))

    pms_report.parse(result_filename)
    print 'done'

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage:', sys.argv[0], '<settings>.yaml'
        sys.exit(1)
    main(sys.argv[1])
