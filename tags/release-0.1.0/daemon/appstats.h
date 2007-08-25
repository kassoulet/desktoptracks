/***************************************************************************
 *  appstats.h
 *  Author:  Gautier Portet <kassoulet gmail com>
 *  Copyright (C) 2007 Gautier Portet
 * 
 *  Get focused applications, and manage the statistics
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


typedef struct 
{
	GString* app_name;
	guint app_time;
} AppStats;

void appstats_free(GArray* appstats);

void stats_init();
void stats_free();
void stats_save();
void stats_load();
void stats_clear();

GArray* stats_get_apps();

void stats_update(gint seconds);

//void stats_free();
void stats_increase_application_time(const gchar* app_name, gint seconds);
void stats_increase_idle_time(gint seconds);



