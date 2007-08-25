#!/bin/sh -e

aclocal 
autoheader 
autoconf 
#cp -f $(dirname $(which automake))/../share/automake/mkinstalldirs ./
#cp -f $(dirname $(which automake))/../share/gettext/config.rpath ./
automake -a -c --add-missing --foreign
