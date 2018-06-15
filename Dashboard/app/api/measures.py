from app.api import bp
from flask import jsonify, request
from app.models import Measurements


@bp.route('/measures', methods=['GET'])
def get_all_measures():
	request = Measurements.query_all()
	return jsonify(request)

@bp.route('/measures/<string:year>/<string:month>/<string:day>', methods=['GET'])
def get_measure(year, month, day):
	day = str(year) + '/' + str(month) + '/' + str(day)
	print(day)
	request = Measurements.query_day(day)
	return jsonify(request)

@bp.route('/measures/from/<string:year1>/<string:month1>/<string:day1>/to/<string:year2>/<string:month2>/<string:day2>', methods=['GET'])
def get_interval_of_measures(year1, month1, day1, year2, month2, day2):
	day1 = str(year1) + '/' + str(month1) + '/' + str(day1)
	day2 = str(year2) + '/' + str(month2) + '/' + str(day2)
	request = Measurements.query_interval_of_measures(day1, day2)
	return jsonify(request)

@bp.route('/measures', methods=['POST'])
def add_measurement():
    data = request.get_json() or {}
    if 'day' not in data or 'hour' not in data:
        return bad_request('must include day and hour fields')
    response = jsonify(Measurements.to_dict(data))
    response.status_code = 201
    return response

@bp.route('/measures/dialogflow', methods=['POST'])
def dialogflow():
    req = request.get_json(silent=True, force=True)
    res_params = dialogflow_request(req)
    return make_response(jsonify({'fulfillmentText': res_params}))