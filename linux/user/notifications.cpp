/*
Copyright (©) 2003-2016 Teus Benschop.

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
#include <database/config/general.h>
#include <locale/translate.h>
#include <config/logic.h>
#include <client/logic.h>
#include <assets/header.h>
#include <menu/logic.h>
#include <access/logic.h>
#include <access/bible.h>


string user_notifications_url ()
{
  return "user/notifications";
}


bool user_notifications_acl (void * webserver_request)
{
  // Consultant has access.
  if (Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ())) return true;
  // Whoever can view notes has access.
  if (access_logic_privilege_view_notes (webserver_request)) return true;
  // Whoever has access to a Bible has access to this page.
  bool read, write;
  access_a_bible (webserver_request, read, write);
  if (read) return true;
  // No access.
  return false;
}


string user_notifications (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Config_User database_config_user = Database_Config_User (webserver_request);
  
  string id = request->post ["id"];

  if (id == "editednotessubscription") {
    bool state = !database_config_user.getSubscribeToConsultationNotesEditedByMe ();
    database_config_user.setSubscribeToConsultationNotesEditedByMe (state);
    return get_tick_box (state);
  }

  if (id == "anynotessubscription") {
    bool state = !database_config_user.getNotifyMeOfAnyConsultationNotesEdits ();
    database_config_user.setNotifyMeOfAnyConsultationNotesEdits (state);
    return get_tick_box (state);
  }

  if (id == "emailconfirmationyourposts") {
    bool state = !database_config_user.getNotifyMeOfMyPosts ();
    database_config_user.setNotifyMeOfMyPosts (state);
    return get_tick_box (state);
  }

  if (id == "subscribednotenotification") {
    bool state = !database_config_user.getSubscribedConsultationNoteNotification ();
    database_config_user.setSubscribedConsultationNoteNotification (state);
    return get_tick_box (state);
  }

  if (id == "notesassignment") {
    bool state = !database_config_user.getAssignedToConsultationNotesChanges ();
    database_config_user.setAssignedToConsultationNotesChanges (state);
    return get_tick_box (state);
  }
  
  if (id == "assignednotenotification") {
    bool state = !database_config_user.getAssignedConsultationNoteNotification ();
    database_config_user.setAssignedConsultationNoteNotification (state);
    return get_tick_box (state);
  }
  
  if (id == "suppressemailsfromnotesyouupdated") {
    bool state = !database_config_user.getSuppressMailFromYourUpdatesNotes ();
    database_config_user.setSuppressMailFromYourUpdatesNotes (state);
    return get_tick_box (state);
  }

  if (id == "assignednotesnotification") {
    bool state = !database_config_user.getAssignedNotesStatisticsNotification ();
    database_config_user.setAssignedNotesStatisticsNotification (state);
    return get_tick_box (state);
  }

  if (id == "subscribednotesnotification") {
    bool state = !database_config_user.getSubscribedNotesStatisticsNotification ();
    database_config_user.setSubscribedNotesStatisticsNotification (state);
    return get_tick_box (state);
  }

  if (id == "deletednotenotification") {
    bool state = !database_config_user.getDeletedConsultationNoteNotification ();
    database_config_user.setDeletedConsultationNoteNotification (state);
    return get_tick_box (state);
  }

  if (id == "postponenewnotesmails") {
    bool state = !database_config_user.getPostponeNewNotesMails ();
    database_config_user.setPostponeNewNotesMails (state);
    return get_tick_box (state);
  }

  if (id == "anyonechangesemailnotification") {
    bool state = !database_config_user.getBibleChangesNotification ();
    database_config_user.setBibleChangesNotification (state);
    return get_tick_box (state);
  }

  if (id == "anyonechangesonlinenotifications") {
    bool state = !database_config_user.getGenerateChangeNotifications ();
    database_config_user.setGenerateChangeNotifications (state);
    return get_tick_box (state);
  }

  if (id == "pendingchangenotifications") {
    bool state = !database_config_user.getPendingChangesNotification ();
    database_config_user.setPendingChangesNotification (state);
    return get_tick_box (state);
  }
  
  if (id == "mychangesemailnotifications") {
    bool state = !database_config_user.getUserChangesNotification ();
    database_config_user.setUserChangesNotification (state);
    return get_tick_box (state);
  }
  
  if (id == "mychangesonlinenotifications") {
    bool state = !database_config_user.getUserChangesNotificationsOnline ();
    database_config_user.setUserChangesNotificationsOnline (state);
    return get_tick_box (state);
  }

  if (id == "contributorschangesonlinenotifications") {
    bool state = !database_config_user.getContributorChangesNotificationsOnline ();
    database_config_user.setContributorChangesNotificationsOnline (state);
    return get_tick_box (state);
  }

  if (id == "biblechecksnotification") {
    bool state = !database_config_user.getBibleChecksNotification ();
    database_config_user.setBibleChecksNotification (state);
    return get_tick_box (state);
  }

  if (id == "sprintprogressnotification") {
    bool state = !database_config_user.getSprintProgressNotification ();
    database_config_user.setSprintProgressNotification (state);
    return get_tick_box (state);
  }
  
  string page;

  Assets_Header header = Assets_Header (translate("Notifications"), webserver_request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  page = header.run ();

  Assets_View view;

  view.set_variable ("edited_notes_subscription", get_tick_box (database_config_user.getSubscribeToConsultationNotesEditedByMe ()));
  
  view.set_variable ("any_notes_subscription", get_tick_box (database_config_user.getNotifyMeOfAnyConsultationNotesEdits ()));
  
  view.set_variable ("email_confirmation_your_posts", get_tick_box (database_config_user.getNotifyMeOfMyPosts ()));
  
  view.set_variable ("subscription_notification", get_tick_box (database_config_user.getSubscribedConsultationNoteNotification ()));
  
  view.set_variable ("notes_assignment", get_tick_box (database_config_user.getAssignedToConsultationNotesChanges ()));
  
  view.set_variable ("assignment_notification", get_tick_box (database_config_user.getAssignedConsultationNoteNotification ()));
  
  view.set_variable ("suppress_mails_from_your_updates", get_tick_box (database_config_user.getSuppressMailFromYourUpdatesNotes ()));
  
  view.set_variable ("assigned_notes_notifications", get_tick_box (database_config_user.getAssignedNotesStatisticsNotification ()));
  
  view.set_variable ("subscribed_notes_notifications", get_tick_box (database_config_user.getSubscribedNotesStatisticsNotification ()));
  
  view.set_variable ("deleted_note_notification", get_tick_box (database_config_user.getDeletedConsultationNoteNotification ()));
  
  view.set_variable ("postpone_new_notes_mails", get_tick_box (database_config_user.getPostponeNewNotesMails ()));
  
  view.set_variable ("anyone_changes_email_notification", get_tick_box (database_config_user.getBibleChangesNotification ()));
  
  view.set_variable ("anyone_changes_online_notifications", get_tick_box (database_config_user.getGenerateChangeNotifications ()));
  
  view.set_variable ("pending_changes_notifications", get_tick_box (database_config_user.getPendingChangesNotification ()));
  
  view.set_variable ("my_changes_email_notifications", get_tick_box (database_config_user.getUserChangesNotification ()));
  
  view.set_variable ("my_changes_online_notifications", get_tick_box (database_config_user.getUserChangesNotificationsOnline ()));
  
  view.set_variable ("contributors_changes_online_notifications", get_tick_box (database_config_user.getContributorChangesNotificationsOnline ()));
  
  view.set_variable ("bible_checks_notification", get_tick_box (database_config_user.getBibleChecksNotification ()));
  
  view.set_variable ("sprint_progress_notification", get_tick_box (database_config_user.getSprintProgressNotification ()));
  
  if (config_logic_client_prepared ()) {
    view.enable_zone ("client");
  } else {
    view.enable_zone ("server");
  }

  view.set_variable ("url", client_logic_link_to_cloud (user_notifications_url (), translate("You can set the notifications in Bibledit Cloud.")));

  // The bits accessible to the user depends on the user's privileges.
  bool read_bible, write_bible;
  access_a_bible (webserver_request, read_bible, write_bible);
  if (read_bible) view.enable_zone ("readbible");
  if (write_bible) view.enable_zone ("writebible");
  if (Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ()))
    view.enable_zone ("consultant");

  
  page += view.render ("user", "notifications");

  page += Assets_Page::footer ();

  return page;
}
