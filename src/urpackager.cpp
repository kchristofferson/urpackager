//============================================================================
// Name        : urpackager.cpp
// Author      :    Kurt Christotofferson
// Version     :    1.0
// Copyright   : (c) Ubiquity Robotics
// Description : Ubiquity Robotics Packaging tool
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string>

#include <errno.h>
#include <fstream>
#include <typeinfo>
#include <cstdlib>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>


#include "PackageXML.h"
#include "PackageControl.h"
#include "DebianPackage.h"

const size_t EXEC_SUCCESS = 0x00;
const size_t ERROR_COMMAND_LINE = 1;
const size_t ERROR_CONFIG_FILE = 2;
const size_t ERROR_SPAWN = 3;
const size_t ERROR_FILESYSTEM = 4;
const size_t ERROR_UNHANDLED_EXCEPTION = 9;

/* Static Function Definitions */
static void graceful_exit(boost::filesystem::path, size_t rc);
static void urpackagerConfFileCreate(void);

int main(int argc, const char *argv[]) {
	// Pick up configuration parameters
	boost::program_options::variables_map vm;

	bool bTest, bBuild, bPackage;
	bTest = bBuild = bPackage = false;
	unsigned int iLogRotationSize;
	std::string sWorkingDir;
	std::string sStageDir;
	std::string sInstallPrefix;
	std::string sPackageName;
	std::string sPackageControlFile;
	std::string sArchitecture;
	std::string sSection;
	std::string sCfgfile;
	std::string sRosVersion;
	std::string sPriority;
	std::string sLogFile;
	std::string sLogFilter;
	std::string sLogFormat;
	std::string sDebianLocation;
	std::string sDebianType;
	std::string sDBHost;
	std::string sDBDatabase;
	uint32_t uiDBPort;
	std::string sDBUser;
	std::string sDBPassword;

	boost::filesystem::path idir;
	// capture the directory from which urpackager was run.
	// we return the user to this directory when we exit.
	idir = boost::filesystem::initial_path();

	// work is run out of the users home directory
	sWorkingDir = getenv("HOME");
	if (sWorkingDir.empty()) // not a  popular linux version configuration
		sWorkingDir = getenv("USERPROFILE");
	if (sWorkingDir.empty()) {
		std::cout << "The user running this utility must have a home directory" << std::endl;
		return (ERROR_FILESYSTEM);
	}

	// If this is the first time run, configuration file and control files will not exist
	sWorkingDir += "/.urpackager";
	boost::filesystem::path wdir(sWorkingDir);
	if (!boost::filesystem::is_directory(wdir)) { // First time the utility was run by this user.
		if (!boost::filesystem::create_directory(wdir)) {
			std::cout << "Unable to write to the user's home directory" << std::endl;
			return (ERROR_FILESYSTEM);
		}
		boost::filesystem::current_path(wdir);
		boost::filesystem::create_directory("./pkgcontrol");
		boost::filesystem::create_directory("./log");
		urpackagerConfFileCreate();
		std::cout << "This is the first time you have run \"urpackager.\"" << std::endl;
		std::cout << "Edit the file '~/.urpackager/urpackager.conf'" << std::endl;
		return (EXEC_SUCCESS);
	}
	boost::filesystem::current_path(wdir);

	// get run parameters
	try {
		/** Define and parse the program options */
		boost::program_options::options_description generic("Generic Options", 255); // command line only options
		generic.add_options()
				("help,h", "Print help messages")
				("version,v", "Version 1.0.0");

		boost::program_options::options_description config("Configuration Options"); // command line or configuration file and environment
		config.add_options()
		("package", boost::program_options::value<std::string>(),
				"The name of the package for which to create a debian")
		("architecture", boost::program_options::value<std::string>(),
				"Architecture for which you wish to create a debian file")
		("section", boost::program_options::value<std::string>()->default_value("amd64"),
				"repository section for which you wish to create a debian file")
		("test", boost::program_options::value<bool>()->default_value(false),
				"prints out variables but does no work")
		("stagedir", boost::program_options::value<std::string>()->default_value("unique_path"),
				"The directory under which the debian manifest will be temporarily created")
		("installprefix", boost::program_options::value<std::string>()->default_value("/opt/magni/v4"),
				"Prefix directory into which files will be (default) installed")
		("config", boost::program_options::value<std::string>()->default_value("./urpackager.conf"),
				"The configuration file")
		("log.file", boost::program_options::value<std::string>()->default_value("./log/urpackager.log"),
				"The log file")
		("log.filter", boost::program_options::value<std::string>()->default_value("INFO"),
				"The log level")
		("log.rotationsize", boost::program_options::value<int>()->default_value(100),
				"The maximum size for the log file in MB")
		("log.format", boost::program_options::value<std::string>()->default_value("[%TimeStamp%]: %Message%"),
				"The format log messages take")
		("ros.version", boost::program_options::value<std::string>()->default_value("indigo"),
				"The version of ROS this build - appended to <run_depend> package names")
		("debian.type", boost::program_options::value<std::string>()->default_value("deb"),
				"Package either a binary (type = \"deb\") or a micro binary (type = \"udeb\")")
		("debian.location", boost::program_options::value<std::string>(),
				"Directory into which to place the debian file")
    ("database.host", boost::program_options::value<std::string>(),
	      "The host where the urpackage db is running")
    ("database.database", boost::program_options::value<std::string>(),
        "The mysql schema")
    ("database.port", boost::program_options::value<int>(),
        "The port on which the database is listening")
    ("database.user", boost::program_options::value<std::string>(),
        "The user of the database")
    ("database.password", boost::program_options::value<std::string>(),
        "Password to log into the database");
		boost::program_options::options_description hidden("Hidden"); // command line or configuration file/environment but not shown

		hidden.add_options()("administrator", boost::program_options::value<std::string>(),
				"Administration capabilities");

		boost::program_options::options_description cmdline_options;
		cmdline_options.add(generic).add(config).add(hidden);

		boost::program_options::options_description cfgfile_options;
		cfgfile_options.add(config).add(hidden);

		boost::program_options::options_description visible("Allowed options");
		visible.add(generic).add(config);

    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdline_options), vm); // can throw

		/* --help option */
		if (vm.count("help")) {
			std::cout << "Foo help...." << std::endl
					<< config << std::endl;
			graceful_exit(idir, EXEC_SUCCESS);
		}

		if (vm.count("version")) {
			std::cout << "urpackager Version 1.0.0" << std::endl
					<< config << std::endl;
			graceful_exit(idir, EXEC_SUCCESS);
		}

		if (vm.count("package")) {
			sPackageName = vm["package"].as<std::string>();
		}

		if (vm.count("architecture")) {
			sArchitecture = vm["architecture"].as<std::string>();
		}

		if (vm.count("section")) {
			sSection = vm["section"].as<std::string>();
		}

		if (vm.count("test")) {
			bTest = vm["test"].as<bool>();
		}

		if (vm.count("stagedir")) {
			sStageDir = vm["stagedir"].as<std::string>();
		}

		if (vm.count("installprefix")) {
			sInstallPrefix = vm["installprefix"].as<std::string>();
		}

		if (vm.count("config"))
			sCfgfile = (const std::string&) vm["config"].as<std::string>();

		std::ifstream cfgfile(sCfgfile.c_str(), std::ios::in);
		if (cfgfile.fail())
			std::cout << "Config file failed to open" << std::endl;

		boost::program_options::store(parse_config_file(cfgfile, cfgfile_options, true), vm);
		cfgfile.close();

		boost::program_options::notify(vm); // throws on error, so do after help in case
		// there are any problems
	} catch (boost::program_options::error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		graceful_exit(idir, ERROR_COMMAND_LINE);
	}

	try {
		if (vm.count("log.file"))
			sLogFile = (const std::string&) vm["log.file"].as<std::string>();

		if (vm.count("log.filter"))
			sLogFilter = (const std::string&) vm["log.filter"].as<std::string>();

		if (vm.count("log.rotationsize"))
			iLogRotationSize = (int) vm["log.rotationsize"].as<int>();

		if (vm.count("log.format"))
			sLogFormat = (const std::string&) vm["log.format"].as<std::string>();

		if (vm.count("debian.location"))
			sDebianLocation = (const std::string&) vm["debian.location"].as<std::string>();

		if (vm.count("debian.type"))
			sDebianType = (const std::string&) vm["debian.type"].as<std::string>();

		if (vm.count("ros.version"))
			sRosVersion = (const std::string&) vm["ros.version"].as<std::string>();

		if (sPackageName.empty()) {
			std::cout << "Execution of this utility requires a package name" << std::endl;
			graceful_exit(idir, ERROR_COMMAND_LINE);
		}

		if (sArchitecture.empty()) {
			sPackageControlFile = sWorkingDir + "/pkgcontrol/"
					              + sPackageName + ".pkgctl";
		} else {
			sPackageControlFile = sWorkingDir + "/pkgcontrol/" + sPackageName +
					              "." + sArchitecture + ".pkgctl";
		}
    if (vm.count("database.host"))
      sDBHost = (const std::string&) vm["database.host"].as<std::string>();

    if (vm.count("database.database"))
      sDBDatabase = (const std::string&) vm["database.database"].as<std::string>();

    if (vm.count("database.port"))
      uiDBPort = (unsigned int) vm["database.port"].as<int>();

    if (vm.count("database.user"))
      sDBUser = (const std::string&) vm["database.user"].as<std::string>();

    if (vm.count("database.password"))
      sDBPassword = (const std::string&) vm["database.password"].as<std::string>();

	} catch (std::exception& e) {
		std::cerr << "Unhandled Exception reached after applying all options: "
				<< e.what() << ", application will now exit" << std::endl;
		graceful_exit(idir, ERROR_UNHANDLED_EXCEPTION);
	}

	/* Ready to run the utility
	 *
	 */
	ur::PackageControl pc(sPackageControlFile);
	pc.readControlFile();
	pc.setStageBase(sStageDir);
	ur::PackageXML px;
	px.readFile(pc.getPackageXML());

	if(bTest) {
		graceful_exit(idir, EXEC_SUCCESS);
	}

	ur::DebianPackage pkg(pc, px,
			              sArchitecture,
					      sSection,
					      sDebianType,
					      sDebianLocation,
					      sDBHost,
					      sDBDatabase,
					      sDBUser,
					      sDBPassword,
					      uiDBPort);

	pkg.createStageDirectory();
	pkg.stageManifest();
	pkg.writeDebianControleFile();
	pkg.generateChecksums();
	pkg.generateDebianPackage();
	pkg.moveDebianPackage();
	pkg.deleteStaging();

	return (EXEC_SUCCESS);
}

/* static functions */
void graceful_exit(boost::filesystem::path p, size_t rc) {
	boost::filesystem::current_path(p);
	exit (rc);
}

static void urpackagerConfFileCreate(void) {
	std::ofstream of((const char *) "./urpackager.conf", std::ostream::out);
    of << "stagedir = /tmp" << std::endl;
	of << "installprefix = /opt/magni/v4" << std::endl;
	of << "#" << std::endl << std::endl;
	of << "[log]" << std::endl;
	of << "file = urpackager.log" << std::endl;
	of << "filter = INFO" << std::endl;
	of << "rotationsize = 100" << std::endl;
	of << "format = [%TimeStamp%]: %Message%" << std::endl;
	of << "#" << std::endl << std::endl;
	of << "[debian]" << std::endl;
	of << "location = /mnt/build/stage/debs" << std::endl;
	of << "type = deb" << std::endl;
	of << "#" << std::endl << std::endl;
  of << "[ros]" << std::endl;
  of << "version = indigo" << std::endl;
  of << "#" << std::endl << std::endl;

  of << "# These values MUST be set for the connection to a mysql db" << std::endl;
  of << "[database]" << std::endl;
  of << "host = " << std::endl;
  of << "database = packager" << std::endl;
  of << "port = " << std::endl;
  of << "user = " << std::endl;
  of << "password = " << std::endl;
  of << "#" << std::endl << std::endl;
	of.close();
}
