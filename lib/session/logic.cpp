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


#include <session/logic.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <webserver/request.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <config/globals.h>


using namespace std;


/*

The PHP persistent storage of sessions works well, normally.
On shared hosts, the hosting provider may influence how PHP sessions work.
In such a case, the session mechanism does not always work as desired.
This may result in frequently being locked out.
A possible solution would be to work around this behaviour.
Bibledit-Web has another solution:
It has its own persistent session storage mechanism.
This mechanism is independent of any hosting provider or any PHP session mechanism.

The current mechanism makes the following situation possible, although not likely:
* One user logs in.
* Another user, with exactly the same signature, is automatically logged in too.
The above is undesired.
The behaviour can be prevented when one of the users sets another user agent.

To improve the persistent login mechanism, the following could be done:
1. On sending the login screen, Bibledit-Web generates a unique cookie with unique name and the correct path.
2. When the user logs in, the browser will send this new cookie.
3. Bibledit-Web stores the cookie details as it receives the cookie details through the header during login.
4. Next time when the user requests any page page, the browser will again send this cookie.
   Bibledit-Web checks the details from the submitted header with its database.
   If the details match, the user is logged in.
Optionally it sets the cookie to expire after a week, or a month, or to never expire.
Upon regenerating the cookie, it will keep storing the old cookie signature, and will also store the new cookie signature.
Both are now acceptable for the time being.
This requires two columns in the table, where the stuff is shifted from new to old, to swapped away.

After the port to C++, the PHP session mechanism is no longer a good option.

*/


Session_Logic::Session_Logic (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
  Open ();
}


Session_Logic::~Session_Logic ()
{
}


// Call this when logging in.
void Session_Logic::Open ()
{
  if (openAccess ()) return;
  if (clientAccess ()) return;

  string address = remoteAddress ();
  string agent = ((Webserver_Request *) webserver_request)->user_agent;
  Database_Users database_users = Database_Users ();
  string username = database_users.getUsername (address, agent, fingerprint ());
  if (username != "") {
    setUsername (username);
    logged_in = true;
    database_users.pingTimestamp (username);
  } else {
    setUsername ("");
    logged_in = false;
  }
}


void Session_Logic::setUsername (string name)
{
  username = name;
}


bool Session_Logic::openAccess ()
{
  // Open access if it is flagged as such.
  if (config_globals_open_installation) {
    setUsername ("admin");
    level = Filter_Roles::admin ();
    logged_in = true;
    return true;
  }
  return false;
}


// Returns IP blocks of remote address.
string Session_Logic::remoteAddress ()
{
  vector <string> blocks = filter_string_explode (((Webserver_Request *) webserver_request)->remote_address, '.');
  string address = "";
  unsigned int num_blocks = abs (check_ip_blocks);
  if (num_blocks > blocks.size ()) num_blocks = blocks.size ();
  for (unsigned int i = 0; i < num_blocks; i++) {
    address += blocks [i] + ".";
  }
  return address;
}


// Returns a fingerprint from the user's browser.
string Session_Logic::fingerprint ()
{
  string fingerprint = "";
  // fingerprint += $_SERVER ['HTTP_CONNECTION']; Unstable fingerprint. No use for persistent login.
  // fingerprint += $_SERVER ['HTTP_ACCEPT_ENCODING']; Unstable fingerprint. No use for persistent login.
  fingerprint += ((Webserver_Request *) webserver_request)->accept_language;
  return fingerprint;
}


// Attempts to log into the system.
// Returns boolean success.
bool Session_Logic::attemptLogin (string user_or_email, string password)
{
  Database_Users database = Database_Users ();
  bool login_okay = false;
  // Match username and email.
  if (database.matchUsernamePassword (user_or_email, password)) {
    login_okay = true;
  }
  // Match password and email.
  if (database.matchEmailPassword (user_or_email, password)) {
    login_okay = true;
    // Fetch username that belongs to the email address that was used to login.
    user_or_email = database.getEmailToUser (user_or_email);
  }

  if (login_okay) {
    Open ();
    setUsername (user_or_email);
    logged_in = true;
    string security1 = remoteAddress ();
    string security2 = ((Webserver_Request *) webserver_request)->user_agent;
    string security3 = fingerprint ();
    database.setTokens (user_or_email, security1, security2, security3);
    currentLevel (true);
    return true;
  }
  return false;
}


// Returns true if the user is logged in.
bool Session_Logic::loggedIn ()
{
  // The logged-in status is stored in the object, so that if it is requested twice,
  // the session system is queried only once. It has been seen on some sites that if the php session
  // system was queried more than once, it did not behave consistently.
  // Buffering the status in the object resolved this.
  // After the session system was dropped, the above comment is no longer relevant.
  // The information this comment contains remains relevant for the future.
  if (openAccess ()) return true;
  return logged_in;
}


string Session_Logic::currentUser ()
{
  return username;
}


// Returns the current level of the session as an integer.
int Session_Logic::currentLevel (bool force)
{
  if (openAccess ()) return level;
  if ((level == 0) || force) {
    if (loggedIn()) {
      Database_Users database = Database_Users();
      level = database.getUserLevel (currentUser());
    } else {
      level = Filter_Roles::guest ();
    }
  }
  return level;
}


void Session_Logic::logout ()
{
  string username = currentUser ();
  Database_Users database_users = Database_Users ();
  database_users.removeTokens (username);
  setUsername ("");
  level = Filter_Roles::guest();
}


bool Session_Logic::clientAccess ()
{
  // If client mode is prepared, 
  // log in as the first username in the user database,
  // or as the admin in case no user has been set up yet.
  if (config_globals_client_prepared) {
    Database_Users database_users = Database_Users ();
    vector <string> users = database_users.getUsers ();
    string user;
    if (users.empty ()) {
      user = "admin";
      level = Filter_Roles::admin ();
    } else {
      user = users [0];
      level = database_users.getUserLevel (user);
    }
    setUsername (user);
    logged_in = true;
    return true;
  }
  return false;
}
