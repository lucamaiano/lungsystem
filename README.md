# Lung System

A project developed for the Pervasive Systems exam of the master degree in Engineering in Computer Science. It takes trace of the quality of the air you breathe. Data are collected into the cloud through the use of sensors attached to a STM32 Nucleo board. Access to the data is provided by a visualization system and a chat bot assistant.

## Slides

If you are interested in the project, you can give a look at the presentations:
1. [MVP](https://www.slideshare.net/LucaMaiano1/student-group-projects-initial-concept-what-are-you-breathing) 
2. [Final Project Presentation](https://www.slideshare.net/LucaMaiano1/pervasive-systems-final-project-presentation-the-sparkle-lung-system) 


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

### Some Insight
Once that you will be registered to the system and start to collect data, you will be able to visualize the data as follows.

![Dashboard 1](images/Dashboard2.png)

You will also have a table in which you can control the last 100 measures.

![Dashboard 2](images/Dashboard1.png)

Last but not least, you can even ask measures to the personal assistant


## Project Members
1. [Luca Maiano](https://www.linkedin.com/in/lucamaiano/) 
2. [Maria Carmela Dipinto](https://www.linkedin.com/in/maria-carmela-dipinto/) 
