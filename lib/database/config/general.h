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


#pragma once


#include <iostream>
#include <cstdlib>
#include <vector>
#include <webserver/request.h>


using namespace std;


class Database_Config_General
{
public:
  Database_Config_General ();
  ~Database_Config_General ();
  string getSiteMailName();
  void setSiteMailName (string value);
  string getSiteMailAddress();
  void setSiteMailAddress (string value);
  string getMailStorageHost();
  void setMailStorageHost (string value);
  string getMailStorageUsername();
  void setMailStorageUsername (string value);
  string getMailStoragePassword();
  void setMailStoragePassword (string value);
  string getMailStorageSecurity();
  void setMailStorageSecurity (string value);
  string getMailStoragePort();
  void setMailStoragePort (string value);
  string getMailSendHost();
  void setMailSendHost (string value);
  string getMailSendAuthentication();
  void setMailSendAuthentication (string value);
  string getMailSendUsername();
  void setMailSendUsername (string value);
  string getMailSendPassword();
  void setMailSendPassword (string value);
  string getMailSendSecurity();
  void setMailSendSecurity (string value);
  string getMailSendPort();
  void setMailSendPort (string value);
  string getTimerMinute();
  void setTimerMinute (string value);
  string getTimezone();
  void setTimezone (string value);
  string getSiteURL();
  void setSiteURL (string value);
  string getSiteLanguage ();
  void setSiteLanguage (string value);
  string getClientMode ();
  void setClientMode (string value);
  string getServerAddress ();
  void setServerAddress (string value);
  string getRepeatSendReceive ();
  void setRepeatSendReceive (string value);
private:
  string file (const char * key);
  string getValue (const char * key, const char * default_value);
  void setValue (const char * key, string value);
  vector <string> getList (const char * key);
  void setList (const char * key, vector <string> values);
};


