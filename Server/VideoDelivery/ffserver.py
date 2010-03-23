import subprocess

class Manager():
    """
    Manages ffserver processes on the video delivery server.
    """
    
    def __init__(self):
        self.ffserver_procs = []
        self.ffmpeg_procs = []
        self.reserved_ports = {}
        self.config_files = {}
    
    def launchInstance(self, infile, outfile, inconf=None, outconf=None):
        print("launch")
        # determine ports/files/urls to use
        # create a new config file with correct parameters
        # start a new ffserver instance
        # start a new ffmpeg instance
        # detach these processes
        args = ['ffmpeg']
        if inconf is not None:
            for k, v in inconf.items():
                args.append(k)
                args.append(v)
        args.append('-i')
        args.append(infile)
        if outconf is not None:
            for k, v in outconf.items():
                args.append(k)
                args.append(v)
        args.append(outfile)
        
        devnull = open('/dev/null', 'w').fileno()
        ffmpeg_proc = subprocess.Popen(args)
        print("launched")
        pass
    
    def killInstance(self, pid1, pid2):
        print("kill")
        pass
