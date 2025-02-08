#include "logger.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <sched.h>
#include <sqldriver.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define DB_NAME "db_test.db"

int main() {

  Logger log = *new Logger();

  log.log(LogLevel::INFO, "Opening up sqlite connection");
  SqlDriver sql_instance = *new SqlDriver(DB_NAME);

  sql_instance.init();

  log.log(LogLevel::INFO, "Create Table if it doesn't exist");
  auto create_stmt = sql_instance.stage(
      "CREATE TABLE IF NOT EXISTS Person (name varchar(255))");
  if (sql_instance.run_query(create_stmt) == SQLD_RUN_ERROR) {
    handle_error("run_query");
  }

  log.log(LogLevel::INFO, "Insert into table");
  auto insert_stmt =
      sql_instance.stage("INSERT INTO Person (name) VALUES ('My name is max')");

  if (sql_instance.run_query(insert_stmt) == SQLD_RUN_ERROR) {
    handle_error("run_query");
  }

  log.log(LogLevel::INFO, "Display the table");
  auto select_stmt = sql_instance.stage("SELECT * FROM Person");

  if (sql_instance.run_query(select_stmt) == SQLD_RUN_ERROR) {
    handle_error("run_query");
  }

  sql_instance.close();

  return 0;
}
