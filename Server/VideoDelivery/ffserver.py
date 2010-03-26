import subprocess

def launch(vfeed):
    """
    Launch a new ffserver/ffmpeg live feed
    """
    pass

def kill(vfeed):
    """
    Kill an existing ffserver/ffmpeg live feed
    """
    pass

def ffserver_args(vfeed):
    return ('ffserver', '-f', vfeed.config_file)

def ffmpeg_args(vfeed):
    return (
        'ffmpeg',
        '-r', '15',
        '-b', '1024k',
        '-i', vfeed.feed_url,
        settings.LIVE_FEED_URLS.format(settings.CURRENT_IP, vfeed.port,
                                       vfeed.feed_name)
    )

