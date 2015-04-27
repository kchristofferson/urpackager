/*
 * MysqlResult.h
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#ifndef MYSQLRESULT_H_
#define MYSQLRESULT_H_

#include "MysqlRow.h"

namespace ur {

class MysqlResult {
private:
  uint32_t m_uiNumRows;
  MYSQL_RES *m_Result;
  MysqlRow *m_CurrentRow;

public:
  MysqlResult(MYSQL_RES *r);
  virtual ~MysqlResult();

  void close(void);
  MysqlRow *getCurrentRow(void);
  uint32_t getRowCount(void);
  uint32_t next(void);
};

} /* namespace ur */

#endif /* MYSQLRESULT_H_ */
