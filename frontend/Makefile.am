

appdir = $(bindir)
app_SCRIPTS = desktoptracks

desktoptracks: desktoptracks.py
	sed \
	-e s!\@datadir\@!$(datadir)/desktoptracks! \
	-e s!\@bindir\@!$(bindir)! \
	-e s!\@version\@!$(VERSION)! \
	< $< > $@


gladedir = $(datadir)/desktoptracks
dist_glade_DATA = desktoptracks.glade 


EXTRA_DIST = desktoptracks.py
CLEANFILES = desktoptracks
