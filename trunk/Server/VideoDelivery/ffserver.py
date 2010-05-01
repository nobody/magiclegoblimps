from __future__ import print_function, with_statement
import os
import signal
import subprocess
import settings
from VidFeed import VidFeed
from datetime import datetime
from time import time
from logger import log

def launch(vfeed):
    """
    Launch a new ffserver/ffmpeg live feed
    """
    if settings.DEBUG:
        dummy_launch(vfeed)
        return

    vfeed.ffserver_proc = subprocess.Popen(ffserver_args(vfeed),
                                           stdin=subprocess.PIPE,
                                           stdout=subprocess.PIPE)
    log('started ffserver proc {0} with config file {1}'.format(
        str(vfeed.ffserver_proc.pid), vfeed.config_file))

    vfeed.ffmpeg_proc = subprocess.Popen(ffmpeg_args(vfeed),
                                         stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE)
    log('started ffmpeg proc ' + str(vfeed.ffmpeg_proc.pid))

def kill(vfeed):
    """
    Kill an existing ffserver/ffmpeg live feed
    """
    os.kill(vfeed.ffserver_proc.pid, signal.SIGTERM)
    vfeed.ffserver_proc.wait()
    log('ffserver proc {0} terminated with status {1}'.format(
        str(vfeed.ffserver_proc.pid), vfeed.ffserver_proc.returncode))

    os.kill(vfeed.ffmpeg_proc.pid, signal.SIGTERM)
    vfeed.ffmpeg_proc.wait()
    log('ffmpeg proc {0} terminated with status {1}'.format(
        str(vfeed.ffmpeg_proc.pid), vfeed.ffmpeg_proc.returncode))

def ffserver_args(vfeed):
    return ['ffserver', '-f', vfeed.config_file]

def ffmpeg_args(vfeed):
    return [
        'ffmpeg',
        '-f', 'mjpeg',
        '-i', vfeed.feed_url,
        settings.FEEDER_URLS.format(vfeed.port, vfeed.feed_name)
    ]

def get_full_path(arch_fname):
    """
    Returns the complete path (may be absolute or relative) in which to store
    the archive file.
    """
    return settings.ARCHIVE_DIR + '/' + arch_fname

def capture_archive(feed_url, object_id, qos):
    """
    Saves 30 seconds of the given feed to the archives directory.
    Returns the filename for the archive file.
    """
    # def capture_archvie(feed_url, object_id, qos)
    # ffmpeg -f mjpeg -i http://10.176.14.66/video.cgi -t 30 -f flv clip.flv
    archive_file = '{0}-{1}-{2}.flv'.format(
        str(qos), str(object_id), str(int(time())))
    proc = None
    if not settings.DEBUG:
        proc = subprocess.Popen(
                    ['ffmpeg', '-f', 'mjpeg', '-i', feed_url,
                     '-t', str(settings.ARCHIVE_DURATION), '-f', 'flv',
                     get_full_path(archive_file)],
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE)
    log('attempting to archive feed: {0}, object:{1}, qos: {2}'.format(
        feed_url, str(object_id), str(qos)))
    return (archive_file, proc)

def capture_screenshot(feed_url, arch_video_url):
    """
    Creates a screenshot for the specified feed.
    Returns the filename for the screenshot.
    """
    # def capture_screenshot(feed_url, arch_video_url):
    screenshot_file = arch_video_url.replace('.flv', '.jpg')
    proc = None
    if not settings.DEBUG:
        proc = subprocess.Popen(
                ['ffmpeg', '-f', 'mjpeg', '-i', feed_url,
                 '-f', 'image2', '-t', '0.001', '-s',
                 settings.ARCHIVE_THUMB_SIZE, get_full_path(screenshot_file)],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE)
    log('attempting to take screenshot for feed: ' + screenshot_file)
    return (screenshot_file, proc)

def dummy_launch(vfeed):
    """
    Runs dummy processes that don't do anything in place of ffmpeg and
    ffserver processes.
    """
    vfeed.ffserver_proc = subprocess.Popen(['./ffserver-dummy'])
    log('started ffserver proc {0} with config file {1}'.format(
        str(vfeed.ffserver_proc.pid), vfeed.config_file))

    vfeed.ffmpeg_proc = subprocess.Popen(['./ffmpeg-dummy'])
    log('started ffmpeg proc ' + str(vfeed.ffmpeg_proc.pid))

# Testing for the archiving functions
if __name__ == '__main__':
    vf = VidFeed()
    vf.feed_url = 'http://fake-url/video.cgi'
    fname = capture_archive(vf, 3, 0.76)
    capture_screenshot(vf, fname)
