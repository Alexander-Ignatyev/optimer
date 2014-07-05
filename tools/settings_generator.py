#!/usr/bin/python
#
# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.
#

from __future__ import print_function
from copy import deepcopy
import yaml

class Settings(object):
    def __init__(self, lines = None):
        if not lines:
            self.__lines = []
        else:
            self.__lines = lines

    def append(self, line):
        return Settings(deepcopy(self.__lines)+[line])
    
    def write(self, file):
        for line in self.__lines:
            file.write(line+'\n')
            
    def __str__(self):
        # http://www.skymind.com/~ocrow/python_string/
        return ''.join([line+'\n' for line in self.__lines])

    def __repr__(self):
        return self.__str__()

class _Node(object):
    def __init__(self):
        self.childs = []

    def add_child(self, child):
        self.childs.append(child)
    
    def __process_childs(self, settings, settings_list):
        for child in self.childs:
            child.process(settings, settings_list)
        if not self.childs:
            settings_list.append(settings)
    
    def _process_property(self, root_settings, settings_list):
        for value in self.values:
            line = self.name + '=' + str(value)
            settings = root_settings.append(line)
            self.__process_childs(settings, settings_list)
    
    def _process_section(self, root_settings, settings_list):
        settings = root_settings.append('['+self.name+']')
        self.__process_childs(settings, settings_list)

def Section(name):
    node = _Node()
    node.name = name
    node.process = node._process_section
    return node

def Property(name, values):
    node = _Node()
    node.name = name
    node.values = values
    node.process = node._process_property
    return node

def _read_config_section(data):
    data = data['section']
    node = Section(data['name'])
    _read_config_childs(data, node)
    return node

def _read_config_property(data):
    data = data['property']
    node = Property(data['name'], data['values'])
    _read_config_childs(data, node)
    return node

def _read_config_childs(data, parent):
    if not 'childs' in data:
        return
    for child in data['childs']:
        if 'section' in child:
            node = _read_config_section(child)
            parent.add_child(node)
        elif 'property' in child:
            node = _read_config_property(child)
            parent.add_child(node)
        else:
            raise Exception('incorrect child node:' + child)

def read_config(filepath):
    data = yaml.load(open(filepath))
    root = None
    if 'section' in data:
        data = data['section']
        root = Section(data['name'])
    elif 'property' in data:
        data = data['property']
        root = Property(data['name'], data['values'])
    else:
        raise Exception('incorrect config file: ' + filepath)
    _read_config_childs(data, root)
    return root

def _test():
    root_node = Section('main')
    settings_list = []
    param_branching_rule = Property('branching_rule', [3,4])
    root_node.add_child(param_branching_rule)

    param_valuation_type_serial = Property('valuation_type', ['serial'])
    param_valuation_type_parallel = Property('valuation_type', ['parallel'])
    param_branching_rule.add_child(param_valuation_type_serial)
    param_branching_rule.add_child(param_valuation_type_parallel)

    section_scheduller = Section('scheduller')
    param_valuation_type_parallel.add_child(section_scheduller)

    param_threads = Property('num_threads', range(2,10,2))
    section_scheduller.add_child(param_threads)

    root_node.process(Settings(), settings_list)

    for settings in settings_list:
        print(settings)
        print('')

def _test_config(filename):
    root = read_config(filename)
    settings_list = []
    root.process(Settings(), settings_list)
    
    for settings in settings_list:
        print(settings)
        print('')   

if __name__ == '__main__':
    _test_config('config.yaml')
    
    


