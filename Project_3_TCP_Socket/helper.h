#ifndef __HELPER_H__
#define __HELPER_H__
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "potato.h"
#include "server.h"

#define NextIPSize 100
#define NumFDs 3

// Sends a message to a socket and checks for errors
int send_message(int socket_fd, const void * message, size_t length, int flags) {
  int send_result = send(socket_fd, message, length, flags);
  if (send_result == -1) {
    cerr << "Error: Failed to send message to socket" << endl;
  }
  return send_result;
}

// Receives a message from a socket and checks for errors
int receive_message(int socket_fd, void * buffer, size_t length, int flags) {
  int receive_result = recv(socket_fd, buffer, length, flags);
  if (receive_result == -1) {
    cerr << "Error: Failed to receive message from socket" << endl;
  }
  return receive_result;
}

#endif