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
    vfurl = settings.ARCHIVE_FEED_URLS + vid_fname
    ssfurl = 'NULL'
    if thumb_fname is not None:
        ssfurl = settings.ARCHIVE_FEED_URLS + thumb_fname
    db.addArchiveFootage(conn, vfurl, object_id, object_qos, ssfurl)
    db.close(conn)
    
def create_archive(feed_url, object_id, object_qos):
    """
    Creates and archive video and screen shot for the given VidFeed object and
    waits for the archive to complete before updating the database.
    """
    # create the archives
    (vfname, vidproc) = ffserver.capture_archive(feed_url, object_id,
                                                 object_qos)
    (ssfname, thumbproc) = ffserver.capture_screenshot(feed_url, vfname)

    vidproc.wait()
    thumbproc.wait()

    if vidproc.returncode != 0 and thumbproc.returncode != 0:
        log('archiving and thumbnail failed for feed: ' + feed_url)
    elif vidproc.returncode != 0 and thumbproc.returncode == 0:
        log('archiving failed for feed: ' + feed_url)
        pass # TODO: delete thumbnail file
    elif vidproc.returncode == 0 and thumbproc.returncode != 0:
        # video feed ok, but no thumbnail failed
        log('create archive clip, but could not create thumbnail for feed: '
            + feed_url)
        update_database(vfname, None, object_id, object_qos)
    else:
        log('archive completed successfully for feed: ' + feed_url)
        update_database(vfname, ssfname, object_id, object_qos)

def get_datetime(timestamp):
    return datetime.strptime(timestamp, '%Y-%b-%d %H:%M:%S')

if __name__ == '__main__':
    args = sys.argv
    if len(args) != 4:
        log('usage: python archiveproc.py video_url obj_id obj_qos')
        exit(1)
    feed_url = args[1]
    object_id = args[2]
    object_qos = args[3]
    create_archive(feed_url, object_id, object_qos)
