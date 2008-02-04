INCLUDES = -fPIC @DBUSGLIB_CFLAGS@ @X11_CFLAGS@ @GTK_CFLAGS@ \
	-DDATADIR=\"$(datadir)/${PACKAGE}\" \
	-DBINDIR=\"$(bindir)\"

bin_PROGRAMS = desktoptracksd

desktoptracksd_SOURCES = desktoptracksd.c appstats.c desktoptracksd.h appstats.h

desktoptracksd_LDADD= @DBUSGLIB_LIBS@ @X11_LIBS@ @GTK_LIBS@

BUILT_SOURCES = desktoptracksd-dbus-glue.h

desktoptracksd-dbus-glue.h: desktoptracksd.xml
	@DBUSBINDINGTOOL@ --mode=glib-server --prefix=desktoptracks \
		$(srcdir)/desktoptracksd.xml > desktoptracksd-dbus-glue.h

EXTRA_DIST = desktoptracksd.xml 
DISTCLEANFILES = desktoptracksd-dbus-glue.h
