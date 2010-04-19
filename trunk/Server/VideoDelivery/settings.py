"""
Application settings for the video delivery server controller.
"""

from getip import getip

# General settings
DEBUG = True
LOGFILE = 'logs/vidserver.log'
TRANSCRIPT_FILE = 'logs/transcript.log'
ROOT_DIR = [
        '/home/jordan/mlb/magiclegoblimps/Server/VideoDelivery',
        '/Users/jordanbach/Desktop/Spring2010/csproject/magiclegoblimps/Server/VideoDelivery', '/home/duck/Desktop/school/magiclegoblimps/Server/VideoDelivery']
LIVE_CSV_FILE = '/var/www/mlb/live-feeds.csv'
ARCHIVE_CSV_FILE = '/var/www/mlb/archive-feeds.csv'
ARCHIVE_HTML = './archives/index.html'

# QoS server settings
QOS_SERVER_URL = '10.21.1.110'
QOS_SERVER_PORT = 20000
MAX_CONNECTION_ATTEMPTS = 10
CONNECTION_RETRY_INTERVAL = 10

# Video server settings
CURRENT_IP = getip()
FEEDER_URLS = 'http://localhost:{0}/{1}.ffm'
LIVE_FEED_URLS = 'http://{0}:{1}/{2}.flv' # domain, port, filename
ARCHIVE_FEED_URLS = 'http://{0}/{2}.flv' # domain, filename

CONFIG_TEMPLATE = 'config-template.txt'
CONFIG_FILE_DIR = './'
ARCHIVE_DIR = './archives'

CONFIG_FNAME = 'ffserver-{0}.conf'
STREAM_NAME = 'live-{0}'
FEED_NAME = 'feed-{0}'

FFMPEG_ARGS = ('ffmpeg', '-r', '15', '-i', 'input-stream', '-f', 'flv',
    'output-stream')

FFSERVER_ARGS = ('ffserver', '-f', 'config-file')
