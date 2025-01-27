#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() {

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9500);
  server_address.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1)
    handle_error("bind");

  if (listen(server_socket, 5) == -1)
    handle_error("listen");

  int client_socket = accept(server_socket, nullptr, nullptr);
  char buffer[1024] = {0};

  recv(client_socket, buffer, sizeof(buffer), 0);

  std::cout << "Message from client: " << buffer << std::endl;

  close(server_socket);

  return 0;
}
