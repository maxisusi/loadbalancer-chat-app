#include <cassert>
#include <cstring>
#include <format>
#include <sqldriver.hpp>
#include <sqlite3.h>
#include <string>

using namespace std;

void SqlDriver::init() {
  sqlite3_open(_name, &_db_instance);

  if (_db_instance == NULL)
    throw SqlDriverError(sqlite3_errmsg(_db_instance));
}

// TODO: overlord this function with a size arg
// in order to pass it to sqlite3_prepare_v2
sqlite3_stmt *SqlDriver::stage(const char *statement) {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(_db_instance, statement, -1, &stmt, nullptr);

  if (stmt == NULL) {
    sqlite3_finalize(stmt); /** Clean up bad stmt */
    throw SqlDriverError(format("The staging statement provided is incorrect, "
                                "the previous statement was probably not "
                                "flushed or the syntax is incorrect\nGot={}",
                                statement)
                             .c_str());
  }

  return stmt;
}

void SqlDriver::run(const char *query) {
  auto stmt = stage(query);
  dispatch_query(stmt);
}

void SqlDriver::dispatch_query(sqlite3_stmt *statement) {
  while (int step = sqlite3_step(statement) != SQLITE_DONE) {

    if (sqlite3_errcode(_db_instance) < 0) {
      throw SqlDriverError(
          format("An error occured while stepping over tables\nGot={}",
                 sqlite3_errmsg(_db_instance))
              .c_str());
    };

    // > Note: could be useful for displaying row informations

    // for (int i = 0; i < sqlite3_column_count(statement); ++i) {
    //   switch (sqlite3_column_type(statement, i)) {
    //
    //   case SQLITE_TEXT: {
    //     sqlite3_column_text(statement, i);
    //     break;
    //   }
    //   case SQLITE_INTEGER: {
    //     sqlite3_column_int(statement, i);
    //     break;
    //   }
    //   case SQLITE_FLOAT: {
    //     sqlite3_column_double(statement, i);
    //     break;
    //   }
    //   default: {
    //     throw SqlDriverError("Row type not implemented yet");
    //   }
    //   }
    // }
  }

  if (sqlite3_finalize(statement) > 0) {
    throw SqlDriverError(
        format("An error occured while clearing the statement\nGot={}",
               sqlite3_errmsg(_db_instance))
            .c_str());
  };
}

void SqlDriver::close() {
  if (sqlite3_close(_db_instance)) {
    throw SqlDriverError(
        format("An error occured while clearing the statment\nGot={}",
               sqlite3_errmsg(_db_instance))
            .c_str());
  }
}
