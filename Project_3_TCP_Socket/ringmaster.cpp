#include <vector>

#include "potato.h"
#include "server.h"

using namespace std;

int main(int argc, char * argv[]) {
  if (argc != 4) {
    cout << "The server should be invoked as: " << endl;
    cout << "ringmaster<port_num> <num_players> <num_hops>" << endl;
    cout << "example: ./ringmaster 1234 3 100" << endl;
    return 1;
  }

  vector<int> fd;
  vector<string> ip;
  vector<int> port;

  int num_players = atoi(argv[2]);
  int num_hops = atoi(argv[3]);

  cout << "Potato Ringmaster" << endl;
  cout << "Players = " << num_players << endl;
  cout << "Hops = " << num_hops << endl;

  Server ringmaster;
  ringmaster.initStatus(NULL, argv[1]);
  ringmaster.createSocket();

  close(ringmaster.socket_fd);

  return 0;
}
