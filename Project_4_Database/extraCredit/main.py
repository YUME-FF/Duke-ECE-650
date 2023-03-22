from sqlalchemy import create_engine
from sqlalchemy import and_
from helper import makeColor, makePlayer, makeState, makeTeam, createTable
from sqlalchemy.orm import sessionmaker

from query_funcs import *

engine = create_engine(
    'postgresql://postgres:passw0rd@localhost:5432/ACC_BBALL')
session = sessionmaker(bind=engine)()


def exerciser():
    print("\n ===========Test======================")
    print("\n ===========Test1 no filters==========")
    query1(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)  # no filters
    print("\n ===========Test1 end=================")

    print("\n ===========Test2 multiple filters====")
    query1(1, 35, 40, 0, 0, 0, 0, 5, 10, 1, 0,
           0, 0, 0, 0, 0, 0, 0)  # multiple filters
    print("\n ===========Test2 end=================")

    print("\n ===========Test3 color RED===========")
    query2("Red")  # color filter match
    print("\n ===========Test3 end=================")

    print("\n =====Test4 university GeorgiaTech====")
    query3("GeorgiaTech")  # university filter match
    print("\n ===========Test4 end=================")

    print("\n =====Test5 state and color============")

    query4("NC", "DarkBlue")  # state and color filter match
    print("\n ===========Test5 end=================")

    print("\n ==============Test6 limit============")
    query5(13)  # limit is matching
    print("\n ===========Test6 end=================")

    return


if __name__ == '__main__':
    createTable()
    makeColor()
    makeState()
    makeTeam()
    makePlayer()
    exerciser()
