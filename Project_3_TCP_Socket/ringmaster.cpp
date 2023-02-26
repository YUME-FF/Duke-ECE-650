#include <vector>

#include "helper.h"
#include "potato.h"
#include "server.h"
using namespace std;

/**
 * This function establishes connections with all players and gets their IP, port,
 * and file descriptor in order to communicate with them.
 * @param ringmaster a Server object representing the ringmaster
 * @param num_players number of players in the game
 * @param fd file descriptors for each player
 * @param ip IP addresses for each player
 * @param port port numbers for each player
 */
void establishConnections(Server & ringmaster,
                          int num_players,
                          vector<int> & fd,
                          vector<string> & ip,
                          vector<int> & port) {
  for (int i = 0; i < num_players; i++) {
    //get ip
    ringmaster.acceptConnection(ip[i]);
    fd.push_back(ringmaster.client_connection_fd);

    // send player_id to client
    send_message(fd[i], &i, sizeof(i), 0);

    // send num_players to client
    send_message(fd[i], &num_players, sizeof(num_players), 0);
    // receive port from client
    receive_message(fd[i], &port[i], sizeof(port[i]), MSG_WAITALL);

    cout << "Player " << i << " is ready to play " << endl;
  }
}

/**
 * This function sends each player the IP address and port number of the next player in the ring
 * so that they can pass the potato to them.
 * @param num_players number of players in the game
 * @param ip IP addresses for each player
 * @param port port numbers for each player
 * @param fd file descriptors for each player
 */
void sendNextPlayerInfo(int num_players,
                        const vector<string> & ip,
                        const vector<int> & port,
                        const vector<int> & fd) {
  for (int i = 0; i < num_players; i++) {
    int nextID = (i == num_players - 1) ? 0 : i + 1;
    int nextPort = port[nextID];
    char nextIP[NextIPSize];
    memset(nextIP, 0, sizeof(nextIP));
    strcpy(nextIP, ip[nextID].c_str());

    // send next player ip to client
    send_message(fd[i], &nextIP, sizeof(nextIP), 0);
    // send next player port to client
    send_message(fd[i], &nextPort, sizeof(nextPort), 0);
  }
}

/**
 * This function sends the potato to a random player and waits for it to be passed around the ring
 * until it reaches the designated hop count.
 * @param ringmaster a Server object representing the ringmaster
 * @param num_players number of players in the game
 * @param fd  file descriptors for each player
 * @param potato Potato object that will be passed
 */
void sendAndReceivePotato(Server & ringmaster,
                          int num_players,
                          const vector<int> & fd,
                          Potato & potato) {
  srand((unsigned int)time(NULL) + num_players);
  int random = rand() % num_players;

  cout << "Ready to start the game, sending potato to player " << random << endl;

  //On success, these calls return the number of bytes sent.
  int sd = send_message(fd[random], &potato, sizeof(potato), 0);

  fd_set readfds;
  FD_ZERO(&readfds);
  for (int i = 0; i < num_players; i++) {
    FD_SET(fd[i], &readfds);
  }

  select(ringmaster.client_connection_fd + 1, &readfds, NULL, NULL, NULL);
  for (int i = 0; i < num_players; i++) {
    if (FD_ISSET(fd[i], &readfds)) {
      int rec = receive_message(fd[i], &potato, sizeof(potato), MSG_WAITALL);
      break;
    }
  }
}

void shutDownGame(int num_players, const vector<int> & fd, const Potato & potato) {
  for (int i = 0; i < num_players; i++) {
    int sd = send_message(fd[i], &potato, sizeof(potato), 0);
  }
}

int main(int argc, char * argv[]) {
  if (argc != 4) {
    cout << "The server should be invoked as: " << endl;
    cout << "ringmaster<port_num> <num_players> <num_hops>" << endl;
    cout << "example: ./ringmaster 1234 3 100" << endl;
    return 1;
  }

  int num_players = atoi(argv[2]);
  int num_hops = atoi(argv[3]);

  vector<int> fd;
  vector<string> ip(num_players);
  vector<int> port(num_players);

  cout << "Potato Ringmaster" << endl;
  cout << "Players = " << num_players << endl;
  cout << "Hops = " << num_hops << endl;

  Server ringmaster;
  ringmaster.initStatus(NULL, argv[1]);
  ringmaster.createSocket();

  //Establish N network socket connections with N number of players
  establishConnections(ringmaster, num_players, fd, ip, port);

  //and provide relevant information to each player
  sendNextPlayerInfo(num_players, ip, port, fd);

  //Create a “potato” object
  Potato potato;
  potato.num_hops = num_hops;

  //Randomly select a player and send the
  //“potato” to the selected player
  if (num_hops > 0) {
    sendAndReceivePotato(ringmaster, num_players, fd, potato);
  }
  //Shut the game down by sending a message to each player
  shutDownGame(num_players, fd, potato);

  //print a trace of the potato to the screen
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
