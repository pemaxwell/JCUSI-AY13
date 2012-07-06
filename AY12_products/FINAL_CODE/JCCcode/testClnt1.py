#!/usr/local/bin/python
# coding: utf-8
#simple illustration of thread module

# two clients connect to server; each client repeatedly sends a letter,
# stored in the variable k, which the server appends to a global string
# v, and reports v to the client; k = ’’ means the client is dropping
# out; when all clients are gone, server prints the final string v



import socket # networking module
import sys
import time

#create Internet TCP socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = 'localhost'#sys.argv[1] # server address
port = 4208#int(sys.argv[2]) # server port

#connect to server
s.connect((host, port))

while(1):
    time.sleep(2.0)
    #get letter
    k = '0,1,00,3,4,5,6,7,8,9,10,11,12'#raw_input('enter a letter:')
    s.send(k) # send k to server
    #if stop signal, then leave loop
    if k == '': break
    #v = s.recv(1024) # receive v from server (up to 1024 bytes)
    print k
    
s.close() # close socket
