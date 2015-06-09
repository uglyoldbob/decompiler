AC_PREREQ([2.62])
AC_INIT([decompiler], [0.1])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([ac_config.h:config.h.in])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_OBJC
AC_CHECK_TOOL([STRIP],[strip])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
  then AC_MSG_WARN([Doxygen not found - documentation cannot be built])
fi
AM_CONDITIONAL([HAVE_DOXYGEN],
 [test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN],
[AC_CONFIG_FILES([Doxyfile])])

m4_include([m4/libs.m4])

#BUILD_FROM = system doing the compiling
#BUILD_FOR  = system the program will run on

#detect os so the correct options can be setup for
#the expected build configuration
AC_CANONICAL_HOST
case $build_os in
  darwin* )
	#mac
	BUILD_FROM="MAC"
	AC_MSG_NOTICE("Building on mac")
	;;
  linux* )
	#linux
	BUILD_FROM="LINUX"
	AC_MSG_NOTICE("Building on linux")
	;;
  cygwin* )
    #windows
	BUILD_FROM="WINDOWS"
	AC_MSG_NOTICE("Building from windows")
	;;
  * )
	#unknown (probably broken)
	BUILD_FROM="BROKEN"
	AC_MSG_ERROR("Possibly invalid build system $build_os")
	;;
esac

case $host_os in
  darwin* )
	#mac
	BUILD_FOR="MAC"
	AC_DEFINE(MAC, [1], [Enable code specific to mac])
	AC_CHECK_FRAMEWORK([Cocoa], [main], [], [AC_MSG_ERROR("Cocoa framework not found")])
	AC_MSG_NOTICE("Building for mac")
	;;
  linux* )
	BUILD_FOR="LINUX"
	AC_DEFINE(LINUX, [1], [Enable code specific to linux])
	AC_CHECK_LIB([udis86], [main], [], [AC_MSG_ERROR("udis86 Library not found")])
	AC_MSG_NOTICE("Building for linux")
	;;
  mingw32* | mingw32msvc* )
	BUILD_FOR="WINDOWS"
	AC_DEFINE(WINDOWS, [1], [Enable code specific to windows])
	USE_DLLS="mingwm10.dll"	
	AC_CHECK_LIB([ws2_32], [puts], [], [AC_MSG_ERROR("ws2_32 Library not found")])
	AC_CHECK_LIB([udis86], [main], [], [AC_MSG_ERROR("udis86 Library not found")])
	AC_MSG_NOTICE("Building for windows")
	;;
  * )
	BUILD_FOR="BROKEN"
	AC_MSG_ERROR("Possibly invalid build target $host_os")
	;;
esac

AC_C_BIGENDIAN(
  AC_DEFINE(ENDIAN_BIG, 1, [machine is bigendian]),
  AC_DEFINE(ENDIAN_LITTLE, 1, [machine is littleendian]),
  AC_MSG_ERROR(unknown endianess),
  AC_MSG_ERROR(universial endianess not supported)
)

AC_SUBST(BUILD_FOR)
AC_SUBST(BUILD_FROM)

AM_CONDITIONAL([WINDOWS], [test \"$BUILD_FOR\" = \"WINDOWS\"])
AM_CONDITIONAL([LINUX], [test \"$BUILD_FOR\" = \"LINUX\"])
AM_CONDITIONAL([MAC], [test \"$BUILD_FOR\" = \"MAC\"])

AC_CONFIG_FILES([
	Makefile
])
AC_OUTPUT
