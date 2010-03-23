#!/usr/bin/python3
"""
Classes for the management and generation of ffserver config files.
"""

import re

class Generator():
    """
    Generate ffserver config files.
    """

    def __init__(self):
        self.defaults = {
            'port': '8090',
            'feed': 'feed0',
            'stream': 'stream0',
            'tmp_file_size': '1M',
            'frame_rate': '15',
            'buf_size': '80000',
            'bit_rate': '200',
            'q_min': '1',
            'q_max': '5',
            'size': '640x480',
        }

    def repf(self, matchobj):
        """ re.sub() replace function """
        var = matchobj.group(0)[1:]
        if var in self.params.keys():
            return self.params[var]
        elif var in self.defaults.keys():
            return self.defaults[var]
        else:
            raise KeyError
        

    def replace(self, line, params):
        """
        Replaces any variables of the form $var with the value corresponding to
        the given key in the params dictionary. If no key exists matching the
        variable name, then a KeyError.
        """
        self.params = params
        return re.sub(r'\$(\w+)', self.repf, line)

    def render(self, params, template_file, conf_file):
        """
        Creates a config file for the given parameters using the given template
        file.
        """
        lines = None
        with open(template_file, 'r') as ftemplate:
            lines = ftemplate.readlines()
        for i in range(len(lines)):
            lines[i] = self.replace(lines[i], params)
        with open(conf_file, 'w') as fconf:
            fconf.writelines(lines)

if __name__ == '__main__':
    """
    Render a sample ffserver config file
    """
    params = {
        'feed': 'feed7',
        'stream': 'stream11',
        'port': '7642',
    }
    gen = Generator()
    gen.render(params, 'config-template.txt', 'sample.conf')
