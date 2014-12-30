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


#include <bible/manage.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <webserver/request.h>
#include <database/versifications.h>
#include <locale/translate.h>
#include <dialog/entry.h>
#include <dialog/yes.h>
#include <access/bible.h>


string bible_manage_url ()
{
  return "bible/manage";
}


bool bible_manage_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string bible_manage (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  
  page = Assets_Page::header (gettext ("Bibles"), webserver_request, "");
  
  Assets_View view = Assets_View ();
  
  string success_message;
  string error_message;

  // New Bible handler.
  if (request->query.count ("new")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("manage", gettext("Please enter a name for the new empty Bible"), "", "new", "");
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("new")) {
    string bible = request->post ["entry"];
    vector <string> bibles = request->database_bibles ()->getBibles ();
    if (find (bibles.begin(), bibles.end(), bible) != bibles.end()) {
      error_message = gettext("This Bible already exists");
    } else {
      request->database_bibles ()->createBible (bible);
      // Check / grant access.
      if (!access_bible_write (request, bible)) {
        request->database_users ()->grantAccess2Bible (request->session_logic ()->currentUser (), bible);
      }
      success_message = gettext("The Bible was created");
    }
  }
  

  view.set_variable ("success_message", success_message);
  view.set_variable ("error_message", error_message);
  vector <string> bibles = access_bible_bibles (request);
  string bibleblock;
  for (auto & bible : bibles) {
    bibleblock.append ("<li><a href=\"settings?bible=" + bible + "\">" + bible + "</a></li>\n");
  }
  view.set_variable ("bibleblock", bibleblock);
                       

  page += view.render ("bible", "manage");
  
  page += Assets_Page::footer ();
  
  return page;
}

/* Todo port this.
 // Delete Bible handler.
 if (isset (_GET["delete"])) {
 bible = _GET["delete"];
 @confirm = _GET["confirm"];
 if (confirm != "") {
 ignore_user_abort (true);
 set_time_limit (0);
 // User needs write access for delete operation.
 if (access_bible_write (bible)) {
 Bible_Logic::deleteBible (bible);
 gitdirectory = Filter_Git::git_directory (bible);
 if (file_exists (gitdirectory)) {
 Filter_Rmdir::rmdir (gitdirectory);
 }
 } else {
 Assets_Page::error ("Insufficient privileges to complete action");
 }
 } else {
 dialog_yes = new Dialog_Yes (NULL, gettext("Would you like to delete Bible bible?"), "delete");
 die;
 }
 }
 
 
 
 // Copy Bible handler.
 @copy = _GET["copy"];
 if (isset (copy)) {
 dialog_entry = new Dialog_Entry (array ("origin" => copy), gettext("Please enter a name for where to copy the Bible to"), "", "", "");
 die;
 }
 @origin = _GET["origin"];
 if (isset (origin)) {
 destination = _POST["entry"];
 if (isset (destination)) {
 bibles = database_bibles.getBibles ();
 if (in_array (destination, bibles)) {
 error_message = gettext("Cannot copy Bible because the destination Bible already exists.");
 } else {
 ignore_user_abort (true);
 set_time_limit (0);
 // User needs read access to the original.
 if (Access_Bible::read (origin)) {
 database_bibles.createBible (destination);
 books = database_bibles.getBooks (origin);
 for (books as book) {
 chapters = database_bibles.getChapters (origin, book);
 for (chapters as chapter) {
 data = database_bibles.getChapter (origin, book, chapter);
 Bible_Logic::storeChapter (destination, book, chapter, data);
 }
 }
 success_message = gettext("The Bible was copied.");
 // Check / grant access to destination Bible.
 if (!access_bible_write (destination)) {
 database_users.grantAccess2Bible (session_logic.currentUser (), destination);
 }
 }
 }
 }
 }
 
 
*/
