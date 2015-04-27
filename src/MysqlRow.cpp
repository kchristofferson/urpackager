/*
 * MysqlRow.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#include <sys/types.h>
#include <stdint.h>
#include <string>
#include <mysql/mysql.h>

#include "MysqlRow.h"

namespace ur {

MysqlRow::MysqlRow(MYSQL_RES *res, MYSQL_ROW row) {
  // TODO Auto-generated constructor stub
  m_Result = res;
  m_Fields = row;
}

MysqlRow::~MysqlRow() {
  // TODO Auto-generated destructor stub
  if ( !is_closed() )
    close();
}

std::string MysqlRow::getField(int idx)
{
  if ( is_closed() )
    throw "Row is closed.";

  if ( idx < 1 || idx > getFieldCount() )
    throw "Field index is out of bounds.";

  std::string ret = m_Fields[idx - 1];
  return ret;
}

uint32_t MysqlRow::getFieldCount(void)
{
  if ( is_closed() )
    throw "Row is closed.";

  return mysql_num_fields(m_Result);
}

bool MysqlRow::is_closed(void)
{
  return ( m_Fields == (MYSQL_ROW)NULL );
}

void MysqlRow::close(void)
{
  if ( is_closed() )
    throw "Row is closed.";

  m_Fields = (MYSQL_ROW)NULL;
  m_Result = (MYSQL_RES *)NULL;
}


} /* namespace ur */
