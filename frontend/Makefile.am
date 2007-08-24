

appdir = $(bindir)
app_SCRIPTS = desktoptracks

desktoptracks: desktoptracks.py
	sed \
	-e s!\@datadir\@!$(datadir)/desktoptracks! \
	-e s!\@bindir\@!$(bindir)! \
	-e s!\@version\@!$(VERSION)! \
	< $< > $@





EXTRA_DIST = desktoptracks.py
CLEANFILES = desktoptracks
