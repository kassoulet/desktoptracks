SUBDIRS = daemon frontend data

EXTRA_DIST = autogen.sh
DISTCLEANFILES = 

release: dist
	svn -m "release $(VERSION)" copy . https://kassoulet@desktoptracks.googlecode.com/svn/tags/release-$(VERSION) 
