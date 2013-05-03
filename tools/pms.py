#!/usr/bin/python
#
# Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

import os
import sys
import time
import subprocess
from copy import copy
from tempfile import NamedTemporaryFile

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

class RunningResult(object):
    def __init__(self, process):
        timer = Timer()
        self.stdout, self.stderr = process.communicate()
        self.runnimg_time = timer.elapsed()
        self.return_code = process.returncode

    def __str__(self):
        return 'return: {0}, total time: {1}'.format(self.return_code, self.runnimg_time)

def run_task(args):
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return RunningResult(process)

class ConfigParams(object):
    default_containers = ['lifo', 'priority']
    default_schedulers = ['giving', 'requesting']
    default_num_threads = [4]
    default_num_minimum_nodes = [8]
    default_num_maximum_nodes = [80]
    
    def __init__(self
        , containers = None
        , schedulers = None
        , num_threads = None
        , num_minimum_nodes = None
        , num_maximum_nodes = None):
        if containers:
            self.containers = containers
        else:
            self.containers = copy(ConfigParams.default_containers)

        if schedulers:
            self.schedulers = schedulers
        else:
            self.schedulers = copy(ConfigParams.default_schedulers)

        if num_threads:
            self.num_threads = num_threads
        else:
            self.num_threads = copy(ConfigParams.default_num_threads)

        if num_minimum_nodes:
            self.num_minimum_nodes = num_minimum_nodes
        else:
            self.num_minimum_nodes = copy(ConfigParams.default_num_minimum_nodes)

        if num_maximum_nodes:
            self.num_maximum_nodes = num_maximum_nodes
        else:
            self.num_maximum_nodes = copy(ConfigParams.default_num_maximum_nodes)

class Config(object):
    def generate_tasks(self, params, problem_path):
        self.params = params
        general = {}
        general['problem_path'] = problem_path

        config = {}
        config['general'] = general

        self.config_files = {}
        self.__generate_serial_tasks(config)
        self.__generate_parallel_lock_tasks(config)

        return self.config_files

    def __generate_serial_tasks(self, config):
        general = config['general']
        general['valuation_type'] = 'serial'
        for container in self.params.containers:
            general['container_type'] = container
            name = Config.__build_name(config)
            self.config_files[name] = Config.__save_config(config)

    def __generate_parallel_lock_tasks(self, config):
        general = config['general']
        general['valuation_type'] = 'parallel-lock'
        scheduler = {}
        config['scheduler'] = scheduler
        for container in self.params.containers:
            general['container_type'] = container
            for scheduler_type in self.params.schedulers:
                scheduler['type'] = scheduler_type
                for num_threads in self.params.num_threads:
                    scheduler['num_threads'] = num_threads
                    for num_minimum_nodes in self.params.num_minimum_nodes:
                        scheduler['num_minimum_nodes'] = num_minimum_nodes
                        if scheduler_type == 'giving':
                            for num_maximum_nodes in self.params.num_maximum_nodes:
                                scheduler['num_maximum_nodes'] = num_maximum_nodes
                                name = Config.__build_name(config)
                                self.config_files[name] = Config.__save_config(config)
                        else:
                            name = Config.__build_name(config)
                            self.config_files[name] = Config.__save_config(config)

    @staticmethod
    def __build_name(config):
        general = config['general']
        parts = []
        parts.append(os.path.basename(general['problem_path']))
        parts.append(general['valuation_type'])
        parts.append(general['container_type'])
        if general['valuation_type'] == 'parallel-lock':
            scheduler = config['scheduler']
            parts.append(scheduler['type'])
            parts.append('{0: 3}'.format(scheduler['num_threads']))
            parts.append('{0: 3}'.format(scheduler['num_minimum_nodes']))
            if scheduler['type'] == 'giving':
                parts.append('{0: 3}'.format(scheduler['num_maximum_nodes']))
        return ':'.join(parts)

    @staticmethod
    def __save_config(config):
        lines = []
        for section_name, section in config.iteritems():
            lines.append('[{0}]\n'.format(section_name))
            for key, value in section.iteritems():
                lines.append('{0} = {1}\n'.format(key, value))
        with NamedTemporaryFile(delete=False) as file:
            filename = file.name
            file.writelines(lines)
        return filename

def main():
    module = 'build_clang++_release/atsp'
    
    params = ConfigParams()
    params.schedulers = ['requesting']
    params.num_threads = [2, 4, 8]
    params.num_minimum_nodes = [4, 8]
    params.num_maximum_nodes = [10, 20, 40, 80]

    config = Config()
    config_files = config.generate_tasks(params, 'data/ftv38.atsp')
    for task in sorted(config_files.iterkeys()):
        filename = config_files[task]
        print task, 
        res = run_task([module, filename])
        print res
        if res.return_code != 0:
            print res.stdout
            print res.stderr
        os.unlink(filename)

if __name__ == '__main__':
    main()
