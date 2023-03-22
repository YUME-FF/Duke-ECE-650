from sqlalchemy import create_engine
from sqlalchemy import insert
from sqlalchemy.orm import sessionmaker

from models import Color, State, Team, Player

engine = create_engine(
    'postgresql://postgres:passw0rd@localhost:5432/ACC_BBALL')
session = sessionmaker(bind=engine)()


def load_table(table_name, file_path, column_names):
    with open(file_path) as f:
        lines = f.readlines()
        for line in lines:
            line = line.replace("\n", "")
            attributes = line.split(" ")
            values = {column_names[i]: attributes[i+1]
                      for i in range(len(column_names))}
            query = insert(table_name).values(**values)
            with engine.connect() as conn:
                conn.execute(query)


def createTable():
    tables = [Player, Team, State, Color]
    with engine.connect() as conn:
        for table in tables:
            if engine.dialect.has_table(conn, table.__tablename__):
                table.__table__.drop(engine)

    for table in tables[::-1]:
        table.__table__.create(engine)
    return


def makeColor():
    load_table(Color, 'data/color.txt', ['name'])
    return


def makeState():
    load_table(State, 'data/state.txt', ['name'])
    return


def makeTeam():
    load_table(Team, 'data/team.txt',
               ['name', 'state_id', 'color_id', 'wins', 'losses'])
    return


def makePlayer():
    load_table(Player, 'data/player.txt', ['team_id', 'uniform_num',
               'first_name', 'last_name', 'mpg', 'ppg', 'rpg', 'apg', 'spg', 'bpg'])
    return
