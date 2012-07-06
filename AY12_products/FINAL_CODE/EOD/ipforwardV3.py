# This python script sets up ip packet forwarding on the packbot using ip tables.
# After changing the IPs below for the payload, packbot and ocu run this script on 
# the packbot by using the command: python ipforward.py.
# Once installed when the ocu sends tcp packets to the packbot and the port is between
# 50000 and 60000 the ip table will forward the tcp packets to the payload.
# So from the point of the view of the ocu, the process on the payload
# would seem like it was actually running on the packbot.
import socket
import os


PICO =  "172.16.86.100"#updated to PICO IP (JCF, 31JAN12)
#Robot 22306: 172.16.87.34 172.17.87.34 
#Robot 22261: 172.16.86.245 172.17.86.245
#Robot 22240: 172.16.86.224 172.17.86.224
pbWired = "172.16.87.34" #updated to packbot IP (JCF, 31JAN12)
pbWireless = "172.17.87.34" #updated to packbot IP (JCF, 31JAN12)
ocu = "172.17.170.52" #updated to OCU IP, second octet might be wrong (JCF, 31JAN12)
#OCU 3: 172.16.171.36
port = "50000:60000"  #Range of port numbers to forward ip packets on #changed to 1 from 50000 to forward all packets (JCF, 31JAN12)
port1 = "50000:55000"  #Range of port numbers to forward ip packets on #changed to 1 from 50000 to forward all packets (JCF, 31JAN12)
port2 = "55001:60000"  #Range of port numbers to forward ip packets on #changed to 1 from 50000 to forward all packets (JCF, 31JAN12)

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
cmd = "iptables -t nat -A PREROUTING -s %s -d %s -p udp --destination-port %s -j DNAT --to-destination %s" % \
    (ocu,pbWireless,port,PICO)
runSys(cmd)
cmd = "iptables -t nat -A POSTROUTING -s %s -d %s -p udp --destination-port %s -j SNAT --to-source %s" % \
    (ocu,PICO,port,pbWired)
runSys(cmd)
cmd = "iptables -t nat -A PREROUTING -s %s -d %s -p tcp --destination-port %s -j DNAT --to-destination %s" % \
    (ocu,pbWireless,port,PICO)
runSys(cmd)
cmd = "iptables -t nat -A POSTROUTING -s %s -d %s -p tcp --destination-port %s -j SNAT --to-source %s" % \
    (ocu,PICO,port,pbWired)
runSys(cmd)

## payload -> ocu
cmd = "iptables -t nat -A PREROUTING -s %s -d %s -p udp --destination-port %s -j DNAT --to-destination %s" % \
    (PICO, pbWired, port,ocu)
runSys(cmd)
cmd = "iptables -t nat -A POSTROUTING -s %s -d %s -p udp --destination-port %s -j SNAT --to-source %s" % \
    (PICO, ocu, port, pbWireless)
runSys(cmd)
cmd = "iptables -t nat -A PREROUTING -s %s -d %s -p tcp --destination-port %s -j DNAT --to-destination %s" % \
    (PICO, pbWired, port,ocu)
runSys(cmd)
cmd = "iptables -t nat -A POSTROUTING -s %s -d %s -p tcp --destination-port %s -j SNAT --to-source %s" % \
    (PICO, ocu, port, pbWireless)
runSys(cmd)
