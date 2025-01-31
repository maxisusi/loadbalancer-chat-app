#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <sched.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define DB_NAME "db_test.db"
#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

enum LogLevel { INFO, WARNING, ERROR, CRITICAL };

class Logger {
  bool disable = false;

public:
  Logger() {};

public:
  void log(LogLevel loglvl, std::string message) {

    auto timestamp = time(NULL);
    struct tm date_time = *localtime(&timestamp);
    char date[50];
    strftime(date, 50, "[%I:%M:%S]", &date_time);

    switch (loglvl) {
    case LogLevel::CRITICAL:
      std::cout << "[CRITICAL]" << date << " " << message << std::endl;
      break;

    case LogLevel::ERROR:
      std::cout << "[ERROR]" << date << " " << message << std::endl;
      break;

    case LogLevel::INFO:
      std::cout << "[INFO]" << date << " " << message << std::endl;
      break;

    case LogLevel::WARNING:
      std::cout << "[WARNING]" << date << " " << message << std::endl;
      break;
    };
  }
};

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
    Logger log = *new Logger();

    setup_socket();
    if (bind(server_socket, (struct sockaddr *)&sock_add,
             sizeof(this->sock_add)) == -1) {
      log.log(LogLevel::CRITICAL, "Socket couln't bind to port");
      handle_error("bind");
    }

    if (listen(server_socket, 5) == -1) {
      log.log(LogLevel::CRITICAL, "Socket was unable to listen");
      handle_error("listen");
    }

    char port_info[40];
    sprintf(port_info, "Listening on port %d", port);
    log.log(LogLevel::INFO, port_info);

    set_status(NodeStatus::RUNNING);
    while (status == NodeStatus::RUNNING) {

      int client_socket = accept(server_socket, nullptr, nullptr);
      char buffer[1024] = {0};

      if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
        log.log(LogLevel::CRITICAL,
                "Socket was unable to read client data transfer");
        handle_error("recv");
      }

      log.log(LogLevel::INFO, buffer);

      // Send the process id back to the client
      char f_pid[16];
      snprintf(f_pid, sizeof(f_pid), "PONG - %d", getpid());

      if (write(client_socket, f_pid, strlen(f_pid)) == -1) {
        log.log(LogLevel::CRITICAL,
                "Socket was unable to write to client file descriptor");
        handle_error("write");
      }

      close(client_socket);
    }

    close(server_socket);
  }
};

int main() {

  int process_port[] = {7878, 6969, 2929, 3030};
  int process_port_len = sizeof(process_port) / sizeof(int);

  Logger log = *new Logger();
  log.log(LogLevel::INFO, "Opening up sqlite connection...");

  sqlite3 *db;
  sqlite3_open(DB_NAME, &db);

  if (db == NULL) {
    log.log(LogLevel::CRITICAL, "Process couln't open sqlite file descriptor");
    handle_error("sqlite3_open");
  }

  const char *create = "CREATE TABLE Persons (PersonID INT);";
  const char *insert = "INSERT INTO Persons (PersonID) VALUES (1);";
  const char *get_all = "SELECT * FROM Persons;";

  sqlite3_stmt *stmt;
  int prep = sqlite3_prepare_v2(db, get_all, -1, &stmt, nullptr);

  if (stmt == NULL) {
    log.log(LogLevel::CRITICAL, "Process couln't compile SQL statement");
    handle_error("sqlite3_prepare_v2");
  }

  while (sqlite3_step(stmt) != SQLITE_DONE) {
    int nums_col = sqlite3_column_count(stmt);

    for (int i = 0; i < nums_col; ++i) {

      switch (sqlite3_column_type(stmt, i)) {
      case SQLITE_TEXT: {
        std::string column_text = std::string(
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
        log.log(LogLevel::INFO, column_text);
        break;
      }
      case SQLITE_INTEGER: {
        auto column = std::to_string(sqlite3_column_int(stmt, i));
        log.log(LogLevel::INFO, column);
        break;
      }

      case SQLITE_FLOAT: {
        auto column = sqlite3_column_double(stmt, i);
        log.log(LogLevel::INFO, std::to_string(column));
        break;
      }
      default: {
        log.log(LogLevel::WARNING, "Couldn't find correspondig time table");
      }
      }
    }
  }

  sqlite3_finalize(stmt);

  log.log(LogLevel::INFO, "Success executing SQLite query");
  sqlite3_close(db);

  // if (result == (SQLITE_MISUSE | SQLITE_ERROR)) {
  //   log.log(LogLevel::CRITICAL, "Process couln't step SQL statement
  //   (create)"); handle_error("sqlite3_prepare_v2");
  // }
  //
  // if (result == SQLITE_DONE) {
  //   log.log(LogLevel::INFO, "SQL sucessfully processed");
  // }
  //
  // sqlite3_reset(fn_create_stmt);

  // const char *insert_stmt = "INSERT INTO Persons ('1', 'Balej', 'Max', 'Rue
  // de "
  //                           "Gottefrey 38', 'Saxon')";
  //
  // sqlite3_stmt *fn_insert_stmt;
  // sqlite3_prepare_v2(db, insert_stmt, -1, &fn_insert_stmt, nullptr);
  //
  // if (fn_insert_stmt == NULL) {
  //   log.log(LogLevel::CRITICAL,
  //           "Process couln't compile SQL statement (insert)");
  //   handle_error("sqlite3_prepare_v2");
  // }
  //
  // auto result_insert = sqlite3_step(fn_insert_stmt);
  // if (result_insert == SQLITE_MISUSE) {
  //   log.log(LogLevel::CRITICAL, "Process couln't step SQL statement
  //   (insert)"); handle_error("sqlite3_prepare_v2");
  // }
  //
  // if (result_insert == SQLITE_DONE) {
  //   log.log(LogLevel::INFO, "SQL sucessfully processed");
  // }
  //
  // sqlite3_reset(fn_insert_stmt);

  // log.log(LogLevel::INFO, "Instantiate servers");

  // for (int i = 0; i < process_port_len; ++i) {
  //   pid_t pid = fork();
  //
  //   if (pid == -1) {
  //     log.log(LogLevel::CRITICAL, "Couldn't instantiate another
  //     process"); handle_error("fork");
  //   }
  //
  //   if (pid == 0) {
  //     Node *server = new Node(process_port[i]);
  //     server->run();
  //   }
  // }
  //
  // for (int i = 0; i < process_port_len; ++i) {
  //   wait(NULL);
  // }

  return 0;
}
