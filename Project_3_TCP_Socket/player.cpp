#include <vector>

#include "client.h"
#include "helper.h"
#include "potato.h"
#include "server.h"

using namespace std;

int main(int argc, char * argv[]) {
  if (argc != 3) {
    cout << "The client should be invoked as: " << endl;
    cout << "player <machine_name> <port_num>" << endl;
    cout << "example: ./player vcm-xxxx.vm.duke.edu 1234" << endl;
    return 1;
  }

  int player_id;
  int num_players;

  //Player as client: communicate with ringmaster
  Client player2master;
  player2master.initStatus(argv[1], argv[2]);
  player2master.createSocket();
  receive_message(player2master.socket_fd, &player_id, sizeof(player_id), 0);
  receive_message(player2master.socket_fd, &num_players, sizeof(num_players), 0);
  //cout << "player client success" << endl;

  //Player as server: can communicate with left
  Server player2left;
  player2left.initStatus(NULL, "");
  player2left.createSocket();
  int port = player2left.getPort();
  send_message(player2master.socket_fd, &port, sizeof(port), 0);

  cout << "Connected as player " << player_id << " out of " << num_players
       << " total players" << endl;

  // with the player to the right
  int nextPort;
  char nextIP[NextIPSize];
  receive_message(player2master.socket_fd, &nextIP, sizeof(nextIP), MSG_WAITALL);
  receive_message(player2master.socket_fd, &nextPort, sizeof(nextPort), MSG_WAITALL);

  char _port[9];
  sprintf(_port, "%d", nextPort);

  Client Player2right;
  Player2right.initStatus(nextIP, _port);
  Player2right.createSocket();
  int right_fd = Player2right.socket_fd;

  //get id of the player to the left
  string tmp;
  player2left.acceptConnection(tmp);
  int left_fd = player2left.client_connection_fd;

  //Keep listening to the three channels as “potato” can arrive
  //on any of these three channels.
  // Properly handle everything received based on game rules.
  Potato potato;
  vector<int> fd = {right_fd, left_fd, player2master.socket_fd};
  fd_set readfds;
  srand((unsigned int)time(NULL) + player_id);
  int nfds = 1 + max(right_fd, left_fd);

  while (1) {
    FD_ZERO(&readfds);
    for (int i = 0; i < NumFDs; i++) {
      FD_SET(fd[i], &readfds);
    }
    select(nfds, &readfds, NULL, NULL, NULL);
    int rec;
    for (int i = 0; i < NumFDs; i++) {
      if (FD_ISSET(fd[i], &readfds)) {
        rec = receive_message(fd[i], &potato, sizeof(potato), MSG_WAITALL);
        break;
      }
    }

    // must check if rec == 0
    // otherwise other player will recieve msg
    if (potato.num_hops == 0 || rec == 0) {
      break;
    }
    else {
      potato.num_hops--;
      cout << potato.num_hops << endl;
      potato.path[potato.cnt++] = player_id;
      if (potato.num_hops == 0) {
        int sd = send_message(fd[2], &potato, sizeof(potato), 0);
        cout << "I'm it" << endl;
      }
      else {
        int random = rand() % 2;
        int id = (random == 0) ? (player_id + 1) % num_players
                               : (player_id + num_players - 1) % num_players;
        int sd = send_message(fd[random], &potato, sizeof(potato), 0);
        cout << "Sending potato to " << id << endl;
      }
    }
  }

  close(player2master.socket_fd);
  close(player2left.socket_fd);
  close(Player2right.socket_fd);
  return 0;
}
