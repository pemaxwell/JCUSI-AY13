

#python register_target.py <new_targetID> <taskedRobotSerial>
#changed for updating database by lough feb 9 2012
#accept dictionary of useful data - put in database

import MySQLdb

def db_connect():

   try:
      return MySQLdb.connect ('127.0.0.1','root','abc','commandcenter_db')
   except MySQLdb.Error, e:
      print "ugh"


def state_update(VariableDict):
   
   
   ###############################################
   #if the robot is not in database, register it - ask MAJ Larkin help
   #cursor.execute ("INSERT INTO robots (serialNum,robotType,currentMission,ipaddr) VALUES ("+rob_id+",(SELECT typeID FROM robottypelookup WHERE type = '"+robotType+"'),1,1)")
   ###############################
   
   time = VariableDict['Time_Stamp'] 
   rob_id = VariableDict['RobotID'] 
   n_s = VariableDict['N_S']
   longitude = VariableDict['Long']
   #longitude = -73.952409
   e_w = VariableDict['E_W'] 
   lat = VariableDict['Lat']
   #lat = 41.391487


   
   elev = VariableDict['Elev'] 
   bearing = VariableDict['Bearing'] 
   speed = VariableDict['Speed'] 
   life = VariableDict['Life']
   mission = VariableDict['Mission']
   status = VariableDict['Status']


   
   conn = db_connect()
   cursor = conn.cursor()
   robotType = 'LandBot' # can be LandBot, AirBot, or SeaBot
   
   cursor.execute ("INSERT INTO robotstate (serialNum,latitude,longitude,elevation,speed,bearing) VALUES ("+rob_id+","+str(lat)+","+str(longitude)+","+elev+","+speed+","+bearing+")")
   #cursor.execute("INSERT INTO robotstate (serialNum,timeStamp,latitude,longitude,elevation,bearing,speed) VALUES ("+str(rob_id)+",CONCAT(CURDATE(),' ',CURTIME()),"+str(lat)+","+str(longitude)+","+str(elev)+","+str(bearing)+","+str(speed))
   #cursor.execute("INSERT INTO robotstate (targetID,taskedRobot,timeIdentified) VALUES ("+targetID+","+serial+",CONCAT(CURDATE(),' ',CURTIME()))")
   cursor.close()
   conn.commit()
   conn.close()


   #cursor.execute ("INSERT INTO robots (serialNum,robotType,currentMission) VALUES ("+str(serialNumber)+",(SELECT typeID FROM robottypelookup WHERE type = '"+robotType+"'),1)")
   #cursor.close ()
   #connection.commit ()



def target_update(VariableDict):


   targetID = VariableDict['TargetID']


   TimeStamp = VariableDict['Time_Stamp']
   rob_ID = VariableDict['RobotID']
   N_S = VariableDict['Target_N_S']
   y = VariableDict['Target_Long']
   E_W = VariableDict['Target_E_W'] 
   x = VariableDict['Target_Lat'] 
   targ_elev = VariableDict['Target_Elev']
   targ_bearing = VariableDict['Target_Bearing']
   targ_speed = VariableDict['TargetSpeed'] 
   status = VariableDict['Target_Status']
   
      
   conn = db_connect()  
   cursor = conn.cursor()
   cursor.execute("INSERT INTO targetlocations (targetID,timeUpdated,latitude,longitude) VALUES ("+targetID+",CONCAT(CURDATE(),' ',CURTIME()),"+x+","+y+")")
   cursor.close()
   conn.commit()
   conn.close()
