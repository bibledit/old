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


#ifndef INCLUDED_DATABASE_CONFIG_GENERAL_H
#define INCLUDED_DATABASE_CONFIG_GENERAL_H


#include <config/libraries.h>
#include <webserver/request.h>


class Database_Config_General
{
public:
  Database_Config_General ();
  ~Database_Config_General ();
  static string getSiteMailName();
  static void setSiteMailName (string value);
  static string getSiteMailAddress();
  static void setSiteMailAddress (string value);
  static string getMailStorageHost();
  static void setMailStorageHost (string value);
  static string getMailStorageUsername();
  static void setMailStorageUsername (string value);
  static string getMailStoragePassword();
  static void setMailStoragePassword (string value);
  static string getMailStorageSecurity();
  static void setMailStorageSecurity (string value);
  static string getMailStoragePort();
  static void setMailStoragePort (string value);
  static string getMailSendHost();
  static void setMailSendHost (string value);
  static string getMailSendAuthentication();
  static void setMailSendAuthentication (string value);
  static string getMailSendUsername();
  static void setMailSendUsername (string value);
  static string getMailSendPassword();
  static void setMailSendPassword (string value);
  static string getMailSendSecurity();
  static void setMailSendSecurity (string value);
  static string getMailSendPort();
  static void setMailSendPort (string value);
  static string getTimerMinute();
  static void setTimerMinute (string value);
  static string getTimezone();
  static void setTimezone (string value);
  static string getSiteURL();
  static void setSiteURL (string value);
  static string getSiteLanguage ();
  static void setSiteLanguage (string value);
  static string getClientMode ();
  static void setClientMode (string value);
  static string getServerAddress ();
  static void setServerAddress (string value);
  static string getRepeatSendReceive ();
  static void setRepeatSendReceive (string value);
  static string getInstalledVersion ();
  static void setInstalledVersion (string value);
private:
  static string file (const char * key);
  static string getValue (const char * key, const char * default_value);
  static void setValue (const char * key, string value);
  vector <string> getList (const char * key);
  static void setList (const char * key, vector <string> values);
};


#endif
