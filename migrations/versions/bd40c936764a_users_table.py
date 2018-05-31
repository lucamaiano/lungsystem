"""users table

Revision ID: bd40c936764a
Revises: 
Create Date: 2018-05-30 22:58:18.484049

"""
from alembic import op
import sqlalchemy as sa


# revision identifiers, used by Alembic.
revision = 'bd40c936764a'
down_revision = None
branch_labels = None
depends_on = None


def upgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.create_table('measurements',
    sa.Column('day', sa.String(length=64), nullable=False),
    sa.Column('hour', sa.String(length=64), nullable=False),
    sa.Column('pressure', sa.Float(), nullable=True),
    sa.Column('temperature', sa.Float(precision=64), nullable=True),
    sa.Column('humidity', sa.Float(), nullable=True),
    sa.Column('gas', sa.Float(), nullable=True),
    sa.PrimaryKeyConstraint('day', 'hour')
    )
    op.create_table('user',
    sa.Column('id', sa.Integer(), nullable=False),
    sa.Column('username', sa.String(length=64), nullable=True),
    sa.Column('email', sa.String(length=120), nullable=True),
    sa.Column('password_hash', sa.String(length=128), nullable=True),
    sa.Column('name', sa.String(length=64), nullable=True),
    sa.Column('surname', sa.String(length=64), nullable=True),
    sa.Column('address', sa.String(length=140), nullable=True),
    sa.Column('country', sa.String(length=64), nullable=True),
    sa.Column('city', sa.String(length=64), nullable=True),
    sa.Column('zipcode', sa.String(length=64), nullable=True),
    sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_user_email'), 'user', ['email'], unique=True)
    op.create_index(op.f('ix_user_username'), 'user', ['username'], unique=True)
    # ### end Alembic commands ###


def downgrade():
    # ### commands auto generated by Alembic - please adjust! ###
    op.drop_index(op.f('ix_user_username'), table_name='user')
    op.drop_index(op.f('ix_user_email'), table_name='user')
    op.drop_table('user')
    op.drop_table('measurements')
    # ### end Alembic commands ###
