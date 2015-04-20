/*
 * PackageControl.cpp
 *
 *  Created on: Feb 26, 2015
 *      Author: kurt
 */

#include "PackageControl.h"

namespace ur {

PackageControl::PackageControl(std::string &cf) {
	// TODO Auto-generated constructor stub
	m_bIsRos = false;
	m_uiInstallSize = 0;
	m_pathControlFile = boost::filesystem::path(cf.c_str());
}

PackageControl::~PackageControl() {
	// TODO Auto-generated destructor stub
}

bool PackageControl::is_ros(void) {
	return m_bIsRos;
}

unsigned int PackageControl::getInstallSize(void) {
	return m_uiInstallSize;
}

std::string PackageControl::getROSVersion(void) {
	return m_sROSVersion;
}

std::string PackageControl::getPriority(void) {
	return m_sPriority;
}

std::string PackageControl::getProgPackageName(void) {
	return m_sProgPackageName;
}

boost::filesystem::path PackageControl::getInstallPrefix(void) {
	return m_pathInstallPrefix;
}

boost::filesystem::path PackageControl::getPackageXML(void) {
	return m_pathPackageXML;
}

boost::filesystem::path  PackageControl::getStageBase(void) {
	return m_pathStageBase;
}

ur::vTransfer PackageControl::getFiles(void) {
	return m_vFiles;
}

void PackageControl::setStageBase(std::string &sb) {
		boost::filesystem::path idir, sdir;
		idir = boost::filesystem::initial_path();

		//test to see if stagebase was set in the control file.  This would
		//take precedence over command line or configuration file.
		if (m_pathStageBase.empty()) { // no stage base in the control file
			if (sb.compare("unique_path") == 0) { // No stage directory set at all
				boost::filesystem::current_path("/tmp");
			} else { // stage base set from command line or configuration
				boost::filesystem::current_path(sb);
			}
		} else {  // got stage bse out of the control file
			boost::filesystem::current_path(m_pathStageBase);
		}
		sdir = boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");
		boost::filesystem::create_directory(sdir);
		boost::filesystem::permissions(sdir, (boost::filesystem::perms) 0755);

		sdir = boost::filesystem::absolute(sdir);
		boost::filesystem::current_path(idir);
		m_pathStageBase = sdir;

		boost::filesystem::current_path(idir);
}

void PackageControl::readControlFile(void) {
	boost::property_tree::ptree pt;
	std::string s;

    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    boost::property_tree::read_xml( m_pathControlFile.c_str(), pt );

    if (pt.get_optional<std::string>("package.ros")) {
    	m_bIsRos = true;
	    m_sROSVersion = pt.get<std::string>("package.ros");
	    resolveString(m_sROSVersion);
    }

    if (pt.get_optional<std::string>("package.name")) {
    	m_sProgPackageName = pt.get<std::string>("package.name");
    	resolveString(m_sProgPackageName);
    }

    if (pt.get_optional<std::string>("package.install_prefix")) {
    	s = pt.get<std::string>("package.install_prefix");
    	resolveString(s);
       	m_pathInstallPrefix = boost::filesystem::path(s.c_str());
    }

    if (pt.get_optional<std::string>("package.packagexml")) {
    	s = pt.get<std::string>("package.packagexml");
    	resolveString(s);
       	m_pathPackageXML = boost::filesystem::path(s.c_str());
    }

    if (pt.get_optional<std::string>("package.priority")) {
    	m_sPriority = pt.get<std::string>("package.priority");
    } else {
    	m_sPriority.assign("optional");
    }

    if (pt.get_optional<std::string>("package.stagebase")) {
    	s = pt.get<std::string>("package.stagebase");
    	resolveString(s);
       	m_pathStageBase = boost::filesystem::path(s.c_str());
    }

    if (pt.get_optional<std::string>("package.files")) {
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
            pt.get_child("package.files")) {
    	ur::transfer ft;
    	ft.sp = v.second.get<std::string>("source_path");
    	resolveString(ft.sp);

    	ft.tp = v.second.get<std::string>("target_path");
    	resolveString(ft.tp);

    	ft.fsize = 0;
    	m_vFiles.push_back(ft);
        }
    }
}

void PackageControl::resolveString(std::string &s) {
	std::string key;
	char *val;
	std::size_t bpos, epos;
	const char * begin_delim = "${";
	const char * end_delim = "}";

	if ( s.find(begin_delim, 0, 2) == std::string::npos )
		return;// did not find any environment variables

	bpos = 0;
	while ((bpos = s.find(begin_delim, bpos, 2)) != std::string::npos) {
		epos = s.find(end_delim, bpos+1, 1);
		key = s.substr(bpos + 2, epos - bpos - 2);
		if ( (val = std::getenv(key.c_str())) == NULL ) // no environment set
			s.replace(bpos + 1, epos - bpos + 1, "");
		else
			s.replace(bpos, epos - bpos + 1, val);
		bpos = epos;
	}
	return;
}

void PackageControl::debugDump(void) {

}

} /* namespace ur */
