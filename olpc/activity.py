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

