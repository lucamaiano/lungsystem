## import the serial library
import serial
import time
import os
import queue
import threading
import requests
import json
import socket
import signal
# create an ipv4 (AF_INET) socket object using the tcp protocol (SOCK_STREAM)
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)



queue = queue.Queue(1000)


from time import gmtime, strftime

## open text file to store the current 
##gps co-ordinates received from the rover 
def open_file (i_giorno,i_ora):  
    fname = "sensorData_"+i_giorno+"_"+i_ora+".csv"
    if os.path.isfile(fname):
    	t_file = open(fname, 'a')
    else:
    	t_file = open(fname, 'w')
    	t_file.write("DateRil;Temperature;Humidity;Pressure;Gas\n")
    return t_file
	
class SIGINT_handler():
    def __init__(self):
        self.SIGINT = False

    def signal_handler(self, signal, frame):
        print('You pressed Ctrl+C!')
        self.SIGINT = True
        client.send(b'POST / HTTP/1.1\r\n')
        exit()
		


	

fname = "dataToLoad.txt"
t_file = open(fname, 'a')
IpServer = input('Insert Ip Address of Server on your board: ')
client.connect((IpServer, 80))
print("Connected to server") 
while 1:
	giorno=time.strftime("%Y/%m/%d")
	ora=time.strftime("%H:%M:%S")
	handler = SIGINT_handler()
	signal.signal(signal.SIGINT, handler.signal_handler)	
	
# connect the client
# client.connect((target, port))

# send some data (in this case a HTTP GET request)
	client.send(b'GET / HTTP/1.1\r\n')
#client.send(b'hello\r\n\r\n')
# receive the response data (4096 is recommended buffer size)
	response = client.recv(4096)

	print (response)
	valori = str(response)
	valori=valori.replace("\n","")
	valori=valori.replace("\r","")
	valori=valori.replace("'","")
	valori=valori.replace("b","")
	pressure=0
	temperature=0
	humidity=0
	gas=0
	if valori != "" and valori != "\n":
	   dati=valori.split(";",4)
	   temperature=dati[0]
	   humidity=dati[1]
	   pressure=dati[2]
	   gas = dati[3]
	if temperature != 0 and gas != "0.0":
		dictToSend = {'day':giorno,'hour':ora,'pressure':pressure,'humidity':humidity,'temperature':temperature,'gas':gas}
		try:
			res = requests.post('http://localhost:5000/api/measures', json=dictToSend)
		except requests.exceptions.RequestException as e:  # This is the correct syntax
			print("No Connection to server.Data saved locally")
			t_file.write(json.dumps(dictToSend)+"\n")		
	##	print dictToSend

	time.sleep(1)

client.close()	
t_file.close()
del client