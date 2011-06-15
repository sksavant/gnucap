## $Id: Makefile.am,v 1.1 2009-10-23 12:01:23 felix Exp $
##
## Top level automake file for gnucap

SUBDIRS=	doc examples modelgen src tests

#obsolete...
if IVERILOG
	SUBDIRS+=vvpg
endif

if MODULES
	SUBDIRS+=modules
endif

EXTRA_DIST=	autogen.sh Makefile.template configure.old

