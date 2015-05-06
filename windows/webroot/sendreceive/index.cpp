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


#include <sendreceive/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/notes.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <access/bible.h>
#include <dialog/list.h>
#include <sendreceive/logic.h>
#include <config/logic.h>
#include <demo/logic.h>
#include <client/logic.h>


string sendreceive_index_url ()
{
  return "sendreceive/index";
}


bool sendreceive_index_acl (void * webserver_request)
{
  // The role of Translator or higher enables send/receive.
  bool enable = Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
  if (!enable) {
    // In Client mode, also a Consultant can send/receive.
    if (client_logic_client_enabled ()) {
      enable = Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
    }
  }
  return enable;
}


string sendreceive_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  string page;
  Assets_Header header = Assets_Header (translate("Send/Receive"), request);
  page = header.run ();
  Assets_View view = Assets_View ();
  
  
  string bible;
  if (request->query.count ("bible")) {
    bible = request->query["bible"];
    if (bible == "") {
      Dialog_List dialog_list = Dialog_List ("index", translate("Select a Bible"), "", "");
      vector <string> bibles = access_bible_bibles (request);
      for (auto & bible : bibles) {
        // Select Bibles the user has write access to.
        if (access_bible_write (request, bible)) {
          dialog_list.add_row (bible, "bible", bible);
        }
      }
      page += dialog_list.run();
      return page;
    } else {
      request->database_config_user()->setBible (bible);
    }
  }
  
  
  bible = access_bible_clamp (request, request->database_config_user()->getBible ());
  view.set_variable ("bible", bible);

  
  if (request->query.count ("runbible")) {
    sendreceive_queue_bible (bible);
    view.set_variable ("successbible", translate("Will send and receive."));
  }
  
  
  if (request->query.count ("repeatbible")) {
    Database_Config_Bible::setRepeatSendReceive (bible, !Database_Config_Bible::getRepeatSendReceive (bible));
  }
  view.set_variable ("repeatbible", get_tick_box (Database_Config_Bible::getRepeatSendReceive (bible)));
  
  
  if (Database_Config_Bible::getRemoteRepositoryUrl (bible) == "") {
    view.set_variable ("errorbible", translate("Collaboration has not been set up for this Bible"));
  }
  
  
  if (request->query.count ("runsync")) {
    if (sendreceive_sync_queued ()) {
      view.set_variable ("errorsync", translate("Still sending and receiving from the last time."));
    }
    sendreceive_queue_sync (-1);
    view.set_variable ("successsync", translate("Will send and receive."));
  }
  
  
  if (config_logic_client_prepared ()) {
    if (client_logic_client_enabled ()) {
      view.enable_zone ("clienton");
    } else {
      view.enable_zone ("clientoff");
    }
  }
  
  
  if (!config_logic_client_prepared ()) {
    view.enable_zone ("server");
  }

  
  if (request->query.count ("repeatsync")) {
    int repeatsync = convert_to_int (request->query["repeatsync"]);
    if (repeatsync < 0) repeatsync = 0;
    if (repeatsync > 2) repeatsync = 2;
    Database_Config_General::setRepeatSendReceive (repeatsync);
  }
  int repeatsync = Database_Config_General::getRepeatSendReceive ();
  string repeatsynczone = "repeatsync" + convert_to_string (repeatsync);
  view.enable_zone (repeatsynczone);
  
  
  if (Database_Config_General::getServerAddress () == "") {
    view.set_variable ("errorsync", translate("Collaboration has not been set up for the Bibles and Consultation Notes"));
  }

  
  view.set_variable ("demo", demo_client_warning ());

  
  page += view.render ("sendreceive", "index");
  page += Assets_Page::footer ();
  return page;
}
