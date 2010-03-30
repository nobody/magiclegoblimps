#!/usr/bin/python3

import re
import subprocess

def getip():
    """
    Gets the ipaddress of this computer from th e output of the ifconfig
    command. I don't know where else to get it from.
    """
    proc = subprocess.Popen(['ifconfig', 'eth0'], stdout=subprocess.PIPE)
    lines = proc.stdout.readlines()
    match = re.search(r'inet addr:(\d+\.\d+\.\d+\.\d+)', str(lines[1]))
    return match.group(1)

if __name__ == '__main__':
    print("'" + getip() + "'")
