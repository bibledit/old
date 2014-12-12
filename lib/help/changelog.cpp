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


#include <help/changelog.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_changelog_url ()
{
  return "help/changelog";
}


bool help_changelog_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_changelog (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("ChangeLog"), webserver_request, "");

  Assets_View view = Assets_View (0);

  page += view.render ("help", "changelog");

  page += Assets_Page::footer ();

  return page;
}
