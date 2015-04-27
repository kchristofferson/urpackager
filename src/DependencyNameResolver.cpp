/*
 * DependencyNameResolver.cpp
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#include <stdint.h>
#include <string>

#include "MysqlConnection.h"
#include "DependencyNameResolver.h"

namespace ur {

DependencyNameResolver::DependencyNameResolver(std::string host, std::string db, std::string user, std::string pass, uint32_t port) :
  m_sHost(host), m_sDB(db), m_sUser(user), m_sPassword(pass), m_uiPort(port)
{

  m_db = (MysqlConnection *)NULL;
  connectDatabase();
}

DependencyNameResolver::~DependencyNameResolver() {
  // TODO Auto-generated destructor stub

  m_db->close();
}

bool DependencyNameResolver::in_release(std::string pkg, std::string rel)
{
  bool ret;
  std::string sql;
  ur::MysqlResult *result;
  ur::MysqlRow *row;

  sql = "SELECT COUNT(ident) FROM ros_packages WHERE ros_packages.name='" + pkg
        + "' AND ros_packages.release_ident=(SELECT ident FROM ros_releases WHERE ros_releases.name='"
        + rel +"')";

  result = m_db->query(sql);

  if ( result->getRowCount() != 1 )
    throw "sql errror in in_release()";

  row = result->getCurrentRow();
  ret = row->getField(1).compare("1") == 0 ? true : false;

  delete result;
  return ret;
}

// Private functions

void DependencyNameResolver::connectDatabase(void)
{
  *m_db = MysqlConnection(m_sHost, m_sDB, m_sUser, m_sPassword, m_uiPort);
}


} /* namespace ur */
