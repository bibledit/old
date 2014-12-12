/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <help/installosx.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/builder.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_installosx_url ()
{
  return "help/installosx";
}


bool help_installosx_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_installosx (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("Installation"), webserver_request, "");

  Assets_View view = Assets_View (0);
  Assets_Builder builder;

  view.set_variable ("title", gettext("Install Bibledit on OSX"));

  builder.clear();
  builder.sentence (gettext("This assumes a standard installation of the operating system."));
  builder.sentence (gettext("Bibledit runs on all recent versions of OSX."));
  builder.sentence (gettext("The steps below have been tested on OSX 10.7 Lion."));
  builder.sentence (gettext("Open a terminal to enter some of the commands:"));
  view.set_variable ("intro", builder.result());

  view.set_variable ("xcode", gettext("Install Xcode."));

  view.set_variable ("macports", gettext("Install MacPorts."));

  view.set_variable ("server", gettext("Install the Apache web server:"));

  view.set_variable ("activate", gettext("Activate Apache:"));

  view.set_variable ("verify", gettext("To verify Apache is running, point your browser to:"));
  view.set_variable ("page", gettext("The page should say:"));

  view.set_variable ("php", gettext("Install PHP plus modules:"));
  view.set_variable ("register", gettext("Register PHP with Apache:"));

  view.set_variable ("update", gettext("Update the Apache configuration:"));
  view.set_variable ("searchfor", gettext("In that file, search for:"));
  view.set_variable ("replacewith", gettext("and change it to:"));
  view.set_variable ("addline", gettext("Add the following line at the end of that file:"));
  view.set_variable ("save", gettext("Save the file."));

  view.set_variable ("configphp", gettext("Configure PHP:"));

  view.set_variable ("restart", gettext("Restart Apache for the new configuration to take effect:"));

  view.set_variable ("download", gettext("Download"));
  view.set_variable ("unzip", gettext("Unzip Bibledit into this folder:"));
  view.set_variable ("permissions", gettext("Set correct permissions on folder bibledit-web for the web server:"));

  view.set_variable ("browseto", gettext("To configure Bibledit, point your browser to:"));
  view.set_variable ("browsesteps", gettext("Follow all the steps in the browser."));

  view.set_variable ("openbibledit", gettext("To use Bibledit, point your browser to the same address:"));

  view.set_variable ("production", gettext("Once everything works, put PHP in production mode:"));

  page += view.render ("help", "installosx");

  page += Assets_Page::footer ();

  return page;
}
