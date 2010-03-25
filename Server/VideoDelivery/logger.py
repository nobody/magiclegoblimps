from datetime import datetime
import settings

def timestamp():
    d = datetime.today()
    return '{0:02d}:{1:02d}:{2:02d} {3:02d}/{4:02d}/{5}'.format(
                    d.hour, d.minute, d.second, d.month, d.day, d.year)

def log(message):
    m = '[{0}] {1}\n'.format(timestamp(), message)
    with open(settings.LOGFILE, 'a') as f:
        f.write(m)

