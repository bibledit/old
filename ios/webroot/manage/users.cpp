/*
Copyright (©) 2003-2015 Teus Benschop.

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


#include <manage/users.h>
#include <assets/view.h>
#include <assets/page.h>
#include <dialog/entry.h>
#include <dialog/list.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <tasks/logic.h>
#include <webserver/request.h>
#include <journal/index.h>
#include <database/config/general.h>
#include <database/logs.h>
#include <access/user.h>
#include <locale/translate.h>


string manage_users_url ()
{
  return "manage/users";
}


bool manage_users_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string manage_users (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;

  page = Assets_Page::header (translate ("Users"), webserver_request, "");

  Assets_View view = Assets_View ();


  string currentUser = request->session_logic ()->currentUser ();
  int currentLevel = request->session_logic ()->currentLevel ();
  
  
  // New user creation.
  if (request->query.count ("new")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("users", translate("Please enter a name for the new user"), "", "new", "");
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("new")) {
    string user = request->post["entry"];
    if (request->database_users ()->usernameExists (user)) {
      page += Assets_Page::error (translate("User already exists"));
    } else {
      request->database_users ()->addNewUser(user, user, Filter_Roles::member (), "");
      page += Assets_Page::success (translate("User created"));
    }
  }
  
  
  // The username to act on.
  string user = request->query["user"];
  
  
  // Delete a user.
  if (request->query.count ("delete")) {
    int level = request->database_users ()->getUserLevel (user);
    string role = Filter_Roles::text (level);
    string email = request->database_users ()->getUserToEmail (user);
    string message = "Deleted user " + user + " with role " + role + " and email " + email;
    Database_Logs::log (message, Filter_Roles::admin ());
    request->database_users ()->removeUser (user);
    page += Assets_Page::success (message);
  }
  
  
  // The user's role.
  if (request->query.count ("level")) {
    string level = request->query ["level"];
    if (level == "") {
      Dialog_List dialog_list = Dialog_List ("users", translate("Select a role for") + " " + user, "", "");
      dialog_list.add_query ("user", user);
      for (int i = Filter_Roles::lowest (); i <= Filter_Roles::highest (); i++) {
        if (i <= currentLevel) {
          dialog_list.add_row (Filter_Roles::text (i), "level", convert_to_string (i));
        }
      }
      page += dialog_list.run ();
      return page;
    } else {
      request->database_users ()->updateUserLevel (user, convert_to_int (level));
    }
  }
  
  
  // User's email address.
  if (request->query.count ("email")) {
    string email = request->query ["email"];
    if (email == "") {
      string question = translate("Please enter an email address for") + " " + user;
      string value = request->database_users ()->getUserToEmail (user);
      Dialog_Entry dialog_entry = Dialog_Entry ("users", question, value, "email", "");
      dialog_entry.add_query ("user", user);
      page += dialog_entry.run ();
      return page;
    }
  }
  if (request->post.count ("email")) {
    string email = request->post["entry"];
    if (filter_url_email_is_valid (email)) {
      page += Assets_Page::success (translate("Email address was updated"));
      request->database_users ()->updateUserEmail (user, email);
    } else {
      page += Assets_Page::error (translate("The email address is not valid"));
    }
  }
  
  
  // Fetch the Bibles the current user has access to.
  vector <string> allbibles = request->database_bibles ()->getBibles ();
  vector <string> accessibleBibles;
  for (auto & bible : allbibles) {
    if (request->database_users ()->hasAccess2Bible (currentUser, bible)) {
      accessibleBibles.push_back (bible);
    }
  }
  sort (accessibleBibles.begin(), accessibleBibles.end());
  
  
  // Add Bible to user account.
  if (request->query.count ("addbible")) {
    string addbible = request->query["addbible"];
    if (addbible == "") {
      Dialog_List dialog_list = Dialog_List ("users", translate("Would you like to grant the user access to a Bible?"), "", "");
      dialog_list.add_query ("user", user);
      for (auto bible : accessibleBibles) {
        dialog_list.add_row (bible, "addbible", bible);
      }
      page += dialog_list.run ();
      return page;
    } else {
      Assets_Page::success (translate("The user has become a member of the translation team that works on this Bible"));
      request->database_users ()->grantAccess2Bible (user, addbible);
    }
  }
  
  
  // Remove Bible from user.
  if (request->query.count ("removebible")) {
    string removebible = request->query ["removebible"];
    request->database_users ()->revokeAccess2Bible (user, removebible);
    Assets_Page::success (translate("The user is no longer a member of the translation team that works on this Bible"));
  }
  
  
  // The level and Bibles of the user who works on this page.
  // The admin has access to all Bibles.
  int mylevel = request->session_logic ()->currentLevel ();
  vector <string> mybibles = request->database_users ()->getBibles4User (currentUser);
  if (mylevel >= Filter_Roles::admin ()) {
    mybibles = request->database_bibles ()->getBibles ();
  }
  
  
  // User accounts to display.
  vector <string> tbody;
  // Retrieve assigned users.
  vector <string> users = access_user_assignees (webserver_request);
  for (auto & username : users) {
    int level = request->database_users ()->getUserLevel (username);
    vector <string> userBibles = request->database_users ()->getBibles4User (username);
    // Gather details for this user account.
    string namedrole = Filter_Roles::text (level);
    string email = request->database_users ()->getUserToEmail (username);
    if (email == "") email = "--";
    tbody.push_back ("<tr>");
    tbody.push_back ("<td><a href=\"?user=" + username + "&delete\">✗</a> " + username + "</td>");
    tbody.push_back ("<td>│</td>");
    tbody.push_back ("<td><a href=\"?user=" + username + "&level\">" + namedrole + "</a></td>");
    tbody.push_back ("<td>│</td>");
    tbody.push_back ("<td><a href=\"?user=" + username + "&email\">" + email + "</a></td>");
    tbody.push_back ("<td>│</td>");
    tbody.push_back ("<td>");
    // List no more than those Bibles the currently logged-in user has access to.
    // So the currently logged-in user cannot remove any Bibles he or she has no access to.
    sort (userBibles.begin(), userBibles.end());
    vector <string> userBiblesIntersec;
    set_intersection (userBibles.begin(), userBibles.end(), accessibleBibles.begin(), accessibleBibles.end(), back_inserter(userBiblesIntersec));
    for (auto & bible : userBiblesIntersec) {
      bool writer = (level >= Filter_Roles::translator ());
      tbody.push_back ("<a href=\"?user=" + username + "&removebible=" + bible + "\">✗</a>");
      tbody.push_back ("<a href=\"/bible/settings?bible=" + bible + "\">" + bible + "</a>");
      if (writer) {
        tbody.push_back ("<a href=\"write?user=" + username + "&bible=" + bible + "\">");
        int readwritebooks = 0;
        vector <int> books = request->database_bibles ()->getBooks (bible);
        for (auto book : books) {
          if (!request->database_users ()->hasReadOnlyAccess2Book (username, bible, book)) readwritebooks++;
        }
        tbody.push_back ("(" + convert_to_string (readwritebooks) + "/" + convert_to_string (books.size ()) + ")");
        tbody.push_back ("</a>");
      }
      tbody.push_back ("|");
    }
    tbody.push_back ("<a href=\"?user=" + username + "&addbible=\">➕</a>");
    tbody.push_back ("</td>");
    tbody.push_back ("</tr>");
  }

  view.set_variable ("tbody", filter_string_implode (tbody, "\n"));

  page += view.render ("manage", "users");

  page += Assets_Page::footer ();

  return page;
}
