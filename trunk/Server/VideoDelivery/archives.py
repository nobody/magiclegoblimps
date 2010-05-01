#!/usr/bin/python2.6
from __future__ import print_function, with_statement
import os
import signal
from datetime import datetime
import ffserver
import settings
from logger import log
import db
import subprocess
import VidFeed
import MySQLdb

class Archive():

    def __init__(self):
        """
        Sets up all of the lists that are needed
        """
        self.filenames = []
        self.objects = []
        self.qos = []
        self.thumb = []
        self.objects_qos = {}

    def get_ArchiveFeeds(self):
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
        
        for x in tmpfilenames:
            parts = x.split('.')

            # make sure this is a valid archive video entry
            if not x.endswith('.flv') or len(parts) != 3:
                continue

            thumb = parts[0] + '.' + parts[1]
            self.thumb.append(thumb)

        for x in range(len(self.filenames)):
            self.objects_qos[self.objects[x]] = self.qos[x]
        log('Checking archive folder for files')

    def checkDatabase(self,conn,filename):
        """
        Grabs the information from the database and then checks if videos in
        archive database match those in folder
        """
        try:
            cursor = conn.cursor()
            sql = 'SELECT archives_url FROM archives'
            cursor.execute(sql)
            results = cursor.fetchall()
            for y in results:
                if settings.ARCHIVE_FEED_URLS+filename == y[0]:
                    # File is already in the database
                    return False
                else:
                    # File in the folder doesn't exist on the database must add
                    # it to the database
                    return True
        except MySQLdb.Error as e:
            log("Error %d: %s" % (e.args[0], e.args[1]))

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

    def updateInital(self,conn):
        """
        -update the SQL database, with the current information
        -using there stored procedure
        """
        self.get_ArchiveFeeds()
        try:
            db.deleteTable(conn,'archives')
            for x in range(len(self.filenames)):
                if self.checkDatabase(conn,self.filenames[x]):
                    pass
                    # Archive is currently there no need to add
                else:
                    # Archive is not there need to update database
                    db.addArchiveFootage(conn,settings.ARCHIVE_FEED_URLS+self.filenames[x],
                                        self.objects[x],self.qos[x],
                                        settings.ARCHIVE_FEED_URLS+'images/'+self.thumb[x]+'.jpg')
            log('Updated the Client database with Archived Videos')
        except MySQLdb.Error as e:
            log("Error %d: %s" % (e.args[0], e.args[1]))

def create_archive(vfeed, obj_i):
    """
    Launches the archiveproc.py program that handles creating archives or
    cleanup up failed attempts to creat archives. This function will not
    archive a feed if it is already being archived.
    """
    # check to see if this feed is currently being archived
    if vfeed.archive_proc is not None:
        vfeed.archive_proc.poll()
        if vfeed.archive_proc.returncode is None:
            log('refused archive (already archiving) for ' + str(vfeed))
            return
        else:
            # done archiving
            log('archiveproc returned {0} for {1}'.format(
                    str(vfeed.archive_proc.returncode),
                    str(vfeed)))
            vfeed.archive_proc = None

    # everything's ok, let's create an archive clip
    args = ['python',
            settings.ARCHIVE_PROC_NAME,
            vfeed.feed_url,
            str(vfeed.objects[obj_i][0]), # object id
            str(vfeed.objects[obj_i][1])] # object qos

    log('starting archive proc for ' + str(vfeed))
    vfeed.archive_proc = subprocess.Popen(args, stdin=subprocess.PIPE,
                                          stdout=subprocess.PIPE,
                                          stderr=subprocess.PIPE)

def cancel_archive(vfeed):
    """
    Terminates an outstanding archives processes. The archive process is
    expected to clean up after itself.
    """
    if vfeed.archive_proc is not None:
        os.kill(vfeed.archive_proc.pid, signal.SIGTERM)
        vfeed.archive_proc.wait()
        log('cancelled archive for {0} with return code {1}'.format(
                str(vfeed), str(vfeed.archive_proc.returncode)))

if __name__ == '__main__':
    ar = Archive()
    conn = db.connect()
    ar.updateInital(conn)
    
    vf = VidFeed.VidFeed()
    vf.feed_url = 'http://archive-test/video.mjpeg'
    vf.objects = [(-1, 0.01), (-2, 0.02)]
    create_archive(vf, 1)
