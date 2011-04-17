#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# $Id: configure.ac,v 1.2 2009-12-13 17:55:01 felix Exp $
#
#                            COPYRIGHT
#
#  Copyright (C) 2005, 2006 Dan McMahill
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
#  02110-1301, USA.

AC_PREREQ(2.67)
AC_INIT([fucap],[1.1])
AM_INIT_AUTOMAKE(fucap, 2009-12-07)

AC_PROG_CXX

AC_PROG_MAKE_SET 

AC_CONFIG_SRCDIR([src/main.cc])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])

AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT("/usr/local")

# libdir_const="${prefix}/lib"

#sysconfdir='/bar/etc'

#

#AC_SUBST([PREFIX])
#AC_SUBST([LIBDIR])




AC_MSG_CHECKING([if nongpl code should be compiled in])
AC_ARG_ENABLE([nongpl],
[  --enable-nongpl            Enable building of non-GPL code. [[default: disabled]]],
[
if test "X$enable_nongpl" = "Xno" ; then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
	enable_nongpl=yes
fi
],
[
	AC_MSG_RESULT([no])
	enable_nongpl=no
])

dnl Ivl for d_vvp.cc
iverilog=no
AC_ARG_WITH([iverilog],
  [AS_HELP_STRING([--with-iverilog], [directory where ivl source resides])],
  [],
  [with_iverilog=no])


IVERILOG=
AS_IF([ test "x$with_iverilog" != xno],
	[
		AC_CHECK_LIB([vvp], [main],
				  [
					AC_SUBST([IVERILOG], ["-lvvp"])
					AC_SUBST([IVERILOG_INCLUDE], ["-I$with_iverilog"])
					  AC_DEFINE([HAVE_IVERILOG], [1], [Define if you have libreadline])
				  ],
				  [
					AC_MSG_FAILURE([--with-iverilog was given, but test failed
	consider LDFLAGS=-L/path/to/libvvp.so/ configure"])
				  ],
				  [-L$with_iverilog/lib])
	]
)


echo AAAAAAAAA: $with_iverilog $IVERILOG

#	IVERILOG_PATH=${iverilog}
#	IVERILOG_INCLUDE=${iverilog}/include/iverilog
#	AC_DEFINE(IVERILOG_PATH, [1], [${iverilog}])
#else
#	IVERILOG_PATH=
#	IVERILOG_INCLUDE=
#fi

AC_SUBST(IVERILOG_PATH)
AC_SUBST(IVERILOG_INCLUDE)
AM_CONDITIONAL(IVERILOG, test x$with_iverilog != xno )

dnl debugging
AC_MSG_CHECKING([if debug code should be compiled in])
AC_ARG_ENABLE([debug],
[  --enable-debug            Enable building of debug code. [[default: disabled]]],
[
if test "X$enable_debug" = "Xno" ; then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
	enable_debug=yes
fi
],
[
	AC_MSG_RESULT([no])
	enable_debug=no
])

# Checks for programs.
AC_PROG_CXX

# If we are cross compiling, then we need to search for a
# gnucap-modelgen program to use for our build.  This can
# either be an installed modelgen or it can be specified
# like:
#   env MODELGEN=/build/i686--linux/modelgen/gnucap-modelgen /srcs/gnucap/configure
#           --host=alpha--netbsd --build=i686--linux
#
if test "$cross_compiling" = yes; then
	AC_PATH_PROG(MODELGEN, gnucap-modelgen)
else
	MODELGEN=../modelgen/gnucap-modelgen
	AC_SUBST([MODELGEN])
fi

# for building the documentation
#AC_PATH_PROG(DVIPDFM, dvipdfm, notfound)
#AM_CONDITIONAL(MISSING_DVIPDFM, test x$DVIPDFM = xnotfound)
#AC_PATH_PROG(HACHA, hacha, notfound)
#AC_PATH_PROG(HEVEA, hevea, notfound)
#AM_CONDITIONAL(MISSING_HEVEA, test x$HEVEA = xnotfound -o x$HACHA = xnotfound)
#AC_PATH_PROG(LATEX, latex, notfound)
#AC_PATH_PROG(MAKEINDEX, makeindex, notfound)
#AM_CONDITIONAL(MISSING_LATEX, test x$LATEX = xnotfound -o x$MAKEINDEX = xnotfound)

# Checks for libraries.

AC_LANG([C++])
AC_CHECK_LIB([m], [cos])

# probably unneeded.
AC_CHECK_LIB([gslcblas], [cblas_dgemm])
AC_CHECK_LIB([gsl], [gsl_blas_dgemm])
AC_CHECK_LIB([gsl], [gsl_fit_linear])
AC_CHECK_FUNCS([sqrt])

AC_ARG_WITH([readline],
[AS_HELP_STRING([--with-readline],
  [support command line editing @<:@default=yes@:>@])],
[],
[with_readline=yes])
 
if test "x$with_readline" != xno ; then
	AC_CHECK_LIB([termcap], [main])	
	AC_CHECK_LIB([readline], [main])
fi    

AC_CHECK_LIB([dl], [dlopen])

LT_INIT([disable-static])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL


# Checks for header files.
#AC_CHECK_HEADERS([fcntl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

if test "$enable_debug" = "yes" ; then
	CPPFLAGS="$CPPFLAGS -DTRACE_UNTESTED"
else
	CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi
# if we have gcc and we've asked for debugging then add lots of -W
if test "x$GCC" = "xyes" -a "$enable_debug" = "yes"; then
	for flag in -DTRACE_UNTESTED -Wall -W -Wno-sign-compare \
		-Wpointer-arith -Wcast-qual \
		-Wwrite-strings -Wconversion \
		-Woverloaded-virtual -O2 -Wlong-long \
		-Wsign-compare -Wcast-align ; do
		case " ${CFLAGS} " in
			*\ ${flag}\ *)
				# flag is already present
				;;
			*)
				CFLAGS="$CFLAGS ${flag}"
				;;
		esac
		case " ${CXXFLAGS} " in
			*\ ${flag}\ *)
				# flag is already present
				;;
			*)
				CXXFLAGS="$CXXFLAGS ${flag}"
				;;
		esac
	done
fi

# exports symbols to plugins
LDFLAGS="$LDFLAGS -rdynamic"


#AC_OUTPUT([
#	Makefile
#	doc/Makefile
#	examples/Makefile
#	man/Makefile
#	man/Addmodel/Makefile
#	man/Behavior/Makefile
#	man/Circuit/Makefile
#	man/Commands/Makefile
#	man/Tech/Makefile
#	modelgen/Makefile
#	src/Makefile
#	])


# stupid hack.
if test "x${prefix}" = "xNONE"; then
		AC_DEFINE_UNQUOTED( PREFIX, "${ac_default_prefix}", "Prefix")
		AC_DEFINE_UNQUOTED( LIBDIR, "${ac_default_prefix}/lib", "Libdir")
		AC_DEFINE_UNQUOTED( SODIR, "${ac_default_prefix}/lib", "Sodir")
else
		AC_DEFINE_UNQUOTED( PREFIX, "${prefix}", "Prefix")
		AC_DEFINE_UNQUOTED( LIBDIR, "${prefix}/lib", "Libdir")
		AC_DEFINE_UNQUOTED( SODIR, "${prefix}/lib", "Sodir")
fi



AC_OUTPUT([
	Makefile
	doc/Makefile
	examples/Makefile
	modelgen/Makefile
	src/Makefile])

if test "X$enable_nongpl" = "Xyes"; then
	AC_OUTPUT([ non-GPL/Makefile ])
fi


AC_MSG_RESULT([
** Configuration summary for $PACKAGE $VERSION:

   prefix:     $prefix
   exec_prefix:$exec_prefix
   libdir:     $libdir
   Icarus V:   $iverilog
   CPPFLAGS:   $CPPFLAGS
   CFLAGS:     $CFLAGS
   CXXFLAGS:   $CXXFLAGS
   LDFLAGS:    $LDFLAGS
   LIBS:       $LIBS
   debug:      $enable_debug
   non-GPL:    $enable_nongpl
   shared:     $shared

])


