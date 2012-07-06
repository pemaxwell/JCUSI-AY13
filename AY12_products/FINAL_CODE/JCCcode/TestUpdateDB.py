import updateDB


def Assignment(List):
    VariableDict = {}      
    if List[2] == '00':
        #Transition
        print 'transition'
        VariableDict['Time_Stamp'] = List[0]
        VariableDict['RobotID'] = List[1]
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


        updateDB.state_update(VariableDict)
        
def parse(data):
    #print data
    #print 'data.split:'
    #workingList = data.split()
    workingList = data
    format_string = workingList.split(',')
    

    Assignment(format_string)




String = "0,11,00,2,3,4,5,6,7,8,9,10,11,12"
parse(String)



