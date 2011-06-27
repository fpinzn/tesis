import socket
import sys

HOST, PORT = "192.168.2.2", 9999
data = "fsdaasfdfdsa"

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to server and send data
sock.connect((HOST, PORT))
sock.send(data + "\n")

# Receive data from the server and shut down

received="f"
while received!="":
	received = sock.recv(56000)
	if(received!=""):
		print received
		continue
	
sock.close()

print "Sent:     %s" % data
print "Received: %s" % received