/*
 * PackageXML.cpp
 *
 *  Created on: Feb 26, 2015
 *      Author: kurt
 */

#include "PackageXML.h"

namespace ur {

PackageXML::PackageXML() {
	// TODO Auto-generated constructor stub

}

PackageXML::~PackageXML() {
	// TODO Auto-generated destructor stub
}

std::string PackageXML::getProgPackageName(void) {
	return m_sProgPackageName;
}

std::string PackageXML::getVersion(void) {
	return m_sVersion;
}

std::string PackageXML::getDescription(void) {
	return m_sDescription;
}

ur::vs PackageXML::getLicenses(void) {
	return m_vLicense;
}

ur::vPerson PackageXML::getMaintainers(void) {
	return m_vMaintainer;
}

ur::vPerson PackageXML::getAuthors(void) {
	return m_vAuthor;
}

ur::vUrl    PackageXML::getUrls(void) {
	return m_vUrl;
}
ur::vRelation PackageXML::getBuildtoolDependencies(void) {
	return m_vBuildtoolDepend;
}

ur::vRelation PackageXML::getBuildDependencies(void) {
	return m_vBuildDepend;
}

ur::vRelation PackageXML::getRunDependencies(void) {
	return m_vRunDepend;
}

ur::vRelation PackageXML::getTestDependencies(void) {
	return m_vTestDepend;
}

ur::vRelation PackageXML::getConflicts(void) {
	return m_vConflict;
}

ur::vRelation PackageXML::getReplacements(void) {
	return m_vReplace;
}

void PackageXML::readFile(boost::filesystem::path p) {
	boost::property_tree::ptree pt;

	if (!boost::filesystem::exists(p) || !boost::filesystem::is_regular_file(p)) // does the file exist
	    return;

    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    boost::property_tree::read_xml(p.c_str(), pt);

	m_sProgPackageName = pt.get<std::string>("package.name");

	m_sVersion = pt.get<std::string>("package.version");

	m_sDescription = pt.get<std::string>("package.description");

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
            pt.get_child("package")) {
    	if (v.first == "license") {
    		std::string s;
    		s = v.second.get<std::string>("");
            m_vLicense.push_back( s );
    	}
    }

    readPersons(pt, "package", "maintainer", m_vMaintainer);
    readPersons(pt, "package", "author", m_vAuthor);
    readUrls(pt,"package", "url", m_vUrl);
    readRelations(pt,"package", "buildtool_depend", m_vBuildtoolDepend);
    readRelations(pt,"package", "build_depend", m_vBuildDepend);
    readRelations(pt,"package", "run_depend", m_vRunDepend);
    readRelations(pt,"package", "test_depend", m_vTestDepend);
    readRelations(pt,"package", "conflict", m_vConflict);
    readRelations(pt,"package", "replace", m_vReplace);

}

//	void writeFile(void);
//	void testFile(void);
void PackageXML::debugDump(void) {
	bool first = true;
	std::vector<std::string>::iterator its;
	std::string s;
	std::vector<ur::url>::iterator iturl;
	ur::url u;
	std::vector<ur::person>::iterator itperson;
	ur::person p;
	std::vector<ur::relation>::iterator itrel;
	ur::relation r;

	std::cout << "Package: " << m_sProgPackageName << std::endl;
	std::cout << "Version: " << m_sVersion << std::endl;
	std::cout << "Description: " << m_sDescription << std::endl;

	std::cout << "Found " << m_vLicense.size() << " licenses." << std::endl;
	if ( !m_vLicense.empty() ) {
	   std::cout << "License: ";
	   for ( its = m_vLicense.begin(); its != m_vLicense.end(); ++its ) {
		   if (!first)
			   std::cout << ", ";
		   else
			   first = false;
		   std::cout << *its;
	   }
	   std::cout << std::endl;
	}

	first = true;
	std::cout << "Found " << m_vMaintainer.size() << " maintainers." << std::endl;
	if ( !m_vMaintainer.empty() ) {
	   std::cout << "Maintainer: ";
	   for ( itperson = m_vMaintainer.begin(); itperson != m_vMaintainer.end(); ++itperson ) {
		   if (!first) {
			   std::cout << ", ";
		   } else {
			   first = false;
		   }
		   p = *itperson;
		   std::cout << p.fullname << " <" << p.emailaddr << ">" << std::endl;
	   }
	   std::cout << std::endl;
	}

	first = true;
	std::cout << "Found " << m_vAuthor.size() << " authors." << std::endl;
	if ( !m_vAuthor.empty() ) {
		std::cout << "Author: ";
		for ( itperson = m_vAuthor.begin(); itperson != m_vAuthor.end(); ++itperson ) {
		   if (!first) {
			   std::cout << ", ";
		   } else {
			   first = false;
		   }
		   p = *itperson;
		   std::cout << p.fullname << " <" << p.emailaddr << ">" << std::endl;
	   }
	   std::cout << std::endl;
	}

	std::cout << "Found " << m_vUrl.size() << " URLs." << std::endl;
	if ( !m_vUrl.empty() ) {
		for ( iturl = m_vUrl.begin(); iturl != m_vUrl.end(); ++iturl ) {
			u = *iturl;
		   std::cout << "URL: " << u.sUrl;
		   switch (u.iType) {
		   case URL_WEBSITE :
			   std::cout << " (website)";
			   break;
		   case URL_BUGTRACKER :
			   std::cout << " (bugdb)";
			   break;
		   case URL_REPOSITORY :
			   std::cout << " (repository)";
			   break;
		   case URL_OTHER :
			   std::cout << " (other)";
			   break;
           default:
        	   break;
		   }
	   }
	   std::cout << std::endl;
	}

	std::cout << "Found " << m_vBuildtoolDepend.size() << " buildtool dependencies." << std::endl;
	if ( !m_vBuildtoolDepend.empty() ) {
	   for ( itrel = m_vBuildtoolDepend.begin();
			   itrel != m_vBuildtoolDepend.end(); ++itrel ) {
		   r = *itrel;
		   std::cout << "Dependency: " << r.sVal;
		   switch (r.iVerType) {
		   case REL_VERSION_TYPE_NONE :
			   std::cout << " (no filter)";
			   break;
		   case REL_VERSION_TYPE_LT :
			   std::cout << " (version_lt)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_LTE :
			   std::cout << " (version_lte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_EQ :
			   std::cout << " (version_eq)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GTE :
			   std::cout << " (version_gte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GT :
			   std::cout << " (version_gt)" << r.sVer;
			   break;
           default:
        	   break;
		   }
		   std::cout << std::endl;
	   }
	  std::cout << std::endl;
	}

	std::cout << "Found " << m_vBuildDepend.size() << " build dependencies." << std::endl;
	if ( !m_vBuildDepend.empty() ) {
	   for ( itrel = m_vBuildDepend.begin();
			   itrel != m_vBuildDepend.end(); ++itrel ) {
		   r = *itrel;
		   std::cout << "Dependency: " << r.sVal;
		   switch (r.iVerType) {
		   case REL_VERSION_TYPE_NONE :
			   std::cout << " (no filter)";
			   break;
		   case REL_VERSION_TYPE_LT :
			   std::cout << " (version_lt)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_LTE :
			   std::cout << " (version_lte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_EQ :
			   std::cout << " (version_eq)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GTE :
			   std::cout << " (version_gte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GT :
			   std::cout << " (version_gt)" << r.sVer;
			   break;
           default:
        	   break;
		   }
	   }
	  std::cout << std::endl;
	}

	std::cout << "Found " << m_vRunDepend.size() << " runtime dependencies." << std::endl;
	if ( !m_vRunDepend.empty() ) {
	   for ( itrel = m_vRunDepend.begin(); itrel != m_vRunDepend.end(); ++itrel ) {
		   r = *itrel;
		   std::cout << "Dependency: " << r.sVal;
		   switch (r.iVerType) {
		   case REL_VERSION_TYPE_NONE :
			   std::cout << " (no filter)";
			   break;
		   case REL_VERSION_TYPE_LT :
			   std::cout << " (version_lt)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_LTE :
			   std::cout << " (version_lte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_EQ :
			   std::cout << " (version_eq)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GTE :
			   std::cout << " (version_gte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GT :
			   std::cout << " (version_gt)" << r.sVer;
			   break;
           default:
        	   break;
		   }
	   }
	  std::cout << std::endl;
	}

	std::cout << "Found " << m_vConflict.size() << " conflicts." << std::endl;
	if ( !m_vConflict.empty() ) {
	   for ( itrel = m_vConflict.begin(); itrel != m_vConflict.end(); ++itrel ) {
		   r = *itrel;
		   std::cout << "Conflict: " << r.sVal;
		   switch (r.iVerType) {
		   case REL_VERSION_TYPE_NONE :
			   std::cout << " (no filter)";
			   break;
		   case REL_VERSION_TYPE_LT :
			   std::cout << " (version_lt)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_LTE :
			   std::cout << " (version_lte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_EQ :
			   std::cout << " (version_eq)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GTE :
			   std::cout << " (version_gte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GT :
			   std::cout << " (version_gt)" << r.sVer;
			   break;
           default:
        	   break;
		   }
	   }
	  std::cout << std::endl;
	}

	std::cout << "Found " << m_vReplace.size() << " replacements." << std::endl;
	if ( !m_vReplace.empty() ) {
	   for ( itrel = m_vReplace.begin(); itrel != m_vReplace.end(); ++itrel ) {
		   r = *itrel;
		   std::cout << "Replace: " << r.sVal;
		   switch (r.iVerType) {
		   case REL_VERSION_TYPE_NONE :
			   std::cout << " (no filter)";
			   break;
		   case REL_VERSION_TYPE_LT :
			   std::cout << " (version_lt)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_LTE :
			   std::cout << " (version_lte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_EQ :
			   std::cout << " (version_eq)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GTE :
			   std::cout << " (version_gte)" << r.sVer;
			   break;
		   case REL_VERSION_TYPE_GT :
			   std::cout << " (version_gt)" << r.sVer;
			   break;
           default:
        	   break;
		   }
	   }
	  std::cout << std::endl;
	}

}

/* Private Methods */


void PackageXML::readPersons(boost::property_tree::ptree &t,
		    std::string child,
			std::string first,
			ur::vPerson &result) {
	ur::person p;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
            t.get_child(child)) {
    	if (v.first.compare(first) == 0) {
    		ur::person p;

    		p.fullname = v.second.get<std::string>("");
    		if ( v.second.get_optional<std::string>("<xmlattr>.email") )
    			p.emailaddr = v.second.get<std::string>("<xmlattr>.email");
            result.push_back( p );
    	}
    }
}

void PackageXML::readRelations(boost::property_tree::ptree &t,
		        std::string child,
				std::string first,
				ur::vRelation &result) {
	ur::relation r;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
            t.get_child(child)) {
    	if (v.first.compare(first) == 0) {
        	r.sVal = v.second.get<std::string>("");
    		if ( v.second.get_optional<std::string>("<xmlattr>.version_lt") ) {
    			r.iVerType = REL_VERSION_TYPE_LT;
    			r.sVer = v.second.get<std::string>("<xmlattr>.version_lt");
    		} else if ( v.second.get_optional<std::string>("<xmlattr>.version_lte") ) {
    			r.iVerType = REL_VERSION_TYPE_LTE;
    			r.sVer = v.second.get<std::string>("<xmlattr>.version_lte");
    		} else if ( v.second.get_optional<std::string>("<xmlattr>.version_eq") ) {
    			r.iVerType = REL_VERSION_TYPE_EQ;
    			r.sVer = v.second.get<std::string>("<xmlattr>.version_eq");
    		} else if ( v.second.get_optional<std::string>("<xmlattr>.version_gte") ) {
    			r.iVerType = REL_VERSION_TYPE_GTE;
    			r.sVer = v.second.get<std::string>("<xmlattr>.version_gte");
    		} else if ( v.second.get_optional<std::string>("<xmlattr>.version_gt") ) {
    			r.iVerType = REL_VERSION_TYPE_GT;
    			r.sVer = v.second.get<std::string>("<xmlattr>.version_gt");
    		} else {
    			r.iVerType = REL_VERSION_TYPE_NONE;
    		}
    		result.push_back(r);
    	}
    }
}

void PackageXML::readUrls(boost::property_tree::ptree &t,
		           std::string child,
				   std::string first,
				   ur::vUrl &result) {
	ur::url u;

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
            t.get_child(child)) {
    	if (v.first.compare(first) == 0) {
    		std::string sUrlType;

    		u.sUrl = v.second.get<std::string>("");
    		if ( v.second.get_optional<std::string>("<xmlattr>.type") ) {
    			sUrlType = v.second.get<std::string>("<xmlattr>.type");
    		}
    		if (sUrlType.empty() || sUrlType.compare("website") == 0)
    			u.iType = URL_WEBSITE;
    		else if (sUrlType.compare("bugtracker") == 0)
    			u.iType = URL_BUGTRACKER;
    		else if (sUrlType.compare("repository") == 0)
    			u.iType = URL_REPOSITORY;
    		else
    			u.iType = URL_OTHER;

            result.push_back( u );
    	}
    }
}



} /* namespace ur */
