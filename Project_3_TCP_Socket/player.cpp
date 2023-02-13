#include <vector>

#include "client.h"
#include "potato.h"
#include "server.h"

using namespace std;

int main(int argc, char * argv[]) {
  if (argc != 2) {
    cout << "The client should be invoked as: " << endl;
    cout << "player <machine_name> <port_num>" << endl;
    cout << "example: ./player vcm-xxxx.vm.duke.edu 1234" << endl;
    return 1;
  }

  int player_id;
  int num_players;

  Client player;
  player.initStatus(argv[1], argv[2]);
  player.createSocket();

  // receive player_id from master
  recv(player.socket_fd, &player_id, sizeof(player_id), 0);
  // receive num_players from master
  recv(player.socket_fd, &num_players, sizeof(num_players), 0);

  //Player --> Server
  Server playerMaster;
  playerMaster.initStatus(NULL, NULL);
  playerMaster.createSocket();

  int port = playerMaster.getPort();
  send(player.socket_fd, &port, sizeof(port), 0);

  close(player.socket_fd);

  return 0;
}
