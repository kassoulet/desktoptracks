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

#include "desktoptracksd.h"
#include "desktoptracksd-dbus-glue.h"
#include "appstats.h"

#define DBUS_STRUCT_STRING_UINT (dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_UINT, G_TYPE_INVALID))


G_DEFINE_TYPE(DesktopTracks, desktoptracks, G_TYPE_OBJECT);

gint update_time = 0;
#define update_save = (60 *10) 

void desktoptracks_class_init(DesktopTracksClass *class) 
{
	// Nothing here
}

void desktoptracks_init(DesktopTracks *server) 
{
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	int request_ret;
	
	// Initialise the DBus connection
	server->connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (server->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free(error);
		return;
	}
	
	dbus_g_object_type_install_info(desktoptracks_get_type(), &dbus_glib_desktoptracks_object_info);
	
	// Register DBUS path
	dbus_g_connection_register_g_object(server->connection, "/org/kassoulet/DesktopTracks", G_OBJECT(server));

	// Register the service name, the constants here are defined in dbus-glib-bindings.h
	driver_proxy = dbus_g_proxy_new_for_name(server->connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy, "org.kassoulet.DesktopTracks", 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free(error);
	}
	
	g_object_unref(driver_proxy);
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
	g_print("DesktopTracks::getAppStats().\n");
	
	g_return_val_if_fail (obj != NULL, FALSE);
	g_return_val_if_fail (stats_data != NULL, FALSE);
	
	*stats_data = g_ptr_array_new ();

	GValue *value;

	GArray* appstats = stats_get_apps();
	guint i;
	
	for(i = 0; i < appstats->len; i++)
	{
		AppStats *stats;
		stats = g_array_index(appstats, AppStats*, i);
		
		//printf("getAppStats: %s %d\n", stats->app_name->str, stats->app_time);
		add_application_time (*stats_data, stats->app_name->str, stats->app_time);
	}

	appstats_free(appstats);

	return TRUE;
}


gboolean update(gpointer data)
{
	stats_update(1);
	
	update_time += 1;
	
	if (update_time > update_save) {
		update_time = 0;
		stats_save();
	}
		
	return TRUE;
}

GMainLoop* main_loop;

void signal_handler(int signo)
{
	g_print("Exiting...\n");
		
	g_main_loop_quit(main_loop);
	stats_save();
	stats_free();
}


int main (int argc, char *argv[]) 
{
	DesktopTracks *server;
	GKeyFile* keyfile;
	gchar* filename;
	gchar* contents;
	
	g_print("Starting DesktopTracks daemon " VERSION "\n");
	
	g_type_init();
	
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	
	stats_init();
	stats_load();
	g_timeout_add(1000, update, NULL);
	
	
	server = g_object_new(desktoptracks_get_type(), NULL);
	
	main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(main_loop);	
	
	return 0;
}

