/*
 * DebianPackage.h
 *
 *  Created on: Feb 26, 2015
 *      Author: kurt
 */

#ifndef DEBIANPACKAGE_H_
#define DEBIANPACKAGE_H_

#include <time.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>

#include "PackageControl.h"
#include "PackageXML.h"

namespace ur {

const unsigned int RELATION_DEPENDS = 				0x01;
const unsigned int RELATION_RECOMMENDS =			0x02;
const unsigned int RELATION_SUGGESTS = 				0x03;
const unsigned int RELATION_PRE_DEPENDS = 			0x04;
const unsigned int RELATION_BUILD_DEPENDS =			0x05;
const unsigned int RELATION_BUILD_DEPENDS_INDEP = 	0x06;
const unsigned int RELATION_BUILD_USING =			0x07;
const unsigned int RELATION_ENHANCES =				0x08;
const unsigned int RELATION_BREAKS =				0x09;
const unsigned int RELATION_CONFLICTS =				0x0A;
const unsigned int RELATION_BUILD_CONFLICTS =		0x0B;
const unsigned int RELATION_BUILD_CONFLICTS_INDEP =	0x0C;
const unsigned int RELATION_PROVIDES =				0x0D;
const unsigned int RELATION_REPLACES = 				0x0E;

class DebianPackage {
private:
	bool m_bIsRos;
	unsigned int m_uiInstallSize;
	std::string m_sSection;
	std::string m_sArchitecture;
	std::string m_sDebianType;
	std::string m_sROSVersion;
	std::string m_sPriority;
	boost::filesystem::path m_pathInstallPrefix;
	boost::filesystem::path m_pathStageDir;
	boost::filesystem::path m_pathStageBase;
	boost::filesystem::path m_pathDebianLocation;
	ur::vTransfer m_vFiles;
	std::string m_sProgPackageName;
	std::string m_sDebianPackageName;
	std::string m_sDebianPackageFileName;
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

    void setDebianPackageName(void);
    void outputRelationVector(const unsigned int reltype,
    		                  ur::vRelation &rel,
							  std::fstream &f);

public:
	DebianPackage(ur::PackageControl &pc,
			      ur::PackageXML &px,
				  std::string &arch,
				  std::string &sect,
				  std::string &type,
				  std::string &location);
	virtual ~DebianPackage();

	void stageManifest(void);
	void writeDebianControleFile(void);
	void createStageDirectory(void);
	void deleteStaging(void);
	void generateChecksums(void);
	void generateDebianPackage(void);
	void moveDebianPackage(void);
};

} /* namespace ur */

#endif /* DEBIANPACKAGE_H_ */
