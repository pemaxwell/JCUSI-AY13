#echo server program
import socket
import csv
import updateDB
import thread

def parse(c):

    global v,nclnt,vlock,numSCClock 

    while 1:
        # recieve string from client c
        data = c.recv(1024)
        #conn.send(data)
        if data == 'q': break
        else:
            workingList = data
            format_string = workingList.split(',')
            Assignment(format_string)


    
    #print data
    #print 'data.split:'
    #workingList = data.split()
    
def Assignment(List):
    VariableDict = {}      
    if List[2] == '0' or List[2] == '00':
        #Transition
        print 'transition'
        VariableDict['Time_Stamp'] = List[0]
        robid = List[1]
        VariableDict['RobotID'] = robid[2:]
        VariableDict['PacketType'] = List[2]
        VariableDict['N_S'] = List[3]
        VariableDict['Long'] = List[4]
        VariableDict['E_W'] = List[5]
        VariableDict['Lat'] = List[6]
        VariableDict['Elev'] = List[7]
        VariableDict['Bearing'] = List[8]
        VariableDict['Speed'] = List[9]
        VariableDict['Life'] = List[10]
        VariableDict['Mission'] = List[11]
        VariableDict['Status'] = List[12]
        #write everything in dictionary to screen
        for key,value in VariableDict.items():
            print key,value
        
        #put transition into db
        #
        updateDB.state_update(VariableDict) 
    elif List[2] == '01':
        #Executing
        print 'executing'
        VariableDict['Time_Stamp'] = List[0]
        VariableDict['RobotID'] = List[1]
        VariableDict['PacketType'] = List[2]
        VariableDict['Target_N_S'] = List[3]
        VariableDict['Target_Long'] = List[4]
        VariableDict['Target_E_W'] = List[5]
        VariableDict['Target_Lat'] = List[6]
        VariableDict['Target_Elev'] = List[7]
        VariableDict['Target_Bearing'] = List[8]
        VariableDict['TargetSpeed'] = List[9]
        VariableDict['Target_Status'] = List[10]
        #Target ID needed 
        VariableDict['TargetID'] = List[11]
        for key,value in VariableDict.items():
            print key,value
    elif List[2] == '02':
        #Mission Complete
        print 'Mission_Complete'
        
    elif List[2] == '04':
        #Standing By
        print 'StandBy'
    elif List[2] == '05':
        #Manual Control
        print 'ManCont'
    else:
        #invalid input
        print 'invalid input'
    
    


HOST = ''
PORT = 4208
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
print 'listening'
s.listen(5)
print 'still listening'

# initialize working string, v
v = ''
# set up a lock to guard v
vlock = thread.allocate_lock()

# initialize for the number of service centers
numSCC = 3
# set up a lock to guard numSCC
numSCClock = thread.allocate_lock()


#accept calls from the clients
#can be used as 'while true' so that i can use accept as many as possible
for i in range(numSCC):
    #wait for call, then get a new socket to use for this client,
    #and get the client's address/port 
    (conn,addr) = s.accept()
    #start thread for this client, with serveclient() as the thread's
    #function, with parameter clnt; note that parameter set must be a
    #tuple; in this case, the tuple is of length 1, so a comma is needed
    thread.start_new_thread(parse, (conn,))
    print 'connected by', addr
    
    

s.serve_forever()




