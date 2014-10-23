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
#include <sqlite3.h>
#include <webserver/request.h>


using namespace std;


class Session_Logic
{
public:
  Session_Logic (Webserver_Request * request_in);
  ~Session_Logic ();
  void setUsername (string name);
  bool attemptLogin (string user_or_email, string password);
  bool loggedIn ();
  string currentUser ();
  int currentLevel (bool force = false);
  void logout ();
private:
  int level = 0;               // The level of the user.
  int check_ip_blocks = 3;     // How many numbers from IP use in fingerprint?
  bool logged_in;              // Whether user is logged in.
  string username;             // The username.
  Webserver_Request * request; // The web request.
  void Open ();
  bool openAccess ();
  string remoteAddress ();
  string fingerprint ();
  bool clientAccess ();
};


