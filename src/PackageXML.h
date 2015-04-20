/*
 * PackageXML.h
 *
 *  Created on: Feb 26, 2015
 *      Author: kurt
 */

#ifndef PACKAGEXML_H_
#define PACKAGEXML_H_

#include <string>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <vector>

namespace ur {

const unsigned int URL_WEBSITE = 	0x01;
const unsigned int URL_BUGTRACKER =	0x02;
const unsigned int URL_REPOSITORY =	0x03;
const unsigned int URL_OTHER =		0x04;

typedef std::vector<std::string> vs;

struct person {
	std::string fullname;
	std::string emailaddr;
};

typedef std::vector<ur::person> vPerson;

struct url {
	int iType;
	std::string sUrl;
};

typedef std::vector<ur::url> vUrl;

const int REL_VERSION_TYPE_NONE = 0x00;
const int REL_VERSION_TYPE_LT = 0x01;
const int REL_VERSION_TYPE_LTE = 0x02;
const int REL_VERSION_TYPE_EQ = 0x03;
const int REL_VERSION_TYPE_GTE = 0x04;
const int REL_VERSION_TYPE_GT = 0x05;

struct relation {
	int iVerType;
	std::string sVer;
	std::string sVal;
};

typedef std::vector<ur::relation> vRelation;

class PackageXML {
private:
	std::string m_sProgPackageName;
	std::string m_sVersion;
	std::string m_sDescription;
	ur::vs m_vLicense;
	ur::vPerson m_vMaintainer;
	ur::vPerson m_vAuthor;
	ur::vUrl m_vUrl;
    ur::vRelation m_vBuildtoolDepend;
    ur::vRelation m_vBuildDepend;
    ur::vRelation m_vRunDepend;
    ur::vRelation m_vTestDepend;
    ur::vRelation m_vConflict;
    ur::vRelation m_vReplace;

    void readPersons(boost::property_tree::ptree &t,
    		                std::string child,
							std::string first,
							ur::vPerson &result);
    void readRelations(boost::property_tree::ptree &t,
    		         std::string child,
					 std::string first,
					 ur::vRelation &result);
    void readUrls(boost::property_tree::ptree &t,
    		      std::string child,
				  std::string first,
				  ur::vUrl &result);

public:
	PackageXML();
	virtual ~PackageXML();

	std::string getProgPackageName(void);
	std::string getVersion(void);
	std::string getDescription(void);
	ur::vs      getLicenses(void);
	ur::vPerson getMaintainers(void);
	ur::vPerson getAuthors(void);
	ur::vUrl    getUrls(void);
    ur::vRelation getBuildtoolDependencies(void);
    ur::vRelation getBuildDependencies(void);
    ur::vRelation getRunDependencies(void);
    ur::vRelation getTestDependencies(void);
    ur::vRelation getConflicts(void);
    ur::vRelation getReplacements(void);

	void readFile(boost::filesystem::path p);
//	void writeFile(void);
//	void testFile(void);
	void debugDump(void);
};

} /* namespace ur */

#endif /* PACKAGEXML_H_ */
