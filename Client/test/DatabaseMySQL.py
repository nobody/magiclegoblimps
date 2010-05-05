import MySQLdb
import random
from Robot import Robot

class DatabaseMySQL():
    """
    This class wraps access to a mySQL database which contains
    the data for the live zoocam website.
    """

    def __init__(self, host='localhost', user='root', passwd='pass', db='test.db'):
        self.HOST = host
        self.USER = user
        self.PASSWD = passwd
        self.DB = db

# ROBOT/SERVICE METHODS
    def insertAllRobots(self, robotList):
        """
        Removes every robot from the camera table (truncates) and inserts our own.
        """
        self.c.execute ("TRUNCATE TABLE cameras")
        for r in robotList:
            self.c.execute ("INSERT INTO `cameras` (`camera_id`, `camera_type`, `camera_curr_x`, `camera_curr_y`, `camera_resolution`, `camera_fps`, `camera_orientation`) VALUES (\'%s\', \'0\', \'%s\', \'%s\', \'160 120\', \'32\', \'%s\');", (r.robotid, r.x, r.y, r.orient))
        self.commit()

    def saveAllRobots(self, robotList):
        """
        Delete all in the service table and repopulate it. Update the camera table
        with information about the robots
        """
        self.c.execute ("TRUNCATE TABLE service")
        for r in robotList:
            for animal in r.animals:
                randQOS = round(random.random(), 2)
                self.c.execute ("INSERT INTO `service` (`service_id`, `service_camera_id`, `service_object_id`, `service_qos`) VALUES (NULL, \'%s\', \'%s\', \'%s\');", (r.robotid, animal, randQOS))
        for r in robotList:
            if len(r.animals) == 0:
                self.c.callproc('setCameraPosition', (r.robotid, r.x, r.y, None, r.orient))
            else:
                self.c.callproc('setCameraPosition', (r.robotid, r.x, r.y, r.animals[0], r.orient))
            self.c.callproc('addCameraFeed', (r.robotid, r.feed))
        self.commit()

# COMMAND QUEUE METHODS

    def insertCommand(self, robotid, action, degrees=0):
        """
        Inserts a command into a command table. This is used simply for the simulation.
        """
        self.c.execute ("INSERT INTO `commands` (`robotid`, `action`, `degrees`) VALUES (%s, %s, %s);", (robotid, action, degrees))
        self.commit()

    def clearCommandQueue(self):
        self.c.execute('DELETE FROM commands;')
        self.commit()
    
    def processCommandQueue(self, robotList):
        """
        This goes through every command and executes that command for whatever robot. 
        The robot might not be in our robotlist and that's an error. Catch the exception.
        """
        self.c.execute('select * from commands')
        for command in self.c:
            try:
                i = robotList.index(Robot(command[0]))
                r = robotList[i]
                r.action(command[1], robotList)
                print 'performing action %s on robot %s' % (command[1], command[0])
            except Exceptions as inst:
                print '-----------------------------------'
                print inst
                print '-----------------------------------'
        self.clearCommandQueue()
        self.commit()

# USER VOTING METHODS
    def vote(self, animalid, num):
        self.c.execute('UPDATE animals SET animal_num_views=animal_num_views+%s WHERE animal_id=%s', (num, animalid))
        self.c.execute('UPDATE total_votes SET total_votes_num=total_votes_num+%s', (num))
        self.commit()

# BASIC DB METHODS

    def connect(self):
        self.conn = MySQLdb.connect (host = self.HOST, user = self.USER, passwd = self.PASSWD, db = self.DB)
        self.c = self.conn.cursor()
    def close(self):
        self.conn.close()
    def commit(self):
        self.conn.commit()
