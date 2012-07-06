#!/usr/bin/python

import Tkinter
import SCCclient2
import SCCserver2
import tkSimpleDialog

#TODO
#Make UI more specific, shift from the handUserInput of the SCC client

class SCCapp(Tkinter.Tk):
	#sets global dictionary variable
	VariableDict = {}
	#initilize VariableDict with filler until other script is complete.
	VariableDict['Time_Stamp'] = 0
	VariableDict['RobotID'] = 'AR11'
	VariableDict['PacketType'] = 2
	VariableDict['N_S'] = 'N'
	VariableDict['Long'] = 4
	VariableDict['E_W'] = 'E'
	VariableDict['Lat'] = 6
	VariableDict['Elev'] = 7
	VariableDict['Bearing'] = 8
	VariableDict['Speed'] = 9
	VariableDict['Life'] = 10
	VariableDict['Mission'] = 11
	VariableDict['Status'] = 12
	
	def updateCMD(self, inputMessage):
		message = inputMessage
		messList = message.split(',')
		if len(messList)>=3:
			self.Display(messList)
	
	def Display(self, List):
		if List[2] == '0' or List[2] == '00':
			#Transition
			self.VariableDict['Time_Stamp'] = List[0]
			self.VariableDict['RobotID'] = List[1]
			self.VariableDict['PacketType'] = List[2]
			self.VariableDict['N_S'] = List[3]
			self.VariableDict['Long'] = List[4]
			self.VariableDict['E_W'] = List[5]
			self.VariableDict['Lat'] = List[6]
			self.VariableDict['Elev'] = List[7]
			self.VariableDict['Bearing'] = List[8]
			self.VariableDict['Speed'] = List[9]
			self.VariableDict['Life'] = List[10]
			self.VariableDict['Mission'] = List[11]
			self.VariableDict['Status'] = List[12]
			self.update()

	def __init__(self, parent):
		Tkinter.Tk.__init__(self, parent)
		self.updateScreen=True
		self.parent=parent
		self.usPort=50000 #the port for the update server
		self.jccPort=5111 #the port the JCC is listening on
		self.jccIP='192.1.1.2' #the address of the JCC
		self.cmdClient=SCCclient2.SCCclient()
		self.initialize() #create the GUI elements				
		
	#Create the GUI elements of the app
	def initialize(self):
		self.grid() #create a grid layout manager
		
		#-----------The message passer buttons
		button=Tkinter.Button(self, text=u"Start (restart) State Messages", command=self.startPassing)
		button.grid(column=0, row=7, sticky='EW')
		
		#-----------The command message buttons
		self.promptText=Tkinter.StringVar()
		promptLabel=Tkinter.Label(self, textvariable=self.promptText, anchor="w", fg="white", bg="blue")
		promptLabel.grid(column=0, row=1, columnspan=2, sticky='EW')
		self.promptText.set(u"Command Messages")
		 
		button=Tkinter.Button(self, text=u"Connect to Robot 22306", command=self.robot22306)
		button.grid(column=0, row=2, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Connect to Robot 22261", command=self.robot22261)
		button.grid(column=0, row=3, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Connect to Robot 22240", command=self.robot22240)
		button.grid(column=0, row=4, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Other Connection", command=self.funN)
		button.grid(column=0, row=5, sticky='EW')
		
		button=Tkinter.Button(self, text=u"End Command Connection", command=self.endConnection)
		button.grid(column=0, row=6, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Stop Msg", command=self.fun0)
		button.grid(column=1, row=2, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Waypoint Msg", command=self.fun1)
		button.grid(column=1, row=3, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Patrol Msg", command=self.fun2)
		button.grid(column=1, row=4, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Follow Msg", command=self.fun3)
		button.grid(column=1, row=5, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Pause Msg", command=self.fun4)
		button.grid(column=1, row=6, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Resume Msg", command=self.fun5)
		button.grid(column=1, row=7, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Return to base Msg", command=self.fun6)
		button.grid(column=1, row=8, sticky='EW')

		button=Tkinter.Button(self, text=u"Start Manual Control", command=self.manualControl)
		button.grid(column=1, row=9, sticky='EW')
		# -----------End Command Msg Buttons

		#Space Holder
		self.spacer = Tkinter.StringVar()
		label = Tkinter.Label(self, textvariable=self.spacer, anchor="w", fg="white")
		label.grid(column=3, row=1, columnspan=1,sticky='EW')
		
		#-----------Robot State Labels and Displays

		self.stateLabel=Tkinter.StringVar()
		label = Tkinter.Label(self, textvariable=self.stateLabel, anchor="w", fg="white", bg="blue")
		label.grid(column=4, row=1, columnspan=2,sticky='EW')
		self.stateLabel.set(u"Current Robot State")
		
		#Time stamp label display
		self.timeStamp = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Time Stamp:", anchor="w",fg="black",bg="gray")
		label.grid(column=4,row=2,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.timeStamp, anchor="w",fg="black",bg="gray")
		label.grid(column=5,row=2,sticky='EW')
		self.timeStamp.set('----')

		#robot ID label display
		self.robotID = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Robot ID:", anchor="w",fg="black",bg="white")
		label.grid(column=4,row=3,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.robotID, anchor="w",fg="black",bg="white")
		label.grid(column=5,row=3,sticky='EW')
		self.robotID.set('----')#static text placeholder until streamed data variable is used

		#Packet type label display
		self.packetType = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Packet Type:", anchor="w",fg="black",bg="gray")
		label.grid(column=4,row=4,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.packetType, anchor="w",fg="black",bg="gray")
		label.grid(column=5,row=4,sticky='EW')
		self.packetType.set('----')

		#Latitude type label display
		self.latitude = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Latitude:", anchor="w",fg="black",bg="white")
		label.grid(column=4,row=5,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.latitude, anchor="w",fg="black",bg="white")
		label.grid(column=5,row=5,sticky='EW')
		self.latitude.set('----')
		
		#Longitude label display
		self.longitude = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Longitude:", anchor="w",fg="black",bg="gray")
		label.grid(column=4,row=6,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.longitude, anchor="w",fg="black",bg="gray")
		label.grid(column=5,row=6,sticky='EW')
		self.longitude.set('----')

		#Elevation type label display
		self.elevation = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Elevation:", anchor="w",fg="black",bg="white")
		label.grid(column=4,row=7,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.elevation, anchor="w",fg="black",bg="white")
		label.grid(column=5,row=7,sticky='EW')
		self.elevation.set('----')

		#Speed type label display
		self.speed = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Speed:", anchor="w",fg="black",bg="gray")
		label.grid(column=4,row=8,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.speed, anchor="w",fg="black",bg="gray")
		label.grid(column=5,row=8,sticky='EW')
		self.speed.set('----')

		#Operating Life type label display
		self.opLife = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Operating Life:", anchor="w",fg="black",bg="white")
		label.grid(column=4,row=9,sticky='EW')
 
		label = Tkinter.Label(self,textvariable=self.opLife, anchor="w",fg="black",bg="white")
		label.grid(column=5,row=9,sticky='EW')
		self.opLife.set('----')

		#Robot Bearing type label display
		self.rBearing = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Robot Bearing:", anchor="w",fg="black",bg="gray")
		label.grid(column=4,row=10,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.rBearing, anchor="w",fg="black",bg="gray")
		label.grid(column=5,row=10,sticky='EW')
		self.rBearing.set('----')

		#Current Mission type label display
		self.curMission = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Current Mission:", anchor="w",fg="black",bg="white")
		label.grid(column=4,row=11,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.curMission, anchor="w",fg="black",bg="white")
		label.grid(column=5,row=11,sticky='EW')
		self.curMission.set('----')

		#Robots current status label display
		self.status = Tkinter.StringVar()
		label = Tkinter.Label(self,text="Status:", anchor="w",fg="black",bg="gray")
		label.grid(column=4,row=12,sticky='EW')

		label = Tkinter.Label(self,textvariable=self.status, anchor="w",fg="black",bg="gray")
		label.grid(column=5,row=12,sticky='EW')
		self.status.set('----')
		
		button=Tkinter.Button(self, text=u"Resume", command=self.resume)
		button.grid(column=4, row=13, sticky='EW')
		
		button=Tkinter.Button(self, text=u"Pause", command=self.pause)
		button.grid(column=5, row=13, sticky='EW')
		
		# -----------End Robot State Labels and Displays
		
		#self.grid_columnconfigure(0, weight=1) #attached weight to column (when expanded horizontally)
		self.resizable(False, False) #lock the size of the window
			
	def funN(self): self.onCMD('N')
	def funD(self): self.onCMD('D')
	def fun0(self): self.onCMD('0')
	def fun1(self): self.onCMD('1')
	def fun2(self): self.onCMD('2')
	def fun3(self): self.onCMD('3')
	def fun4(self): self.onCMD('4')
	def fun5(self): self.onCMD('5')
	def fun6(self): self.onCMD('6')
	
	class MyDialog(tkSimpleDialog.Dialog):
	
		def __init__(self, parent, text):
			self.prompt=text
			tkSimpleDialog.Dialog.__init__(self, parent)
			print text
		
		def body(self, master):
			self.input=""
			tkSimpleDialog.Label(master, text=self.prompt).grid(row=0)
			self.e1 = tkSimpleDialog.Entry(master)
			self.e1.grid(row=1, column=0)
			return self.e1 # initial focus
				
		def apply(self):
			self.input = (self.e1.get())	
	
	def update(self):
		if self.updateScreen:
			self.status.set(self.VariableDict['Status'])
			self.curMission.set(self.VariableDict['Mission'])
			self.rBearing.set(self.VariableDict['Bearing'])
			self.opLife.set(self.VariableDict['Life'])
			self.speed.set(self.VariableDict['Speed'])
			self.latitude.set(str(self.VariableDict['Lat'])+" "+self.VariableDict['E_W'])
			self.longitude.set(str(self.VariableDict['Long'])+" "+self.VariableDict['N_S'])
			self.packetType.set(self.VariableDict['PacketType'])
			self.robotID.set(self.VariableDict['RobotID'])
			self.timeStamp.set(self.VariableDict['Time_Stamp'])
			self.elevation.set(self.VariableDict['Elev'])

	def manualControl(self):
		msg=self.cmdClient.manualControlMsg(self.VariableDict['RobotID'])
		self.cmdClient.passMsg(msg)
		print 'Entering Manual Mode'		
		while [1]:
			uIn=raw_input()
			self.cmdClient.passMsg(uIn)
			if input=='exit':
				print 'Exiting Manual Mode'
				break

	def pause(self):
		self.updateScreen=False
	
	def resume(self):
		self.updateScreen=True
	
	def onCMD(self, option):
		self.cmdClient.handUserInput(option, self.userPrompt)
	
	def robot22306(self):
		self.cmdClient.HOST='172.17.87.34'
		self.cmdClient.connectToSCC()
	
	def robot22261(self):
		self.cmdClient.HOST='172.17.86.245'
		self.cmdClient.connectToSCC()
		
	def robot22240(self):
		self.cmdClient.HOST='172.17.86.224'
		self.cmdClient.connectToSCC()
			
	def userPrompt(self, text):
		newPrompt=self.MyDialog(self, text)
		return newPrompt.input
		
	def endConnection(self):
		self.cmdClient.passMsg(self.cmdClient.headerMsg(self.VariableDict['RobotID'])+',09')		
		self.funD()	

	def startPassing(self):	
		passer=SCCserver2.SCCserver(self,  self.jccIP, self.jccPort, self.usPort)
		passer.start()
		#Here we tell the robot what it's name is and what port to communicate on
		msg=self.cmdClient.beginSendingMsg(self.VariableDict['RobotID'], self.usPort)
		self.cmdClient.passMsg(msg)
		#don't need the line below.
		#it is useful because if the server isn't stopped gracefully, then the port is still bound
		#to resolve this, we use a different port next time
		self.usPort=self.usPort+1
		
if __name__=="__main__": #runs when executed from the command line
        gui=SCCapp(None)
        gui.title("SCC GUI")
        gui.mainloop()
