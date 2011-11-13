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
AC_LANG(C++)

AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT("/usr/local")

# libdir_const="${prefix}/lib"

#sysconfdir='/bar/etc'

#AC_SUBST([PREFIX])
#AC_SUBST([LIBDIR])


dnl ----------------- doc ------------------
AC_MSG_CHECKING([doc])
AC_ARG_ENABLE([doc],
[  --enable-doc            Enable building of modules. [[default: disabled]]],
[
if test "X$enable_doc" = "Xno" ; then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
	enable_doc=yes
fi
],
[
	AC_MSG_RESULT([no])
	enable_doc=no
])

if test "X$enable_doc" = "Xyes"; then
	AC_OUTPUT([ doc/Makefile ])
fi
AM_CONDITIONAL([DOC], [test x$enable_doc != xno] )
dnl ----------------- man ------------------
AC_MSG_CHECKING([man])
AC_ARG_ENABLE([man],
[  --enable-man            Enable building of modules. [[default: disabled]]],
[
if test "X$enable_man" = "Xno" ; then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
	enable_man=yes
fi
],
[
	AC_MSG_RESULT([no])
	enable_man=no
])
dnl ------------- modules -------------------
dnl MODULES=
AC_MSG_CHECKING([if modules should be compiled in])
AC_ARG_ENABLE([modules],
[  --enable-modules            Enable building of modules. [[default: disabled]]],
[
if test "X$enable_modules" = "Xno" ; then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
	enable_modules=yes
fi
],
[
	AC_MSG_RESULT([no])
	enable_modules=no
])
AM_CONDITIONAL([MODULES], [test x$enable_modules != xno] )

dnl --------------- adms -----------------
#ADMS=
AC_MSG_CHECKING([compile adms va devices])
AC_ARG_ENABLE([adms],
  [  --enable-adms            Enable adms devices. [[default: disabled]]],
  [ if test "X$enable_adms" = "Xno" ; then
	   AC_MSG_RESULT([no])
    else
	   AC_MSG_RESULT([yes])
	   enable_adms=yes
	   with_adms=admsXml
	   AC_DEFINE([HAVE_ADMS], [1], [make adms devices])
    fi
  ],
  [
    AC_CHECK_PROG(found_admsxml, admsXml, yes, no)
	 # FIXME: check version. 2.2.9 works.
    AC_MSG_RESULT([$found_admsxml])
	 enable_adms=$found_admsxml
	 # with_adms=admsXml
  ])

dnl ------------ icarus verilog -------------------
iverilog=no
AC_ARG_WITH([iverilog],
  [AS_HELP_STRING([--with-iverilog], [directory where ivl source resides])],
  [],
  [with_iverilog=no])

# BUG. does not check $with_iverilog for correctness
IVERILOG=
AS_IF([ test "x$with_iverilog" != xno],
	[
	   echo "testing iverilog availibility..."
		AC_CHECK_LIB([vvp], [main],
				  [
					IVL_CPPFLAGS+=-I$with_iverilog
#					AC_SUBST([IVERILOG], ["-lvvp"])
# using dlmopen for now.
					#AC_SUBST([IVERILOG], ["-lvvp_compile"]) 
					AC_SUBST([IVERILOG], ["-lvvp"]) 
					AC_SUBST([IVERILOG_SO], ["$with_iverilog/lib/libvvp.so"])
					AC_SUBST([IVERILOG_PATH], ["$with_iverilog"])
					AC_SUBST([IVERILOG_INCLUDE], ["-I$with_iverilog"])
					AC_DEFINE([HAVE_IVERILOG], [1], [Define if you have ivl])
				  ],
				  [
					AC_MSG_FAILURE([--with-iverilog was given, but test failed
	consider LDFLAGS=-L/path/to/libvvp.so/ configure"])
				  ],
				  [-lvvp])
	]
)

CPPFLAGS_SAVE="$CPPFLAGS"
CPPFLAGS="$IVL_CPPFLAGS $CPPFLAGS"
AS_IF([ test "x$with_iverilog" != xno], [
AC_CHECK_HEADERS(vvp/schedule.h, [ ],
					AC_MSG_FAILURE([there should be vvp headers in $with_iverilog  ])
					) ] )
CPPFLAGS="$CPPFLAGS_SAVE"


AC_SUBST(IVERILOG_INCLUDE)
AC_SUBST(IVL_CPPFLAGS)

dnl ------------------- debugging -------------------
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
AC_CHECK_LIB([lapack_atlas], [clapack_dgesv])
AC_CHECK_LIB([lapack], [dgels_])
AC_CHECK_LIB([gsl], [gsl_blas_dgemm])
AC_CHECK_LIB([gsl], [gsl_fit_linear])
AC_CHECK_FUNCS([sqrt])

dnl ----------------------------------------------------
# some more checks. some should depend on switches. but the switches arent
# there yet
AC_CHECK_HEADER([atlas/clapack.h],
                [AC_DEFINE([HAVE_CLAPACK_H], [1],
                           [Define to 1 if you have atlas/clapack.h])],
                [AC_MSG_ERROR([please install package libatlas-dev])])

AC_CHECK_HEADER([boost/algorithm/string.hpp],
                [AC_DEFINE([HAVE_STRING_HPP], [1],
                           [Define to 1 if there is boost/algorithm/string.hpp])],
                [AC_MSG_ERROR([please install the boost package])])

AC_CHECK_HEADER([boost/algorithm/string/split.hpp],
                [AC_DEFINE([HAVE_SPLIT_HPP], [1],
                           [Define to 1 if there is boost/algorithm/string/split.hpp])],
                [AC_MSG_ERROR([please install the boost package])])

AC_CHECK_HEADER([readline/readline.h],
                [AC_DEFINE([HAVE_READLINE_H], [1],
                           [Define to 1 if you have readline/readline.h])],
                [AC_MSG_ERROR([please install the libreadline-dev package])])

AC_CHECK_HEADER([readline/history.h],
                [AC_DEFINE([HAVE_HISTORY_H], [1],
                           [Define to 1 if you have readline/history.h])],
                [AC_MSG_ERROR([please install the libreadline-dev package])])


AC_ARG_WITH([gsl],
[AS_HELP_STRING([--with-gsl],
  [support gls @<:@default=yes@:>@])],
[],
[with_gsl=yes])

if test "x$with_gsl" != xno ; then
	AC_CHECK_HEADER([gsl/gsl_fit.h],
	[AC_DEFINE([HAVE_GSL_FIT_H], [1],
		[Define to 1 if you have gsl/gsl_fit.h])],
	[AC_MSG_ERROR([please install the libgsl0-dev package])])
fi    

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
# remove additional added  -OX
    FOUNDOX="no"
    NEWCXXFLAGS=""
	for flag in ${CXXFLAGS} ; do
        case " -O0 -O1 -O2 -O3 " in 
			*\ ${flag}\ *)
              if test "${FOUNDOX}" = "no"; then
                  NEWCXXFLAGS="${NEWCXXFLAGS} ${flag}" 
              fi
              FOUNDOX="yes"
              ;;
            *) 
              NEWCXXFLAGS="${NEWCXXFLAGS} ${flag}" 
              ;;
        esac
    done
    CXXFLAGS=${NEWCXXFLAGS}
#    echo "CXXFLAGS: ${CXXFLAGS}"
    FOUNDOX="no"
    NEWCFLAGS=""
	for flag in ${CFLAGS} ; do
        case " -O0 -O1 -O2 -O3 " in 
			*\ ${flag}\ *)
              if test "${FOUNDOX}" = "no"; then
                  NEWCFLAGS="${NEWCFLAGS} ${flag}" 
              fi
              FOUNDOX="yes"
              ;;
            *) 
              NEWCFLAGS="${NEWCFLAGS} ${flag}" 
              ;;
        esac
    done
    CFLAGS=${NEWCFLAGS}
#    echo "CFLAGS: ${CFLAGS}"

fi

# exports symbols to plugins
LDFLAGS="$LDFLAGS -rdynamic"


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


AM_CONDITIONAL([ADMS], [test x$enable_adms != xno] )
AM_CONDITIONAL(IVERILOG, test x$with_iverilog != xno )
AM_CONDITIONAL(IVERILOG_VVP, test x$iverilog_vvp_old = xyes )

AC_OUTPUT([
	Makefile
	examples/Makefile
	modelgen/Makefile ])


if test "X$enable_adms" = "Xyes"; then
	AC_OUTPUT([ adms/Makefile ])
	AC_MSG_NOTICE( [adms Makefile...])
fi

AC_OUTPUT( [ src/Makefile ])

if test "X$enable_modules" = "Xyes"; then
	AC_OUTPUT([ modules/Makefile ])
fi



# todo
#if test "X$enable_man" = "Xyes"; then
	#AC_OUTPUT([
		#man/Makefile
		#man/Addmodel/Makefile
		#man/Behavior/Makefile
		#man/Circuit/Makefile
		#man/Commands/Makefile
		#man/Tech/Makefile
	#])
#fi



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
   modules:    $enable_modules
   admst:      $enable_adms
   shared:     $shared
   doc:        $enable_doc

])


