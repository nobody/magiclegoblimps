#!/usr/bin/python2.6
from __future__ import print_function, with_statement
import os
import time
import socket
import config
import ffserver
import settings
import MySQLdb
from vidcontrol import change_working_directory
from logger import log

def connect():
    """
    Connect to the database
    """
    try:
        conn = MySQLdb.connect (host = settings.MySQL_HOST,
                                user = settings.MySQL_USER,
                                passwd = settings.MySQL_PASS,
                                db = settings.MySQL_DATABASE)
        log('Connecting to MySQL Host: ' + settings.MySQL_HOST +
            ' and Database: ' + settings.MySQL_DATABASE)
    except MySQLdb.Error as e:
        log("Error %d: %s" % (e.args[0], e.args[1]))
        raise e
    return conn

def close(conn):
    """
    Close the connection to the database
    """
    try:
        conn.close()
        log('Closed connection to Database')
    except MySQLdb.Error as e:
        log("Error %d: %s" % (e.args[0], e.args[1]))

def addCameraFeed(conn,cameraId,cameraFeed):
    """
    Add Live Camera Feed to the MySQL Database
    """
    try:
        cursor = conn.cursor()
        sql = 'CALL addCameraFeed(' + str(cameraId) + ',"' + cameraFeed + '")'
        cursor.execute(sql)
        conn.commit()
        cursor.close()
        log('Added camera feed to db: ' + cameraFeed)
    except MySQLdb.Error as e:
        log("Error %d: %s" % (e.args[0], e.args[1]))

def removeCameraFeed(conn, camId, camFeed):
    """
    Remove a live camera feed from the MySQL database.
    """
    try:
        c = conn.cursor()
        sql = 'update cameras set camera_feed = NULL where camera_id = ' \
              + str(camId)
        c.execute(sql)
        conn.commit()
        c.close()
        log('Removed camera feed from db: ' + camFeed)
    except MySQLdb.Error as e:
        log("Error %d: %s" % (e.args[0], e.args[1]))

def addArchiveFootage(conn,archiveUrl,objectId,qos,thumbUrl):
    """
    Add Archive Footage to the MySQL Database
    """
    try:
        cursor = conn.cursor()
        sql = 'CALL addArchiveFootage("'+archiveUrl+'",'+objectId+','+qos+',"'+thumbUrl+'")'
        cursor.execute(sql)
        conn.commit()
        cursor.close()
        log('Adding Archive Footage to Database')
    except MySQLdb.Error as e:
        log("Error %d: %s" % (e.args[0], e.args[1]))

def displayTable(conn,tableName):
    """
    Display a table
    """
    try:
        cursor = conn.cursor()
        sql = 'SELECT * FROM '+tableName
        cursor.execute(sql)
        results = cursor.fetchall()
        print(str(results))
        cursor.close()
    except MySQLdb.Error as e:
        log("Error %d: %s" % (e.args[0], e.args[1]))

if __name__ == '__main__':
    c = connect()
    addCameraFeed(c,'1','http://something-cooler-than-test.html')
    displayTable(c,'cameras')
    removeCameraFeed(c, 1, 'http://something-cooler-than-test.html')
    close(c)
    c1 = connect()
    displayTable(c1,'cameras')
    close(c1)
