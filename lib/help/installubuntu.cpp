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


#include <help/installubuntu.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/builder.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_installubuntu_url ()
{
  return "help/installubuntu";
}


bool help_installubuntu_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_installubuntu (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("Installation"), webserver_request, "");

  Assets_View view = Assets_View (0);
  Assets_Builder builder;

  view.set_variable ("title", gettext("Install Bibledit on Ubuntu"));

  builder.clear();
  builder.sentence (gettext("Follow the steps below to install Bibledit."));
  builder.sentence (gettext("It needs some basic knowledge of working with Linux servers."));
  view.set_variable ("intro", builder.result());

  view.set_variable ("terminal", gettext("Open a terminal."));

  view.set_variable ("update", gettext("Update the software sources:"));

  view.set_variable ("tasksel", gettext("Start installing the web server:"));

  view.set_variable ("lamp", gettext("Enable the LAMP server, and install."));

  view.set_variable ("testserver1", gettext("Test the web server by opening the site in the browser, for example:"));
  view.set_variable ("testserver2", gettext("or"));
  view.set_variable ("testserver3", gettext("When the web server works, the browser displays the default Apache page."));

  view.set_variable ("mysql", gettext("Remove the MySQL server to save memory:"));

  view.set_variable ("dependencies", gettext("Install the software Bibledit relies on:"));

  view.set_variable ("reload", gettext("Reload the Apache web server so that the new modules take effect:"));

  view.set_variable ("download", gettext("Download Bibledit:"));

  view.set_variable ("", gettext("Install Bibledit in folder /var/www/html as user root:"));

  view.set_variable ("chown", gettext("Make everything to be owned by the web server user:"));

  view.set_variable ("website", gettext("Bibledit will be accessible through:"));

  view.set_variable ("open", gettext("Open the web address in the browser, and follow the steps on the screen to configure Bibledit and log in."));

  page += view.render ("help", "installubuntu");

  page += Assets_Page::footer ();

  return page;
}
