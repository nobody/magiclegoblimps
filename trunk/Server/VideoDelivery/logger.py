from __future__ import print_function, with_statement
from datetime import datetime
import settings

def clearlogs():
    with open(settings.LOGFILE, 'w') as f:
        pass
    with open(settings.TRANSCRIPT_FILE, 'w') as f:
        pass

def timestamp():
    return str(datetime.today())

def log(message):
    m = '[{0}] {1}\n'.format(timestamp(), message)
    with open(settings.LOGFILE, 'a') as f:
        f.write(m)

def transcribe(text):
    with open(settings.TRANSCRIPT_FILE, 'a') as f:
        f.write(text)
