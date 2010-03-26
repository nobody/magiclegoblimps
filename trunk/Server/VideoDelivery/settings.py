"""
Application settings for the video delivery server controller.
"""

# General settings
LOGFILE = 'vidserver.log'
ROOT_DIR = '/Users/jordanbach/Desktop/Spring2010/csproject/magiclegoblimps/Server/VideoDelivery'

# QoS server settings
QOS_SERVER_URL = ''

# Video server settings
CURRENT_IP = '0.0.0.0'
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

# Database settings
DATABASE_SERVER = 'url'
DATABASE_NAME = 'dbname'
DATABASE_USER = 'dbuser'
DATABASE_PASSWORD = 'secret'
