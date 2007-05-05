INCLUDES = @DBUSGLIB_CFLAGS@ @X11_CFLAGS@ -fPIC -Wall -O0

bin_PROGRAMS = desktoptracksd

desktoptracksd_SOURCES = desktoptracksd.c appstats.c

desktoptracksd_LDADD= @DBUSGLIB_LIBS@ @X11_LIBS@

BUILT_SOURCES = desktoptracksd-dbus-glue.h

desktoptracksd-dbus-glue.h: desktoptracksd.xml
	@DBUSBINDINGTOOL@ --mode=glib-server --prefix=desktoptracks \
		$(srcdir)/desktoptracksd.xml > desktoptracksd-dbus-glue.h

EXTRA_DIST = desktoptracksd.xml 
DISTCLEANFILES = desktoptracksd-dbus-glue.h
