#!/usr/bin/env python

# DesktopTracks - GTK Frontend
# Copyright (C) 2007 Gautier Portet < kassoulet gmail com>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

NAME="DesktopTracks"
VERSION="@version@"
DATADIR="@datadir@"
BINDIR="@bindir@"
URL="http://code.google.com/p/desktoptracks/"

import sys, os, inspect
from random import random

import pygtk
pygtk.require('2.0')
import gobject, gtk, gtk.glade
import gnomecanvas

from gtk import gdk
from datetime import datetime
from time import time, sleep
import math
from math import cos, sin
from subprocess import Popen

try:
	import dbus
except ImportError:
	print "python-dbus not found."
	sys.exit(1)

try:	
	bus = dbus.SessionBus()
except :
	print "cannot connect to D-Bus."
	sys.exit(2)

daemon = None
retries = 3
while retries:
	try:
		daemon = bus.get_object('org.kassoulet.DesktopTracks', '/org/kassoulet/DesktopTracks')
		retries = None
	except dbus.DBusException:
		print("trying to start DesktopTracks D-Bus daemon...")
		gobject.spawn_async([BINDIR + "/desktoptracksd"])
		retries -= 1
		sleep(1)
		

#Tango colors taken from 
#http://tango.freedesktop.org/Tango_Icon_Theme_Guidelines
TANGO_COLOR_BUTTER_LIGHT	 = "fce94f"
TANGO_COLOR_BUTTER_MID		 = "edd400"
TANGO_COLOR_BUTTER_DARK		 = "c4a000"
TANGO_COLOR_ORANGE_LIGHT	 = "fcaf3e"
TANGO_COLOR_ORANGE_MID		 = "f57900"
TANGO_COLOR_ORANGE_DARK		 = "ce5c00"
TANGO_COLOR_CHOCOLATE_LIGHT  = "e9b96e"
TANGO_COLOR_CHOCOLATE_MID	 = "c17d11"
TANGO_COLOR_CHOCOLATE_DARK	 = "8f5902"
TANGO_COLOR_CHAMELEON_LIGHT  = "8ae234"
TANGO_COLOR_CHAMELEON_MID	 = "73d216"
TANGO_COLOR_CHAMELEON_DARK	 = "4e9a06"
TANGO_COLOR_SKYBLUE_LIGHT	 = "729fcf"
TANGO_COLOR_SKYBLUE_MID		 = "3465a4"
TANGO_COLOR_SKYBLUE_DARK	 = "204a87"
TANGO_COLOR_PLUM_LIGHT		 = "ad7fa8"
TANGO_COLOR_PLUM_MID		 = "75507b"
TANGO_COLOR_PLUM_DARK		 = "5c3566"
TANGO_COLOR_SCARLETRED_LIGHT = "ef2929"
TANGO_COLOR_SCARLETRED_MID	 = "cc0000"
TANGO_COLOR_SCARLETRED_DARK  = "a40000"
TANGO_COLOR_ALUMINIUM1_LIGHT = "eeeeec"
TANGO_COLOR_ALUMINIUM1_MID	 = "d3d7cf"
TANGO_COLOR_ALUMINIUM1_DARK  = "babdb6"
TANGO_COLOR_ALUMINIUM2_LIGHT = "888a85"
TANGO_COLOR_ALUMINIUM2_MID	 = "555753"
TANGO_COLOR_ALUMINIUM2_DARK  = "2e3436"

chart_colors = (
	TANGO_COLOR_SCARLETRED_MID,
	TANGO_COLOR_PLUM_MID,
	TANGO_COLOR_SKYBLUE_MID,
	TANGO_COLOR_CHAMELEON_MID,
	TANGO_COLOR_CHOCOLATE_MID,
	TANGO_COLOR_ORANGE_MID,
	TANGO_COLOR_BUTTER_MID,

	TANGO_COLOR_SCARLETRED_LIGHT,
	TANGO_COLOR_PLUM_LIGHT,
	TANGO_COLOR_SKYBLUE_LIGHT,
	TANGO_COLOR_CHAMELEON_LIGHT,
	TANGO_COLOR_CHOCOLATE_LIGHT,
	TANGO_COLOR_ORANGE_LIGHT,
	TANGO_COLOR_BUTTER_LIGHT,
	
)

border_colors = (
	TANGO_COLOR_ALUMINIUM1_LIGHT,
	
	TANGO_COLOR_SCARLETRED_DARK,
	TANGO_COLOR_PLUM_DARK,
	TANGO_COLOR_SKYBLUE_DARK,
	TANGO_COLOR_CHAMELEON_DARK,
	TANGO_COLOR_CHOCOLATE_DARK,
	TANGO_COLOR_ORANGE_DARK,
	TANGO_COLOR_BUTTER_DARK,
	
	TANGO_COLOR_SCARLETRED_MID,
	TANGO_COLOR_PLUM_MID,
	TANGO_COLOR_SKYBLUE_MID,
	TANGO_COLOR_CHAMELEON_MID,
	TANGO_COLOR_CHOCOLATE_MID,
	TANGO_COLOR_ORANGE_MID,
	TANGO_COLOR_BUTTER_MID,
)

hover_colors = (
	TANGO_COLOR_SCARLETRED_LIGHT,
	TANGO_COLOR_PLUM_LIGHT,
	TANGO_COLOR_SKYBLUE_LIGHT,
	TANGO_COLOR_CHAMELEON_LIGHT,
	TANGO_COLOR_CHOCOLATE_LIGHT,
	TANGO_COLOR_ORANGE_LIGHT,
	TANGO_COLOR_BUTTER_LIGHT,
	
	TANGO_COLOR_SCARLETRED_MID,
	TANGO_COLOR_PLUM_MID,
	TANGO_COLOR_SKYBLUE_MID,
	TANGO_COLOR_CHAMELEON_MID,
	TANGO_COLOR_CHOCOLATE_MID,
	TANGO_COLOR_ORANGE_MID,
	TANGO_COLOR_BUTTER_MID,
)

def hex_to_rgb(color):
	r = int(color[0:2],16) / 255.0
	g = int(color[2:4],16) / 255.0
	b = int(color[4:6],16) / 255.0
	return r,g,b

chart_colors = [hex_to_rgb(c) for c in chart_colors]
border_colors = [hex_to_rgb(c) for c in border_colors]
hover_colors = [hex_to_rgb(c) for c in hover_colors]
outline_color = hex_to_rgb(TANGO_COLOR_ALUMINIUM1_DARK)
border_color = hex_to_rgb(TANGO_COLOR_ALUMINIUM1_LIGHT)

def seconds_for_display(secs):
	d = (
		(60*60*24, '%dd'),
		(60*60, '%02dh'),
		(60, '%02dm'),
		(1, '%02ds'),
	)
	s = []
	if secs == 0:
		return "0"
	for duration, pattern in d:
		if secs >= duration:
			s.append(pattern % (secs/duration))
			secs -= secs/duration * duration
	s = " ".join(s[:2])
	if s[0] == "0":
		s = s[1:]
	return s

class PieChart(gtk.DrawingArea):
	# signals
	__gsignals__ = dict(time_changed=(gobject.SIGNAL_RUN_FIRST,
									  gobject.TYPE_NONE,
									  (gobject.TYPE_INT, gobject.TYPE_INT)))

	def __init__(self):
		super(PieChart, self).__init__()

		# gtk.Widget signals
		self.connect("expose_event", self.expose)
		self.connect("motion_notify_event", self.motion_notify)

		# make them private
		self._time = None # the time on the clock face
		self._minute_offset = 0 # the offset of the minutes hand
		self._dragging = False # true if the interface is being dragged

		# unmask events
		self.add_events(gdk.BUTTON_PRESS_MASK |
						gdk.BUTTON_RELEASE_MASK |
						gdk.POINTER_MOTION_MASK)

		self.update()
		# update the clock once a second
		gobject.timeout_add(100, self.update)
		self.a = 0.0
		self.x = 0
		self.y = 0

		self._last_redraw = 0
		self.items = []
		self.legends = []
		self.values = []
  
	def set_items(self, items):
		
		self.items = items[:]
		self.legends = []
		self.values = []
		for k,v in self.items:
			self.legends.append(k)
			self.values.append(v)


	def expose(self, widget, event):
		context = widget.window.cairo_create()

		# set a clip region for the expose event
		context.rectangle(event.area.x, event.area.y,
						  event.area.width, event.area.height)
		context.clip()

		self.draw(context)

		return False


	def motion_notify(self, widget, event):
		self.x = event.x
		self.y = event.y
		
		now = time()
		if now > self._last_redraw + 0.02:
			self._last_redraw = now
			self.redraw_canvas()

		if self._dragging:
			self.emit_time_changed_signal(event.x, event.y)



	def draw(self, context):
		#print "draw()"

		rect = self.get_allocation()

		x = rect.x + rect.width / 2.0
		y = rect.y + rect.height / 2.0
		radius = min(rect.width / 2.0, rect.height / 2.0) - 8

		s = self.values
		a = -25.0
		c = 0
		active = None
		alone = len(self.values) == 1
		
		if len(self.values) < 1:
			context.arc(x, y, radius, 0, 2.0 * math.pi)
			context.set_source_rgb(0, 0, 1)
			#context.fill_preserve()
			context.set_source_rgba(1, 0, 0,0.5)
			context.stroke()
			return
		
		for name, p in self.items:
			zz = p
			start = a
			size = zz
			a += zz
			
			context.save()
			context.move_to(x, y)
			context.arc(x, y, radius , start * 2.0 * math.pi, 
				(start+size) * 2.0 * math.pi)
			if not alone:
				context.line_to(x, y)
			if context.in_fill(self.x, self.y):
				active = name
				r,g,b = hover_colors[c]
			else:
				r,g,b = chart_colors[c]
			context.set_source_rgb( r,g,b )
			context.fill_preserve()
			r,g,b = border_colors[0]
			r,g,b = border_color
			context.set_source_rgb( r,g,b )
			if alone:
				context.set_source_rgba( r,g,b,0 )
				
			context.set_line_width(3)
			context.stroke()
			
			context.restore()
			c += 1
			
			
		context.arc(x, y, radius, 0, 2.0 * math.pi)
		context.set_source_rgba(1,1,1,0.5)
		context.set_line_width(2)
		context.stroke()
		
		context.arc(x, y, radius+2, 0, 2.0 * math.pi)
		r,g,b = outline_color
		context.set_source_rgb(r,g,b)
		context.set_line_width(2)
		context.stroke()
		context.save()

		a = -25
		for name, p in self.items:
			zz = p
			start = a
			size = zz
			a += zz
			
			if not name == active:
				continue
				
			cx = x+0.6*radius*cos((start+size/2) * 2.0 * math.pi)
			cy = y+0.6*radius*sin((start+size/2) * 2.0 * math.pi)

			if alone:
				cx, cy = x,y

			s1 = "%d%%" % int(p*100)
			context.set_font_size(16)
			xbearing, ybearing, width1, height1, xadvance, yadvance = context.text_extents(s1)

			s2 = name
			context.set_font_size(12)
			xbearing, ybearing, width2, height2, xadvance, yadvance = context.text_extents(s2)

			width = max(width1, width2) + 8
			height = height1 + height2 + 8
			
			context.set_source_rgba(1,1,1,0.5)
			context.rectangle(cx-width/2, cy-height1-4 , width, height)
			context.fill_preserve()
			context.set_source_rgba(0,0,0,0.8)
			context.set_line_width(1)
			context.stroke()
			
			context.set_source_rgba(0,0,0,0.8)
			context.set_font_size(16)
			context.move_to(cx-width1/2, cy )
			context.show_text(s1)
			context.fill()
			
			context.set_source_rgba(0,0,0,0.9)
			context.set_font_size(12)
			context.move_to(cx-width2/2, cy+height1)
			context.show_text(s2)
			context.fill()
			
			
		context.restore()
			
		context.save()
		context.set_font_size(48)
		s = "DesktopTracks"
		xbearing, ybearing, width, height, xadvance, yadvance = context.text_extents(s)
		context.move_to(x-width/2, y+radius+40)
		context.set_source_rgba( 0,0,0 , 0.05)
		self.a = min(self.a + 0.1, 1.0)
		context.show_text(s)
		context.fill()
		context.restore()


	def redraw_canvas(self):
		if self.window:
			alloc = self.get_allocation()
			#rect = gdk.Rectangle(alloc.x, alloc.y, alloc.width, alloc.height)
			#self.window.invalidate_rect(rect, True)
			self.queue_draw_area(alloc.x, alloc.y, alloc.width, alloc.height)
			self.window.process_updates(True)

	def update(self):
		
		self.redraw_canvas()


		return True # keep running this event


class DesktopTracksWindow:

	def __init__(self, daemon=None):
		self.error = None
		self.window = None
		
		if not daemon:
			self.critical_error("DesktopTracks D-Bus daemon is not responding or not running.")
		self.daemon = daemon
			

	def critical_error(self, msg):
		self.error = True
		if self.window:
			self.window.set_sensitive(False)
		gtk.MessageDialog(parent=self.window, flags=0, 
		type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_CLOSE, 
		message_format=msg).run()
		sys.exit(1)
			
	def update_stats(self):
		if self.error:
			return False
		
		self.liststore.clear()
		r = self.daemon.getAppStats()
				
		r.sort(key=lambda x: -x[1])
		
		items = []
		count = 6
		values = [int(v[1]) for v in r]

		rest = sum(values[count:])

		if rest:
			values = values[:count] 
			values.append(rest)

		total = sum(values)
		values = [float(v)/total for v in values]
		
		for f in r:
			iter = self.liststore.append()
			self.liststore.set(iter, 0, f[0])
			self.liststore.set(iter, 1, seconds_for_display(f[1]))
		
		i = 0
		for f in r[:count]:
			items.append( (f[0],values[i]) )
			i += 1
		if rest:
			items.append( ("Others",values[i]) )
		self.chart.set_items(items)
		
		self.chart.update()
		
		return True

	def on_refresh_clicked(self,*args):
		self.update_stats()
		
	def on_clear_clicked(self,*args):
		self.daemon.clearStats()
		self.update_stats()

	def connect(self, glade, objects):
		dicts = {}
		for o in [self] + objects:
			for name, member in inspect.getmembers(o):
				dicts[name] = member
		glade.signal_autoconnect(dicts)


	def item_event(self, widget, event=None):
		if event.type == gtk.gdk.ENTER_NOTIFY:
			# Make the outline wide.
			widget.set(width_units=3)
			return True

		elif event.type == gtk.gdk.LEAVE_NOTIFY:
			# Make the outline thin.
			widget.set(width_units=1)
			return True
		elif event.type == gtk.gdk.BUTTON_PRESS:
			if event.button == 1:
				# Remember starting position.
				widget.move(event.x, event.y)
				return True
			
			
			
		return False

	def on_delete_event(self, widget, event, data=None):
		return False

	# Another callback
	def on_destroy(self, widget, data=None):
		gtk.main_quit()

	def on_chart_toggled(self, widget, *args):
		if widget.get_active():
			self.notebook.set_current_page(0)
		
	def on_stats_toggled(self, widget, *args):
		if widget.get_active():
			self.notebook.set_current_page(1)

	def show(self):

		gladefile = os.path.join(DATADIR, "%s.glade" % NAME.lower())
		self.glade = gtk.glade.XML(gladefile)
		
		self.window = self.glade.get_widget("window_main")
		self.applist = self.glade.get_widget("treeview")
		self.notebook = self.glade.get_widget("notebook")

		column = gtk.TreeViewColumn("Application", gtk.CellRendererText(), text=0)
		column.set_sort_column_id(0)
		self.applist.append_column(column)

		column = gtk.TreeViewColumn("Time", gtk.CellRendererText(), text=1)
		column.set_sort_column_id(1)
		self.applist.append_column(column)
		
		self.liststore = gtk.ListStore(gobject.TYPE_STRING, gobject.TYPE_STRING)
		self.applist.set_model(self.liststore)
 
		self.connect(self.glade, [])

		self.window.show()

		canvas = self.glade.get_widget("canvas_container")

		chart = PieChart()
		canvas.add(chart)
		chart.show_all()
		canvas.show_all()
		self.chart = chart

		gobject.timeout_add( 1000, self.update_stats)
		self.update_stats()
		

w = DesktopTracksWindow(daemon)
w.show()

gtk.main()












