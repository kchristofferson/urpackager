/*
 * MysqlConnection.h
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#ifndef MYSQLCONNECTION_H_
#define MYSQLCONNECTION_H_

#include <mysql/mysql.h>
#include "MysqlResult.h"

namespace ur {

class MysqlConnection {
private:
  uint32_t m_uiAffectedRows;
  MYSQL mysql;
  MYSQL *conn;

public:
  MysqlConnection(std::string host, std::string db, uint32_t port);
  MysqlConnection(std::string host, std::string db, std::string user, std::string pass, uint32_t port);
  virtual ~MysqlConnection();

  void close(void);
  void connect(std::string host, std::string db, std::string user, std::string pass, uint32_t port);
  uint32_t affectedRows(void);
  std::string getError(void);
  bool is_connected(void);
  ur::MysqlResult *query(std::string stmt);
};

} /* namespace ur */

#endif /* MYSQLCONNECTION_H_ */
