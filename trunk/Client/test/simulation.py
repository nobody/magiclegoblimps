#!/usr/bin/python
import MySQLdb
import socket
import random
import time
from Robot import Robot
from User import User
from DatabaseMySQL import DatabaseMySQL
from DatabaseSQLite import DatabaseSQLite


# START MODIFYABLE VARIABLES FOR THE TEST
turn_prob = 5
forward_prob = 3
looseanimal_prob = 5
gainanimal_prob = 10
num_animals = 8

user_arrive_prob = 20 # 0-20 users will arrive every 5 seconds
user_leave_prob = 12 # 0-12 users will leave every 5 seconds
user_cap = 2000 # total number of users possible
user_change_page_prob = 15

TESTDB = 'init_data/test1.db'

HOST = "0-22-19-13-ab-d8.dyn.utdallas.edu"
USER = "testing"
PASSWD = "1EA96F710"
DB = "zoocam"
# END MODIFYABLE VARIABLES FOR THE TEST

totalusernum = 0

# DO ROBOT STUFF:
def doRobotStuff(db, robotList):
    db.processCommandQueue(robotList)
    print '-- Robots -------------------------'
    for r in robotList:
        if r.automove == False:
            continue
        # turn
        if random.randint(1, turn_prob) == 1:
            if random.randint(1, 2) == 1:
                r.turnLeft()
            else:
                r.turnRight()
        # go forward
        if random.randint(1, forward_prob) == 1:
            r.moveForward(robotList)
        if random.randint(1, gainanimal_prob) == 1:
            r.addAnimal(random.randint(1, num_animals))
        if random.randint(1, looseanimal_prob) == 1:
            r.looseAnimal()
        print r
    db.saveAllRobots(robotList)

# DO USER STUFF:
def doUserStuff(db, userList):
    print '-- Users --------------------------'
    currentusernum = len(userList)
    animalList = []
    for i in range(num_animals):
        animalList.append(0)
    newusernum = random.randint(0, user_arrive_prob)
    for i in range(newusernum):
        if len(userList)+1 <= user_cap:
            userList.append(User())
    leavingusernum = random.randint(0, user_leave_prob)
    for i in range(leavingusernum):
        randuser = random.choice(userList)
    
    for u in userList:
        if random.randint(1, user_change_page_prob) == 1:
            u.change_page(random.randint(1, num_animals))
        if u.needs_vote:
            animalList[u.page-1] += 1
            u.needs_vote = False
        u.increment_time(5)
    sumofvotes=0
    for i in range(num_animals):
        db.vote(i+1, animalList[i])
        sumofvotes+=animalList[i]
    print 'Number of users:', len(userList)
    print 'Number of animal votes: ', sumofvotes
    
#user_arrive_prob = 20 # 0-20 users will arrive every 5 seconds
#user_leave_prob = 12 # 0-12 users will leave every 5 seconds
#user_cap = 2000 # total number of users possible
#user_change_page_prob = 15

# DO SOME STUFF:
def doStuff():
    # get local robot info
    ldb = DatabaseSQLite(TESTDB)
    ldb.connect()
    robotList = ldb.getAllRobots()
    userList = []
    ldb.close()

    db = DatabaseMySQL(host = HOST, user = USER, passwd = PASSWD, db = DB)
    db.connect()
    db.insertAllRobots(robotList)
    while True:
        try:
            doRobotStuff(db, robotList)
            doUserStuff(db, userList)
        except Exception as inst:
            print '-----------------------------------'
            print inst
            print '-----------------------------------'
        time.sleep(5)

if __name__ == '__main__':
    doStuff()
