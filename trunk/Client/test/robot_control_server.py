#!/usr/bin/python
import SocketServer
import re
from DatabaseMySQL import DatabaseMySQL


# START MODIFYABLE VARIABLES FOR THE TEST
DB_HOST = "localhost"
DB_USER = "testing"
DB_PASSWD = "1EA96F710"
DB_NAME = "zoocam"
# END MODIFYABLE VARIABLES FOR THE TEST

class MyTCPHandler(SocketServer.BaseRequestHandler):
    """
    The RequestHandler class for our robot command server.
    """
    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).strip()
        print "%s wrote: %s" % (self.client_address[0], self.data)
        result = self.parseCommand(self.data)

        if len(result) == 4:
            self.saveResult(result[1], result[0], result[3])
            print 'Saved Command: Robot %s, Action %s, Degrees %s' % (result[1], result[0], result[3])
        else:
            self.saveResult(result[1], result[0])
            print 'Saved Command: Robot %s, Action %s' % (result[1], result[0])

    def parseCommand(self, data):
        splitter = re.compile(r'\D')
        a = splitter.split(data) # split into a list
        a = a[:-1]
        return a # action, robotid, degrees

    def saveResult(self, robotid, action, degrees=0):
        db = DatabaseMySQL(host = DB_HOST, user = DB_USER, passwd = DB_PASSWD, db = DB_NAME)
        db.connect()
        db.insertCommand(robotid, action, degrees)
        db.close()

if __name__ == "__main__":
    HOST, PORT = "localhost", 10000

    # Create the server, binding to localhost on port 10000
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    print '-----------------------------------'
    print '----- STARTING ROBOT SERVER -------'
    print '-----------------------------------'
    server.serve_forever()
