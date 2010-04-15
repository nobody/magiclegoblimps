#!/usr/bin/python3

import os
import time
import socket
import config
import ffserver
import settings
from vidcontrol import change_working_directory

class Archive():

    def __init__(self):
        self.filenames = []
        self.objects = []
        self.qos = []
        self.objects_qos = {}

    def get_Archive(self):
        self.filenames = os.listdir(settings.ARCHIVE_DIR)
        for x in self.filenames:
            parts = x.split('-')
            qos = parts[0]
            self.qos.append(qos)
            obj = parts[1]
            self.objects.append(obj)
        for x in range(len(self.filenames)):
            self.objects_qos[self.objects[x]] = self.qos[x]

if __name__ == '__main__':
    change_working_directory()
    ar = Archive()
    ar.get_Archive()
    print(str(ar.objects_qos))