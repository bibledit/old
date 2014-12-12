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


#include <help/installandroid.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/builder.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_installandroid_url ()
{
  return "help/installandroid";
}


bool help_installandroid_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_installandroid (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("Installation"), webserver_request, "");

  Assets_View view = Assets_View (0);
  Assets_Builder builder;

  view.set_variable ("title", gettext("Install Bibledit on Android"));

  builder.clear();
  builder.sentence (gettext("Bibledit runs on all recent versions of Android."));
  builder.sentence (gettext("The steps below have been tested on a Google Nexus 10 running Android 4.4."));
  view.set_variable ("intro", builder.result());

  view.set_variable ("play", gettext("Install Bibledit from Google Play:"));

  view.set_variable ("start", gettext("Start the Bibledit app."));

  builder.clear();
  builder.sentence (gettext("Install and run a web server."));
  builder.sentence (gettext("Bibledit provides instructions."));
  view.set_variable ("server", builder.result());

  view.set_variable ("open", gettext("Open Bibledit in a browser."));

  view.set_variable ("configure", gettext("Follow the steps in the browser to complete the installation."));

  builder.clear();
  builder.sentence (gettext("Connect to the demo Server to try Bibledit."));
  builder.sentence (gettext("Connect to your own production Server for production work."));
  view.set_variable ("connect", builder.result());

  page += view.render ("help", "installandroid");

  page += Assets_Page::footer ();

  return page;
}
