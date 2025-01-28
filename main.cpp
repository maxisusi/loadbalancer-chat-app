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

#define PORT 9500

int main() {

  int opt = 1;

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

  sockaddr_in server_address = {
      .sin_family = AF_INET,
      .sin_port = htons(PORT),
  };

  server_address.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1)
    handle_error("bind");

  if (listen(server_socket, 5) == -1)
    handle_error("listen");

  std::cout << "Listening on port: " << PORT << std::endl;

  while (true) {

    int client_socket = accept(server_socket, nullptr, nullptr);
    char buffer[1024] = {0};

    recv(client_socket, buffer, sizeof(buffer), 0);
    std::cout << "Message from client: " << buffer << std::endl;

    char response[] = "PONG";
    if (write(client_socket, response, sizeof(response)) == -1)
      handle_error("write");

    close(client_socket);
  }

  close(server_socket);

  return 0;
}
