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

  //Establish three network socket connections for communication:
  //Player as client
  Client playerClient;
  playerClient.initStatus(argv[1], argv[2]);
  playerClient.createSocket();
  recv(playerClient.socket_fd, &player_id, sizeof(player_id), 0);
  recv(playerClient.socket_fd, &num_players, sizeof(num_players), 0);

  //Player as server
  Server playerServer;
  playerServer.initStatus(NULL, NULL);
  playerServer.createSocket();
  int port = playerServer.getPort();
  send(playerClient.socket_fd, &port, sizeof(port), 0);

  cout << "Connected as player " << player_id << " out of " << num_players
       << " total players" << endl;

  //with the player to the left
  string tmp;
  playerServer.acceptConnection(tmp);

  // with the player to the right
  int nextPort;
  char nextIP[100];
  recv(playerClient.socket_fd, &nextIP, sizeof(nextIP), MSG_WAITALL);
  recv(playerClient.socket_fd, &nextPort, sizeof(nextPort), MSG_WAITALL);
  char _port[9];
  sprintf(_port, "%d", nextPort);

  Client nextPlayerClient;
  nextPlayerClient.initStatus(nextIP, _port);
  nextPlayerClient.createSocket();

  //Keep listening to the three channels as “potato” can arrive
  //on any of these three channels.
  Potato potato;
  vector<int> fd = {
      nextPlayerClient.socket_fd, playerServer.socket_fd, playerClient.socket_fd};
  fd_set readfds;
  srand((unsigned int)time(NULL) + player_id);
  int nfds = 1 + max(nextPlayerClient.socket_fd, playerServer.socket_fd);

  // Properly handle everything received based on game rules.
  while (1) {
    FD_ZERO(&readfds);
    for (int i = 0; i < 3; i++) {
      FD_SET(fd[i], &readfds);
    }
    select(nfds, &readfds, NULL, NULL, NULL);

    for (int i = 0; i < 3; i++) {
      if (FD_ISSET(fd[i], &readfds)) {
        int rec = recv(fd[i], &potato, sizeof(potato), MSG_WAITALL);
        if (rec - sizeof(potato)) {
          cerr << "Error: potate being recieve is broken" << endl;
        }
        break;
      }
    }
    if (!potato.num_hops) {
      break;
    }
    else {
      potato.num_hops--;
      potato.path[potato.cnt++] = player_id;
      if (!potato.num_hops) {
        int sd = send(playerServer.socket_fd, &potato, sizeof(potato), 0);
        while (sd - sizeof(potato)) {
          cerr << "Error: message buffer being sent is broken" << endl;
          cout << "retrying..." << endl;
          sd = send(playerServer.socket_fd, &potato, sizeof(potato), 0);
        }
        cout << "I'm it" << endl;
      }
      else {
        int random = rand() % 2;
        int id = (random == 0) ? (player_id + num_players - 1) % num_players
                               : (player_id + 1) % num_players;
        int sd = send(fd[id], &potato, sizeof(potato), 0);
        while (sd - sizeof(potato)) {
          cerr << "Error: message buffer being sent is broken" << endl;
          cout << "retrying..." << endl;
          sd = send(fd[id], &potato, sizeof(potato), 0);
        }
        cout << "Sending potato to " << id << endl;
      }
    }
  }

  close(playerClient.socket_fd);
  close(playerServer.socket_fd);
  close(nextPlayerClient.socket_fd);
  return 0;
}
