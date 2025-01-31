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

class SqlDriver {

  std::string db_name;
  sqlite3 *db_instance;

public:
  SqlDriver(std::string db_n) { db_name = db_n; };

public:
  int init() {
    sqlite3_open(DB_NAME, &db_instance);

    if (db_instance == NULL) {
      {
        // log.log(LogLevel::CRITICAL,
        //         "Process couln't open sqlite file descriptor");
        return -1;
        // TODO: Refacto error message with either enum or macros
      }
    }
    return 0;
  }

public:
  sqlite3_stmt *stage(const char *statement) {

    sqlite3_stmt *stmt;

    int prep = sqlite3_prepare_v2(db_instance, statement, -1, &stmt, nullptr);
    if (stmt == NULL) {
      return nullptr;
      // TODO: write to std error the message
      // log.log(LogLevel::CRITICAL, "Process couln't compile SQL statement");
      // handle_error("sqlite3_prepare_v2");
    }

    return stmt;
  }

public:
  int run_query(sqlite3_stmt *statement) {

    Logger log = *new Logger();
    while (sqlite3_step(statement) != SQLITE_DONE) {
      int nums_col = sqlite3_column_count(statement);

      for (int i = 0; i < nums_col; ++i) {
        switch (sqlite3_column_type(statement, i)) {
        case SQLITE_TEXT: {
          std::string column_text = std::string(reinterpret_cast<const char *>(
              sqlite3_column_text(statement, i)));
          log.log(LogLevel::INFO, column_text);
          break;
        }
        case SQLITE_INTEGER: {
          auto column = std::to_string(sqlite3_column_int(statement, i));
          log.log(LogLevel::INFO, column);
          break;
        }

        case SQLITE_FLOAT: {
          auto column = sqlite3_column_double(statement, i);
          log.log(LogLevel::INFO, std::to_string(column));
          break;
        }
        default: {
          // TODO: Better error management
          return -1;
          log.log(LogLevel::WARNING, "Couldn't find correspondig time table");
        }
        }
      }
    }

    sqlite3_finalize(statement);

    return 1;
  }

public:
  void close() {
    sqlite3_close(db_instance); // TODO: Handle error }
  }
};

int main() {

  Logger log = *new Logger();
  log.log(LogLevel::INFO, "Opening up sqlite connection...");

  SqlDriver sql_instance = *new SqlDriver(DB_NAME);

  sql_instance.init();
  auto stmt = sql_instance.stage("SELECT * FROM Persons;");
  sql_instance.run_query(stmt);
  sql_instance.close();

  return 0;
}
