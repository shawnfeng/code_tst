#!/usr/bin/env python 

import socket 

host = '' 
port = 8000
backlog = 5 
size = 10240
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
s.bind((host,port)) 
s.listen(backlog) 
while True: 
    client, address = s.accept() 
    data = client.recv(size) 
    print data
    if data: 
        client.send(data) 
    client.close()
