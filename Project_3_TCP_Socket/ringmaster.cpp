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

  //Establish N network socket connections with N number of players
  for (int i = 0; i < num_players; i++) {
    //get ip
    fd.push_back(ringmaster.acceptConnection(ip[i]));

    // send player_id to client
    send(fd[i], &i, sizeof(i), 0);
    // send num_players to client
    send(fd[i], &num_players, sizeof(num_players), 0);
    // receive port from client
    recv(fd[i], &port[i], sizeof(port[i]), 0);

    cout << "Player " << i << " is ready to play " << endl;
  }

  //and provide relevant information to each player
  for (int i = 0; i < num_players; i++) {
    int nextID = (i == num_players - 1) ? 0 : i + 1;
    int nextPort = port[nextID];
    char address[100];
    memset(address, 0, sizeof(address));
    strcpy(address, ip[nextID].c_str());

    // send next player id to client
    send(fd[i], &nextPort, sizeof(nextPort), 0);
    // send next player port to client
    send(fd[i], &address, sizeof(address), 0);
  }

  //Create a “potato” object
  Potato potato;
  potato.num_hops = num_hops;

  //Randomly select a player and send the
  //“potato” to the selected player
  if (num_hops) {
    srand((unsigned int)time(NULL) + num_players);
    int random = rand() % num_players;

    cout << "Ready to start the game, sending potato to player " << random << endl;

    //On success, these calls return the number of bytes sent.
    int sd = send(fd[random], &potato, sizeof(potato), 0);
    while (sd - sizeof(potato)) {
      cerr << "Error: message buffer being sent is broken" << endl;
      cout << "retrying..." << endl;
      sd = send(fd[random], &potato, sizeof(potato), 0);
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    for (int i = 0; i < num_players; i++) {
      FD_SET(fd[i], &readfds);
    }
    select(ringmaster.client_connection_fd + 1, &readfds, NULL, NULL, NULL);
    for (int i = 0; i < num_players; i++) {
      if (FD_ISSET(fd[i], &readfds)) {
        int rec = recv(fd[i], &potato, sizeof(potato), MSG_WAITALL);
        if (rec - sizeof(potato)) {
          cerr << "Error: potate being recieve is broken" << endl;
        }
        break;
      }
    }
  }

  for (int i = 0; i < num_players; i++) {
    int sd = send(fd[i], &potato, sizeof(potato), 0);
    while (sd - sizeof(potato)) {
      cerr << "Error: message buffer being sent is broken" << endl;
      cout << "retrying..." << endl;
      sd = send(fd[i], &potato, sizeof(potato), 0);
    }
  }

  cout << "Trace of potato:" << endl;
  for (int i = 0; i < potato.cnt; i++) {
    cout << potato.path[i];
    char out = (i == potato.cnt - 1) ? '\n' : ',';
    cout << out;
  }

  for (int i = 0; i < num_players; i++) {
    close(fd[i]);
  }
  close(ringmaster.socket_fd);

  return 0;
}
