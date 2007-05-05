#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

Display *display;

static unsigned char xerror;
static int (*old_handler) (Display *, XErrorEvent *);


#define STATS_FILE "desktoptracks.stats"

int xerror_handler(Display * d, XErrorEvent * ev)
{
	xerror = ev->error_code;
	return True;
}

static void set_trap()
{
	xerror = 0;
	old_handler = XSetErrorHandler(xerror_handler);
}

static int remove_trap()
{
	XSetErrorHandler(old_handler);
	return xerror;
}

//#define MAX 500
GString* get_focused_application()
{
	Window wx;
	int i;
	Window parent_win;
	Window root_win;
	Window *dummy_child_list;
	Window w;
	unsigned int dummy_n_children;
	char *n;

	XGetInputFocus(display, &wx, &i);	
	/* wx == the current focused window in the following */
	
	/* XGetInputFocus returns RevertToPointerRoot for most windows * but
	   RevertToParent for GTK+ 2 windows.  In the latter case * the window 
	   name is given by a parent window.  We look up the * tree to find a
	   window title below. */

	// proceed to get first valid window name of nearest parent
	root_win = 0;
	for (w = wx; root_win == 0 || w != root_win; w = parent_win) {
		set_trap();
		i = XFetchName(display, w, &n);
		if (remove_trap() == BadWindow) {
			/* 
			 * Wait for next timeout. Let bad window be gc"d.
			 */
			break;
		} else {
			if (i) {
				// XFetchName succeeded
				break;
			}
		}
		XQueryTree(display, w, &root_win, &parent_win,
			   &dummy_child_list, &dummy_n_children);
	}

	if (i) {	
		// last XFetchName succeeded
		//printf("%x -> %s\n", wx, n);
		XFree(n);
	}

	int status;
	XClassHint class_hints_return;

	GString* str = NULL;

	status = XGetClassHint(display, w, &class_hints_return);
/*	printf("name: '%s' class: '%s' (%d)\n",
	       class_hints_return.res_name,
	       class_hints_return.res_class, status); */
	if (strcmp(class_hints_return.res_name, "desktop_window") != 0) {
		// Not the desktop window
		//strcpy(name, class_hints_return.res_class);

		// TODO: also remove the frontend window
		str = g_string_new(class_hints_return.res_class);
	}

	XFree(class_hints_return.res_name);
	XFree(class_hints_return.res_class);
	
	return str;
}


GKeyFile* keyfile = NULL;


void stats_init()
{
	g_print("connecting to display %s\n", XDisplayName(0));
	
	if (!(display = XOpenDisplay(0))) {
		fprintf(stderr, "can't open display %s\n", XDisplayName(0));
		exit(1);
	}

	keyfile = g_key_file_new();
}

void stats_free()
{
	g_key_file_free(keyfile);
	XCloseDisplay(display);
}

static gint stats_get_application_time(const gchar* app_name)
{
	GError* error = NULL;
	gint seconds;
	
	seconds = g_key_file_get_integer(keyfile, "apps", app_name, &error);
	if ( error != NULL) {
		seconds = 0;
	}
	return seconds;
}

static void stats_set_application_time(const gchar* app_name, gint seconds)
{
	g_key_file_set_integer(keyfile, "apps", app_name, seconds);
}

static gint stats_get_idle_time()
{
	GError* error = NULL;
	gint seconds;
	
	seconds = g_key_file_get_integer(keyfile, "global", "idle_time", &error);
	if ( error != NULL) {
		seconds = 0;
	}
	return seconds;
}

static void stats_set_idle_time(gint seconds)
{
	g_key_file_set_integer(keyfile, "global", "idle_time", seconds);
}

void stats_increase_application_time(const gchar* app_name, gint add_seconds)
{
	gint seconds;
	seconds = stats_get_application_time(app_name);
	stats_set_application_time(app_name, seconds + add_seconds);
}

void stats_increase_idle_time(gint add_seconds)
{
	gint seconds;
	seconds = stats_get_idle_time();
	stats_set_idle_time(seconds + add_seconds);
}

void stats_update(gint seconds)
{
	GString *appname;
	appname = get_focused_application();
	if (appname) {
		stats_increase_application_time(appname->str, seconds);
		g_string_free(appname, TRUE);
	} else {
		stats_increase_idle_time(seconds);
	}
}

void stats_save()
{
	gchar* filename;
	gchar* contents;

	contents = g_key_file_to_data(keyfile, NULL, NULL);
	filename = g_build_filename(g_get_user_config_dir(), STATS_FILE, NULL);
	
	// TODO: error checking
	g_file_set_contents(filename, contents, -1, NULL);

	//g_print("stats written in '%s'\n", filename);

	g_free(filename);
	g_free(contents);
}

void stats_load()
{
	gchar* filename;
	
	filename = g_build_filename(g_get_user_config_dir(), STATS_FILE, NULL);
	if (! g_key_file_load_from_file(keyfile, filename,  G_KEY_FILE_NONE, NULL) ) {
		fprintf(stderr, "can't load stats file '%s'\n", filename);
	}
}

GString* stats_get()
{
	GString* str = NULL;
	
	gchar** apps;
	GError* error;
	
	apps = g_key_file_get_keys(keyfile, "apps", NULL, &error);
	if (apps && *apps) {
		str = g_string_new(NULL);
		while (*apps) {
			g_string_append_printf(str, "%s\t%d\n", *apps, stats_get_application_time(*apps));
			apps++;
		}
		
	}
	
	return str;
}


