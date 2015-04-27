/*
 * MysqlResult.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */
#include <stdint.h>
#include <string>
#include <mysql/mysql.h>

#include "MysqlRow.h"
#include "MysqlResult.h"

namespace ur {

MysqlResult::MysqlResult(MYSQL_RES *r) {
  // TODO Auto-generated constructor stub
  m_uiNumRows = 0;
  m_Result = r;
  m_CurrentRow = (MysqlRow *)NULL;
}

MysqlResult::~MysqlResult() {
  // TODO Auto-generated destructor stub
}

void MysqlResult::close(void)
{
  if ( m_Result == (MYSQL_RES *)NULL )
    return;

  mysql_free_result(m_Result);
  m_Result = (MYSQL_RES *)NULL;
}

MysqlRow *MysqlResult::getCurrentRow(void)
{
  if ( m_Result == (MYSQL_RES *)NULL )
  {
    throw "Result set is closed.";
  }

  return m_CurrentRow;
}

uint32_t MysqlResult::getRowCount(void)
{
  if ( m_Result == (MYSQL_RES *)NULL )
  {
    throw "Result set is closed.";
  }

  m_uiNumRows = mysql_num_rows(m_Result);
  return m_uiNumRows;
}

uint32_t MysqlResult::next(void)
{
  MYSQL_ROW row;

  if ( m_Result ==  (MYSQL_RES *)NULL )
    throw "Result set is closed.";

  if ( (row = mysql_fetch_row(m_Result)) == NULL )
  {
    m_CurrentRow = (MysqlRow *)NULL;
    return 0;
  }
  else
  {
    m_CurrentRow = new MysqlRow(m_Result, row);
    return 1;
  }

}

} /* namespace ur */
