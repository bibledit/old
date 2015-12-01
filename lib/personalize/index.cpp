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


#include <personalize/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <database/config/general.h>
#include <database/config/user.h>
#include <locale/translate.h>
#include <dialog/entry.h>
#include <styles/sheets.h>
#include <styles/logic.h>
#include <menu/logic.h>


string personalize_index_url ()
{
  return "personalize/index";
}


bool personalize_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::member ());
}


string personalize_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;
  string success;
  string error;


  // Store new fon sizes before displaying the header,
  // so that the page displays the new font sizes immediately.
  if (request->post.count ("fontsizegeneral")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 50) && (value <= 300)) {
      request->database_config_user ()->setGeneralFontSize (value);
    } else {
      error = translate ("Incorrect font size in percents");
    }
  }
  if (request->post.count ("fontsizemenu")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 50) && (value <= 300)) {
      request->database_config_user ()->setMenuFontSize (value);
    } else {
      error = translate ("Incorrect font size in percents");
    }
  }
  
  
  Assets_Header header = Assets_Header (translate("Personalize"), webserver_request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  page = header.run ();

  
  Assets_View view;
  
  
  // Font size for everything.
  if (request->query.count ("fontsizegeneral")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("index", translate("Please enter a font size between 50 and 300 percent"), convert_to_string (request->database_config_user ()->getGeneralFontSize ()), "fontsizegeneral", "");
    page += dialog_entry.run ();
    return page;
  }
  view.set_variable ("fontsizegeneral", convert_to_string (request->database_config_user ()->getGeneralFontSize ()));

  
  // Font size for the menu.
  if (request->query.count ("fontsizemenu")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("index", translate("Please enter a font size between 50 and 300 percent"), convert_to_string (request->database_config_user ()->getMenuFontSize ()), "fontsizemenu", "");
    page += dialog_entry.run ();
    return page;
  }
  view.set_variable ("fontsizemenu", convert_to_string (request->database_config_user ()->getMenuFontSize ()));
  
  
  // Font size for the Bible editors.
  if (request->query.count ("fontsizeeditors")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("index", translate("Please enter a font size between 50 and 300 percent"), convert_to_string (request->database_config_user ()->getBibleEditorsFontSize ()), "fontsizeeditors", "");
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("fontsizeeditors")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 50) && (value <= 300)) {
      request->database_config_user ()->setBibleEditorsFontSize (value);
      styles_sheets_create_all ();
    } else {
      error = translate ("Incorrect font size in percents");
    }
  }
  view.set_variable ("fontsizeeditors", convert_to_string (request->database_config_user ()->getBibleEditorsFontSize ()));
  
  
  // Font size for the resources.
  if (request->query.count ("fontsizeresources")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("index", translate("Please enter a font size between 50 and 300 percent"), convert_to_string (request->database_config_user ()->getResourcesFontSize ()), "fontsizeresources", "");
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("fontsizeresources")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 50) && (value <= 300)) {
      request->database_config_user ()->setResourcesFontSize (value);
    } else {
      error = translate ("Incorrect font size in percents");
    }
  }
  view.set_variable ("fontsizeresources", convert_to_string (request->database_config_user ()->getResourcesFontSize ()));
  
  
  // Font size for Hebrew resources.
  if (request->query.count ("fontsizehebrew")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("index", translate("Please enter a font size between 50 and 300 percent"), convert_to_string (request->database_config_user ()->getHebrewFontSize ()), "fontsizehebrew", "");
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("fontsizehebrew")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 50) && (value <= 300)) {
      request->database_config_user ()->setHebrewFontSize (value);
    } else {
      error = translate ("Incorrect font size in percents");
    }
  }
  view.set_variable ("fontsizehebrew", convert_to_string (request->database_config_user ()->getHebrewFontSize ()));
  
  
  // Font size for Greek resources.
  if (request->query.count ("fontsizegreek")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("index", translate("Please enter a font size between 50 and 300 percent"), convert_to_string (request->database_config_user ()->getGreekFontSize ()), "fontsizegreek", "");
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("fontsizegreek")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 50) && (value <= 300)) {
      request->database_config_user ()->setGreekFontSize (value);
    } else {
      error = translate ("Incorrect font size in percents");
    }
  }
  view.set_variable ("fontsizegreek", convert_to_string (request->database_config_user ()->getGreekFontSize ()));
  
  
  // Vertical caret position in chapter editors.
  if (request->query.count ("caretposition")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("index", translate("Please enter a caret position 20 and 80 percent"), convert_to_string (request->database_config_user ()->getVerticalCaretPosition ()), "caretposition", "");
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("caretposition")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 20) && (value <= 80)) {
      request->database_config_user ()->setVerticalCaretPosition (value);
    } else {
      error = translate ("Incorrect caret position in percents");
    }
  }
  view.set_variable ("caretposition", convert_to_string (request->database_config_user ()->getVerticalCaretPosition ()));
  

  // Whether to display bread crumbs.
  if (request->query.count ("breadcrumbs")) {
    bool state = request->database_config_user ()->getDisplayBreadcrumbs ();
    request->database_config_user ()->setDisplayBreadcrumbs (!state);
  }
  string on_off = styles_logic_off_on_inherit_toggle_text (request->database_config_user ()->getDisplayBreadcrumbs ());
  view.set_variable ("breadcrumbs", on_off);
  
  
  view.set_variable ("success", success);
  view.set_variable ("error", error);
  page += view.render ("personalize", "index");

  page += Assets_Page::footer ();
  
  return page;
}
