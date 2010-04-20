#!/usr/bin/python3

import os
import time
import socket
import config
import ffserver
import settings
import MySQLdb
from vidcontrol import change_working_directory
from logger import log

class Archive():

    def __init__(self):
        """
        Sets up all of the lists that are needed
        """
        self.filenames = []
        self.objects = []
        self.qos = []
        self.objects_qos = {}

    def get_Archive(self):
        """
        Grabs all of the filenames in the archive directory
        Splits out the information in the filenames
        """
        tmpfilenames = os.listdir(settings.ARCHIVE_DIR)
        for x in tmpfilenames:
            parts = x.split('-')

            # make sure this is a valid archive video entry
            if not x.endswith('.flv') or len(parts) != 3:
                continue

            self.filenames.append(x)
            qos = parts[0]
            self.qos.append(qos)
            obj = parts[1]
            self.objects.append(obj)

        for x in range(len(self.filenames)):
            self.objects_qos[self.objects[x]] = self.qos[x]
        log('Checking archive folder for files')
    
    def create_HTML(self):
        """
        Create a file that has the links and information for the current archived directory
        """
        with open(settings.ARCHIVE_HTML, 'w') as f:
            f.write('<html>\n')
            f.write('<head>\n')
            f.write('<title>Current Archived Videos</title>\n')
            f.write('</head>\n')
            f.write('<body>\n')
            f.write('<h1>Current Archived Videos</h1>\n')
            for x in range(len(self.filenames)):
                f.write('<a href=\"{0}/{1}\">{2}</a>\n'.format(settings.ARCHIVE_DIR, self.filenames[x], self.filenames[x]))
            f.write('</body>\n')
            f.write('</html>\n')
        log('Created HTML page with latest archives')

    def update(self):
        """
        -update the SQL database, with the current information
        -using there stored procedure
        """
        
        log('Updated the Client database with Archived Videos')

if __name__ == '__main__':
    change_working_directory()
    ar = Archive()
    ar.get_Archive()
    ar.create_HTML()
    print(str(ar.objects_qos))
