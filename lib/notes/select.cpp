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


#include <notes/select.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/notes.h>
#include <notes/logic.h>
#include <access/bible.h>
#include <ipc/focus.h>
#include <notes/index.h>


string notes_select_url ()
{
  return "notes/select";
}


bool notes_select_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string notes_select (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Notes database_notes = Database_Notes (webserver_request);
  Notes_Logic notes_logic = Notes_Logic (webserver_request);
  
  string page;
  
  Assets_Header header = Assets_Header (translate("Select notes"), request);
  page += header.run();
  
  Assets_View view = Assets_View ();
  
  
  if (request->query.count ("passageselector")) {
    int passage_selector = convert_to_int (request->query["passageselector"]);
    if ((passage_selector < 0) || (passage_selector > 3)) passage_selector = 0;
    request->database_config_user()->setConsultationNotesPassageSelector (passage_selector);
  }
  
  
  if (request->query.count ("editselector")) {
    int edit_selector = convert_to_int (request->query["editselector"]);
    if ((edit_selector < 0) || (edit_selector > 4)) edit_selector = 0;
    request->database_config_user()->setConsultationNotesEditSelector (edit_selector);
  }
  
  
  if (request->query.count ("noneditselector")) {
    int non_edit_selector = convert_to_int (request->query["noneditselector"]);
    if ((non_edit_selector < 0) || (non_edit_selector > 5)) non_edit_selector = 0;
    request->database_config_user()->setConsultationNotesNonEditSelector(non_edit_selector);
  }
  
  
  /* Todo
  
  @status_selector = request->query["statusselector"];
  if (isset (status_selector)) {
    request->database_config_user()->setConsultationNotesStatusSelector(status_selector);
  }
  
  
  @bible_selector = request->query["bibleselector"];
  if (isset (bible_selector)) {
    request->database_config_user()->setConsultationNotesBibleSelector (bible_selector);
    // Also set the active Bible for the user.
    if (bible_selector != "") {
      request->database_config_user()->setBible (bible_selector);
    }
  }
  
  
  @assignment_selector = request->query["assignmentselector"];
  if (isset (assignment_selector)) {
    request->database_config_user()->setConsultationNotesAssignmentSelector(assignment_selector);
  }
  
  
  @subscription_selector = request->query["subscriptionselector"];
  if (isset (subscription_selector)) {
    if (subscription_selector == 1) subscription_selector = 1; else subscription_selector = 0;
    request->database_config_user()->setConsultationNotesSubscriptionSelector(subscription_selector);
  }
  
  
  @severity_selector = request->query["severityselector"];
  if (isset (severity_selector)) {
    request->database_config_user()->setConsultationNotesSeveritySelector(severity_selector);
  }
  
  
  @text_selector = request->query["textselector"];
  if (isset (text_selector)) {
    request->database_config_user()->setConsultationNotesTextSelector (text_selector);
    @search_text = request->post["text"];
    if (isset (search_text)) {
      request->database_config_user()->setConsultationNotesSearchText (search_text);
      Assets_Page::success (translate("Search text saved"));
    }
  }
  
  
  @passage_inclusion_selector = request->query["passageinclusionyselector"];
  if (isset (passage_inclusion_selector)) {
    request->database_config_user()->setConsultationNotesPassageInclusionSelector (passage_inclusion_selector);
  }
  
  
  @text_inclusion_selector = request->query["textinclusionyselector"];
  if (isset (text_inclusion_selector)) {
    request->database_config_user()->setConsultationNotesTextInclusionSelector (text_inclusion_selector);
  }
  
  
  view = new Assets_View (__FILE__);
  */
  
  
  string active_class = "class=\"active\"";
  
  string passage_selector = to_string (request->database_config_user()->getConsultationNotesPassageSelector());
  view.set_variable ("passageselector" + passage_selector, active_class);

  string edit_selector = to_string (request->database_config_user()->getConsultationNotesEditSelector());
  view.set_variable ("editselector" + edit_selector, active_class);
  
  string non_edit_selector = to_string (request->database_config_user()->getConsultationNotesNonEditSelector());
  view.set_variable ("noneditselector" + non_edit_selector, active_class);
                     
  
  /* Todo
  
   
                                                              possible_statuses = database_notes->getPossibleStatuses();
                                                              for (possible_statuses as possible_status) {
                                                                status_ids [] = possible_status[0];
                                                                status_localizations [] = possible_status[1];
                                                              }
                                                              view.set_variable ("statusids = status_ids;
                                                                                  view.set_variable ("statuslocs = status_localizations;
                                                                                                      
                                                                                                      
                                                                                                      status_selector = request->database_config_user()->getConsultationNotesStatusSelector();
                                                                                                      view.set_variable ("statusselector = status_selector;
                                                                                                                          
                                                                                                                          
                                                                                                                          // The information about available Bibles could be gathered from the notes database.
                                                                                                                          // But multiple teams can be hosted, the information about available Bibles
                                                                                                                          // is gathered from the Bibles the user has access to.
                                                                                                                          bibles = access_bible_bibles ();
                                                                                                                          view.set_variable ("bibles = bibles;
                                                                                                                                              
                                                                                                                                              
                                                                                                                                              bible_selector = request->database_config_user()->getConsultationNotesBibleSelector();
                                                                                                                                              view.set_variable ("bibleselector = bible_selector;
                                                                                                                                                                  
                                                                                                                                                                  
                                                                                                                                                                  assignees = database_notes->getAllAssignees (bibles);
                                                                                                                                                                  view.set_variable ("assignees = assignees;
                                                                                                                                                                                      if (assignment_selector != "") {
                                                                                                                                                                                        if (!in_array (assignment_selector, assignees)) {
                                                                                                                                                                                          view.set_variable ("nonexistingassignee = true;
                                                                                                                                                                                                              }
                                                                                                                                                                                                              }
                                                                                                                                                                                                              
                                                                                                                                                                                                              
                                                                                                                                                                                                              assignment_selector = request->database_config_user()->getConsultationNotesAssignmentSelector();
                                                                                                                                                                                                              view.set_variable ("assignmentselector = assignment_selector;
                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                  subscription_selector = request->database_config_user()->getConsultationNotesSubscriptionSelector();
                                                                                                                                                                                                                                  view.set_variable ("subscriptionselector = subscription_selector;
                                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                      view.set_variable ("severities = database_notes->getPossibleSeverities();
                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                          severity_selector = request->database_config_user()->getConsultationNotesSeveritySelector();
                                                                                                                                                                                                                                                                          view.set_variable ("severityselector = severity_selector;
                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                              text_selector = request->database_config_user()->getConsultationNotesTextSelector();
                                                                                                                                                                                                                                                                                              view.set_variable ("textselector = text_selector;
                                                                                                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                                                                  search_text = request->database_config_user()->getConsultationNotesSearchText();
                                                                                                                                                                                                                                                                                                                  view.set_variable ("searchtext = filter_string_sanitize_html (search_text);
                                                                                                                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                                                                                      passage_inclusion_selector = request->database_config_user()->getConsultationNotesPassageInclusionSelector();
                                                                                                                                                                                                                                                                                                                                      view.set_variable ("passageinclusionselector = passage_inclusion_selector;
                                                                                                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                          text_inclusion_selector = request->database_config_user()->getConsultationNotesTextInclusionSelector();
                                                                                                                                                                                                                                                                                                                                                          view.set_variable ("textinclusionselector = text_inclusion_selector;
                                                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                                              // The admin disables notes selection on Bibles, so the admin sees all notes, even notes referring to non-existing Bibles.
                                                                                                                                                                                                                                                                                                                                                                              if (request->session_logic ()->currentLevel () == Filter_Roles::admin ()) bibles = NULL;
                                                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                                              book = Ipc_Focus::getBook ();
                                                                                                                                                                                                                                                                                                                                                                              chapter = Ipc_Focus::getChapter ();
                                                                                                                                                                                                                                                                                                                                                                              verse = Ipc_Focus::getVerse ();
                                                                                                                                                                                                                                                                                                                                                                              identifiers = database_notes->selectNotes (bibles, book, chapter, verse,
                                                                                                                                                                                                                                                                                                                                                                                                                           passage_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           edit_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           non_edit_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           status_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           bible_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           assignment_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           subscription_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           severity_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           text_selector,
                                                                                                                                                                                                                                                                                                                                                                                                                           search_text,
                                                                                                                                                                                                                                                                                                                                                                                                                           NULL);
                                                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                                              count = count (identifiers);
                                                                                                                                                                                                                                                                                                                                                                              view.set_variable ("count = count;
                                                                                                                                                                                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                                                                                                                                                  
*/
  
  
  
  page += view.render ("notes", "select");
  
  page += Assets_Page::footer ();
  
  return page;
}
