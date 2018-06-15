from flask import render_template, request, flash, redirect, url_for,request
from flask_login import current_user, login_required
from app.models import User, Measurements
from werkzeug.urls import url_parse
from datetime import datetime
from app import db
from app.main.forms import EditProfileForm
from app.main import bp




@bp.before_request
def before_request():
    if current_user.is_authenticated:
        current_user.last_seen = datetime.utcnow()
        db.session.commit()


@bp.route('/')
@bp.route('/index')
@bp.route('/dashboard')
@login_required
def index():

    return render_template('index.html', title='Dashboard', last_day='prova', last_week='prova2')


@bp.route('/table', methods=['GET', 'POST'])
@login_required
def table():
    measures = Measurements.query.order_by(Measurements.day.desc()).all()
    if len(measures) > 50:
        measures = measures[:50]
    response = []
    for m in measures:
        m = str(m).split(',') 
        measure = []
        for parameter in m:
            measure.append(parameter)
        response.append(measure)
    return render_template('table.html', title='Measurements', measures=response)


@bp.route('/dialogflow')
@login_required
def dialogflow():
    return render_template('dialogflow.html', title='Chat Assistant In')


@bp.route('/maps')
@login_required
def maps():
    return render_template('maps.html', title='Maps')


@bp.route('/user/<username>', methods=['GET', 'POST'])
@login_required
def user(username):
    user = User.query.filter_by(username=username).first_or_404()
    form = EditProfileForm(current_user.username)
    if form.validate_on_submit():
    	current_user.username = form.username.data
    	current_user.email = form.email.data
    	current_user.name = form.name.data
    	current_user.surname = form.surname.data
    	current_user.address = form.address.data
    	current_user.country = form.country.data
    	current_user.city = form.city.data
    	current_user.zipcode = form.zipcode.data
    	db.session.commit()
    	flash('Your changes have been saved.')
    	return redirect(url_for('main.index'))
    elif request.method == 'GET':
    	form.username.data = current_user.username
    	form.email.data = current_user.email
    	form.name.data = current_user.name
    	form.surname.data = current_user.surname
    	form.address.data = current_user.address
    	form.country.data = current_user.country
    	form.city.data = current_user.city
    	form.zipcode.data = current_user.zipcode
    return render_template('user.html', title='Update Profile',
                           user=user, form=form)












