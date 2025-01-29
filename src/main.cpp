#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

enum NodeStatus { STOP, RUNNING, RESTARTING };

class Node {

  int port;
  int server_socket;
  sockaddr_in sock_add;

  NodeStatus status;

public:
  Node(int port) { this->port = port; }

private:
  void setup_socket() {
    int opt = 1;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) <
        0)
      handle_error("setsockopt");

    sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(this->port),
    };
    server_address.sin_addr.s_addr = INADDR_ANY;
    this->server_socket = server_socket;
    this->sock_add = server_address;
  }

private:
  void set_status(NodeStatus stat) { status = stat; }

public:
  void run() {
    setup_socket();
    if (bind(server_socket, (struct sockaddr *)&sock_add,
             sizeof(this->sock_add)) == -1)
      handle_error("bind");

    if (listen(server_socket, 5) == -1)
      handle_error("listen");

    std::cout << "Listening on port: " << this->port << std::endl;

    set_status(NodeStatus::RUNNING);

    while (status == NodeStatus::RUNNING) {

      int client_socket = accept(server_socket, nullptr, nullptr);
      char buffer[1024] = {0};

      if (recv(client_socket, buffer, sizeof(buffer), 0) == -1)
        handle_error("recv");

      printf("Message from client: %s", buffer);

      // Send the process id back to the client
      char f_pid[16];
      snprintf(f_pid, sizeof(f_pid), "PONG - %d", getpid());

      if (write(client_socket, f_pid, strlen(f_pid)) == -1)
        handle_error("write");

      close(client_socket);
    }

    close(server_socket);
  }
};

int main() {

  int process_port[] = {7878, 6969, 2929, 3030};
  int process_port_len = sizeof(process_port) / sizeof(int);

  for (int i = 0; i < process_port_len; ++i) {
    pid_t pid = fork();

    if (pid == -1)
      handle_error("fork");

    if (pid == 0) {

      Node *server = new Node(process_port[i]);
      server->run();
    }
  }

  for (int i = 0; i < process_port_len; ++i) {
    wait(NULL);
  }

  return 0;
}
