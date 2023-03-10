#include "exerciser.h"

void exercise(connection * C) {
  cout << "\n ===========Test======================" << endl;
  cout << "\n ===========Test1 no filters==========" << endl;
  query1(C, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);  // no filters
  cout << "\n ===========Test1 end=================" << endl;

  cout << "\n ===========Test2 multiple filters====" << endl;
  query1(C, 1, 35, 40, 0, 0, 0, 0, 5, 10, 1, 0, 0, 0, 0, 0, 0, 0, 0);  // multiple filters
  cout << "\n ===========Test2 end=================" << endl;

  cout << "\n ===========Test3 color RED===========" << endl;
  query2(C, "Red");  // color filter match
  cout << "\n ===========Test3 end=================" << endl;

  cout << "\n ===========Test4 color Purple========" << endl;
  query2(C, "Purple");  // color filter no match
  cout << "\n ===========Test4 end=================" << endl;

  cout << "\n =====Test5 university GeorgiaTech====" << endl;
  query3(C, "GeorgiaTech");  // university filter match
  cout << "\n ===========Test5 end=================" << endl;

  cout << "\n =====Test6 university MIT============" << endl;
  query3(C, "MIT");  // university filter no match
  cout << "\n ===========Test6 end=================" << endl;

  cout << "\n =====Test7 state and color============" << endl;
  query4(C, "NC", "DarkBlue");  // state and color filter match
  cout << "\n ===========Test7 end=================" << endl;

  cout << "\n =====Test8 state and color============" << endl;
  query4(C, "TX", "Green");  // state and color filter no match
  cout << "\n ===========Test8 end=================" << endl;

  cout << "\n ==============Test9 limit============" << endl;
  query5(C, 13);  // limit is matching
  cout << "\n ===========Test9 end=================" << endl;

  cout << "\n ==============Test10 limit============" << endl;
  query5(C, 20);  // limit is higher than number of matching results
  cout << "\n ===========Test10 end=================" << endl;
}
