"""
Application settings for the video delivery server controller.
"""

# General settings
DEBUG = True
TESTMODE = True
LOGFILE = 'logs/vidserver.log'
TRANSCRIPT_FILE = 'logs/transcript.log'
ROOT_DIR = [
        '/home/jordan/mlb/magiclegoblimps/Server/VideoDelivery',
        '/Users/jordanbach/Desktop/Spring2010/csproject/magiclegoblimps/Server/VideoDelivery', '/home/duck/Desktop/school/magiclegoblimps/Server/VideoDelivery']
LIVE_CSV_FILE = '/var/www/mlb/live-feeds.csv'

# QoS server settings
#QOS_SERVER_URL = '10.21.1.110'
# QOS_SERVER_URL = '10.176.14.94'
# QOS_SERVER_PORT = 20000
QOS_SERVER_URL = 'localhost'
QOS_SERVER_PORT = 5678
MAX_CONNECTION_ATTEMPTS = 10
CONNECTION_RETRY_INTERVAL = 30

# Video server settings
# CURRENT_IP = '10.176.14.65'
CURRENT_IP = 'localhost'
# CURRENT_IP = '00-22-19-13-c5-ad.dyn.utdallas.edu'
FEEDER_URLS = 'http://localhost:{0}/{1}.ffm'
LIVE_FEED_URLS = 'http://{0}:{1}/{2}.flv' # domain, port, filename

# archive settings
ARCHIVE_FEED_URLS = 'http://'+CURRENT_IP+'/archives/' # domain, filename
ARCHIVE_CSV_FILE = '/var/www/mlb/archive-feeds.csv'
ARCHIVE_HTML = './archives/index.html'
ARCHIVE_DURATION = 30
ARCHIVE_QOS_THRESHOLD = 0.49
ARCHIVE_THUMB_SIZE = '60x44'

CONFIG_TEMPLATE = 'config-template.txt'
CONFIG_FILE_DIR = './'
ARCHIVE_DIR = './archives'
# ARCHIVE_DIR = '/var/www/mlb/archives'

CONFIG_FNAME = 'ffserver-{0}.conf'
STREAM_NAME = 'live-{0}'
FEED_NAME = 'feed-{0}'

FFMPEG_ARGS = ('ffmpeg', '-r', '15', '-i', 'input-stream', '-f', 'flv',
    'output-stream')

FFSERVER_ARGS = ('ffserver', '-f', 'config-file')

# MySQL settings
MySQL_HOST = '0-22-19-13-ab-d8.dyn.utdallas.edu'
MySQL_USER = 'servergroup'
MySQL_PASS = 'agrajag'
MySQL_DATABASE = 'zoocam'
