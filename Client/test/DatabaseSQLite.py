import sqlite3
from Robot import Robot
import random

class DatabaseSQLite():
    """
    This class wraps access to an SQLite database which contains
    init data for the simulation. The database simply contains
    information robots currently.
    """
    def __init__(self, DB='init_data/test.db'):
        self.DB = DB

    def getAllRobots(self):
        """ 
        Returns all the robots in the current database as a list of Robot object 
        """
        self.c.execute('select * from robots order by robotid')
        rows = self.c.fetchall()
        robotList = []
        for row in rows:
            animallist = []
            r = Robot(row[0], row[1],(row[2],row[3]),row[4], animallist ,row[5])
            robotList.append(r)
        return robotList

# BASIC DB METHODS
    def connect(self):
        self.conn = sqlite3.connect(self.DB)
        self.c = self.conn.cursor()
    def close(self):
        self.conn.close()
    def commit(self):
        self.conn.commit()
