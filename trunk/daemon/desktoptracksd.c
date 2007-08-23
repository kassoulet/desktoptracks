/***************************************************************************
 *  desktoptracksd.c
 *  Author:  Gautier Portet <kassoulet gmail com>
 *  Copyright (C) 2007 Gautier Portet
 * 
 *  dbus daemon
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include<signal.h>
//#include <stdio.h>
//#include <string.h>
#include <glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <config.h>
#include <gtk/gtk.h>

#include "desktoptracksd.h"
#include "desktoptracksd-dbus-glue.h"
#include "appstats.h"

#define DBUS_STRUCT_STRING_UINT (dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_UINT, G_TYPE_INVALID))
#define STATUS_ICON_FILENAME DATADIR "/desktoptracks-status.png"

GtkStatusIcon *status_icon = NULL;
GtkWidget *menu;
GtkWidget *menu_item;
	
G_DEFINE_TYPE(DesktopTracks, desktoptracks, G_TYPE_OBJECT);

gint update_time = 0;
#define UPDATE_SAVE (60 *10) 

void cb_status_icon_activate(GtkStatusIcon *status_icon, gpointer user_data);
void cb_status_icons_popup_menu(GtkStatusIcon *status_icon,
                    guint          button,
                    guint          activate_time,
                    gpointer       user_data);
void cb_menu_quit_activate(GtkMenuItem *item, gpointer user_data);
void cb_menu_about_activate(GtkMenuItem *item, gpointer user_data);

GMainLoop* main_loop;

void desktoptracks_class_init(DesktopTracksClass *class) 
{
	// Nothing here
}

void desktoptracks_init(DesktopTracks *server) 
{
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	guint request_ret;
	
	// Initialise the DBus connection
	server->connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (server->connection == NULL) {
		g_warning("Unable to connect to dbus: %s.", error->message);
		g_error_free(error);
		return;
	}
	
	dbus_g_object_type_install_info(desktoptracks_get_type(), &dbus_glib_desktoptracks_object_info);
	
	// Register DBUS path
	dbus_g_connection_register_g_object(server->connection, "/org/kassoulet/DesktopTracks", G_OBJECT(server));

	// Register the service name, the constants here are defined in dbus-glib-bindings.h
	driver_proxy = dbus_g_proxy_new_for_name(server->connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy, "org.kassoulet.DesktopTracks", 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s.", error->message);
		g_error_free(error);
	}
	
	g_object_unref(driver_proxy);
	
	// Status Icon
	status_icon = gtk_status_icon_new_from_file(STATUS_ICON_FILENAME);
	if (!status_icon) {
		g_warning("Cannot create status icon.");
	}
	gtk_status_icon_set_tooltip(status_icon, "DesktopTracks");
	gtk_status_icon_set_visible(status_icon, TRUE);
	
	g_signal_connect (G_OBJECT(status_icon), "activate",
		GTK_SIGNAL_FUNC (cb_status_icon_activate),
		NULL);
	g_signal_connect (G_OBJECT(status_icon), "popup-menu",
		GTK_SIGNAL_FUNC (cb_status_icons_popup_menu),
		NULL);
		
	menu = gtk_menu_new();
	
	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	gtk_widget_show (menu_item);
	g_signal_connect (G_OBJECT(menu_item), "activate",
	GTK_SIGNAL_FUNC (cb_menu_about_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_item));

	menu_item = gtk_separator_menu_item_new();
	gtk_widget_show (menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_item));
	
	
	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
	gtk_widget_show (menu_item);
	g_signal_connect (G_OBJECT(menu_item), "activate",
	GTK_SIGNAL_FUNC (cb_menu_quit_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menu_item));
}

void desktoptracks_destroy() 
{
	/*g_free(menu_item);
	g_free(menu);
	g_free(status_icon);*/
}

void cb_menu_about_activate(GtkMenuItem *item, gpointer user_data)
{
	gchar *authors[] = { "Gautier Portet", NULL };
	
	gtk_show_about_dialog (NULL, 
		"program-name", "DesktopTracks",
		"comments", "Desktop Big Browser",
		"logo-icon-name", "desktoptracks",
		"title", "About",
		"authors", authors,
		"copyright", "© 2007 Gautier Portet",
		"version", VERSION,
		"website", "http://code.google.com/p/desktoptracks/",
		NULL);
}

void cb_menu_quit_activate(GtkMenuItem *item, gpointer user_data)
{
	g_main_loop_quit(main_loop);
}

void cb_status_icon_activate(GtkStatusIcon *status_icon, gpointer user_data)
{
	GError *error;
	if (! g_spawn_command_line_async(BINDIR "/desktoptracks", &error)) {
		g_print("error");
	}
}

void cb_status_icons_popup_menu(GtkStatusIcon *status_icon,
                    guint          button,
                    guint          activate_time,
                    gpointer       user_data)
{
	gtk_menu_popup(GTK_MENU(menu),
		NULL, NULL, NULL, NULL, button, activate_time);
}



gboolean desktoptracks_get_stats(DesktopTracks *obj, gchar **stats, GError **error)
{
	g_print("DesktopTracks::getStats().\n");


	*stats = g_strdup("DEPRECATED");
	
	return TRUE;
}


static void add_application_time(GPtrArray *array, const gchar *app_name, guint app_time)
{
	GValue *value;

	value = g_new0 (GValue, 1);
	g_value_init (value, DBUS_STRUCT_STRING_UINT);
	g_value_take_boxed (value, dbus_g_type_specialized_construct (DBUS_STRUCT_STRING_UINT));
	dbus_g_type_struct_set (value, 0, app_name, 1, app_time, G_MAXUINT);
	g_ptr_array_add (array, g_value_get_boxed (value));
	g_free (value);
}


gboolean desktoptracks_get_app_stats(DesktopTracks *obj, GPtrArray **stats_data, GError **error)
{
	g_return_val_if_fail (obj != NULL, FALSE);
	g_return_val_if_fail (stats_data != NULL, FALSE);
	
	*stats_data = g_ptr_array_new ();

	GArray* appstats = stats_get_apps();
	guint i;
	
	for(i = 0; i < appstats->len; i++)
	{
		AppStats *stats;
		stats = g_array_index(appstats, AppStats*, i);
		add_application_time (*stats_data, stats->app_name->str, stats->app_time);
	}

	appstats_free(appstats);

	return TRUE;
}

gboolean desktoptracks_clear_stats(DesktopTracks *obj, GError **error)
{
	g_return_val_if_fail (obj != NULL, FALSE);
	
	stats_clear();
	
	return TRUE;
}

gint sort_stats(const AppStats* s1, const AppStats* s2)
{
	if (s1->app_time > s2->app_time)
		return 1;
	if (s1->app_time < s2->app_time)
		return -1;
	return 0;
}

gboolean update(gpointer data)
{
	stats_update(1);
	
	update_time += 1;
	
	if (update_time > UPDATE_SAVE) {
		update_time = 0;
		stats_save();
	}
	
	
	GArray* appstats = stats_get_apps();
	guint i, len;
	GString *tooltip;
	
	tooltip = g_string_new("DesktopTracks\n");
	
	g_array_sort(appstats, sort_stats);
	
	const guint max = 8;
	guint total = 0, others = 0;
	
	for(i = 0; i < appstats->len; i++) {
		AppStats *stats;
		stats = g_array_index(appstats, AppStats*, i);
		
		total += stats->app_time;
		if (i > max) {
			others += stats->app_time;
		}
	}
	
	len = MIN(max, appstats->len);
	
	for(i = 0; i < len; i++) {
		AppStats *stats;
		stats = g_array_index(appstats, AppStats*, i);
		g_string_append_printf(tooltip, "\n%s (%d%%) %d", stats->app_name->str,
			100 * stats->app_time / total, stats->app_time);
	}
	g_string_append_printf(tooltip, "\nothers: %d%%", 100 * others / total);

	gtk_status_icon_set_tooltip(status_icon, tooltip->str);

	appstats_free(appstats);
	//g_free(tooltip);
	
	return TRUE;
}



void signal_handler(int signo)
{
	g_print("Exiting...\n");
		
	g_main_loop_quit(main_loop);
	stats_save();
	stats_free();
}


int main (int argc, char *argv[]) 
{
	/*if (fork()) {
		return 0;
	}*/
	
	DesktopTracks *server;
	GKeyFile* keyfile;
	gchar* filename;
	gchar* contents;
	
	g_print("Starting DesktopTracks daemon " VERSION "\n");
	
	g_type_init();
	gtk_init(&argc, &argv);
	
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	
	stats_init();
	stats_load();
	g_timeout_add(1000, update, NULL);
	
	
	server = g_object_new(desktoptracks_get_type(), NULL);
	
	main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(main_loop);	
	
	desktoptracks_destroy();
	
	return 0;
}

