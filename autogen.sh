#! /bin/sh
#
# $Id: autogen.sh,v 1.1 2009-10-23 12:01:23 felix Exp $
#
# Run the various GNU autotools to bootstrap the build
# system.  Should only need to be done once.

# for now avoid using bash as not everyone has that installed
CONFIG_SHELL=/bin/sh
export CONFIG_SHELL

# HACK? doesnt make sense
# [ -d non-GPL ] || cp -r non-GPL.skel non-GPL

libtoolize -c -f -i


echo "Running aclocal..."
aclocal $ACLOCAL_FLAGS || exit 1

echo "Running autoheader..."
autoheader || exit 1

echo "Running automake..."
automake -a -c --gnu  || exit 1

echo "Running autoconf..."
autoconf || exit 1

if [ -r config.status ]; then
	CMD="./config.status --recheck"
	echo "Running $CMD $@ ..."
	$CMD
else
	echo run "./configure --enable-maintainer-mode"
fi
# echo "Running $CMD $@ ..."


