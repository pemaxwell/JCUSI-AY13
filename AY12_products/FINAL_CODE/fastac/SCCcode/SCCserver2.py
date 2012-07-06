#!/usr/local/bin/python

import socket
import csv
import time
import sys
import threading

#in this program the robot is a client and the JCC is a server.
#this server acts as a server for the robot and forwards the messages to the JCC
class SCCserver(threading.Thread):##
	
	def __init__(self, gui=None, JCC_Host = '192.1.1.2', JCC_Port = 5111, Robot_Port= 50001):
		self.gui=gui
		self.serverLog=open('serverMessageLog.txt', 'w') #log file
		self.jccPresent=False #intially there is no JCC connection
		self.jHost=JCC_Host
		self.jPort=JCC_Port
		self.rHost=''
		self.rPort=Robot_Port
		self.communicate= True
		threading.Thread.__init__ ( self )##
	
	def start(self):
		self.connectToJCC()
		threading.Thread.start(self)
		
	#SERVER
	def startRobotServer(self):
		#create Internet TCP socket with the Robot
		s_Robot = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		#Covnnect to client (robot)
		s_Robot.bind((self.rHost, self.rPort))
		print 'listening on port ', str(self.rPort)
		s_Robot.listen(1)	
		robot_connection, addr = s_Robot.accept()
		print 'connected by', addr
		self.robot=robot_connection
			
	def run(self):
		self.startRobotServer()
		while self.communicate:
			data = self.robot.recv(1024)
			if self.jccPresent:
				self.jcc.send(data) #send recieved data to JCC
				self.serverLog.write("To: "+str(self.jHost)+", "+str(self.jPort)+" Recieved on port: "+str(self.rPort)+" Message: "+data+"\n") #send the message to a log file
			else:
				self.serverLog.write("To: No JCC, Recieved on port: "+str(self.rPort)+" Message: "+data+"\n")
			if self.gui == None:
				pass
			else:
				self.gui.updateCMD(data)
				
	def connectToJCC(self):
		try:
			# Create a socket (SOCK_STREAM means a TCP socket)
			self.jcc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.jcc.connect((self.jHost, self.jPort))
			self.jccPresent=True
			print 'Waiting to send data'
		except:
			print 'cannot connect to JCC'

	
	#close the connection with the JCC
	def disconnectFromJCC(self): 
		try:
			self.jcc.close()
			self.jccPresen=False
			print 'Socket Closed'
		except:
			print "Unable to close previous connection or no previous connection."	
		
#----Main---#
if __name__=="__main__": #runs when executed from the command line	
	
	JCC_Port='192.1.1.2'
	JCC_Host=5111
	Robot_Port=50001
	#use the arguments	
	def handleArgs():
		if (len(sys.argv) > 1):
			try:
				JCC_Port=int(sys.argv[1])
			except:
				pass
		if (len(sys.argv) > 2):
			JCC_Host=sys.argv[2]
		if (len(sys.argv) > 3):
			try:
				Robot_Port=int(sys.argv[3])
			except:
				pass

					
	#handleArgs()
	messagePasser=SCCserver(None, JCC_Port, JCC_Host, Robot_Port)
	messagePasser.start()
