# Lung System

A project developed for the Pervasive Systems exam of the master degree in Engineering in Computer Science. It takes trace of the quality of the air you breathe. Data are collected into the cloud through the use of sensors attached to a STM32 Nucleo board. Access to the data is provided by a visualization system and a chat bot assistant.

## Slides

If you are interested in the project, you can give a look at the presentations:
1. [Initial Concept](https://www.slideshare.net/LucaMaiano1/student-group-projects-initial-concept-what-are-you-breathing)
2. [MVP](https://www.slideshare.net/LucaMaiano1/pervasive-systems-mvp-what-has-been-done) 
3. [Final Project Presentation](https://www.slideshare.net/LucaMaiano1/pervasive-systems-final-project-presentation-the-sparkle-lung-system) 


## Project Members
1. [Luca Maiano](https://www.linkedin.com/in/lucamaiano/) 
2. [Maria Carmela Dipinto](https://www.linkedin.com/in/maria-carmela-dipinto/) 

## Table of Contents
1. [Visualization](#visualization)
	1. [Rest API](#rest-api)
	2. [Dialogflow](#dialogflow)
	3. [Some Insight](#some-insight)
2. [Hardware](#hardware)
	1. [Software](#software)
	2. [Prototype](#prototype)

## Visualization

The visualization was developed with Flask. Once that a user creates an account, he/she can explore the data throw three main components. The first component exposes the data through some plots developed with Ploty. The second component is a table, that retrieves the data of the last 100 measurements from the database and print them. The third and last component is a chatbot assistant developed using Dialogflow.

To run it locally, open the terminal and run the following commands (on Mac OS and Linux!):

```
cd Desktop
git clone https://github.com/lucamaiano/lungsystem.git
cd lungsystem
```

Now install the requirements. I suggest you to create a virtual enviromnment.

```
pip install -r requirements.txt
```

The first time that you run the app, you should migrate and update the database.

```
flask db migrate -m "import data"
flask db upgrade
export FLASK_APP=main.py
```

If you want to execute the code on Windows, instead of "export", use this command:
```
set FLASK_APP=main.py
```

Run the app.

```
flask run
```



### Rest API
The visualization includes also a Rest API.

| HTTP Method   | Resource URL | Notes |
| ------------- | ------------ | ----- |
| GET | /api/users/<string:id> | Return a user. |
| GET | /api/users | Return the collection of all users. |
| GET | /api/measures | Return the collection of all measurements. | 
| GET | /api/measures/<string:year>/<string:month>/<string:day> | Return the a measurement. |
| POST | /api/measures | Add a new measurement. |


### Dialogflow
Our system includes also a smart bot that could help you reading the data. You can ask him for measures using natural language. The agent can answer questions like "Give the value of pressure and humidity of today at 12pm" or "Tell me the all the parameters of yesterday at 10am". 

You can import the agent into your Dialogflow console. First create the agent.

![Dialogflow 1](images/dialogflow1.png)


Now you can import it just cliking on "IMPORT FROM ZIP" button

![Dialogflow 1](images/dialogflow2.png)


If you want to use it on the project, you must deploy the application on a web server. The simply add the url of your server on the Fulfilment section of Dialogflow.

### Some Insight
Once that you will be registered to the system and start to collect data, you will be able to visualize the data as follows.

![Dashboard 1](images/Dashboard2.png)


You will also have a table in which you can control the last 100 measures.

![Dashboard 2](images/Dashboard1.png)


Last but not least, you can even ask measures to the personal assistant

![Dialogflow](images/dialogflow.png)


You can use the Dashboard even on mobile, it is completely responsive!

![Dialogflow](images/mobile.png)


## Hardware
Carbon Monoxide is a tasteless and odourless gas unlike smoke from a fire, so its detection in the home environment is impossible without a dedicate detector. Carbon monoxide is produced by malfunctioning fuel burning devices which can be, for example, a house heating system. We have to be careful to detect its presence in a timely manner because CO is highly toxic inhalant and can be sometimes fatal. CO detector is designed to measure the level of gas over time and to set off a local alarm before dangerous level of gas are present. Home safety alarm are designed to link CO detector to people.

In order to achieve this goals our prototype provide for this hardware:

1. P-NUCLEO-IKA02A1- STM32 Nucleo pack (http://www.st.com/en/ecosystems/p-nucleo-ika02a1.html)

![P-NUCLEO-IKA02A1](images/en.p-nucleo-ika02a1_image.jpg)

This evaluation pack provides a reference design for various electrochemical sensors. It consists of the STM32 Nucleo Board (We use a NUCLEO-L053R8 optimized for low power applications) further with electrochemical toxic gas sensor expansion board with the Figaro TGS5141 CO sensor,which is linked with the TSU111 operational amplifier. The pack also includes a gas collector.

![P-NUCLEO-IKA02A1](images/en.p-nucleo-ika02a1_image.jpg)

To getting started with this pack follow [this guide](wiki/Getting-started-with-P-NUCLEO-IKA02A1). 

![P-NUCLEO-IKA02A1](images/en.p-nucleo-ika02a1_image.jpg)


Useful for good and safety monitoring of the air is to take under control also the pressure, humidity, temperature values of the environment. For these reason our prototype is provided for another board:
2. B-L475E-IOT01A - STM32L4 Discovery kit IoT node (http://www.st.com/en/evaluation-tools/b-l475e-iot01a.html)

![B-L475E-IOT01A](images/en.b_l475e_iot01a.jpg)

The B-L475E-IOT01A Discovery kit for IoT node allows users not only to monitor pressure,humidity and temperature but also,thanks to Wi-fi module, to develop applications with direct connection to cloud servers.

![B-L475E-IOT01A](images/en.b_l475e_iot01a.jpg)

This kit is the quickest way to develop Iot application based on rich hardware,simple software and easy support to connect intelligent node service and build powerful services.

To getting started with this pack follow [this guide](wiki/Getting-started-with-the-B-L475E-IOT01A-Discovery-kit).


### Software

⋅⋅* Before using the driver on the boards is necessary installing and launch with the boards connected these softwares:
	⋅⋅* STSW-LINK008:ST-LINK/V2-1 USBdriver (http://www.st.com/en/development-tools/stsw-link008.html)
	⋅⋅* STSW-LINK007:ST-LINK/V2-1 firmware upgrade (http://www.st.com/en/development-tools/stsw-link007.html)

⋅⋅* Tera Term (https://ttssh2.osdn.jp/index.html.en). It’s a serial line monitor that helps to read boards’ values. To set the Tera Term windows
	⋅⋅* on Setup/Terminal window set to Receive AUTO and to Transmit LF
	![B-L475E-IOT01A](images/en.b_l475e_iot01a.jpg)
	⋅⋅* on Setup/SerialPort window set the Baud rate to 115200
	![B-L475E-IOT01A](images/en.b_l475e_iot01a.jpg)
	⋅⋅* For P-NUCLEO-IKA02A1:
		1. IAR Embedded Workbench IDE (https://www.iar.com/iar-embedded-workbench/)
	⋅⋅* For B-L475E-IOT01A:
		1. IAR Embedded Workbench IDE (https://www.iar.com/iar-embedded-workbench/)
		1. ARM MBED OS (https://www.mbed.com/en/) 
		1. ARM MBED Compiler (https://os.mbed.com/handbook/mbed-Compiler)
		
	To discover how to use the board B-L475E-IOT01A in a Mbed project follow this link  https://os.mbed.com/platforms/ST-Discovery-L475E-IOT01A/	


### Prototype
LungSystem needs a Python script (readData.py) to read data passed by the boards on the serial port and to upload this data on the localhost. The file implementing this facility read correctly the formatted data on the serial port. 
If flask is not running the system is able to store locally the data with this line on a file called dataToLoad.txt

### Python script code
```
fname = "dataToLoad.txt"
t_file = open(fname, 'a')
```

With another Python script (loadBatch.py) LungSystem is able to  send automatically the data collected in dataToLoad.txt when Flask runs in a second time .
In particular it opens the file store on the local machine,reads all the lines, tries to connect with the database: if connection succeeds, than it sends the data to the db and deletes that line, if not , than it writes the line on the local file. At the end of these operation on the local machine there’s the file with just the data not sent.

To run the code:
1. Download the readData.py and loadBatch.py files
2. Open two windows on a terminal 
3. On the first one type:
```
python readData.py
```
4. On the other one type:
```
python loadBatch.py
```
To modify the destination of the data, modify the line 66 of readData.py file.
```
res = requests.post('http://localhost:5000/api/measures', json=dictToSend)
```

