AC_INIT(urpackager, 1.0, kurt.christofferson@gmail.com)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/urpackager.cpp)

AM_INIT_AUTOMAKE

AC_PROG_CXX

AX_BOOST_BASE([1.54],,[AC_MSG_ERROR([urpackager requires the boost libraries])])
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_PROGRAM_OPTIONS
AX_LIB_MYSQL

AC_CONFIG_FILES([
   Makefile
   src/Makefile
   ])
AC_OUTPUT
