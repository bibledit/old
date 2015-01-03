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


#include <filter/url.h>
#include <index/index.h>
#include <bootstrap/bootstrap.h>
#include <webserver/http.h>
#include <session/login.h>
#include <session/logout.h>
#include <session/password.h>
#include <session/signup.h>
#include <database/config/general.h>
#include <setup/index.h>
#include <journal/index.h>
#include <config/logic.h>
#include <help/index.h>
#include <email/index.h>
#include <user/notifications.h>
#include <user/account.h>
#include <manage/indexing.h>
#include <manage/users.h>
#include <administration/language.h>
#include <administration/timezone.h>
#include <styles/indext.h>
#include <styles/indexm.h>
#include <styles/sheetm.h>
#include <styles/view.h>
#include <fonts/index.h>
#include <versification/index.h>
#include <versification/system.h>
#include <bible/manage.h>
#include <bible/settings.h>
#include <bible/book.h>
#include <bible/chapter.h>
#include <bible/import_usfm.h>
#include <bible/import_bibleworks.h>
#include <compare/index.h>
#include <jobs/index.h>
#include <bible/abbreviations.h>
#include <bible/order.h>
#include <bible/css.h>
#include <editverse/index.h>
#include <editverse/id.h>
#include <editverse/load.h>
#include <editverse/save.h>
#include <navigation/update.h>
#include <navigation/poll.h>


// This function is the first function to be called when a client requests a page or file.
// Based on the request from the client, it decides what other functions to call to assemble the response.
void bootstrap_index (Webserver_Request * request)
{
  string extension = filter_url_get_extension (request->get);
  string url = request->get.substr (1);
  
  // Serve graphics, stylesheets, JavaScript.
  if ((extension  == "ico") || (extension  == "png") || (extension == "css") || (extension == "js")) http_serve_file (request);
  
  // Force setup.
  else if (config_logic_version () != Database_Config_General::getInstalledVersion ()) request->reply = setup_index (request);

  // Home page.
  else if ((url == index_index_url ()) && index_index_acl (request)) request->reply = index_index (request);
  
  // Login and logout.
  else if ((url == session_login_url ()) && session_login_acl (request)) request->reply = session_login (request);
  else if ((url == session_logout_url ()) && session_logout_acl (request)) request->reply = session_logout (request);
  else if ((url == session_password_url ()) && session_password_acl (request)) request->reply = session_password (request);
  else if ((url == session_signup_url ()) && session_signup_acl (request)) request->reply = session_signup (request);
  
  // Bible menu.
  else if ((url == bible_manage_url ()) && bible_manage_acl (request)) request->reply = bible_manage (request);
  else if ((url == bible_settings_url ()) && bible_settings_acl (request)) request->reply = bible_settings (request);
  else if ((url == bible_book_url ()) && bible_book_acl (request)) request->reply = bible_book (request);
  else if ((url == bible_chapter_url ()) && bible_chapter_acl (request)) request->reply = bible_chapter (request);
  else if ((url == bible_import_usfm_url ()) && bible_import_usfm_acl (request)) request->reply = bible_import_usfm (request);
  else if ((url == bible_import_bibleworks_url ()) && bible_import_bibleworks_acl (request)) request->reply = bible_import_bibleworks (request);
  else if ((url == compare_index_url ()) && compare_index_acl (request)) request->reply = compare_index (request);
  else if ((url == bible_abbreviations_url ()) && bible_abbreviations_acl (request)) request->reply = bible_abbreviations (request);
  else if ((url == bible_order_url ()) && bible_order_acl (request)) request->reply = bible_order (request);
  else if ((url == bible_css_url ()) && bible_css_acl (request)) request->reply = bible_css (request);
  else if ((url == editverse_index_url ()) && editverse_index_acl (request)) request->reply = editverse_index (request);
  
  // Changes menu.
  else if ((url == journal_index_url ()) && journal_index_acl (request)) request->reply = journal_index (request);

  // Settings menu.
  else if ((url == manage_users_url ()) && manage_users_acl (request)) request->reply = manage_users (request);
  else if ((url == manage_indexing_url ()) && manage_indexing_acl (request)) request->reply = manage_indexing (request);
  else if ((url == administration_language_url ()) && administration_language_acl (request)) request->reply = administration_language (request);
  else if ((url == administration_timezone_url ()) && administration_timezone_acl (request)) request->reply = administration_timezone (request);
  else if ((url == email_index_url ()) && email_index_acl (request)) request->reply = email_index (request);
  else if ((url == styles_indext_url ()) && styles_indext_acl (request)) request->reply = styles_indext (request);
  else if ((url == styles_indexm_url ()) && styles_indexm_acl (request)) request->reply = styles_indexm (request);
  else if ((url == styles_sheetm_url ()) && styles_sheetm_acl (request)) request->reply = styles_sheetm (request);
  else if ((url == styles_view_url ()) && styles_view_acl (request)) request->reply = styles_view (request);
  else if ((url == versification_index_url ()) && versification_index_acl (request)) request->reply = versification_index (request);
  else if ((url == versification_system_url ()) && versification_system_acl (request)) request->reply = versification_system (request);
  else if ((url == fonts_index_url ()) && fonts_index_acl (request)) request->reply = fonts_index (request);
  
  // Help menu.
  else if ((help_index_url (url)) && help_index_acl (request, url)) request->reply = help_index (request, url);

  // User menu.  
  else if ((url == user_notifications_url ()) && user_notifications_acl (request)) request->reply = user_notifications (request);
  else if ((url == user_account_url ()) && user_account_acl (request)) request->reply = user_account (request);

  // Items not in any menu.
  else if ((url == jobs_index_url ()) && jobs_index_acl (request)) request->reply = jobs_index (request);

  // AJAX calls.
  else if ((url == navigation_update_url ()) && navigation_update_acl (request)) request->reply = navigation_update (request);
  else if ((url == navigation_poll_url ()) && navigation_poll_acl (request)) request->reply = navigation_poll (request);
  else if ((url == editverse_id_url ()) && editverse_id_acl (request)) request->reply = editverse_id (request);
  else if ((url == editverse_load_url ()) && editverse_load_acl (request)) request->reply = editverse_load (request);
  else if ((url == editverse_save_url ()) && editverse_save_acl (request)) request->reply = editverse_save (request);
  
  // Forward the browser to the default home page.
  else {
    redirect_browser (index_index_url (), request);
  }
}


