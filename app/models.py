from datetime import datetime
from hashlib import md5
from app import db, login
from flask_login import UserMixin
from flask import url_for, jsonify
from werkzeug.security import generate_password_hash, check_password_hash


class PaginatedAPIMixin(object):
    @staticmethod
    def to_collection_dict(query, page, per_page, endpoint, **kwargs):
        resources = query.paginate(page, per_page, False)
        data = {
            'items': [item.to_dict() for item in resources.items],
            '_meta': {
                'page': page,
                'per_page': per_page,
                'total_pages': resources.pages,
                'total_items': resources.total
            },
            '_links': {
                'self': url_for(endpoint, page=page, per_page=per_page,
                                **kwargs),
                'next': url_for(endpoint, page=page + 1, per_page=per_page,
                                **kwargs) if resources.has_next else None,
                'prev': url_for(endpoint, page=page - 1, per_page=per_page,
                                **kwargs) if resources.has_prev else None
            }
        }
        return data


class User(PaginatedAPIMixin, UserMixin, db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(64), index=True, unique=True)
    email = db.Column(db.String(120), index=True, unique=True)
    password_hash = db.Column(db.String(128))
    name = db.Column(db.String(64))
    surname = db.Column(db.String(64))
    address = db.Column(db.String(140))
    country = db.Column(db.String(64))
    city = db.Column(db.String(64))
    zipcode = db.Column(db.String(64))

    def __repr__(self):
        return '<User {}>'.format(self.username) 

    def set_password(self, password):
        self.password_hash = generate_password_hash(password)

    def check_password(self, password):
        return check_password_hash(self.password_hash, password)

    def avatar(self, size):
        digest = md5(self.email.lower().encode('utf-8')).hexdigest()
        return 'https://www.gravatar.com/avatar/{}?d=identicon&s={}'.format(
            digest, size)

    def to_dict(self):
        data = {
            'id': self.id,
            'username': self.username,
            'address': self.address
        }
        return data

    def from_dict(self, data, new_user=False):
        for field in ['username', 'address']:
            if field in data:
                setattr(self, field, data[field])
        if new_user and 'password' in data:
            self.set_password(data['password'])


@login.user_loader
def load_user(id):
    return User.query.get(int(id))



class Measurements(db.Model):
    day = db.Column(db.String(64), primary_key=True)
    hour = db.Column(db.String(64), primary_key=True)
    pressure = db.Column(db.Float())
    temperature = db.Column(db.Float(64))
    humidity = db.Column(db.Float())
    gas = db.Column(db.Float())

    def __repr__(self):
        data = {
            'date': self.day,
            "measures": [
                {
                  "hour": self.hour,
                  "pressure": self.pressure,
                  "humidity": self.humidity,
                  "temperature": self.temperature,
                  "gas": self.gas
                }
            ]    
        }
        return '{},{},{},{},{},{}'.format(self.day, self.hour, self.pressure, self.humidity, self.temperature, self.gas)
    
    @staticmethod
    def query_all():
        resources = Measurements.query.all()
        data = []
        for m in resources:
            m = str(m).split(',')           
            data.append({
                'date': m[0],
                "measure": [
                    {
                      "hour": m[1],
                      "pressure": m[2],
                      "humidity": m[3],
                      "temperature": m[4],
                      "gas": m[5]
                    }
                ]    
            })
        return data

    @staticmethod
    def query_day(day):
        resources = Measurements.query.filter_by(day=day).all()
        data = []
        for m in resources:
            m = str(m).split(',')           
            data.append({
                'date': m[0],
                "measure": [
                    {
                      "hour": m[1],
                      "pressure": m[2],
                      "humidity": m[3],
                      "temperature": m[4],
                      "gas": m[5]
                    }
                ]    
            })
        return data

    @staticmethod
    def query_interval_of_measures(day1, day2):
        resources = ''
        if day1 < day2:
            resources = Measurements.query.filter(Measurements.day >= day1).filter(Measurements.day <= day2).all()
        else:
            resources = Measurements.query.filter(Measurements.day >= day1).filter(Measurements.day <= day2).all()

        data = []
        print(resources)
        for m in resources:
            m = str(m).split(',')           
            data.append({
                'date': m[0],
                "measure": [
                    {
                      "hour": m[1],
                      "pressure": m[2],
                      "humidity": m[3],
                      "temperature": m[4],
                      "gas": m[5]
                    }
                ]    
            })
        return data

    # Measurements.query.filter(Measurements.day >= '2018/05/01').all()
    # Measurements.query.order_by(Measurements.day).all()
    # Measurements.query.filter(Measurements.day >= '2018/04/02').filter(Measurements.day <= '2018/05/10').all()

    @staticmethod
    def to_dict(data):
        day, hour, pressure, humidity, temperature, gas = None, None, None, None, None, None
        if 'day' in data:
            day = data['day']
        if 'hour' in data:
            hour = data['hour']
        if 'pressure' in data:
            pressure = data['pressure']
        if 'humidity' in data:
            humidity = data['humidity']
        if 'temperature' in data:
            temperature = data['temperature']
        if 'gas' in data:
            gas = data['gas']
        data = {
            'day': day,
            'hour': hour,
            'pressure': pressure,
            'humidity': humidity,
            'temperature': temperature,
            'gas': gas
        }
        m = Measurements(day=day, hour=hour, pressure=pressure, humidity=humidity, temperature=temperature,\
         gas=gas)
        db.session.add(m)
        db.session.commit()
        return data





    @staticmethod
    def dialogflow_request(request):
        params = {'pressure': 2, 'humidity': 3, 'temperature': 4, 'gas': 5}
        try:
            # Extract the parameters and search them on local json file
            date = clean_date(request.get('queryResult').get('parameters').get('date'))
            hour = clean_hour(request.get('queryResult').get('parameters').get('hour'))
            parameters = request.get('queryResult').get('parameters').get('params')

            print('date: ' +  str(date))
            print('hour: ' +  str(hour))
            print('parameters: ' +  str(parameters))
            if 'all' in parameters:
                    parameters = []
                    for p in params:
                        parameters.append(p)
            res_params = get_data(date, hour, parameters)
            res_params = params_mean(res_params)
            res_params = format_response(parameters, res_params)
        except AttributeError:
            return 'json error'

    def get_data(date, hour, parameters):
        ''' 
            Read data from a csv file and extract the requested parameters
        '''
        try:
            measures = Measurements.query.filter(Measurements.day == date)
            response = []   # Contains the response values
            hour = hour.split(':')
            hr = hour[0]
            mn = hour[1]
            for measure in measures:
                if measure[1].split(':')[0] ==  hr:
                    res_par = []
                    for p in parameters:
                        res_par.append(measure[params[p]])
                    response.append(res_par)

            return response
        except AttributeError:
            return 'csv error'

    def clean_date(date):
        #   Clean the date from unuseful informations
        d_clean = date.split("T")
        return d_clean[0]

    def clean_hour(hour):
        #   Clean the hour from unuseful informations
        h_clean = hour.split("T")
        h_clean = h_clean[1].split('+')
        return h_clean[0]

    def params_mean(parameters):
        ''' 
            Give a list of parameters[[par1,...,parN], ..., [par1,...,parN]], 
            it calculates the mean for each parameter
        '''
        params = {} # <Parameter position, sum of values> pairs
        length = 0
        if len(parameters) > 0:
            length = len(parameters)
        for p_list in parameters:
            for p in range(0, len(p_list)):
                if p in params:
                    params[p] += float(p_list[p])
                else:
                    params[p] = float(p_list[p])
        
        parameters = []
        for p in params:
            params[p] = params[p]/length
            parameters.append(params[p])
        return parameters

    ONE_PARAM = [
        'The value of {} is {}!'
    ]

    TWO_PARAMS = [
        'The value of {} is {} and the one of {} is {}',
        '{}: {} and {}: {}'
    ]

    THREE_PARAMS = [
        '{}: {}, {}: {} and {}: {}',
        'Mmm let me think... {}: {}, {}: {} and {}: {}. That\'s it!'
    ]

    ALL = [
        'Here all the parameters. \n\t Temperature: {} \n\t Humidity: {} \n\t Pressure: {} \n\t Gas: {}'
    ]

    def format_response(parameters, values):
        if len(values) == 0:
            return 'I\'m sorry, I don\'t have this informations!'
        if len(parameters) == 1:
            res = str(ONE_PARAM[0]).format(parameters[0], values[0])
            print(res)
        elif len(parameters) == 2 and len(values) == 2:
            index = random.randint(0, len(TWO_PARAMS)-1)
            res = str(TWO_PARAMS[index]).format(parameters[0], values[0], parameters[1], values[1])
        elif len(parameters) == 3 and len(values) == 3:
            index = random.randint(0, len(THREE_PARAMS)-1)
            res = str(THREE_PARAMS[index]).format(parameters[0], values[0], parameters[1], values[1], parameters[2], values[2])
        elif 'all' in parameters or len(parameters) == 4:
            res = str(ALL[0]).format(values[0], values[1], values[2], values[3])

        return res

    
