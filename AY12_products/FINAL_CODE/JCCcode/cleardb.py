import MySQLdb

def db_connect():
   try:
      return MySQLdb.connect ('127.0.0.1','root','abc','commandcenter_db')
   except MySQLdb.Error, e:
      print "Error %d: %s" % (e.args[0], e.args[1])
      sys.exit (1)
      
conn = db_connect()
cursor = conn.cursor()
cursor.execute("DELETE FROM robotstate")
cursor.execute("DELETE FROM targets")
cursor.execute("DELETE FROM robots")
cursor.execute("UPDATE missioncomplete SET missionComplete=0")
cursor.close()
conn.commit()
conn.close()