/*
 * MysqlConnection.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include <mysql/mysql.h>

#include "MysqlResult.h"
#include "MysqlConnection.h"

namespace ur {

MysqlConnection::MysqlConnection(void)
{
  // TODO Auto-generated constructor stub
  m_uiAffectedRows = 0;
  m_conn = (MYSQL *)NULL;

//  connect(host, db, (std::string) NULL, (std::string)NULL, port);
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

  mysql_close(m_conn);
  m_conn = (MYSQL *)NULL;
}

void MysqlConnection::connect(std::string host, std::string db, std::string user,
                              std::string pass, uint32_t port)
{
  if ( is_connected() )
    throw "The Connection to the database has already been established.";

  mysql_init(&m_mysql);
  m_conn = mysql_real_connect(&m_mysql, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), port, (const char *)NULL, (uint32_t) 0);

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
    res = mysql_error(m_conn);
  else
    res = mysql_error(&m_mysql);
  return res;
}

bool MysqlConnection::is_connected(void)
{
  return !(!m_conn);
}

ur::MysqlResult *MysqlConnection::query(std::string sql)
{
  int state;
  MYSQL_RES *resultset;

  if ( !is_connected() )
    throw "No connection to the database.";

  state = mysql_query(m_conn, sql.c_str());

  if ( state != 0 )
    throw getError();

  // either an update or an error occurred
  resultset = mysql_store_result(m_conn);
  if ( resultset == (MYSQL_RES *)NULL )
    throw getError();

  return new MysqlResult(resultset);
}


} /* namespace ur */
