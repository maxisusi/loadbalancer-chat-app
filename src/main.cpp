#include "logger/logger.h"
#include "sql_driver/sqldriver.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <sched.h>
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
  log.log(LogLevel::INFO, "Opening up sqlite connection...");

  SqlDriver sql_instance = *new SqlDriver(DB_NAME);

  sql_instance.init();
  auto stmt = sql_instance.stage("SELECT * FROM Persons;");
  sql_instance.run_query(stmt);
  sql_instance.close();

  return 0;
}
