## import the serial library
import serial
import time
import os
import Queue
import threading
import requests
import json

queue = Queue.Queue(1000)


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
	
	
serA = serial.Serial("COM5",9600, timeout=1)
serT = serial.Serial("COM6",115200, timeout=1)

def serial_read(s):
    while 1:
        line = s.readline()
	line =line.replace("\n","")
	line=line.replace("\r","")
	if line != "\n":
		queue.put(line)

##threadA = threading.Thread(target=serial_read, args=(serA,),).start()
##threadT = threading.Thread(target=serial_read, args=(serT,),).start()

fname = "dataToLoad.txt"
t_file = open(fname, 'a')

while 1:
	giorno=time.strftime("%Y/%m/%d")
	ora=time.strftime("%H:%M:%S")
	line = serA.readline()
	line=line.replace("\n","")
	line=line.replace("\r","")
	pressure=0
	temperature=0
	humidity=0
	gas=0
	if line != "" and line != "\n":
	   dati=line.split(";",3)
	   temperature=dati[0]
	   humidity=dati[1]
	   pressure=dati[2]
	line=serT.readline()
	line=line.replace("\n","")
	if line != "" and line != "\n":
		gas = line
	if temperature != 0 and gas != "0.0":
		dictToSend = {'day':giorno,'hour':ora,'pressure':pressure,'humidity':humidity,'temperature':temperature,'gas':gas}
		try:
			res = requests.post('http://localhost:5000/api/measures', json=dictToSend)
		except requests.exceptions.RequestException as e:  # This is the correct syntax
			print("No Connection to server.Data saved locally")
			t_file.write(json.dumps(dictToSend)+"\n")		
	##	print dictToSend
	time.sleep(1)
	
t_file.close()
serA.close()
serT.close()
