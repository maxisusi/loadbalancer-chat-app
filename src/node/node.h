#include <netinet/in.h>

enum NodeStatus { STOP, RUNNING, RESTARTING };
class Node {

  int port;
  int server_socket;
  sockaddr_in sock_add;

  NodeStatus status;

public:
  Node(int port);

private:
  void setup_socket();

private:
  void set_status(NodeStatus stat);

public:
  void run();
};
