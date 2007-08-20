## Process this file with automake to produce Makefile.in

#desktoptracks-logo.svg: desktoptracks.svg
#	sed -e s!48px!192px! < $< > $@

logodir = $(datadir)/desktoptracks
dist_logo_DATA = desktoptracks.svg

icondir = $(datadir)/icons/hicolor/48x48/apps
dist_icon_DATA = desktoptracks.png

svgicondir = $(datadir)/icons/hicolor/scalable/apps
dist_svgicon_DATA = desktoptracks.svg


gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(datadir)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

#desktop_in_files= desktoptracks.desktop.in
#desktop_files=$(desktop_in_files:.desktop.in=.desktop)


#@INTLTOOL_DESKTOP_RULE@

#Applicationsdir = $(datadir)/applications
#Applications_DATA = $(desktop_files)

EXTRA_DIST = $(desktop_in_files) $(desktop_files)

DISTCLEANFILES = $(desktop_files)
