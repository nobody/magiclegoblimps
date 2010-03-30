"""
Application settings for the video delivery server controller.
"""

from getip import getip

# General settings
LOGFILE = 'vidserver.log'
ROOT_DIR = '/home/jordan/mlb/magiclegoblimps/Server/VideoDelivery'
LIVE_CSV_FILE = '/var/www/mlb/live-feeds.csv'
ARCHIVE_CSV_FILE = '/var/www/mlb/archive-feeds.csv'

# QoS server settings
QOS_SERVER_URL = ''

# Video server settings
CURRENT_IP = getip()
LIVE_FEED_URLS = 'rtmp://{0}:{1}/{2}.flv' # domain, port, filename
ARCHIVE_FEED_URLS = 'http://{0}/{2}.flv' # domain, filename

CONFIG_TEMPLATE = 'config-template.txt'
CONFIG_FILE_DIR = './'
ARCHIVE_DIR = './archives'

CONFIG_FNAME = 'ffserver-{0}.conf'
STREAM_NAME = 'stream-{0}'
FEED_NAME = 'feed-{0}'

FFMPEG_ARGS = ('ffmpeg', '-r', '15', '-i', 'input-stream', '-f', 'flv',
    'output-stream')

FFSERVER_ARGS = ('ffserver', '-f', 'config-file')
