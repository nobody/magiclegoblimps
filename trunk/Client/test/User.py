import random

class User:
    def __init__(self, userid = 0, page=0):
        self.userid = userid
        self.page = page
        self.time_on_page = 0
        self.time_since_vote = 0
        self.needs_vote = False

    def increment_time(self, time):
        self.time_on_page += time
        self.time_since_vote += time
        if self.time_since_vote >= 30:
            self.vote()

    def change_page(self, page):
        self.page = page
        self.time_on_page = 0
        self.vote()

    def vote(self):
        self.time_since_vote = 0
        self.needs_vote = True
        # vote for self.page

    def __eq__(self, other):
        return self.userid == other.userid

    def __str__(self):
        return 'User ' + str(self.userid) + ', page: ' + str(self.userid)
