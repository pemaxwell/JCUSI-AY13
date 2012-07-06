#!/usr/local/bin/python
import socket
import sys
import time

class SCCclient:
	def __init__(self, HOST='172.17.86.245', PORT=52000):
		self.HOST, self.PORT=HOST, PORT #must be between 50,000 and 60,000 in order to work
		self.log=open('sentMessageLog.txt', 'w') #log file
		self.rob='AR10' #currently sets the only robot to AR10
		self.connection=False
		#TODO: add functionality for multiple robots/multiple connections
		
	def connectToSCC(self):
		# Create a socket (SOCK_STREAM means a TCP socket)
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((self.HOST, self.PORT))
		self.connection=True
		print 'Waiting to send data'
	
	def disconnectFromSCC(self):
		#Closes the socket, breaking the connection with the server.
		try:
			self.sock.close()
			self.connection=False
			print 'Socket Closed'
		except:
			print "Unable to close previous connection or no previous connection."

	
	#------------------------------------- Messages -----------------------------------------------#
	#create the header for the message
	def headerMsg(self, robot):
		return str(time.time())+','+robot+','+'99'+','
		
	#create a stop message	
	def stopMsg(self, robot):
		return self.headerMsg(robot)+'00'

	#create a way point message
	def wayPointMsg(self, robot, points):
		msg=self.headerMsg(robot)+'01'
		for point in points:
			msg=msg+','+format(point[0])+','+format(point[1])
		return msg

	#create a patrol message (lat/long pair is a waypoint on the patrol route)
	def patrolMsg(self, robot, points):
		msg=self.headerMsg(robot)+'02'
		for point in points:
			msg=msg+','+format(point[0])+','+format(point[1])
		return msg

	#create a message to indicate that the robot should follow a target
	#If a point is given, the robot will go to the point prior to identification of the target to follow
	def followMsg(self, robot, points):
		msg=self.headerMsg(robot)+'03'
		for point in points:
			msg=msg+','+format(point[0])+','+format(point[1])
		return msg
		
	#create a message to pause the robots current execution for a specificied amount of time
	#a negative number indicates indefinately
	def pauseMsg(self, robot, time):
		return self.headerMsg(robot)+'04,'+str(time)
		
	def resumeMsg(self, robot):
		return self.headerMsg(robot)+'05'

	#create a message telling the robot to return to base
	# if a point is given, it is the location of the base
	def returnMsg(self, robot, point):
		msg=self.headerMsg(robot)+'06'
		if not point==[]:
			msg= msg+','+format(point(1))+','+format(point(2))
		return msg
		
	#create a message telling the robot to begin sending up messages
	def beginSendingMsg(self, robot, port):
		msg=self.headerMsg(robot)+'07'
		return msg+','+str(port)

	def manualControlMsg(self, robot):
		msg=self.headerMsg(robot)+'08'
		return msg
	
	def passMsg(self, m):
		self.sock.send(m)
		self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")	

	#---------------------------- Simple  User Interface --------------------------#
	def getPoint(self, inputFunction=raw_input):
		invalidInput=True
		while invalidInput:
			try:
				lat=float(inputFunction('Enter the Latitude: '))
				if lat>90 or lat<-90:
					invalidInput=True
					print 'Invalid input, please try again.'
				else:
					invalidInput=False
			except:
				invalidInput=True
				print 'Invalid input, please try again.'
		invalidInput=True
		while invalidInput:
			try:
				longitude=float(inputFunction('Enter the Longitude: '))
				if longitude<=180 and longitude>=-180:
					invalidInput=False
				else:
					invalidInput=True
					print 'Invalid input, please try again.'
			except:
				invalidInput=True
				print 'Invalid input, please try again.'

		return [lat, longitude]

	def displayMenu(self):
		print '#------- Command Menu ------#'
		print 'D: end connection'
		print 'N: new connection'
		print 'E: exit session'
		print '0: Stop Msg'
		print '1: Waypoint Msg'
		print '2: Patrol Message'
		print '3: Follow Message'
		print '4: Pause Message'
		print '5: Resume Message'
		print '6: Return to base Message'
		print '7: Start updating Message'
		
	def getHost(self, inputFunction=raw_input):
		temp=inputFunction('Enter the host to which you wish to connect: ')
		return temp

	def getPort(self, inputFunction=raw_input):
		noPort=True;
		while noPort:
			try:
				port=int(inputFunction('Enter the port on which you wish to connect: '))
				if port>=50000 and port<=60000:
					noPort=False
				else:
					print 'Invalid Port'
			except:
				print 'Invalid Port'
		return port
	
	def handUserInput(self, uInput, inputFunction=raw_input):
		if uInput=='M' or uInput=='m':
				self.displayMenu()
		elif uInput=='D' or uInput=='d':
			self.disconnectFromSCC()
		elif uInput=='N' or uInput=='n':
			try:
				self.disconnectFromSCC()
			finally:
				choice=inputFunction('Would you like to specify the host and port?(Y/N): ')
				if (choice=='Y' or choice=='y'):
					self.HOST=self.getHost(inputFunction)
					self.PORT=self.getPort(inputFunction)
				self.connectToSCC()
		elif uInput=='E' or uInput=='e':
			try:
				self.disconnectFromSCC()
			finally:
				self.log.close()
		elif uInput>='0' and uInput<='6' and self.connection==False:
			print "no connection established"
		elif uInput=='0':
			m=self.stopMsg(self.rob)
			self.sock.send(m)
			self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")
		elif uInput=='1':
			morePoints=True
			p=[]
			while morePoints:
				p.append(self.getPoint(inputFunction))
				choice=(inputFunction('More Points?(Y/N): '))
				morePoints= (choice=='Y' or choice=='y')
			m=self.wayPointMsg(self.rob, p)
			self.sock.send(m)
			self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")
		elif uInput=='2':
			morePoints=True
			p=[]
			while morePoints:
				p.append(self.getPoint(inputFunction))
				choice=(inputFunction('More Points?(Y/N): '))
				morePoints= (choice=='Y' or choice=='y')
			m=self.patrolMsg(self.rob, p)
			self.sock.send(m)
			self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")
		elif uInput=='3':
			p=[]
			if (inputFunction('Specify a Start Point?(Y/N): ')=='Y'):
				p.append(self.getPoint(inputFunction))
			m=self.followMsg(self.rob, p)
			self.sock.send(m)
			self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")
		elif uInput=='4':
			while [1]:
				try:
					t=float(inputFunction('Enter a pause time (<0 indicates indefinite): '))
					break
				except:
					print 'Invalid time'
					
			m=self.pauseMsg(self.rob, t)
			self.sock.send(m)
			self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")
		elif uInput=='5':
			m=self.resumeMsg(self.rob)
			self.sock.send(m)
			self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")	
		elif uInput=='6':
			p=[]
			if (inputFunction('Specify a Return Point?(Y/N): ')=='Y'):
				p=(self.getPoint)
			m=self.returnMsg(self.rob, p)
			self.sock.send(m)
			self.log.write("Time Place Holder"+", "+str(self.HOST)+", "+str(self.PORT)+": "+m+"\n")
		else:
			print 'Invalid Input'
			self.displayMenu()
	
	def getUserInput(self, inputFunction=raw_input):
		self.displayMenu()
		while [1]:
			choice=inputFunction('Enter a Command (type M to see the command menu): ')
			self.handUserInput(choice, raw_input)
			if choice=='E' or choice=='e':
				break
			
			
			
#----Main---#
if __name__=="__main__": #runs when executed from the command line	
	print 'test'
	#intialize defaults
	aHOST, aPORT = "localhost", 50001 #must be between 50,000 and 60,000 in order to work
		
	#use the arguments	
	def handleArgs():
		if (len(sys.argv) > 1):
			try:
				aPORT=int(sys.argv[1])
			except:
				pass
		if (len(sys.argv) > 2):
			aHOST=sys.argv[2]

					
	handleArgs()
	messagePasser=SCCclient(aHOST, aPORT)
	messagePasser.connectToSCC()
	messagePasser.getUserInput()
