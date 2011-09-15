## $Id: Makefile.am,v 1.1 2009-10-23 12:01:23 felix Exp $
##
## Top level automake file for gnucap

if ADMS
  ADMS_DIR = adms
endif

if MODULES
  MODULES_DIR = modules
endif

SUBDIRS = doc examples modelgen src tests $(ADMS_DIR) $(MODULES_DIR)

if IVERILOG
 ##SUBDIRS+=vvpg
endif



EXTRA_DIST=	autogen.sh Makefile.template configure.old

