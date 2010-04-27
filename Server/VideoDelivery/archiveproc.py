"""
This is a program that spawns an archiving and thumbnailing process for the
given parameters. If the archive/thumbnail processes complete successfully,
this program will update the client group's database. If at least one of the
processes fail, this program will cleanup any artifacts and not update the
client group's database.
"""

#!/usr/bin/python2.6
from __future__ import print_function, with_statement
from datetime import datetime
import ffserver
import settings
from logger import log
import db
import sys

def update_database(vid_fname, thumb_fname, object_id, object_qos):
    """
    Update the client group's database.
    """
    conn = db.connect()
    vfurl = settings.ARCHIVE_FEED_URLS + vfname
    ssfurl = 'NULL'
    if thumb_fname is not None:
        ssfurl = settings.ARCHIVE_FEED_URLS + ssfname
    db.addArchiveFootage(conn, vfurl, object_id, object_qos, ssfurl)
    db.close(conn)
    
def create_archive(last_archived, object_id, object_qos):
    """
    Creates and archive video and screen shot for the given VidFeed object. If
    an archive is currently being captured for this object, then this method
    does nothing to avoid creating duplicate or really similar archive videos.
    In case multiple archives exist in the given VidFeed, obj_i specifies which
    object we are interested in archiving.
    """
    # Crude test to see if we're already archiving video for this object
    if last_archived is not None:
        timediff = datetime.now() - last_archived
        if timediff.seconds < settings.ARCHIVE_DURATION:
            return

    # create the archives
    last_archived = datetime.now()
    # TODO: get vfeed parameters w/o vfeed object
    (vfname, vidproc) = ffserver.capture_archive(vfeed, object_id, object_qos)
    (ssfname, thumbproc) = ffserver.capture_screenshot(vfeed, vfname)

    vidproc.wait()
    thumbproc.wait()

    if vidproc.returncode != 0 and thumbproc.returncode == 0:
        pass
        # TODO: delete thumbnail file
    elif vidproc.returncode == 0 and thumbproc.returncode != 0: 
        update_database(vfname, None, object_id, object_qos)
    else:
        update_database(vfname, ssfname, object_id, object_qos)

if __name__ == '__main__':
    args = sys.argv
    create_archive() #TODO: update args to night require a VidFeed object
