/*
 * MysqlRow.h
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#ifndef MYSQLROW_H_
#define MYSQLROW_H_

namespace ur {

class MysqlRow {
private:
  MYSQL_RES *m_Result;
  MYSQL_ROW m_Fields;

public:
  MysqlRow(MYSQL_RES *res, MYSQL_ROW row);
  virtual ~MysqlRow();

  std::string getField(int idx);
  uint32_t getFieldCount(void);
  bool is_closed(void);
  void close(void);
};

} /* namespace ur */

#endif /* MYSQLROW_H_ */
