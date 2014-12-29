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


#include <bible/settings.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <database/versifications.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <database/mappings.h>
#include <locale/translate.h>
#include <dialog/entry.h>
#include <dialog/yes.h>
#include <dialog/list.h>
#include <access/bible.h>


string bible_settings_url ()
{
  return "bible/settings";
}


bool bible_settings_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string bible_settings (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  
  page = Assets_Page::header (gettext ("Bible"), webserver_request, "");
  
  Assets_View view = Assets_View ();
  
  string success_message;
  string error_message;

  // The Bible.
  string bible = access_bible_clamp (request, request->query["bible"]);
  view.set_variable ("bible", filter_string_sanitize_html (bible));

  // Whether the user has write access to this Bible.
  bool write_access = access_bible_write (request, bible);
  if (write_access) view.enable_zone ("write_access");
  
  // Versification.
  if (request->query.count ("versification")) {
    string versification = request->query["versification"];
    if (versification == "") {
      Dialog_List dialog_list = Dialog_List ("settings", gettext("Would you like to change the versification system?"), gettext ("A versification system determines how many chapters are in each book, and how many verses are in each chapter. Please make your choice below."), "");
      dialog_list.add_query ("bible", bible);
      Database_Versifications database_versifications = Database_Versifications ();
      vector <string> versification_names = database_versifications.getSystems ();
      for (auto & versification_name : versification_names) {
        dialog_list.add_row (versification_name, "versification", versification_name);
      }
      page += dialog_list.run();
      return page;
    } else {
      if (write_access) Database_Config_Bible::setVersificationSystem (bible, versification);
    }
  }
  string versification = Database_Config_Bible::getVersificationSystem (bible);
  view.set_variable ("versification", versification);

  // Verse mapping.
  if (request->query.count ("mapping")) {
    string mapping = request->query["mapping"];
    if (mapping == "") {
      Dialog_List dialog_list = Dialog_List ("settings", gettext("Would you like to change the verse mapping?"), gettext ("A verse mapping can be used to match verses for parallel Bible display. Please make your choice below."), "");
      dialog_list.add_query ("bible", bible);
      Database_Mappings database_mappings = Database_Mappings ();
      vector <string> mapping_names = database_mappings.names ();
      for (auto & mapping_name : mapping_names) {
        dialog_list.add_row (mapping_name, "mapping", mapping_name);
      }
      page += dialog_list.run ();
      return page;
    } else {
      if (write_access) Database_Config_Bible::setVerseMapping (bible, mapping);
    }
  }
  string mapping = Database_Config_Bible::getVerseMapping (bible);
  view.set_variable ("mapping", mapping);

  // Book creation.
  if (request->query.count ("createbook")) {
    string createbook = request->query["createbook"];
    if (createbook == "") {
      // Todo dialog_books = new Dialog_Books (array ("bible"), gettext("Create book"), "", "", "createbook", NULL, database_bibles.getBooks (bible));
      // Todo die;
    } else {
      vector <string> feedback;
      // Todo await dependency if (write_access) Book_Create::create (bible, createbook, NULL, feedback);
    }
  }
  
  
  
  
  
  // Available books.
  string bookblock;
  vector <int> book_ids = filter_passage_get_ordered_books (bible);
  for (auto & book: book_ids) {
    string book_name = Database_Books::getEnglishFromId (book);
    book_name = gettext(book_name);
    bookblock.append ("<a href=\"book?bible=" + bible + "&book=" + convert_to_string (book) + "\">" + book_name + "</a>\n");
  }
  view.set_variable ("bookblock", bookblock);
  view.set_variable ("book_count", convert_to_string ((int)book_ids.size()));


  
  view.set_variable ("success_message", success_message);
  view.set_variable ("error_message", error_message);

  page += view.render ("bible", "settings");
  
  page += Assets_Page::footer ();
  
  return page;
}

/* Todo port this.

 
 
 // Book deletion.
 @deletebook = _GET["deletebook"];
 if (deletebook != "") {
 @confirm = _GET["confirm"];
 if (confirm != "") {
 if (write_access) Bible_Logic::deleteBook (bible, deletebook);
 } else {
 dialog_yes = new Dialog_Yes (array ("bible"), gettext("Would you like to delete this book?"), "deletebook");
 die;
 }
 }
 
 
 if (isset (_GET["viewable"])) {
 if (write_access) database_config_bible.setViewableByAllUsers (bible, !database_config_bible.getViewableByAllUsers (bible));
 }
 view.view.viewable = database_config_bible.getViewableByAllUsers (bible);
 */
