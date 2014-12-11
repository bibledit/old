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


#include <help/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>


const char * help_index_url ()
{
  return "help/index";
}


bool help_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_index (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("Help"), webserver_request, "");

  Assets_View view = Assets_View (0);

  view.set_variable ("title", gettext("Help"));
  view.set_variable ("introduction", gettext("Introduction"));
  view.set_variable ("installation", gettext("Installation"));
  view.set_variable ("methodology", gettext("Methodology"));
  view.set_variable ("navigation", gettext("Navigation"));
  view.set_variable ("shortcuts", gettext("Keyboard shortcuts"));
  view.set_variable ("consultations", gettext("Consultations"));
  view.set_variable ("consistency", gettext("Consistency"));
  view.set_variable ("checks", gettext("Checks"));
  view.set_variable ("teams", gettext("Teams"));
  view.set_variable ("typesetting", gettext("Typesetting"));
  view.set_variable ("tipstricks", gettext("Tips and tricks"));
  view.set_variable ("about", gettext("About"));

  page += view.render ("help", "index");

  page += Assets_Page::footer ();

  return page;
}
