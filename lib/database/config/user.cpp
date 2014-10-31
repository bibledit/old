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


#include <database/config/user.h>
#include <filter/url.h>
#include <filter/string.h>
#include <session/logic.h>
#include <database/users.h>


using namespace std;


Database_Config_User::Database_Config_User (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Database_Config_User::~Database_Config_User ()
{
}


// Functions for getting and setting values or lists of values.


string Database_Config_User::file (string user, const char * key)
{
  return filter_url_create_root_path ("databases", "config", "user", user, key);
}


string Database_Config_User::getValue (const char * key, const char * default_value)
{
  string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
  return getValueForUser (user, key, default_value);
}


bool Database_Config_User::getValue (const char * key, bool default_value)
{
  string value = getValue (key, filter_string_convert_to_string (default_value).c_str());
  return filter_string_convert_to_bool (value);
}


int Database_Config_User::getValue (const char * key, int default_value)
{
  string value = getValue (key, filter_string_convert_to_string (default_value).c_str());
  return filter_string_convert_to_int (value);
}


string Database_Config_User::getValueForUser (string user, const char * key, const char * default_value)
{
  string value;
  string filename = file (user, key);
  if (filter_url_file_exists (filename)) value = filter_url_file_get_contents (filename);
  else value = default_value;
  return value;
}


bool Database_Config_User::getValueForUser (string user, const char * key, bool default_value)
{
  string value = getValueForUser (user, key, filter_string_convert_to_string (default_value).c_str());
  return filter_string_convert_to_bool (value);
}


int Database_Config_User::getValueForUser (string user, const char * key, int default_value)
{
  string value = getValueForUser (user, key, filter_string_convert_to_string (default_value).c_str());
  return filter_string_convert_to_int (value);
}


void Database_Config_User::setValue (const char * key, string value)
{
  string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
  string filename = file (user, key);
  string directory = filter_url_dirname (filename);
  if (filter_url_file_exists (directory)) filter_url_mkdir (directory);
  filter_url_file_put_contents (filename, value);
}


void Database_Config_User::setValue (const char * key, bool value)
{
  setValue (key, filter_string_convert_to_string (value));
}


void Database_Config_User::setValue (const char * key, int value)
{
  setValue (key, filter_string_convert_to_string (value));
}


vector <string> Database_Config_User::getList (const char * key)
{
  string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
  return getListForUser (user, key);
}


vector <string> Database_Config_User::getListForUser (string user, const char * key)
{
  string filename = file (user, key);
  vector <string> list;
  if (filter_url_file_exists (filename)) {
    string value = filter_url_file_get_contents (filename);
    list = filter_string_explode (value, '\n');
  }
  return list;
}


void Database_Config_User::setList (const char * key, vector <string> values)
{
  string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
  string filename = file (user, key);
  string directory = filter_url_dirname (filename);
  if (!filter_url_file_exists (directory)) filter_url_mkdir (directory);
  string value = filter_string_implode (values, "\n");
  filter_url_file_put_contents (filename, value);
}


void Database_Config_User::trim ()
{
  // Reset the sprint month and year after some time.
  // When a user visits the Sprint page after a few days, it will then display the current Sprint.
  // If the Sprint is not reset, the user may enter new tasks in the wrong sprint.
  int time = filter_string_date_seconds_since_epoch () - (2 * 24 * 3600);
  Database_Users database_users = Database_Users ();
  vector <string> users = database_users.getUsers ();
  for (unsigned int i = 0; i < users.size(); i++) {
    string filename = file (users[i], keySprintMonth ());
    if (filter_url_file_exists (filename)) {
      if (filter_url_filemtime (filename) < time) {
        filter_url_unlink (filename);
      }
    }
  }
}


// Named configuration functions.


string Database_Config_User::getStylesheet ()
{
  string sheet = getValue ("stylesheet", "Standard");
  // If the stylesheet does not exist, take the first one available instead.
  /* C++Port
  $database_styles = Database_Styles::getInstance();
  $sheets = $database_styles->getSheets();
  if (!in_array ($sheet, $sheets)) {
    $sheet = $sheets[0];
    $this->setStylesheet ($sheet);
  }
  */
  return sheet;
}
void Database_Config_User::setStylesheet (string sheet)
{
  setValue ("stylesheet", sheet);
}


string Database_Config_User::getBible ()
{
  string bible = getValue ("bible", "");
  // If the Bible does not exist, take the first one available.
  /* C++Port
  $database_bibles = Database_Bibles::getInstance();
  $bibles = $database_bibles->getBibles ();
  if (!in_array ($bible, $bibles)) {
    // There may not even be a first Bible: Suppress error.
    @$bible = $bibles[0];
    $this->setBible ($bible);
  }
  */
  return bible;
}
void Database_Config_User::setBible (string bible)
{
  setValue ("bible", bible);
}


bool Database_Config_User::getSubscribeToConsultationNotesEditedByMe ()
{
  return getValue ("subscribe-to-consultation-notes-edited-by-me", false);
}
void Database_Config_User::setSubscribeToConsultationNotesEditedByMe (bool value)
{
  setValue ("subscribe-to-consultation-notes-edited-by-me", value);
}


bool Database_Config_User::getNotifyMeOfAnyConsultationNotesEdits ()
{
  return getValue ("notify-me-of-any-consultation-notes-edits", false);
}
bool Database_Config_User::getNotifyUserOfAnyConsultationNotesEdits (string username)
{
  return getValueForUser (username, "notify-me-of-any-consultation-notes-edits", false);
}
void Database_Config_User::setNotifyMeOfAnyConsultationNotesEdits (bool value){
  setValue ("notify-me-of-any-consultation-notes-edits", value);
}


bool Database_Config_User::getSubscribedConsultationNoteNotification ()
{
  return getValue ("subscribed-consultation-note-notification", true);
}
bool Database_Config_User::getUserSubscribedConsultationNoteNotification (string username)
{
  return getValueForUser (username, "subscribed-consultation-note-notification", true);
}
void Database_Config_User::setSubscribedConsultationNoteNotification (bool value)
{
  setValue ("subscribed-consultation-note-notification", value);
}


bool Database_Config_User::getAssignedToConsultationNotesChanges ()
{
  return getValue ("get-assigned-to-consultation-notes-changes", false);
}
bool Database_Config_User::getUserAssignedToConsultationNotesChanges (string username)
{
  return getValueForUser (username, "get-assigned-to-consultation-notes-changes", false);
}
void Database_Config_User::setAssignedToConsultationNotesChanges (bool value)
{
  setValue ("get-assigned-to-consultation-notes-changes", value);
}


bool Database_Config_User::getGenerateChangeNotifications ()
{
  return getValue ("generate-change-notifications", false);
}
bool Database_Config_User::getUserGenerateChangeNotifications (string username)
{
  return getValueForUser (username, "generate-change-notifications", false);
}
void Database_Config_User::setGenerateChangeNotifications (bool value)
{
  setValue ("generate-change-notifications", value);
}


bool Database_Config_User::getAssignedConsultationNoteNotification ()
{
  return getValue ("assigned-consultation-note-notification", true);
}
bool Database_Config_User::getUserAssignedConsultationNoteNotification (string username)
{
  return getValueForUser (username, "assigned-consultation-note-notification", true);
}
void Database_Config_User::setAssignedConsultationNoteNotification (bool value)
{
  setValue ("assigned-consultation-note-notification", value);
}


// 0: current verse; 1: current chapter; 2: current book; 3: any passage.
int Database_Config_User::getConsultationNotesPassageSelector ()
{
  return getValue ("consultation-notes-passage-selector", 0);
}
void Database_Config_User::setConsultationNotesPassageSelector (int value)
{
  setValue ("consultation-notes-passage-selector", value);
}


// 0: any time; 1: last 30 days; 2: last 7 days; 3: since yesterday; 4: today.
int Database_Config_User::getConsultationNotesEditSelector ()
{
  return getValue ("consultation-notes-edit-selector", 0);
}
void Database_Config_User::setConsultationNotesEditSelector (int value)
{
  setValue ("consultation-notes-edit-selector", value);
}


// 0: don't care; 1: for last 30 days; 2: for last 7 days; 3: since yesterday; 4: today.
int Database_Config_User::getConsultationNotesNonEditSelector ()
{
  return getValue ("consultation-notes-non-edit-selector", 0);
}
void Database_Config_User::setConsultationNotesNonEditSelector (int value)
{
  setValue ("consultation-notes-non-edit-selector", value);
}


// Status is a string; can be empty as well.
string Database_Config_User::getConsultationNotesStatusSelector ()
{
  return getValue ("consultation-notes-status-selector", "");
}
void Database_Config_User::setConsultationNotesStatusSelector (string value)
{
  setValue ("consultation-notes-status-selector", value);
}


// "": any Bible; <bible>: named Bible.
string Database_Config_User::getConsultationNotesBibleSelector ()
{
  return getValue ("consultation-notes-bible-selector", "");
}
void Database_Config_User::setConsultationNotesBibleSelector (string value)
{
  setValue ("consultation-notes-bible-selector", value);
}


// "": don't care; "user": notes assigned to "user".
string Database_Config_User::getConsultationNotesAssignmentSelector ()
{
  return getValue ("consultation-notes-assignment-selector", "");
}
void Database_Config_User::setConsultationNotesAssignmentSelector (string value)
{
  setValue ("consultation-notes-assignment-selector", value);
}


// 0: don't care; 1: subscribed.
int Database_Config_User::getConsultationNotesSubscriptionSelector ()
{
  return getValue ("consultation-notes-subscription-selector", 0);
}
void Database_Config_User::setConsultationNotesSubscriptionSelector (int value)
{
  setValue ("consultation-notes-subscription-selector", value);
}


int Database_Config_User::getConsultationNotesSeveritySelector ()
{
  return getValue ("consultation-notes-severity-selector", -1);
}
void Database_Config_User::setConsultationNotesSeveritySelector (int value)
{
  setValue ("consultation-notes-severity-selector", value);
}


int Database_Config_User::getConsultationNotesTextSelector ()
{
  return getValue ("consultation-notes-text-selector", 0);
}
void Database_Config_User::setConsultationNotesTextSelector (int value)
{
  setValue ("consultation-notes-text-selector", value);
}


string Database_Config_User::getConsultationNotesSearchText ()
{
  return getValue ("consultation-notes-search-text", "");
}
void Database_Config_User::setConsultationNotesSearchText (string value)
{
  setValue ("consultation-notes-search-text", value);
}


int Database_Config_User::getConsultationNotesPassageInclusionSelector ()
{
  return getValue ("consultation-notes-passage-inclusion-selector", 0);
}
void Database_Config_User::setConsultationNotesPassageInclusionSelector (int value)
{
  setValue ("consultation-notes-passage-inclusion-selector", value);
}


int Database_Config_User::getConsultationNotesTextInclusionSelector ()
{
  return getValue ("consultation-notes-text-inclusion-selector", 0);
}
void Database_Config_User::setConsultationNotesTextInclusionSelector (int value)
{
  setValue ("consultation-notes-text-inclusion-selector", value);
}


bool Database_Config_User::getBibleChangesNotification ()
{
  return getValue ("bible-changes-notification", false);
}
bool Database_Config_User::getUserBibleChangesNotification (string username)
{
  return getValueForUser (username, "bible-changes-notification", false);
}
void Database_Config_User::setBibleChangesNotification (bool value)
{
  setValue ("bible-changes-notification", value);
}


bool Database_Config_User::getDeletedConsultationNoteNotification ()
{
  return getValue ("deleted-consultation-note-notification", false);
}
bool Database_Config_User::getUserDeletedConsultationNoteNotification (string username)
{
  return getValueForUser (username, "deleted-consultation-note-notification", false);
}
void Database_Config_User::setDeletedConsultationNoteNotification (bool value)
{
  setValue ("deleted-consultation-note-notification", value);
}


bool Database_Config_User::getBibleChecksNotification ()
{
  return getValue ("bible-checks-notification", false);
}
bool Database_Config_User::getUserBibleChecksNotification (string username)
{
  return getValueForUser (username, "bible-checks-notification", false);
}
void Database_Config_User::setBibleChecksNotification (bool value)
{
  setValue ("bible-checks-notification", value);
}


bool Database_Config_User::getPendingChangesNotification ()
{
  return getValue ("pending-changes-notification", false);
}
bool Database_Config_User::getUserPendingChangesNotification (string username)
{
  return getValueForUser (username, "pending-changes-notification", false);
}
void Database_Config_User::setPendingChangesNotification (bool value)
{
  setValue ("pending-changes-notification", value);
}


bool Database_Config_User::getUserChangesNotification ()
{
  return getValue ("user-changes-notification", false);
}
bool Database_Config_User::getUserUserChangesNotification (string username)
{
  return getValueForUser (username, "user-changes-notification", false);
}
void Database_Config_User::setUserChangesNotification (bool value)
{
  setValue ("user-changes-notification", value);
}


bool Database_Config_User::getAssignedNotesStatisticsNotification ()
{
  return getValue ("assigned-notes-statistics-notification", false);
}
bool Database_Config_User::getUserAssignedNotesStatisticsNotification (string username)
{
  return getValueForUser (username, "assigned-notes-statistics-notification", false);
}
void Database_Config_User::setAssignedNotesStatisticsNotification (bool value)
{
  setValue ("assigned-notes-statistics-notification", value);
}


bool Database_Config_User::getSubscribedNotesStatisticsNotification ()
{
  return getValue ("subscribed-notes-statistics-notification", false);
}
bool Database_Config_User::getUserSubscribedNotesStatisticsNotification (string username)
{
  return getValueForUser (username, "subscribed-notes-statistics-notification", false);
}
void Database_Config_User::setSubscribedNotesStatisticsNotification (bool value)
{
  setValue ("subscribed-notes-statistics-notification", value);
}


bool Database_Config_User::getNotifyMeOfMyPosts ()
{
  return getValue ("notify-me-of-my-posts", true);
}
bool Database_Config_User::getUserNotifyMeOfMyPosts (string username)
{
  return getValueForUser (username, "notify-me-of-my-posts", true);
}
void Database_Config_User::setNotifyMeOfMyPosts (bool value)
{
  setValue ("notify-me-of-my-posts", value);
}


bool Database_Config_User::getSuppressMailFromYourUpdatesNotes ()
{
  return getValue ("suppress-mail-my-updated-notes", false);
}
bool Database_Config_User::getUserSuppressMailFromYourUpdatesNotes (string username)
{
  return getValueForUser (username, "suppress-mail-my-updated-notes", false);
}
void Database_Config_User::setSuppressMailFromYourUpdatesNotes (bool value)
{
  setValue ("suppress-mail-my-updated-notes", value);
}


vector <string> Database_Config_User::getActiveResources ()
{
  return getList ("active-resources");
}
void Database_Config_User::setActiveResources (vector <string> values)
{
  setList ("active-resources", values);
}


vector <string> Database_Config_User::getConsistencyResources ()
{
  return getList ("consistency-bibles");
}
void Database_Config_User::setConsistencyResources (vector <string> values)
{
  setList ("consistency-bibles", values);
}


const char * Database_Config_User::keySprintMonth ()
{
  return "sprint-month";
}
int Database_Config_User::getSprintMonth ()
{
  return getValue (keySprintMonth (), filter_string_date_numerical_month ());
}
void Database_Config_User::setSprintMonth (int value)
{
  setValue (keySprintMonth (), value);
}


const char * Database_Config_User::keySprintYear ()
{
  return "sprint-year";
}
int Database_Config_User::getSprintYear ()
{
  return getValue (keySprintYear (), filter_string_date_numerical_year ());
}
void Database_Config_User::setSprintYear (int value)
{
  setValue (keySprintYear (), value);
}


bool Database_Config_User::getSprintProgressNotification ()
{
  return getValue ("sprint-progress-notification", false);
}
bool Database_Config_User::getUserSprintProgressNotification (string username)
{
  return getValueForUser (username, "sprint-progress-notification", false);
}
void Database_Config_User::setSprintProgressNotification (bool value)
{
  setValue ("sprint-progress-notification", value);
}


bool Database_Config_User::getUserChangesNotificationsOnline ()
{
  return getValue ("user-changes-notifications-online", false);
}
bool Database_Config_User::getUserUserChangesNotificationsOnline (string username)
{
  return getValueForUser (username, "user-changes-notifications-online", false);
}
void Database_Config_User::setUserChangesNotificationsOnline (bool value)
{
  setValue ("user-changes-notifications-online", value);
}


string Database_Config_User::getWorkbenchURLs ()
{
  return getValue ("workbench-urls", "");
}
void Database_Config_User::setWorkbenchURLs (string value)
{
  setValue ("workbench-urls", value);
}


string Database_Config_User::getWorkbenchWidths ()
{
  return getValue ("workbench-widths", "");
}
void Database_Config_User::setWorkbenchWidths (string value)
{
  setValue ("workbench-widths", value);
}


string Database_Config_User::getWorkbenchHeights ()
{
  return getValue ("workbench-heights", "");
}
void Database_Config_User::setWorkbenchHeights (string value)
{
  setValue ("workbench-heights", value);
}


string Database_Config_User::getActiveWorkbench ()
{
  return getValue ("active-workbench", "");
}
void Database_Config_User::setActiveWorkbench (string value)
{
  setValue ("active-workbench", value);
}


bool Database_Config_User::getPostponeNewNotesMails ()
{
  return getValue ("postpone-new-notes-mails", false);
}
void Database_Config_User::setPostponeNewNotesMails (bool value)
{
  setValue ("postpone-new-notes-mails", value);
}


string Database_Config_User::getRecentlyAppliedStyles ()
{
  return getValue ("recently-applied-styles", "p s add nd f x v");
}
void Database_Config_User::setRecentlyAppliedStyles (string values)
{
  setValue ("recently-applied-styles", values);
}


vector <string> Database_Config_User::getPrintResources ()
{
  return getList ("print-resources");
}
vector <string> Database_Config_User::getPrintResourcesForUser (string user)
{
  return getListForUser (user, "print-resources");
}
void Database_Config_User::setPrintResources (vector <string> values)
{
  setList ("print-resources", values);
}


string Database_Config_User::getPrintPassageFrom ()
{
  return getValue ("print-passage-from", "1.1.1");
}
string Database_Config_User::getPrintPassageFromForUser (string user)
{
  return getValueForUser (user, "print-passage-from", "1.1.1");
}
void Database_Config_User::setPrintPassageFrom (string value)
{
  setValue ("print-passage-from", value);
}


string Database_Config_User::getPrintPassageTo ()
{
  return getValue ("print-passage-to", "1.1.31");
}
string Database_Config_User::getPrintPassageToForUser (string user)
{
  return getValueForUser (user, "print-passage-to", "1.1.31");
}
void Database_Config_User::setPrintPassageTo (string value)
{
  setValue ("print-passage-to", value);
}


string Database_Config_User::getSourceXrefBible ()
{
  return getValue ("source-xref-bible", "");
}
void Database_Config_User::setSourceXrefBible (string bible)
{
  setValue ("source-xref-bible", bible);
}


string Database_Config_User::getTargetXrefBible ()
{
  return getValue ("target-xref-bible", "");
}
void Database_Config_User::setTargetXrefBible (string bible)
{
  setValue ("target-xref-bible", bible);
}


int Database_Config_User::getFocusedBook ()
{
  return getValue ("focused-book", 1);
}
void Database_Config_User::setFocusedBook (int book)
{
  setValue ("focused-book", book);
}


int Database_Config_User::getFocusedChapter ()
{
  return getValue ("focused-chapter", 1);
}
void Database_Config_User::setFocusedChapter (int chapter)
{
  setValue ("focused-chapter", chapter);
}


int Database_Config_User::getFocusedVerse ()
{
  return getValue ("focused-verse", 1);
}
void Database_Config_User::setFocusedVerse (int verse)
{
  setValue ("focused-verse", verse);
}


vector <string> Database_Config_User::getUpdatedSettings ()
{
  return getList ("updated-settings");
}
void Database_Config_User::setUpdatedSettings (vector <string> values)
{
  setList ("updated-settings", values);
}
void Database_Config_User::addUpdatedSetting (string value)
{
  vector <string> settings = getUpdatedSettings ();
  settings.push_back (value);
  settings = filter_string_array_unique (settings);
  setUpdatedSettings (settings);
}
void Database_Config_User::removeUpdatedSetting (string value)
{
  vector <string> settings = getUpdatedSettings ();
  vector <string> against;
  against.push_back (value);
  settings = filter_string_array_diff (settings, against);
  setUpdatedSettings (settings);
}
