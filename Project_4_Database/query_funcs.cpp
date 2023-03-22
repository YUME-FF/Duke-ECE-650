#include "query_funcs.h"

#include <iomanip>
void add_player(connection * C,
                int team_id,
                int jersey_num,
                string first_name,
                string last_name,
                int mpg,
                int ppg,
                int rpg,
                int apg,
                double spg,
                double bpg) {
  work work(*C);
  stringstream instr;
  instr << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, "
           "RPG, APG, SPG, BPG) VALUES ("
        << team_id << ", " << jersey_num << ", " << work.quote(first_name) << ", "
        << work.quote(last_name) << ", " << mpg << ", " << ppg << ", " << rpg << ", "
        << apg << ", " << spg << ", " << bpg << ");";
  work.exec(instr.str());
  work.commit();
}

void add_team(connection * C,
              string name,
              int state_id,
              int color_id,
              int wins,
              int losses) {
  work work(*C);
  stringstream instr;
  instr << "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("
        << work.quote(name) << ", " << state_id << ", " << color_id << ", " << wins
        << ", " << losses << ");";
  work.exec(instr.str());
  work.commit();
}

void add_state(connection * C, string name) {
  work work(*C);
  stringstream instr;
  instr << "INSERT INTO STATE (NAME) VALUES (" << work.quote(name) << ");";
  work.exec(instr.str());
  work.commit();
}

void add_color(connection * C, string name) {
  work work(*C);
  stringstream instr;
  instr << "INSERT INTO COLOR (NAME) VALUES (" << work.quote(name) << ");";
  work.exec(instr.str());
  work.commit();
}

/*
 * All use_ params are used as flags for corresponding attributes
 * a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * a 0 for a use_ param means this attribute is disabled
 */
void query1(connection * C,
            int use_mpg,
            int min_mpg,
            int max_mpg,
            int use_ppg,
            int min_ppg,
            int max_ppg,
            int use_rpg,
            int min_rpg,
            int max_rpg,
            int use_apg,
            int min_apg,
            int max_apg,
            int use_spg,
            double min_spg,
            double max_spg,
            int use_bpg,
            double min_bpg,
            double max_bpg) {
  string names[6] = {"MPG", "PPG", "RPG", "APG", "SPG", "BPG"};
  int use[6] = {use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg};
  int minValue_i[4] = {min_mpg, min_ppg, min_rpg, min_apg};
  int maxValue_i[4] = {max_mpg, max_ppg, max_rpg, max_apg};
  double minValue_d[2] = {min_spg, min_bpg};
  double maxValue_d[2] = {max_spg, max_bpg};

  stringstream query;
  query << "SELECT * FROM PLAYER ";
  int i = 0;
  bool outputWhere = true;
  while (use[i] && i < 6) {
    if (outputWhere) {
      query << "WHERE ";
      outputWhere = false;
    }
    else {
      query << " AND ";
    }

    if (i < 4) {
      query << names[i] << " >= " << minValue_i[i] << " AND " << names[i]
            << " <= " << maxValue_i[i];
    }
    else {
      query << names[i] << " >= " << minValue_d[i - 4] << " AND " << names[i]
            << " <= " << maxValue_d[i - 4];
    }
    ++i;
  }

  query << ";";

  cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG"
       << endl;

  nontransaction nontransaction(*C);
  result result(nontransaction.exec(query.str()));
  result::iterator it = result.begin();
  while (it != result.end()) {
    cout << it[0].as<int>() << " " << it[1].as<int>() << " " << it[2].as<int>() << " "
         << it[3].as<string>() << " " << it[4].as<string>() << " " << it[5].as<int>()
         << " " << it[6].as<int>() << " " << it[7].as<int>() << " " << it[8].as<int>()
         << " ";
    cout << setprecision(1) << fixed << it[9].as<double>() << " " << it[10].as<double>()
         << endl;
    ++it;
  }
}

void query2(connection * C, string team_color) {
  work work(*C);
  stringstream query;
  query << "SELECT TEAM.NAME FROM TEAM, COLOR WHERE TEAM.COLOR_ID = COLOR.COLOR_ID AND "
           "COLOR.NAME = "
        << work.quote(team_color) << ";";

  cout << "NAME" << endl;
  work.commit();
  nontransaction nontransaction(*C);
  result result(nontransaction.exec(query.str()));
  result::iterator it = result.begin();
  while (it != result.end()) {
    cout << it[0].as<string>() << endl;
    ++it;
  }
}

void query3(connection * C, string team_name) {
  work work(*C);
  stringstream query;
  query
      << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM TEAM, PLAYER WHERE TEAM.NAME = "
      << work.quote(team_name) << " AND PLAYER.TEAM_ID = TEAM.TEAM_ID ORDER BY PPG DESC;";

  cout << "FIRST_NAME LAST_NAME" << endl;
  work.commit();
  nontransaction nontransaction(*C);
  result result(nontransaction.exec(query.str()));
  result::iterator it = result.begin();
  while (it != result.end()) {
    cout << it[0].as<string>() << " " << it[1].as<string>() << endl;
    ++it;
  }
}

void query4(connection * C, string team_state, string team_color) {
  work work(*C);
  stringstream query;
  query << "SELECT  UNIFORM_NUM, FIRST_NAME, LAST_NAME FROM PLAYER, STATE, COLOR, TEAM "
           "WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID AND "
           "TEAM.STATE_ID = STATE.STATE_ID AND STATE.NAME = "
        << work.quote(team_state) << " AND "
        << "COLOR.NAME = " << work.quote(team_color) << ";";

  cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
  work.commit();
  nontransaction nontransaction(*C);
  result result(nontransaction.exec(query.str()));
  result::iterator it = result.begin();
  while (it != result.end()) {
    cout << it[0].as<int>() << " " << it[1].as<string>() << " " << it[2].as<string>()
         << endl;
    ++it;
  }
}

void query5(connection * C, int num_wins) {
  work work(*C);
  stringstream query;
  query << "SELECT FIRST_NAME, LAST_NAME, NAME, WINS FROM PLAYER, TEAM WHERE "
           "PLAYER.TEAM_ID = TEAM.TEAM_ID AND WINS >"
        << work.quote(num_wins) << ";";

  cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
  work.commit();
  nontransaction nontransaction(*C);
  result result(nontransaction.exec(query.str()));
  result::iterator it = result.begin();
  while (it != result.end()) {
    cout << it[0].as<string>() << " " << it[1].as<string>() << " " << it[2].as<string>()
         << " " << it[3].as<int>() << endl;
    ++it;
  }
}
