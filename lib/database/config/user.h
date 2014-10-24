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


#pragma once


#include <iostream>
#include <cstdlib>
#include <vector>
#include <webserver/request.h>


using namespace std;


class Database_Config_User
{
public:
  Database_Config_User (Webserver_Request * request_in);
  ~Database_Config_User ();
  void trim ();
  string getStylesheet ();
  void setStylesheet (string sheet);
  string getBible ();
  void setBible (string bible);
  bool getSubscribeToConsultationNotesEditedByMe ();
  void setSubscribeToConsultationNotesEditedByMe (bool value);
  bool getNotifyMeOfAnyConsultationNotesEdits ();
  bool getNotifyUserOfAnyConsultationNotesEdits (string username);
  void setNotifyMeOfAnyConsultationNotesEdits (bool value);
  bool getSubscribedConsultationNoteNotification ();
  bool getUserSubscribedConsultationNoteNotification (string username);
  void setSubscribedConsultationNoteNotification (bool value);
  bool getAssignedToConsultationNotesChanges ();
  bool getUserAssignedToConsultationNotesChanges (string username);
  void setAssignedToConsultationNotesChanges (bool value);
  bool getGenerateChangeNotifications ();
  bool getUserGenerateChangeNotifications (string username);
  void setGenerateChangeNotifications (bool value);
  bool getAssignedConsultationNoteNotification ();
  bool getUserAssignedConsultationNoteNotification (string username);
  void setAssignedConsultationNoteNotification (bool value);
  int getConsultationNotesPassageSelector ();
  void setConsultationNotesPassageSelector (int value);
  int getConsultationNotesEditSelector ();
  void setConsultationNotesEditSelector (int value);
  int getConsultationNotesNonEditSelector ();
  void setConsultationNotesNonEditSelector (int value);
  string getConsultationNotesStatusSelector ();
  void setConsultationNotesStatusSelector (string value);
  string getConsultationNotesBibleSelector ();
  void setConsultationNotesBibleSelector (string value);
  string getConsultationNotesAssignmentSelector ();
  void setConsultationNotesAssignmentSelector (string value);
  int getConsultationNotesSubscriptionSelector ();
  void setConsultationNotesSubscriptionSelector (int value);
  int getConsultationNotesSeveritySelector ();
  void setConsultationNotesSeveritySelector (int value);
  int getConsultationNotesTextSelector ();
  void setConsultationNotesTextSelector (int value);
  string getConsultationNotesSearchText ();
  void setConsultationNotesSearchText (string value);
  int getConsultationNotesPassageInclusionSelector ();
  void setConsultationNotesPassageInclusionSelector (int value);
  int getConsultationNotesTextInclusionSelector ();
  void setConsultationNotesTextInclusionSelector (int value);
  bool getBibleChangesNotification ();
  bool getUserBibleChangesNotification (string username);
  void setBibleChangesNotification (bool value);
  bool getDeletedConsultationNoteNotification ();
  bool getUserDeletedConsultationNoteNotification (string username);
  void setDeletedConsultationNoteNotification (bool value);
  bool getBibleChecksNotification ();
  bool getUserBibleChecksNotification (string username);
  void setBibleChecksNotification (bool value);
  bool getPendingChangesNotification ();
  bool getUserPendingChangesNotification (string username);
  void setPendingChangesNotification (bool value);
  bool getUserChangesNotification ();
  bool getUserUserChangesNotification (string username);
  void setUserChangesNotification (bool value);
  bool getAssignedNotesStatisticsNotification ();
  bool getUserAssignedNotesStatisticsNotification (string username);
  void setAssignedNotesStatisticsNotification (bool value);
  bool getSubscribedNotesStatisticsNotification ();
  bool getUserSubscribedNotesStatisticsNotification (string username);
  void setSubscribedNotesStatisticsNotification (bool value);
  bool getNotifyMeOfMyPosts ();
  bool getUserNotifyMeOfMyPosts (string username);
  void setNotifyMeOfMyPosts (bool value);
  bool getSuppressMailFromYourUpdatesNotes ();
  bool getUserSuppressMailFromYourUpdatesNotes (string username);
  void setSuppressMailFromYourUpdatesNotes (bool value);
  vector <string> getActiveResources ();
  void setActiveResources (vector <string> values);
  vector <string> getConsistencyResources ();
  void setConsistencyResources (vector <string> values);
  int getSprintMonth ();
  void setSprintMonth (int value);
  int getSprintYear ();
  void setSprintYear (int value);
  bool getSprintProgressNotification ();
  bool getUserSprintProgressNotification (string username);
  void setSprintProgressNotification (bool value);
  bool getUserChangesNotificationsOnline ();
  bool getUserUserChangesNotificationsOnline (string username);
  void setUserChangesNotificationsOnline (bool value);
  string getWorkbenchURLs ();
  void setWorkbenchURLs (string value);
  string getWorkbenchWidths ();
  void setWorkbenchWidths (string value);
  string getWorkbenchHeights ();
  void setWorkbenchHeights (string value);
  string getActiveWorkbench ();
  void setActiveWorkbench (string value);
  bool getPostponeNewNotesMails ();
  void setPostponeNewNotesMails (bool value);
  string getRecentlyAppliedStyles ();
  void setRecentlyAppliedStyles (string values);
  vector <string> getPrintResources ();
  vector <string> getPrintResourcesForUser (string user);
  void setPrintResources (vector <string> values);
  string getPrintPassageFrom ();
  string getPrintPassageFromForUser (string user);
  void setPrintPassageFrom (string value);
  string getPrintPassageTo ();
  string getPrintPassageToForUser (string user);
  void setPrintPassageTo (string value);
  string getSourceXrefBible ();
  void setSourceXrefBible (string bible);
  string getTargetXrefBible ();
  void setTargetXrefBible (string bible);
  int getFocusedBook ();
  void setFocusedBook (int book);
  int getFocusedChapter ();
  void setFocusedChapter (int chapter);
  int getFocusedVerse ();
  void setFocusedVerse (int verse);
  vector <string> getUpdatedSettings ();
  void setUpdatedSettings (vector <string> values);
  void addUpdatedSetting (string value);
  void removeUpdatedSetting (string value);
private:
  Webserver_Request * request;
  string file (string user, const char * key);
  string getValue (const char * key, const char * default_value);
  bool getValue (const char * key, bool default_value);
  int getValue (const char * key, int default_value);
  string getValueForUser (string user, const char * key, const char * default_value);
  bool getValueForUser (string user, const char * key, bool default_value);
  int getValueForUser (string user, const char * key, int default_value);
  void setValue (const char * key, string value);
  void setValue (const char * key, bool value);
  void setValue (const char * key, int value);
  vector <string> getList (const char * key);
  vector <string> getListForUser (string user, const char * key);
  void setList (const char * key, vector <string> values);
  const char * keySprintMonth ();
  const char * keySprintYear ();
};


