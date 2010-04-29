#!/usr/bin/python2.6
from __future__ import print_function, with_statement
import os
from datetime import datetime
import ffserver
import settings
from logger import log
import db

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

            self.filenames.append(settings.ARCHIVE_FEED_URLS + x)
            qos = parts[0]
            self.qos.append(qos)
            obj = parts[1]
            self.objects.append(obj)
        
        for x in tmpfilenames:
            parts = x.split('.')

            # make sure this is a valid archive video entry
            if not x.endswith('.flv') or len(parts) != 3:
                continue

            thumb = parts[0] + parts[1] + '.jpg'
            self.thumb.append(thumb)

        for x in range(len(self.filenames)):
            self.objects_qos[self.objects[x]] = self.qos[x]
        log('Checking archive folder for files')

    def checkDatabase(self):
        """
        Grabs the information from the database and then checks if videos in
        archive database match those in folder

        NOT TESTED because I couldn't connect to the database
        """
        getArchives()
        conn = db.connect()
        cursor = conn.cursor()
        sql = 'SELECT archives_url FROM archives'
        cursor.execute(sql)
        results = cursor.fetchall()
        for x in range(len(self.filenames)):
            for y in results:
                if filenames[x] == y[0]:
                    # File is already in the database
                    continue
                else:
                    # File in the folder doesn't exist on the database must add
                    # it to the database
                    conn = db.connect()
                    db.addArchiveFootage(conn,filenames[x],objects[x],qos[x],thumb[x])

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

    def updateInital(self):
        """
        -update the SQL database, with the current information
        -using there stored procedure
        """
        try:
            conn = db.connect()
            for x in range(len(self.filenames)):
                if checkArchive(conn,settings.ARCHIVE_FEED_URL+self.filenames[x]):
                    pass
                    # Archive is currently there no need to add
                else:
                    # Archive is not there need to update database
                    db.addArchiveFootage(conn,settings.ARCHIVE_FEED_URL+self.filenames[x],
                                        self.objects[x],self.qos[x],
                                        settings.ARCHIVE_FEED_URL+'images/'+self.thumb[x]+'.jpg')
            log('Updated the Client database with Archived Videos')
        except MySQL.Error as e:
            log("Error %d: %s" % (e.args[0], e.args[1]))

def create_archive(conn, vfeed, obj_i):
    """
    Creates and archive video and screen shot for the given VidFeed object. If
    an archive is currently being captured for this object, then this method
    does nothing to avoid creating duplicate or really similar archive videos.
    In case multiple archives exist in the given VidFeed, obj_i specifies which
    object we are interested in archiving.
    """
    # Crude test to see if we're already archiving video for this object
    if vfeed.last_archived is not None:
        timediff = datetime.now() - vfeed.last_archived
        if timediff.seconds < settings.ARCHIVE_DURATION:
            return

    # create the archives
    vfeed.last_archived = datetime.now()
    obj_to_archive = vfeed.objects[obj_i]
    vfname = ffserver.capture_archive(vfeed, obj_to_archive[0],
                                     obj_to_archive[1])
    ssfname = ffserver.capture_screenshot(vfeed, vfname)

    # update client database
    vfurl = settings.ARCHIVE_FEED_URLS + vfname
    ssfurl = settings.ARCHIVE_FEED_URLS + ssfname
    db.addArchiveFootage(conn, vfurl, vfeed.objects[obj_i][0],
                         vfeed.objects[obj_i][1], ssfurl)

if __name__ == '__main__':
    ar = Archive()
    ar.get_ArchiveFeeds()
    print(str(ar.thumb))
