# This python script sets up ip packet forwarding on the packbot using ip tables.
# After changing the IPs below for the payload, packbot and ocu run this script on 
# the packbot by using the command: python ipforward.py.
# Once installed when the ocu sends tcp packets to the packbot and the port is between
# 50000 and 60000 the ip table will forward the tcp packets to the payload.
# So from the point of the view of the ocu, the process on the payload
# would seem like it was actually running on the packbot.
import socket
import os

payloadIp =  "192.168.1.101"
packbotWiredIp = "172.16.83.23"
packbotWirelessIp = "172.17.83.23"
ocuWirelessIp = "172.17.167.172"
port = "00000:60000"  #Range of port numbers to forward ip packets on --original starting range = 50000

def runSys(cmd,log=True):
    if log:
        print cmd
    os.system(cmd)

# enable ip fowarding     
cmd = "sysctl -w net.ipv4.ip_forward=1"
runSys(cmd)
    
# reset rules
cmd = "iptables -t nat -P PREROUTING ACCEPT"
runSys(cmd)
cmd = "iptables -t nat -P POSTROUTING ACCEPT"
runSys(cmd)

## ocu -> payload
cmd = "iptables -t nat -A PREROUTING -s %s -d %s -p tcp --destination-port %s -j DNAT --to-destination %s" % \
    (ocuWirelessIp,packbotWirelessIp,port,payloadIp)
runSys(cmd)
cmd = "iptables -t nat -A POSTROUTING -s %s -d %s -p tcp --destination-port %s -j SNAT --to-source %s" % \
    (ocuWirelessIp,payloadIp,port,packbotWiredIp)
runSys(cmd)
