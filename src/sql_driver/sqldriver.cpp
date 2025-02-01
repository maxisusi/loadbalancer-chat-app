#include "sqldriver.h"
#include "../logger/logger.h"
#include <cstring>
#include <sqlite3.h>
#include <string>

SqlDriver::SqlDriver(std::string db_n) { db_name = db_n; };

int SqlDriver::init() {

  char buffer[50];
  strcpy(buffer, db_name.c_str());
  sqlite3_open(buffer, &db_instance);

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
sqlite3_stmt *SqlDriver::stage(const char *statement) {

  sqlite3_stmt *stmt;

  int prep = sqlite3_prepare_v2(db_instance, statement, -1, &stmt, nullptr);
  if (stmt == NULL) {
    return nullptr;
  }

  return stmt;
}

int SqlDriver::run_query(sqlite3_stmt *statement) {
  Logger log = *new Logger();

  while (int step = sqlite3_step(statement) != SQLITE_DONE) {

    if (sqlite3_errcode(db_instance) == SQLITE_ERROR) {
      log.log(LogLevel::INFO, sqlite3_errmsg(db_instance));
      return SQLD_RUN_ERROR;
    }

    int nums_col = sqlite3_column_count(statement);

    for (int i = 0; i < nums_col; ++i) {
      switch (sqlite3_column_type(statement, i)) {
      case SQLITE_TEXT: {
        std::string column_text = std::string(
            reinterpret_cast<const char *>(sqlite3_column_text(statement, i)));
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
        log.log(LogLevel::WARNING, "Unhandled table type");
        return SQLD_ERROR;
      }
      }
    }
  }

  sqlite3_finalize(statement);

  return 0;
}
void SqlDriver::close() {
  sqlite3_close(db_instance); // TODO: Handle error
}
