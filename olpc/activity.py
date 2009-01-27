/*
 ** Copyright (©) 2003-2009 FSF.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

import sys
import os
import os.path
import logging
import ConfigParser

import gtk
import pango

import sugar
from sugar.activity import activity

class BibleditActivity(activity.Activity):

  def __init__(self, handle):
    activity.Activity.__init__(self, handle)
        
    self.set_title('Bibledit')
        
#    toolbox = activity.ActivityToolbox(self)
#    self.set_toolbox(toolbox)
#    toolbox.show()

    socket = gtk.Socket()
    self.set_canvas(socket)
    self.show_all()

    socket.connect("plug-removed", gtk.main_quit)
        
    os.system("PATH=/home/olpc/Activities/Bibledit.activity/bin:$PATH bibledit --xembed %d &" % socket.get_id())

