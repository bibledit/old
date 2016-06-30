/*
Copyright (©) 2003-2016 Teus Benschop.

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


#include <user/logic.h>
#include <filter/string.h>
#include <database/users.h>
#include <database/config/user.h>


void user_logic_software_updates_notify ()
{
  // The first check is to find out whether there's users who have enabled the notifications for software updates.
  bool notify = false;
  Database_Users database_users;
  Database_Config_User database_config_user (NULL);
  vector <string> users = database_users.getUsers ();
  for (auto & user : users) {
    if (database_config_user.getUserAllSoftwareUpdatesNotification (user)) notify = true;
    if (database_config_user.getUserYourSoftwareUpdatesNotification (user)) notify = true;
  }
  // If there's nobody to be notified, bail out.
  if (!notify) return;
  
}
