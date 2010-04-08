#!/usr/bin/python

import os
import time
import socket
import config
import ffserver
import settings

class Archive():

	ARCHIVE_DIR = './archives'

	def __init__(self):
		self.filenames = []
		self.objects = []
		self.qos = []
		self.objects_qos = []

	def get_Archive(self):
		self.filenames = os.listdir(ARCHIVE_DIR)
		for x in self.filenames:
			self.qos.append(self.filenames[:3])
		for x in self.filenames:
			self.objects.append(self.filenames[4:6])
		for x in range(len(self.filenames)):
			self.objects_qos[self.objects[x]] = self.qos[x]

if __name == '__main__':
	os.chdir(settings.ROOT_DIR)
	try:
		ar = Archive()
		ar.get_Archive()
		print self.objects_qos
