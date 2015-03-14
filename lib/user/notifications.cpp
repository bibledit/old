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


#include <user/notifications.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <database/config/user.h>
#include <locale/translate.h>


string user_notifications_url ()
{
  return "user/notifications";
}


bool user_notifications_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string user_notifications (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;

  page = Assets_Page::header (translate("Notifications"), webserver_request, "");

  Assets_View view = Assets_View ();

  Database_Config_User database_config_user = Database_Config_User (webserver_request);
  
  if (request->query.count ("editednotessubscriptiontoggle")) {
    database_config_user.setSubscribeToConsultationNotesEditedByMe (!database_config_user.getSubscribeToConsultationNotesEditedByMe ());
  }
  view.set_variable ("edited_notes_subscription", get_tick_box (database_config_user.getSubscribeToConsultationNotesEditedByMe ()));
  
  if (request->query.count ("anynotessubscriptiontoggle")) {
    database_config_user.setNotifyMeOfAnyConsultationNotesEdits(!database_config_user.getNotifyMeOfAnyConsultationNotesEdits());
  }
  view.set_variable ("any_notes_subscription", get_tick_box (database_config_user.getNotifyMeOfAnyConsultationNotesEdits ()));
  
  if (request->query.count ("emailconfirmationyourpoststoggle")) {
    database_config_user.setNotifyMeOfMyPosts (!database_config_user.getNotifyMeOfMyPosts ());
  }
  view.set_variable ("email_confirmation_your_posts", get_tick_box (database_config_user.getNotifyMeOfMyPosts ()));
  
  if (request->query.count ("subscribednotenotificationtoggle")) {
    database_config_user.setSubscribedConsultationNoteNotification(!database_config_user.getSubscribedConsultationNoteNotification());
  }
  view.set_variable ("subscription_notification", get_tick_box (database_config_user.getSubscribedConsultationNoteNotification ()));
  
  if (request->query.count ("notesassignmenttoggle")) {
    database_config_user.setAssignedToConsultationNotesChanges(!database_config_user.getAssignedToConsultationNotesChanges());
  }
  view.set_variable ("notes_assignment", get_tick_box (database_config_user.getAssignedToConsultationNotesChanges ()));
  
  if (request->query.count ("assignednotenotificationtoggle")) {
    database_config_user.setAssignedConsultationNoteNotification(!database_config_user.getAssignedConsultationNoteNotification());
  }
  view.set_variable ("assignment_notification", get_tick_box (database_config_user.getAssignedConsultationNoteNotification ()));
  
  if (request->query.count ("suppressemailsfromnotesyouupdatedtoggle")) {
    database_config_user.setSuppressMailFromYourUpdatesNotes (!database_config_user.getSuppressMailFromYourUpdatesNotes ());
  }
  view.set_variable ("suppress_mails_from_your_updates", get_tick_box (database_config_user.getSuppressMailFromYourUpdatesNotes ()));
  
  if (request->query.count ("assignednotesnotificationtoggle")) {
    database_config_user.setAssignedNotesStatisticsNotification (!database_config_user.getAssignedNotesStatisticsNotification ());
  }
  view.set_variable ("assigned_notes_notifications", get_tick_box (database_config_user.getAssignedNotesStatisticsNotification ()));
  
  if (request->query.count ("subscribednotesnotificationtoggle")) {
    database_config_user.setSubscribedNotesStatisticsNotification (!database_config_user.getSubscribedNotesStatisticsNotification ());
  }
  view.set_variable ("subscribed_notes_notifications", get_tick_box (database_config_user.getSubscribedNotesStatisticsNotification ()));
  
  if (request->query.count ("deletednotenotificationtoggle")) {
    database_config_user.setDeletedConsultationNoteNotification(!database_config_user.getDeletedConsultationNoteNotification());
  }
  view.set_variable ("deleted_note_notification", get_tick_box (database_config_user.getDeletedConsultationNoteNotification ()));
  
  if (request->query.count ("postponenewnotesmailstoggle")) {
    database_config_user.setPostponeNewNotesMails (!database_config_user.getPostponeNewNotesMails ());
  }
  view.set_variable ("postpone_new_notes_mails", get_tick_box (database_config_user.getPostponeNewNotesMails ()));
  
  if (request->query.count ("biblechangesnotificationtoggle")) {
    database_config_user.setBibleChangesNotification(!database_config_user.getBibleChangesNotification());
  }
  view.set_variable ("bible_changes_notification", get_tick_box (database_config_user.getBibleChangesNotification ()));
  
  if (request->query.count ("changenotificationstoggle")) {
    database_config_user.setGenerateChangeNotifications(!database_config_user.getGenerateChangeNotifications());
  }
  view.set_variable ("change_notifications", get_tick_box (database_config_user.getGenerateChangeNotifications ()));
  
  if (request->query.count ("pendingchangenotificationstoggle")) {
    database_config_user.setPendingChangesNotification (!database_config_user.getPendingChangesNotification ());
  }
  view.set_variable ("pending_changes_notifications", get_tick_box (database_config_user.getPendingChangesNotification ()));
  
  if (request->query.count ("userchangesnotificationstoggle")) {
    database_config_user.setUserChangesNotification (!database_config_user.getUserChangesNotification ());
  }
  view.set_variable ("user_changes_notifications", get_tick_box (database_config_user.getUserChangesNotification ()));
  
  if (request->query.count ("userchangenotificationsonlinetoggle")) {
    database_config_user.setUserChangesNotificationsOnline (!database_config_user.getUserChangesNotificationsOnline ());
  }
  view.set_variable ("user_changes_notifications_online", get_tick_box (database_config_user.getUserChangesNotificationsOnline ()));
  
  if (request->query.count ("biblechecksnotificationtoggle")) {
    database_config_user.setBibleChecksNotification (!database_config_user.getBibleChecksNotification ());
  }
  view.set_variable ("bible_checks_notification", get_tick_box (database_config_user.getBibleChecksNotification ()));
  
  if (request->query.count ("sprintprogressnotificationtoggle")) {
    database_config_user.setSprintProgressNotification (!database_config_user.getSprintProgressNotification ());
  }
  view.set_variable ("sprint_progress_notification", get_tick_box (database_config_user.getSprintProgressNotification ()));
  
  page += view.render ("user", "notifications");

  page += Assets_Page::footer ();

  return page;
}
