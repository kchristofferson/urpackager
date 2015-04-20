/*
 * DebianPackage.cpp
 *
 *  Created on: Feb 26, 2015
 *      Author: kurt
 */

#include "DebianPackage.h"

namespace ur {

DebianPackage::DebianPackage(ur::PackageControl &pc,
		ur::PackageXML &px,
		std::string &arch,
		std::string &section,
		std::string &type,
		std::string &location) {
	// TODO Auto-generated constructor stub

	m_bIsRos = pc.is_ros();
	m_uiInstallSize = pc.getInstallSize();
	if (m_bIsRos)
		m_sROSVersion = pc.getROSVersion();
	m_sArchitecture = arch;
	m_sSection = section;
	m_sDebianType = type;
	m_pathDebianLocation = boost::filesystem::path(location);
	m_sPriority = pc.getPriority();
	m_pathInstallPrefix = pc.getInstallPrefix();
	m_pathStageBase = pc.getStageBase();
	m_vFiles = pc.getFiles();

	// Try to get program name from XML, but allow control to override
	if (!pc.getProgPackageName().empty())
		m_sProgPackageName = pc.getProgPackageName();
	else
		m_sProgPackageName = px.getProgPackageName();

	m_sVersion = px.getVersion();
	m_sDescription = px.getDescription();
	m_vLicense = px.getLicenses();
	m_vMaintainer = px.getMaintainers();
	m_vAuthor = px.getAuthors();
	m_vUrl = px.getUrls();
    m_vBuildtoolDepend = px.getBuildtoolDependencies();
    m_vBuildDepend = px.getBuildDependencies();
    m_vRunDepend = px.getRunDependencies();
    m_vTestDepend = px.getTestDependencies();
    m_vConflict = px.getConflicts();
    m_vReplace = px.getReplacements();
}

DebianPackage::~DebianPackage() {
	// TODO Auto-generated destructor stub
}

void DebianPackage::stageManifest(void) {
	boost::filesystem::path idir, wdir, debname;
	std::string s = "/"; // to make package directory path relative

	idir = boost::filesystem::initial_path();

	// make sure we have a name for the debian package
	if (m_sDebianPackageName.empty()) {
		setDebianPackageName();
		createStageDirectory();
	}
	wdir = m_pathStageDir;
	boost::filesystem::current_path(wdir);

	wdir += "/DEBIAN";
	boost::filesystem::create_directory(wdir);
	boost::filesystem::permissions(wdir, (boost::filesystem::perms) 0755);

	unsigned int i = 0;   // size of each file in the manifest
	if ( !m_vFiles.empty() ) {

		boost::filesystem::path pre = boost::filesystem::current_path();
		pre /= m_pathInstallPrefix;

		std::vector<ur::transfer>::iterator ittrans;

		for (ittrans = m_vFiles.begin(); ittrans != m_vFiles.end(); ++ittrans) {
			ur::transfer ft = *ittrans;
			boost::filesystem::path srcfile = boost::filesystem::path(ft.sp);

			boost::filesystem::path tgtfile = pre;
			tgtfile /= boost::filesystem::path(ft.tp);

			boost::filesystem::path tgtdir = boost::filesystem::path(tgtfile.parent_path());

			// create the directory structure for the target if needed
			boost::filesystem::create_directories(tgtdir);

			// copy the file
			boost::filesystem::copy_file(srcfile, tgtfile);

			// record the size of the file transfered
			i += boost::filesystem::file_size(tgtfile);
		}
	}
	m_uiInstallSize = i/1024 + 1;

	boost::filesystem::current_path(idir);
}

void DebianPackage::writeDebianControleFile(void) {
	// keep track of the initial, stagebase, and working directories as I move around
	boost::filesystem::path idir, wdir, debname;
	//remember the beginning working directory
	idir = boost::filesystem::initial_path();

	std::string s = "/"; // to make package directory path relative

	idir = boost::filesystem::initial_path();

	// make sure we have a name for the debian package
	if (m_sDebianPackageName.empty()) {
		setDebianPackageName();
		createStageDirectory();
	}
	wdir = m_pathStageDir;

	wdir += "/DEBIAN";
	if (!boost::filesystem::exists(wdir)) {
		boost::filesystem::create_directories(wdir);
	    boost::filesystem::permissions(wdir, (boost::filesystem::perms) 0755);
	}
	boost::filesystem::current_path(wdir);

	bool first = true; // first iteration through the vector loops
	std::vector<ur::url>::iterator iturl;
	ur::url u;
	std::vector<ur::person>::iterator itperson;
	ur::person p;

	std::string cfile = "./control";
	std::fstream fControl;
	fControl.open(cfile.c_str(), std::fstream::out | std::fstream::trunc);

	fControl << "Package: " << m_sDebianPackageName << std::endl;
	fControl << "Version: " << m_sVersion << std::endl;

	fControl << "Maintainer: ";
	for ( itperson = m_vMaintainer.begin();
			itperson != m_vMaintainer.end(); ++itperson ) {
		p = *itperson;
		if (!first)
			fControl << ", ";
		else
			first = false;
		fControl << p.fullname << " <" << p.emailaddr << ">" << std::endl;
	}

	fControl << "Section: " << m_sSection << std::endl;
	fControl << "Priority: " << m_sPriority << std::endl;
	fControl << "Architecture: " << m_sArchitecture << std::endl;
	fControl << "Essential: no" << std::endl;
	fControl << "Installed-Size: " << m_uiInstallSize << std::endl;

	// Don't add a line for the default "deb" package type per best practices
	if ( m_sDebianType.compare("deb") != 0 )
		fControl << "Package-Type: " << m_sDebianType << std::endl;

	// Only support URLs for website and repository.  Ignore all others
	// Repositories are only git repositories
	for ( iturl = m_vUrl.begin();
			iturl != m_vUrl.end();
			++iturl ) {
		u = *iturl;
		if ( u.iType == URL_WEBSITE )
			fControl << "Homepage: " << u.sUrl << std::endl;
		else if ( u.iType == URL_REPOSITORY )
			fControl << "Vcs-Git: " << u.sUrl << std::endl;
	}

	// include all the relationship vectors
	// TODO:  Figure out how/if TestDepends is used....
	if( !m_vBuildtoolDepend.empty() )
		outputRelationVector(RELATION_BUILD_USING, m_vBuildtoolDepend, fControl);
	if( !m_vBuildDepend.empty() )
		outputRelationVector(RELATION_BUILD_DEPENDS, m_vBuildtoolDepend, fControl);
	if( !m_vRunDepend.empty() )
		outputRelationVector(RELATION_DEPENDS, m_vRunDepend, fControl);
	//if( !m_vTestDepend.empty() )
	//	outputRelationVector(RELATION_XXXX, m_vTestDepend, fControl);
	if( !m_vConflict.empty() )
		outputRelationVector(RELATION_CONFLICTS, m_vConflict, fControl);
	if( !m_vReplace.empty() )
		outputRelationVector(RELATION_REPLACES, m_vReplace, fControl);

	fControl << "Description: " << m_sDescription << std::endl;

	fControl.close();
	boost::filesystem::current_path(idir);
}

void DebianPackage::createStageDirectory(void) {
	boost::filesystem::path idir, wdir, debname;

	idir = boost::filesystem::initial_path();
	wdir = m_pathStageBase;
	boost::filesystem::current_path(wdir);

	std::string s = "/";

	// make sure we have a name for the debian package
	if (m_sDebianPackageName.empty())
		setDebianPackageName();

	// package directory path is based on name of the debian package
	s += m_sDebianPackageFileName;
	debname = boost::filesystem::path(s);

	wdir += debname;
	boost::filesystem::create_directory(wdir);
	boost::filesystem::permissions(wdir, (boost::filesystem::perms) 0755);

	m_pathStageDir = wdir;

	boost::filesystem::current_path(idir);
}

void DebianPackage::deleteStaging(void) {
	if ( !m_pathStageBase.empty() )
		boost::filesystem::remove_all(m_pathStageBase);
}

void DebianPackage::generateChecksums(void) {
	// keep track of the initial, stagebase, and working directories as I move around
	boost::filesystem::path idir, wdir;

	//remember the beginning working directory
	idir = boost::filesystem::initial_path();

	wdir = boost::filesystem::path(m_pathStageDir);
	boost::filesystem::current_path(wdir);

	// generate the checksums for each file
	system("find . -type f ! -regex '.*?debian-binary.*' ! -regex '.*?DEBIAN.*' -printf '%P ' | xargs md5sum > DEBIAN/md5sums");

	boost::filesystem::current_path(idir);
}

void DebianPackage::generateDebianPackage(void) {
	// keep track of the initial, stagebase, and working directories as I move around
	boost::filesystem::path idir, wdir;

	//remember the beginning working directory
	idir = boost::filesystem::initial_path();

	wdir = boost::filesystem::path(m_pathStageBase);
	boost::filesystem::current_path(wdir);

	// generate the package
	std::string cmdline = "dpkg-deb --build " + m_sDebianPackageFileName;
	system(cmdline.c_str());

	boost::filesystem::current_path(idir);
}

void DebianPackage::moveDebianPackage(void) {
	std::string filename = "/";
	std::string ext = ".";
	boost::filesystem::path target = m_pathDebianLocation;
    boost::filesystem::path source = m_pathStageBase;

    filename += m_sDebianPackageFileName;
    ext += m_sDebianType;
    filename += ext;

	source += boost::filesystem::path(filename);
	target += boost::filesystem::path(filename);

	if (!boost::filesystem::exists(source)) // I didn't create the debian - exit out
		; // eventually throw a file not found...

	// copy the file
	if (boost::filesystem::exists(target))  // A previously built file (stale) is sitting around
		boost::filesystem::remove(target);  // remove the stale file
	boost::filesystem::copy_file(source, target);
	boost::filesystem::remove(source);
}

/*  Private Functions
 *
 */
void DebianPackage::setDebianPackageName(void) {
	bool is_devel = false;
	m_sDebianPackageName = m_sProgPackageName;
	std::replace(m_sDebianPackageName.begin(), m_sDebianPackageName.end(),
			      '_', '-');
    //append "dev" to name if package is made from devel
	if (m_sSection.compare("devel") == 0) {
		is_devel = true;
		m_sDebianPackageName += "-dev";
	}

	//append the number of seconds from epoch to the version for
	//packages made from devel
	if (is_devel) {
		time_t t;
		time(&t);
		std::string s = boost::lexical_cast<std::string>(t);
		m_sVersion += "." + s;
	}

	m_sDebianPackageFileName = m_sDebianPackageName +
			                   "_" + m_sVersion +
			                   "_" + m_sArchitecture;
}

void DebianPackage::outputRelationVector(const unsigned int reltype, ur::vRelation &rel, std::fstream &f) {
    bool first = true;
	std::vector<ur::relation>::iterator itrel;
	ur::relation r;

	switch (reltype) {
	case RELATION_DEPENDS :
		f << "Depends: ";
		break;
	case RELATION_RECOMMENDS :
		f << "Recommends: ";
		break;
	case RELATION_SUGGESTS :
		f << "Suggests: ";
		break;
	case RELATION_PRE_DEPENDS :
		f << "Pre-Depends: ";
		break;
	case RELATION_BUILD_DEPENDS :
		f << "Build-Depends: ";
		break;
	case RELATION_BUILD_DEPENDS_INDEP :
		f << "Build-Depends-Indep: ";
		break;
	case RELATION_BUILD_USING :
		f << "Build-Using: ";
		break;
	case RELATION_ENHANCES :
		f << "Enhances: ";
		break;
	case RELATION_BREAKS :
		f << "Breaks: ";
		break;
	case RELATION_CONFLICTS :
		f << "Conflicts: ";
		break;
	case RELATION_BUILD_CONFLICTS :
		f << "Build-Conflicts: ";
		break;
	case RELATION_BUILD_CONFLICTS_INDEP :
		f << "Build-Conflicts-Indep: ";
		break;
	case RELATION_PROVIDES :
		f << "Provides: ";
		break;
	default:
		break;
	}

	for (itrel = rel.begin(); itrel != rel.end(); ++itrel) {
		r = *itrel;
		if (!first) {
			f << ", " << std::endl << "      ";
		}
		else {
			first = false;
		}

	    f << r.sVal;
	    switch (r.iVerType) {
		   case REL_VERSION_TYPE_LT :
			   f << " (< " << r.sVer << ")" << std::endl;
			   break;
		   case REL_VERSION_TYPE_LTE :
			   f << " (<= " << r.sVer << ")" << std::endl;
			   break;
		   case REL_VERSION_TYPE_EQ :
			   f << " (= " << r.sVer << ")" << std::endl;
			   break;
		   case REL_VERSION_TYPE_GTE :
			   f << " (>= " << r.sVer << ")" << std::endl;
			   break;
		   case REL_VERSION_TYPE_GT :
			   f << " (> " << r.sVer << ")" << std::endl;
			   break;
        default:
     	   break;
		   }
	   }
   f << std::endl;
}

} /* namespace ur */
