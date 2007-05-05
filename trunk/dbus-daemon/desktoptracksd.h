/***************************************************************************
 *  desktoptracksd.h
 *  Author:  Gautier Portet <kassoulet gmail com>
 *  Copyright (C) 2007 Gautier Portet
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

#ifndef _DESKTOPTRACKSD_H
#define _DESKTOPTRACKSD_H

typedef struct
{
	GObject parent;
	DBusGConnection *connection;
} DesktopTracks;

typedef struct
{
	GObjectClass parent_class;
} DesktopTracksClass;

static void desktoptracks_init(DesktopTracks *server);
static void desktoptracks_class_init(DesktopTracksClass *class);

gboolean desktoptracks_get_stats(DesktopTracks *obj, gchar **stats, GError **error);

#endif
