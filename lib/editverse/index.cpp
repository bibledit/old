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


#include <editverse/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/customcss.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <database/config/bible.h>
#include <fonts/logic.h>


string editverse_index_url ()
{
  return "editverse/index";
}


bool editverse_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string editverse_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  
  Assets_Header header = Assets_Header (gettext("Edit USFM"), request);
  header.setNavigator (); // Todo enable navigator display.
  page = header.run ();
  
  Assets_View view = Assets_View ();
  
  // Get active Bible, and check read access to it.
  // If needed, change Bible to one it has read access to.
  string bible = access_bible_clamp (request, request->database_config_user()->getBible ());
  view.set_variable ("bible", bible);
  
  // Write access?
  bool write_access = access_bible_write (request, bible);
  view.set_variable ("write_access", convert_to_string (write_access)); // Todo fix.
  
  // Store the active Bible in the page's javascript.
  // Todo port view.set_variable ("navigationCode", Navigation_Passage::code (bible));
  
  string chapterLoaded = gettext("Loaded"); // Todo test the whole block.
  string chapterSaving = gettext("Saving...");
  string chapterRetrying = gettext("Retrying...");
  string java_write_access = write_access ? "true" : "false";
  string script =
  "var verseEditorVerseLoaded = '" + chapterLoaded + "';\n"
  "var verseEditorVerseSaving = '" + chapterSaving + "';\n"
  "var verseEditorChapterRetrying = '" + chapterRetrying + "';\n"
  "var verseEditorWriteAccess = " + java_write_access + ";\n";
  view.set_variable ("script", script);
  
  string cls = Filter_CustomCSS::getClass (bible); // Todo test direction and font.
  string font = Database_Config_Bible::getTextFont (bible);
  int direction = Database_Config_Bible::getTextDirection (bible);
  view.set_variable ("custom_class", cls);
  view.set_variable ("custom_css", Filter_CustomCSS::getCss (cls, Fonts_Logic::getFontPath (font), direction));
  
  page += view.render ("editverse", "index");
  
  page += Assets_Page::footer ();
  
  return page;
}

// Tests for the USFM editor:
// * Autosave on going to another passage.
// * Autosave on document unload.
// * Autosave shortly after any change.




/*

Todo port it.


@$switchbook = $_GET ['switchbook'];
@$switchchapter = $_GET ['switchchapter'];
if (isset ($switchbook) && isset ($switchchapter)) {
  $ipc_focus = Ipc_Focus::getInstance();
  $switchbook = Filter_Numeric::integer_in_string ($switchbook);
  $switchchapter = Filter_Numeric::integer_in_string ($switchchapter);
  $ipc_focus->set ($switchbook, $switchchapter, 1);
  Navigation_Passage::recordHistory ($switchbook, $switchchapter, 1);
}


$header = new Assets_Header (gettext("Edit USFM"));
$header->setNavigator ();
$header->run ();


@$changebible = $_GET['changebible'];
if (isset ($changebible)) {
  if ($changebible == "") {
    $dialog_list = new Dialog_List2 (gettext("Select which Bible to open in the editor"));
    $bibles = access_bible_bibles ();
    for ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&changebible=$bible");
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($changebible);
  }
}





*/

