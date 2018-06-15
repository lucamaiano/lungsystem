import os
import requests
import json

fname = "dataToLoad.txt"
t_file = open(fname, 'r')
righe = t_file.readlines()
if len(righe) > 0:
	for line in righe:
		try:
			data = json.loads(line)
			res = requests.post('http://localhost:5000/api/measures', json=data)
			righe.remove(line)
		except requests.exceptions.RequestException as e:  # This is the correct syntax
			print("No Connection to server.")
else:
	print ("No more lines to import")

open(fname, 'w').writelines(righe)