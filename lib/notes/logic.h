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


#ifndef INCLUDED_NOTES_LOGIC_H
#define INCLUDED_NOTES_LOGIC_H


#include <config/libraries.h>
#include <sqlite3.h>
#include <filter/passage.h>


class Notes_Logic
{
public:
  Notes_Logic (void * webserver_request_in);
  ~Notes_Logic ();
  static const int lowNoteIdentifier  = 100000000;
  static const int highNoteIdentifier = 999999999;
  // Note notify definitions.
  static const int notifyNoteNew = 1;
  static const int notifyNoteComment = 2;
  static const int notifyNoteUpdate = 3;
  static const int notifyNoteDelete = 4;
  static const int notifyMarkNoteForDeletion = 5;
  // Definitions for note actions.
  static const int noteActionCreate = 1;
  static const int noteActionComment = 2;
  static const int noteActionSummary = 3;
  static const int noteActionSubscribe = 4;
  static const int noteActionUnsubscribe = 5;
  static const int noteActionAssign = 6;
  static const int noteActionUnassign = 7;
  static const int noteActionStatus = 8;
  static const int noteActionPassage = 9;
  static const int noteActionSeverity = 10;
  static const int noteActionBible = 11;
  static const int noteActionMarkDeletion = 12;
  static const int noteActionUnmarkDeletion = 13;
  static const int noteActionDelete = 14;
  static const int noteActionGet = 15;
  void addComment (int identifier, const string& comment);
  void setSummary (int identifier, const string& summary);
  void subscribe (int identifier);
  void unsubscribe (int identifier);
  void assignUser (int identifier, const string& user);
  void unassignUser (int identifier, const string& user);
  void setStatus (int identifier, const string& status);
  void setPassages (int identifier, const vector <Passage> & passages);
  void setRawSeverity (int identifier, int severity);
  void setBible (int identifier, const string& bible);
  void markForDeletion (int identifier);
  void unmarkForDeletion (int identifier);
  void erase (int identifier);
  void handlerNewNote (int identifier);
  void handlerAddComment (int identifier);
  void handlerUpdateNote (int identifier);
  void handlerAssignNote (int identifier, const string& user);
  void handlerMarkNoteForDeletion (int identifier);
  void handlerDeleteNote (int identifier);
  bool handleEmailNew (string from, string subject, string body);
  string generalBibleName ();
private:
  void * webserver_request;
  void notifyUsers (int identifier, int notification);
  void emailUsers (int identifier, const string& label, const vector <string> & users, bool postpone);
  bool handleEmailComment (string from, string subject, string body);
};


#endif
