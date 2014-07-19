#!/usr/bin/env python
#
# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.
#


import sys
import os.path
import re
import yaml
import json

class LineParser(object):
    def __init__(self, re, convert_function, format_function):
        self.re = re
        self.convert_function = convert_function
        self.format_function = format_function

    def parse(self, line):
        res = self.re.search(line)
        if res:
            return self.convert_function(res.groups()[0])
        else:
            return None

def parse_task_name(task_name):
    lines = task_name.split(':')
    entries = {}
    for line in lines:
        parts = line.split('=')
        if len(parts) == 2:
            entries[parts[0].strip()] = parts[1].strip()
    problem_name = os.path.basename(entries['problem_path'])
    if 'problem_size' in entries:
        problem_name += ':' + entries['problem_size']
    solver_settings = []
    solver_settings.append(entries['container_type'])
    if 'branching_rule' in entries:
        solver_settings.append('branching_rule='+entries['branching_rule'])
    val_type = entries['valuation_type']
    if 'num_threads' in entries:
        val_type += ' [' + entries['num_threads'] + ']'
    if 'type' in entries:
        val_type += ':' + entries['type']
    if 'num_minimum_nodes' in entries:
        val_type += '(' + entries['num_minimum_nodes']
        if 'num_maximum_nodes' in entries:
            val_type += ':' + entries['num_maximum_nodes']
        val_type += ')'
    solver_settings.append(val_type)
    return (problem_name, ':'.join(solver_settings))

parsers = {}
parsers['generated sets'] = LineParser(re.compile('\s*Generated sets:\s+(\d+)'), int, str)
parsers['constrained by record'] = LineParser(re.compile('\s*Constrained by record:\s+(\d+)'), int, str)
parsers['number of sets'] = LineParser(re.compile('\s*# of sets per max time:\s+(\d+\.?\d*)'), float, '{:.2f}'.format)
parsers['valuation time'] = LineParser(re.compile('Valuation Time:\s+(\d+\.?\d*)'), float, '{:.3f}'.format)
parsers['thread time variance'] = LineParser(re.compile('\s*Time variance:\s+(\d+\.?\d*)'), float, '{:.5f}'.format)

class LogData(object):
    def __init__(self, log, parsers):
        self.lines = log.split('\n')
        self.parsers = parsers

        self._parse()

    def _parse(self):
        self.result = {}
        for line in self.lines:
            for name, parser in self.parsers.iteritems():
                value = parser.parse(line)
                if value != None:
                    self.result[name] = value
                    break

class TableData(object):
    def __init__(self, header, rows):
        self.header = header
        self.rows = rows


def build_table_data(parsers, all_runs):
    header = parsers.keys()
    formatters = []
    for title in header:
        formatters.append(parsers[title].format_function)
    rows = []
    for task_name, runs in all_runs.iteritems():
        row = [0]*len(header)
        for run in runs:
            idx = 0
            for title in header:
                row[idx] += run['result'].get(title, -1e20)
                idx += 1
        num_runs = len(runs)
        row = [val / num_runs for val in row]
        row = [formatter(value) for formatter, value in zip(formatters, row)]
        rows.append(list(parse_task_name(task_name)) + row)
    rows = sorted(rows, key = lambda row: row[0]+row[1])
    return TableData(['problem name', 'solver_settings']+header, rows)

class HtmlWriter(object):
    MainHtml = """<html>
    <head>
        %(head)s
        <script language="javascript"> %(script)s </script>
    </head>
    <body>
        %(body)s
    </body>
</html>
"""
    TextBlock = '<pre>%s</pre>'
    Line = '<hr />'
    Caption = '<h2> %s </h2>'
    JavaScript = '<script language="javascript"> %s </script>'

    def __init__(self):
        self.contents = {}
        self.contents['head'] = ''
        self.contents['script'] = ''
        self.contents['body'] = ''

    def append_to_head(self, value):
        self.contents['head'] += value + '\n'

    def append_to_script(self, value):
        self.contents['script'] += value + '\n'

    def append_to_body(self, value):
        self.contents['body'] += value + '\n'

    def write_js_variable(self, name, value):
        self.append_to_script('var %s = %s;' % (name, value))

    def write_text(self, text):
        self.append_to_body(HtmlWriter.TextBlock % text)

    def write_line(self):
        self.append_to_body(HtmlWriter.Line)

    def write_caption(self, caption):
        self.append_to_body(HtmlWriter.Caption % caption)

    def __repr__(self):
        return HtmlWriter.MainHtml % self.contents

    def str(self):
        return self.__repr__()

def create_html_table(table_data):
    table = '<table id="tabledata" border="1" cellspacing="0" cellpadding="2"> %s </table>'
    row = '<tr> %s </tr>'
    cell = '<td> %s </td>'
    html = ['<table>']
    html.append('<tr>')
    func = '<a href="#" onclick=\'show_log("%s", "%s", 0);return false;\'>%s</a>'

    rows = []
    row_data = ''
    for title in table_data.header:
        row_data += cell % title
    rows.append(row % row_data)
    for r in table_data.rows:
        row_data = ''
        for item in r:
            row_data += cell % (func % (r[0], r[1], item))
        rows.append(row % row_data)
    return table % '\n'.join(rows)

def main(filename):
    JSFunction="""
var show_log = function (problem_name, solver_settings, index) {
    ctrl = document.getElementById("logdata");
    ctrl.hidden = false;
    ctrl.value = logs[problem_name][solver_settings][index];
    ctrl.selectionStart = 0;
    ctrl.selectionEnd = 0;
    ctrl.style.height = "1px"
    ctrl.style.height = (ctrl.scrollHeight+10)+ "px";

    table = document.getElementById("tabledata");
    ctrl.style.width = "1px";
    ctrl.style.width = table.offsetWidth + "px";

    // show logrefs panel
    logrefs = document.getElementById("logrefs");
    logrefsHtml = ''
    if (logs[problem_name][solver_settings].length > 1) {
        logrefsHtml = "| "
        for (i = 0; i < logs[problem_name][solver_settings].length; ++i) {
            logrefsHtml += '<a href="#" onclick="show_log('
            logrefsHtml += "'" + problem_name + "', ";
            logrefsHtml += "'" + solver_settings + "', ";
            logrefsHtml += i + '); return false;\">';
            logrefsHtml += "[ run # " + (i+1) +  " ] ";
            logrefsHtml += "</a> | "
        }
    }
    logrefs.innerHTML = logrefsHtml;
}
"""
    pms_data = yaml.load(open(filename))['logs']
    entries = {}
    log_outputs = {}
    for row in pms_data:
        task_name = row['task_name']
        entry = {}
        entry['result'] = LogData(row['output'], parsers).result
        if not task_name in entries:
            entries[task_name] = []
        entries[task_name].append(entry)

        problem_name, solver_settings = parse_task_name(task_name)
        if not problem_name in log_outputs:
            log_outputs[problem_name] = {}
        if not solver_settings in log_outputs[problem_name]:
            log_outputs[problem_name][solver_settings] = []
        log_outputs[problem_name][solver_settings] .append(row['output'])

    table_data = build_table_data(parsers, entries)
    report_filename = filename.replace('.yaml', '.html')
    writer = HtmlWriter()
    writer.write_js_variable('logs' ,json.dumps(log_outputs))
    writer.append_to_script(JSFunction)
    writer.append_to_body(create_html_table(table_data))
    writer.append_to_body('<br /><div id="logrefs"></div> <br />')
    writer.append_to_body('<textarea id="logdata" cols="120" rows="20" readonly hidden> </textarea> <br />')
    with open(report_filename, 'w') as f:
        f.write(writer.str())



if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage:', sys.argv[0], '<pms_report>.yaml'
        sys.exit(1)
    main(sys.argv[1])
