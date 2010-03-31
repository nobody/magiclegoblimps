import os
import signal
import subprocess
import settings
from logger import log

def launch(vfeed):
    """
    Launch a new ffserver/ffmpeg live feed
    """
    if settings.DEBUG:
        dummy_launch(vfeed)
        return

    vfeed.ffserver_proc = subprocess.Popen(ffserver_args(vfeed))
    log('started ffserver proc {0} with config file {1}'.format(
        str(vfeed.ffserver_proc.pid), vfeed.config_file))

    vfeed.ffmpeg_proc = subprocess.Popen(ffmpeg_args(vfeed))
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
        '-r', '15',
        '-b', '1024k',
        '-i', vfeed.feed_url,
        settings.LIVE_FEED_URLS.format(settings.CURRENT_IP, vfeed.port,
                                       vfeed.feed_name)
    ]

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
