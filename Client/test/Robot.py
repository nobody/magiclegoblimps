import random
import copy

class Robot:
    def __init__(self, robotid=0, feed='', coords=(0, -1), orient=None, animals=None, automove=None):
        self.robotid = robotid
        self.feed = feed
        self.x = coords[0]
        self.y = coords[1]
        self.orient = orient
        self.animals = animals
        self.automove = automove

    def inGrid(self):
        if self.x < 0 or self.y < 0 or self.x > 5 or self.y > 4:
            return False
        return True
    
    def checkRobotCollision(self, robotList):
        for r in robotList: # robot collision check
            if self.x == r.x and self.y == r.y:
                return False
        return True
    def checkWallCollision(self):
        if not self.inGrid(): # wall collision check
            return False
        return True
    def goForward(self):
        if self.orient == 0:
            self.y += 1
        if self.orient == 2:
            self.y -= 1
        if self.orient == 1:
            self.x += 1
        if self.orient == 3:
            self.x -= 1

    def moveForward(self, robotList):
        t = copy.deepcopy(self)
        t.goForward()
        if t.checkWallCollision() and t.checkRobotCollision(robotList):
            self.x = t.x
            self.y = t.y
        else:
            pass

    def turnLeft(self):
        t = copy.deepcopy(self)
        if t.orient == 0:
            t.orient = 3
        else:
            t.orient -= 1
        t.goForward()
        if t.checkWallCollision():
            self.orient = t.orient

    def turnRight(self):
        t = copy.deepcopy(self)
        if(t.orient == 3):
            t.orient = 0
        else:
            t.orient += 1
        t.goForward()
        if t.checkWallCollision(): # don't turn toward a wall.
            self.orient = t.orient

    def action(self, action, robotlist): # foward: 1 left: 2, right: 3
        if action == 1:
            print 'MANUAL COMMAND: moving forward'
            self.moveForward(robotlist)
            self.automove = False
        elif action == 2:
            print 'MANUAL COMMAND: turning left'
            self.turnLeft()
            self.automove = False
        elif action == 3:
            print 'MANUAL COMMAND: turning right'
            self.turnRight()
            self.automove = False
        elif action == 4:
            print 'MANUAL COMMAND: panning'
            self.automove = False
        elif action == 5:
            pass # Not implemented
        elif action == 6:
            print 'MANUAL COMMAND: release control'
            self.automove = True

    def addAnimal(self, animal):
        if animal not in self.animals:
            self.animals.append(animal)
    def looseAnimal(self):
        if len(self.animals) is not 0:
            rand = random.randint(0, len(self.animals)-1)
            del self.animals[rand]

    def __eq__(self, other):
        return self.robotid == other.robotid

    def __str__(self):
        direction = {0 : '^', 1 : '>', 2 : 'v', 3 : '<' }[self.orient]
        return 'Robot ' + str(self.robotid) + ': (' + str(self.x) + ', ' + str(self.y) + ') ' + direction + ' animals: ' + str(self.animals)
