/*
 * MysqlConnection.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#include <stdint.h>
#include <string>
#include <sstream>
#include <mysql/mysql.h>

#include "MysqlResult.h"
#include "MysqlConnection.h"

namespace ur {

MysqlConnection::MysqlConnection(std::string host, std::string db, uint32_t port)
{
  // TODO Auto-generated constructor stub
  m_uiAffectedRows = 0;
  conn = (MYSQL *)NULL;

  connect(host, db, (std::string) NULL, (std::string)NULL, port);
}

MysqlConnection::MysqlConnection(std::string host, std::string db, std::string user,
                                 std::string pass, uint32_t port)
{
  // TODO Auto-generated constructor stub
  m_uiAffectedRows = 0;
  conn = (MYSQL *) NULL;

  connect(host, db, user, pass, port);
}

MysqlConnection::~MysqlConnection()
{
  // TODO Auto-generated destructor stub
  if ( is_connected() )
    close();
}

void MysqlConnection::close(void)
{
  if ( !is_connected() )
    return;

  mysql_close(conn);
  conn = (MYSQL *)NULL;
}

void MysqlConnection::connect(std::string host, std::string db, std::string user,
                              std::string pass, uint32_t port)
{
  if ( is_connected() )
    throw "The Connection to the database has already been established.";

  mysql_init(&mysql);
  conn = mysql_real_connect(&mysql, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), port, (const char *)NULL, (uint32_t) 0);

  if ( !is_connected() )
    throw getError();
}

uint32_t MysqlConnection::affectedRows(void)
{
  return m_uiAffectedRows;
}

std::string MysqlConnection::getError(void)
{
  std::string res;

  if ( is_connected() )
    res = mysql_error(conn);
  else
    res = mysql_error(&mysql);
  return res;
}

bool MysqlConnection::is_connected(void)
{
  return !(!conn);
}

ur::MysqlResult *MysqlConnection::query(std::string sql)
{
  int state;
  MYSQL_RES *resultset;

  if ( !is_connected() )
    throw "No connection to the database.";

  state = mysql_query(conn, sql.c_str());

  if ( state != 0 )
    throw getError();

  // either an update or an error occurred
  resultset = mysql_store_result(conn);
  if ( resultset == (MYSQL_RES *)NULL )
    throw getError();

  return new MysqlResult(resultset);
}


} /* namespace ur */
