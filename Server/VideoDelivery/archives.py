#!/usr/bin/python

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
        self.objects_qos = []

    def get_Archive(self):
        self.filenames = os.listdir(settings.ARCHIVE_DIR)
        for x in self.filenames:
            self.qos.append(self.filenames[:3])
        for x in self.filenames:
            self.objects.append(self.filenames[4:6])
        for x in range(len(self.filenames)):
            self.objects_qos[self.objects[x]] = self.qos[x]

if __name__ == '__main__':
    change_working_directory()
    ar = Archive()
    ar.get_Archive()
    print(str(self.objects_qos))
