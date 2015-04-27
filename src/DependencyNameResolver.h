/*
 * DependencyNameResolver.h
 *
 *  Created on: Apr 24, 2015
 *      Author: kurt
 */

#ifndef DEPENDENCYNAMERESOLVER_H_
#define DEPENDENCYNAMERESOLVER_H_

#include "MysqlConnection.h"

namespace ur {

class DependencyNameResolver {
private:
  std::string m_sHost;
  uint32_t m_uiPort;
  std::string m_sDB;
  std::string m_sUser;
  std::string m_sPassword;
  ur::MysqlConnection *m_db;

  void connectDatabase(void);

public:
  DependencyNameResolver(std::string host, std::string db, std::string user, std::string pass, uint32_t port);
  virtual ~DependencyNameResolver();

  bool in_release(std::string pkg, std::string rel);
};

} /* namespace ur */

#endif /* DEPENDENCYNAMERESOLVER_H_ */
