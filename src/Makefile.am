bin_PROGRAMS = urpackager
urpackager_SOURCES = urpackager.cpp DebianPackage.cpp DebianPackage.h \
                     PackageControl.cpp PackageControl.h PackageXML.cpp PackageXML.h \
                     DependencyNameResolver.cpp DependencyNameResolver.h \
                     MysqlConnection.cpp MyslqConnection.h MysqlResult.cpp Mysqlresult.h \
                     MysqlRow.cpp MysqlRow.h

urpackager_CPPFLAGS = $(BOOST_CPPFLAGS) $(MYSQL_CXXFLAGS)
urpackager_LDFLAGS = $(BOOST_LDFLAGS) 
urpackager_LDADD = $(BOOST_SYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_FILESYSTEM_LIB) $(MYSQL_LDFLAGS)
