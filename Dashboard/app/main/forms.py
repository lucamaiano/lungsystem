from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, BooleanField, SubmitField,TextAreaField
from wtforms.validators import ValidationError, DataRequired, Email, EqualTo, DataRequired, Length
from app.models import User
from app import db



class EditProfileForm(FlaskForm):
    username = StringField('Username', validators=[DataRequired()])
    email = StringField('Email', validators=[DataRequired(), Email()])
    name = StringField('Name', validators=[Length(min=0, max=64)])
    surname = StringField('Last Name', validators=[Length(min=0, max=64)])
    address = StringField('Address', validators=[Length(min=0, max=140)])
    country = StringField('Country', validators=[Length(min=0, max=64)])
    city = StringField('City', validators=[Length(min=0, max=64)])
    zipcode = StringField('City', validators=[Length(min=0, max=64)])
    submit = SubmitField('Update Profile')

    def __init__(self, original_username, *args, **kwargs):
        super(EditProfileForm, self).__init__(*args, **kwargs)
        self.original_username = original_username

    def validate_username(self, username):
        if username.data != self.original_username:
            user = User.query.filter_by(username=self.username.data).first()
            if user is not None:
                raise ValidationError('Please use a different username.')















    
