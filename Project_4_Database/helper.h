#ifndef __HELPER__
#define __HELPER__
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>

#include "query_funcs.h"
using namespace std;
using namespace pqxx;

void createTable(const string & filename, connection * C) {
  work work(*C);
  ifstream sql_file(filename);
  string sql_commands((istreambuf_iterator<char>(sql_file)), istreambuf_iterator<char>());
  work.exec(sql_commands);
  work.commit();
}

void dropTable(const string & tablename, connection * C) {
  work work(*C);
  string sql_commands = "DROP TABLE IF EXISTS " + tablename + " CASCADE;";
  work.exec(sql_commands);
  work.commit();
}

void insertPlayer(const string & filename, connection * C) {
  string line;
  ifstream data_file(filename);

  while (getline(data_file, line)) {
    // parse the line into player data
    istringstream iss(line);
    int player_id, team_id, jersey_num, mpg, ppg, rpg, apg;
    double spg, bpg;
    string first_name, last_name;

    iss >> player_id >> team_id >> jersey_num >> first_name >> last_name >> mpg >> ppg >>
        rpg >> apg >> spg >> bpg;
    add_player(
        C, team_id, jersey_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);
  }
}

void insertTeam(const string & filename, connection * C) {
  string line;
  ifstream data_file(filename);

  while (getline(data_file, line)) {
    // parse the line into player data
    istringstream iss(line);
    int team_id, state_id, color_id, wins, losses;
    string name;
    iss >> team_id >> name >> state_id >> color_id >> wins >> losses;
    add_team(C, name, state_id, color_id, wins, losses);
  }
}

void insertState(const string & filename, connection * C) {
  string line;
  ifstream data_file(filename);

  while (getline(data_file, line)) {
    // parse the line into player data
    istringstream iss(line);
    int state_id;
    string name;
    iss >> state_id >> name;
    add_state(C, name);
  }
}

void insertColor(const string & filename, connection * C) {
  string line;
  ifstream data_file(filename);

  while (getline(data_file, line)) {
    // parse the line into player data
    istringstream iss(line);
    int color_id;
    string name;
    iss >> color_id >> name;
    add_color(C, name);
  }
}

#endif