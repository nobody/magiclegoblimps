#!/usr/bin/python3

import re
import subprocess

def getip():
    """
    Gets the ipaddress of this computer from th e output of the ifconfig
    command. I don't know where else to get it from.
    """
    proc = subprocess.Popen(['ifconfig', 'eth0'], stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    proc.wait()
    if proc.returncode != 0:
        return '127.0.0.1'
    else: # specific to our linux server
        lines = proc.stdout.readlines()
        match = re.search(r'inet addr:(\d+\.\d+\.\d+\.\d+)', str(lines[1]))
        print(str(dir(match)))
        return match.group(1)

if __name__ == '__main__':
    print("'" + getip() + "'")
