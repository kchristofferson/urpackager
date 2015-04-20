/*
 * PackageControl.h
 *
 *  Created on: Feb 26, 2015
 *      Author: kurt
 */

#ifndef PACKAGECONTROL_H_
#define PACKAGECONTROL_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/foreach.hpp>

namespace ur {

struct transfer {
	std::string sp;
	std::string tp;
	unsigned int fsize;
};

typedef std::vector<ur::transfer> vTransfer;

class PackageControl {
private:
	// obtained from the control file
	bool m_bIsRos;
	unsigned int m_uiInstallSize;
	std::string m_sROSVersion;
	std::string m_sPriority;
	std::string m_sProgPackageName;
	boost::filesystem::path m_pathControlFile;
	boost::filesystem::path m_pathInstallPrefix;
	boost::filesystem::path m_pathPackageXML;
	boost::filesystem::path m_pathStageBase;
	ur::vTransfer m_vFiles;
    void resolveString(std::string &s);

public:
	PackageControl(std::string &cf);
	virtual ~PackageControl();

	bool is_ros(void);
	unsigned int getInstallSize(void);
	std::string getROSVersion(void);
	std::string getPriority(void);
	std::string getProgPackageName(void);
	boost::filesystem::path getInstallPrefix(void);
	boost::filesystem::path getPackageXML(void);
	boost::filesystem::path getStageBase(void);
	ur::vTransfer getFiles(void);

	/*setStageBase()
	 * 	This function MUST be called in order to set the stage base directory.
	 * 	Otherwise the member m_pathStageBase will be empty and the different
	 * 	consuming packagers will throw errors.
	 *
	 *  This function SHOULD be called after readControlFile is called.
	 *
	 * 	Takes a string pointing to the directory where you want to stage
	 * 	all the files needed to build a debian.  The string can be empty, but
	 * 	if it is, then the stage base MUST be set in the control file AND
	 * 	the control file must be read prior to calling this function.  The
	 * 	passed parameter takes precedence over what is in the control file if
	 * 	both are set.
	 */
	void setStageBase(std::string &sb);

	/*readControlFile()
	 * Call this function first after instantiating a PackageControl
	 *
	 * Function parses the ubiquity robots control file for the program
	 * to be packaged.
	 */
	void readControlFile(void);
	void debugDump(void);
};

} /* namespace ur */

#endif /* PACKAGECONTROL_H_ */
