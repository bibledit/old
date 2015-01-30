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


#include <client/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>


string client_index_url ()
{
  return "client/index";
}


bool client_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string client_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  
  page = Assets_Page::header (gettext ("client"), webserver_request, "");
  
  Assets_View view = Assets_View ();

  if (request->post.count ("connect")) {
    
    string address = request->post ["address"];
    Database_Config_General::setServerAddress (address);
    
    string user = request->post ["user"];
    string pass = request->post ["pass"];
    
    /*
     string response = config_logic_setup (user, md5 (pass));
     
    if ($response === false) {
      $view.set_variable ("error = gettext("Could not connect to the server.");
                          } else if (($response >= Filter_Roles::guest ()) && ($response <= Filter_Roles::admin ())) {
                            // Enable client mode upon a successful connection.
                            enable_client ($user, $pass, $response);
                            // Feedback.
                            $view.set_variable ("success = gettext("Connection is okay.");
                                                } else {
                                                  $view.set_variable ("error = $response . ": " . gettext("Check that your username and password exist on the server.");
                                                                      }
      */
  }

  
  page += view.render ("client", "index");
  
  page += Assets_Page::footer ();
  
  return page;
}
/* Todo
 
 
 $database_noteactions = Database_NoteActions::getInstance ();
 $database_bibleactions = Database_BibleActions::getInstance ();
 
 
 
 
 if (request->query.count ("disable"])) {
 config_logic_set (false);
 remove_all_users ();
 Database_Config_General::setRepeatSendReceive (0);
 }
 
 
 
 
 if (isset (request->query["demo"])) {
 
 $address = demo_logic_demo_address ();
 Database_Config_General::setServerAddress ($address);
 
 $user = "admin";
 $pass = "admin";
 
 $response = config_logic_setup ($user, md5 ($pass));
 
 if (($response >= Filter_Roles::guest ()) && ($response <= Filter_Roles::admin ())) {
 // Enable client mode upon a successful connection.
 enable_client ($user, $pass, $response);
 // Feedback.
 $view.set_variable ("success = gettext("Demo connection is okay.");
 } else {
 $view.set_variable ("error = gettext("Could not connect to the demo server.");
 }
 
 }
 
 
 $view.set_variable ("client = config_logic_client_enabled ();
 
 
 $address = Database_Config_General::getServerAddress ();
 $view.set_variable ("address = $address;
 
 
 $users = $database_users->getUsers ();
 for ($users as $user) {
 $level = $database_users->getUserLevel ($user);
 $view.set_variable ("role = Filter_Roles::text ($level);
 }
 
 
 $view.set_variable ("demo = demo_logic_client_demo_warning ();
 
 
 Assets_Page::header (gettext("Client mode"));
 
 
 $view->render ("client");
 
 
 Assets_Page::footer ();
 
 
 function remove_all_users ()
 {
 $database_users = Database_Users::getInstance ();
 $existingusers = $database_users->getUsers ();
 for ($existingusers as $existinguser) {
 $database_users->removeUser ($existinguser);
 }
 }
 
 
 function enable_client ($username, $password, $level)
 {
 // Enable client mode upon a successful connection.
 config_logic_set (true);
 
 // Remove all users from the database, and add the current one.
 remove_all_users ();
 $database_users = Database_Users::getInstance ();
 $database_users->addNewUser ($username, $password, $level, "");
 
 // Clear all pending note actions and Bible actions and settings updates.
 $database_noteactions = Database_NoteActions::getInstance ();
 $database_bibleactions = Database_BibleActions::getInstance ();
 $database_config_user = Database_Config_User::getInstance ();
 $session_logic = Session_Logic::getInstance ();
 $database_noteactions->clear ();
 $database_noteactions->create ();
 $database_bibleactions->clear ();
 $database_bibleactions->create ();
 $session_logic->setUsername ($username);
 request->database_config_user()->setUpdatedSettings (array ());
 
 // Set it repeats sync every so often.
 $database_config_general = Database_Config_General::getInstance ();
 Database_Config_General::setRepeatSendReceive (2);
 
 // Schedule a sync operation straight-away.
 sendreceive_queue_sync (true);
 }

*/