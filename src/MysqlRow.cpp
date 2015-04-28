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
  m_Lengths = mysql_fetch_lengths(m_Result);
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

  char *bytes = m_Fields[idx-1];
  size_t len = (size_t) m_Lengths[idx-1];
  std::string ret(reinterpret_cast<char const *>(bytes), len);
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
