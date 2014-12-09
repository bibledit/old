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


#include <notes/logic.h>
#include <filter/string.h>
#include <filter/url.h>
#include <database/notes.h>
#include <database/noteactions.h>
#include <database/mail.h>
#include <database/logs.h>
#include <database/config/general.h>
#include <config/logic.h>
#include <trash/handler.h>


Notes_Logic::Notes_Logic (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Notes_Logic::~Notes_Logic ()
{
}


// Add a comment to an exiting note identified by identifier.
void Notes_Logic::addComment (int identifier, const string& comment)
{
  // Do nothing if there was no content.
  if (comment == "") return;

  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.addComment (identifier, comment);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionComment, comment);
  } else {
    // Server: do the notifications.
    handlerAddComment (identifier);
  }
}


void Notes_Logic::setSummary (int identifier, const string& summary)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.setSummary (identifier, summary);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionSummary, summary);
  } else {
    // Server: do nothing extra.
  }
}


void Notes_Logic::subscribe (int identifier)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.subscribe (identifier);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionSubscribe, "");
  } else {
    // Server: do nothing extra.
  }
}


void Notes_Logic::unsubscribe (int identifier)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.unsubscribe (identifier);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionUnsubscribe, "");
  } else {
    // Server: do nothing extra.
  }
}


void Notes_Logic::assignUser (int identifier, const string& user)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionAssign, user);
  } else {
    // Server: do the notifications.
    // Assign logic comes before the database action in this particular case.
    handlerAssignNote (identifier, user);
  }
  database_notes.assignUser (identifier, user);
}


void Notes_Logic::unassignUser (int identifier, const string& user)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.unassignUser (identifier, user);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionUnassign, user);
  } else {
    // Server: do nothing extra.
  }
}


void Notes_Logic::setStatus (int identifier, const string& status)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.setStatus (identifier, status);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionStatus, status);
  } else {
    // Server: do nothing extra.
  }
}


// Set the passages for note identifier.
void Notes_Logic::setPassages (int identifier, const vector <Passage> & passages)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.setPassages (identifier, passages);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    vector <string> passagetexts;
    for (Passage passage : passages) {
      passagetexts.push_back (passage.to_text ());
    }
    string serialization = filter_string_implode (passagetexts, "|");
    database_noteactions.record (user, identifier, noteActionPassage, serialization);
  } else {
    // Server: do nothing extra.
  }
}


// Sets the severity as a number for note identifier.
void Notes_Logic::setRawSeverity (int identifier, int severity)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.setRawSeverity (identifier, severity);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionSeverity, convert_to_string (severity));
  } else {
    // Server: do nothing extra.
  }
}


// Sets the bible for note identifier.
void Notes_Logic::setBible (int identifier, const string& bible)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.setBible (identifier, bible);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionBible, bible);
  } else {
    // Server: do nothing extra.
  }
}


void Notes_Logic::markForDeletion (int identifier)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.markForDeletion (identifier);
  trash_consultation_note (webserver_request, identifier);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionMarkDeletion, "");
  } else {
    // Server: notifications.
    handlerMarkNoteForDeletion (identifier);
  }
}


void Notes_Logic::unmarkForDeletion (int identifier)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  database_notes.unmarkForDeletion (identifier);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionUnmarkDeletion, "");
  } else {
    // Server: do nothing extra.
  }
}


void Notes_Logic::erase (int identifier)
{
  Database_Notes database_notes = Database_Notes (webserver_request);
  if (config_logic_enabled ()) {
    // Client: record the action in the database.
    string user = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
    Database_NoteActions database_noteactions = Database_NoteActions ();
    database_noteactions.record (user, identifier, noteActionDelete, "");
  } else {
    // Server: notification.
    handlerDeleteNote (identifier);
  }
  trash_consultation_note (webserver_request, identifier);
  database_notes.erase (identifier);
}


void Notes_Logic::handlerNewNote (int identifier)
{
  notifyUsers (identifier, notifyNoteNew);
}


void Notes_Logic::handlerAddComment (int identifier)
{
  notifyUsers (identifier, notifyNoteComment);
  // If the note status was Done, and a comment is added, mark it Reopened.
  Database_Notes database_notes = Database_Notes (webserver_request);
  string status = database_notes.getRawStatus (identifier);
  if (status == "Done") {
    database_notes.setStatus (identifier, "Reopened");
  }
}


void Notes_Logic::handlerUpdateNote (int identifier)
{
  notifyUsers (identifier, notifyNoteUpdate);
}


void Notes_Logic::handlerAssignNote (int identifier, const string& user)
{
  // Take no action in client mode.
  if (config_logic_enabled ()) return;

  Database_Config_User database_config_user = Database_Config_User (webserver_request);
  if (database_config_user.getUserAssignedConsultationNoteNotification (user)) {
    // Only email the user if the user was not yet assigned this note.
    Database_Notes database_notes = Database_Notes (webserver_request);
    vector <string> assignees = database_notes.getAssignees (identifier);
    if (find (assignees.begin(), assignees.end(), user) == assignees.end()) {
      emailUsers (identifier, gettext("Assigned"), {user}, false);
    }
  }
}


void Notes_Logic::handlerMarkNoteForDeletion (int identifier)
{
  notifyUsers (identifier, notifyMarkNoteForDeletion);
}


void Notes_Logic::handlerDeleteNote (int identifier)
{
  notifyUsers (identifier, notifyNoteDelete);
}


// This handles notifications for the users
// identifier: the note that is being handled.
// notification: the type of action on the consultation note.
void Notes_Logic::notifyUsers (int identifier, int notification)
{
  // Take no action in client mode.
  if (config_logic_enabled ()) return;

  // Databases.
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Notes database_notes = Database_Notes (webserver_request);
  
  // This note's Bible.
  string bible = database_notes.getBible (identifier);

  // Subscription and assignment not to be used for notes marked for deletion,
  // because marking notes for deletion is nearly the same as deleting them straightaway.
  if (notification != notifyMarkNoteForDeletion) {

    // Whether current user gets subscribed to the note.
    if (request->database_config_user ()->getSubscribeToConsultationNotesEditedByMe ()) {
      database_notes.subscribe (identifier);
    }

    // Users to get subscribed to the note, or to whom the note is to be assigned.
    vector <string> users = request->database_users ()->getUsers ();
    for (const string & user : users) {
      if (request->database_users ()->hasAccess2Bible (user, bible)) {
        if (request->database_config_user ()->getNotifyUserOfAnyConsultationNotesEdits (user)) {
          database_notes.subscribeUser (identifier, user);
        }
        if (request->database_config_user ()->getUserAssignedToConsultationNotesChanges (user)) {
          database_notes.assignUser (identifier, user);
        }
      }
    }

  }

  // The recipients who receive a notification by email.
  vector <string> recipients;

  // Subscribers who receive email.
  vector <string> subscribers = database_notes.getSubscribers (identifier);
  for (const string & subscriber : subscribers) {
    if (request->database_config_user ()->getUserSubscribedConsultationNoteNotification (subscriber)) {
      recipients.push_back (subscriber);
    }
  }

  // Assignees who receive email.
  vector <string> assignees = database_notes.getAssignees (identifier);
  for (const string & assignee : assignees) {
    if (request->database_config_user ()->getUserAssignedConsultationNoteNotification (assignee)) {
      recipients.push_back (assignee);
    }
  }

  // In case the consultation note is deleted or marked for deletion,
  // notify only the users with this specific notification set.
  if ((notification == notifyNoteDelete) || (notification == notifyMarkNoteForDeletion)) {
    recipients.clear ();
    vector <string> users = request->database_users ()->getUsers ();
    for (const auto & user : users) {
      if (request->database_config_user ()->getUserDeletedConsultationNoteNotification (user)) {
        if (request->database_users ()->hasAccess2Bible (user, bible)) {
          recipients.push_back (user);
        }
      }
    }
  }

  // Remove duplicates from the recipients.
  set <string> unique (recipients.begin (), recipients.end ());
  recipients.assign (unique.begin (), unique.end());

  // Deal with suppressing mail to the user when he made the update himself.
  string username = request->session_logic ()->currentUser ();
  if (request->database_config_user ()->getUserSuppressMailFromYourUpdatesNotes (username)) {
    recipients.erase (remove (recipients.begin(), recipients.end(), username), recipients.end());
  }

  // Generate the label prefixed to the subject line of the email.
  string label = gettext("General");
  switch (notification) {
    case notifyNoteNew             : label = gettext("New");                 break;
    case notifyNoteComment         : label = gettext("Comment");             break;
    case notifyNoteUpdate          : label = gettext("Updated");             break;
    case notifyNoteDelete          : label = gettext("Deleted");             break;
    case notifyMarkNoteForDeletion : label = gettext("Marked for deletion"); break;
  }

  // Optional postponing sending email.
  bool postpone = false;
  if (notification == notifyNoteNew) {
    if (request->database_config_user ()->getPostponeNewNotesMails ()) {
      postpone = true;
    }
  }

  // Send mail to all recipients.
  emailUsers (identifier, label, recipients, postpone);
}


// This handles email to users.
// identifier: the note that is being handled.
// label: prefix to the subject line of the email.
// users: array of users to be mailed.
// postpone: whether to postpone sending the email till the evening.
void Notes_Logic::emailUsers (int identifier, const string& label, const vector <string> & users, bool postpone)
{
  // Databases.
  Database_Notes database_notes = Database_Notes (webserver_request);
  Database_Mail database_mail = Database_Mail(webserver_request);

  // Send mail to all users.
  string summary = database_notes.getSummary (identifier);
  string passages = filter_passage_display_inline (database_notes.getPassages (identifier));
  string contents = database_notes.getContents (identifier);

  // Include a link to the note on the site.
  contents.append ("<br>\n");
  string link = Database_Config_General::getSiteURL () + "/notes/note.php?id=" + convert_to_string (identifier);
  contents.append ("<p><a href=\"");
  contents.append (link);
  contents.append ("\">View or respond online</a></p>\n");
  string mailto = "mailto:" + Database_Config_General::getSiteMailAddress () + "?subject=(CNIDidentifier)";
  contents.append ("<p><a href=\"");
  contents.append (mailto);
  contents.append ("\">Respond by email</a></p>\n");

  // Deal with possible postponing email till 9 PM.
  int timestamp = filter_string_date_seconds_since_epoch ();
  if (postpone) {
    int postponedtime = timestamp;
    postponedtime -= filter_string_date_numerical_hour (timestamp) * 3600;
    postponedtime -= filter_string_date_numerical_minute (timestamp);
    postponedtime += 21 * 3600;;
    if (postponedtime > timestamp) timestamp = postponedtime;
  }

  // Send (but not in client mode).
  for (auto & user : users) {
    if (!config_logic_enabled ()) {
      string subject = label;
      subject.append (" | ");
      subject.append (passages);
      subject.append (" | ");
      subject.append (summary);
      subject.append (" | (CNID");
      subject.append (convert_to_string (identifier));
      subject.append (")");
      database_mail.send (user, subject, contents, timestamp);
    }
  }
}


// handleEmailComment - handles an email received from from with subject subject and body body.
// Returns true if the mail was processed, else false.
// The email is considered to have been processed if it comments on an existing Consultation Note.
bool Notes_Logic::handleEmailComment (string from, string subject, string body)
{
  // Check whether the Consultation Notes Identifier in the subject exists in the notes database.
  // The CNID looks like: (CNID123456789)
  size_t pos = subject.find ("(CNID");
  if (pos == string::npos) return false;
  subject = subject.substr (pos + 5);
  pos = subject.find (")");
  if (pos == string::npos) return false;
  subject = subject.substr (0, pos);
  // Webserver request.
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  // At this stage, the subject contains an identifier.
  // Check that the identifier is an existing Consultation Note.
  int identifier = convert_to_int (subject);
  Database_Notes database_notes = Database_Notes (webserver_request);
  if (!database_notes.identifierExists (identifier)) return false;
  // Check that the from address of the email belongs to an existing user.
  // Or else use the obfuscated email address as the user name.
  string username;
  // Todo restore from = Filter_Email::extractEmail (from);
  if (request->database_users()->emailExists (from)) {
    username = request->database_users()->getEmailToUser (from);
  } else {
    username = from;
    username = filter_string_str_replace ("@", " ", username);
    username = filter_string_str_replace (".", " ", username);
  }
  // Clean the email's body.
  string year = convert_to_string (filter_string_date_numerical_year ());
  string sender = Database_Config_General::getSiteMailName();
  body = ""; // Todo restore: = Filter_Email::extractBody (body, year, sender);
  // Remove any new lines from the body. This cleans up the email considerably,
  // because some emails that get posted would otherwise look untidy,
  // when the many new lines take up a lot of space.
  body = filter_string_str_replace ("\n", " ", body);
  // Make comment on the consultation note.
  string sessionuser = request->session_logic ()->currentUser ();
  request->session_logic ()->setUsername (username);
  addComment (identifier, body);
  request->session_logic ()->setUsername (sessionuser);
  // Mail confirmation to the username.
  if (request->database_config_user()->getUserNotifyMeOfMyPosts (username)) {
    Database_Mail database_mail = Database_Mail (webserver_request);
    string subject = gettext("Your comment was posted");
    subject.append (" [CNID");
    subject.append (convert_to_string (identifier));
    subject.append ("]");
    database_mail.send (username, subject, body);
  }
  // Log operation.
  Database_Logs::log ("Comment posted: " + body);
  // Job done.
  return true;
}


// handleEmailNew - handles an email received from from with subject subject and body body.
// Returns true if the mail was processed, else false.
// The email is considered to have been processed if it created a new Consultation Note.
bool Notes_Logic::handleEmailNew (string from, string subject, string body)
{
  // Webserver request.
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  // Store the original subject.
  string originalSubject = subject;
  // Check that the subject indicates that a new consultation note is to be created.
  size_t pos = unicode_string_casefold (subject).find ("new note");
  if (pos == string::npos) return false;
  // There is a new note. Remove that bit from the subject.
  subject = subject.substr (0, pos) + subject.substr (pos + 8);
  // Clean the subject line.
  subject = filter_string_trim (subject);
  subject = filter_string_str_replace (".", " ", subject);
  subject = filter_string_str_replace (":", " ", subject);
  subject = filter_string_str_replace ("  ", " ", subject);
  subject = filter_string_str_replace ("  ", " ", subject);
  subject = filter_string_str_replace ("  ", " ", subject);
  subject = filter_string_str_replace ("  ", " ", subject);
  // Check that the from address of the email belongs to an existing user.
  // Todo restore from = Filter_Email::extractEmail (from);
  if (!request->database_users()->emailExists (from)) return false;
  string username = request->database_users()->getEmailToUser (from);
  // Extract book, chapter, verse, and note summary from the subject
  int book = -1;
  int chapter = -1;
  int verse = -1;
  string summary;
  vector <string> subjectlines = filter_string_explode (subject, ' ');
  if (!subjectlines.empty()) {
    book = filter_passage_interpret_book (subjectlines[0]);
    subjectlines.erase (subjectlines.begin());
  }
  if (!subjectlines.empty()) {
    chapter = convert_to_int (subjectlines[1]);
    subjectlines.erase (subjectlines.begin());
  }
  if (!subjectlines.empty()) {
    verse = convert_to_int (subjectlines[2]);
    subjectlines.erase (subjectlines.begin());
  }
  summary = filter_string_implode (subjectlines, " ");
  // Check book, chapter, verse, and summary. Give feedback if there's anything wrong.
  string noteCheck;
  if (book <= 0) noteCheck.append (gettext("Unknown book"));
  if (chapter < 0) {
    noteCheck.append (" ");
    noteCheck.append (gettext("Unknown chapter"));
  }
  if (verse < 0) {
    noteCheck.append (" ");
    noteCheck.append (gettext("Unknown verse"));
  }
  if (summary.empty ()) {
    noteCheck.append (" ");
    noteCheck.append (gettext("Unknown summary"));
  }
  // Mail user if the note could not be posted.
  Database_Mail database_mail = Database_Mail (webserver_request);
  if (noteCheck != "") {
    subject = gettext("Your new note could not be posted");
    database_mail.send (username, subject  + ": " + originalSubject, noteCheck);
    return false;
  }
  // Clean the email's body.
  // Todo restore: body = Filter_Email::extractBody (body);
  // Post the note.
  string sessionuser = request->session_logic()->currentUser ();
  request->session_logic()->setUsername (username);
  Database_Notes database_notes = Database_Notes(webserver_request);
  int identifier = database_notes.storeNewNote ("", book, chapter, verse, summary, body, false);
  handlerNewNote (identifier);
  request->session_logic()->setUsername (sessionuser);
  // Mail confirmation to the username.
  if (request->database_config_user()->getUserNotifyMeOfMyPosts (username)) {
    subject = gettext("Your new note was posted");
    database_mail.send (username, subject + ": " + originalSubject, body);
  }
  // Log operation.
  Database_Logs::log ("New note posted : " + body);
  // Job done.
  return true;
}


string Notes_Logic::generalBibleName ()
{
  string name;
  name.append ("[");
  name.append (gettext("no Bible"));
  name.append ("]");
  return name;
}

