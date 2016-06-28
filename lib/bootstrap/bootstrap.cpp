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


#include <bootstrap/bootstrap.h>
#include <webserver/request.h>
#include <webserver/http.h>
#include <filter/url.h>
#include <index/index.h>
#include <session/login.h>
#include <session/logout.h>
#include <session/password.h>
#include <session/signup.h>
#include <session/switch.h>
#include <database/config/general.h>
#include <database/cache.h>
#include <setup/index.h>
#include <journal/index.h>
#include <config/logic.h>
#include <config/globals.h>
#include <help/index.h>
#include <email/index.h>
#include <user/notifications.h>
#include <user/account.h>
#include <manage/index.h>
#include <manage/users.h>
#include <manage/exports.h>
#include <manage/hyphenation.h>
#include <manage/write.h>
#include <manage/privileges.h>
#include <administration/language.h>
#include <administration/timezone.h>
#include <collaboration/index.h>
#include <collaboration/flash.h>
#include <collaboration/open.h>
#include <collaboration/password.h>
#include <collaboration/secure.h>
#include <collaboration/direction.h>
#include <styles/indext.h>
#include <styles/indexm.h>
#include <styles/sheetm.h>
#include <styles/view.h>
#include <versification/index.h>
#include <versification/system.h>
#include <bible/manage.h>
#include <bible/settings.h>
#include <bible/book.h>
#include <bible/chapter.h>
#include <bible/import.h>
#include <bible/abbreviations.h>
#include <bible/order.h>
#include <bible/css.h>
#include <bible/editing.h>
#include <compare/index.h>
#include <jobs/index.h>
#include <editverse/index.h>
#include <editverse/load.h>
#include <editverse/save.h>
#include <navigation/update.h>
#include <navigation/poll.h>
#include <editusfm/index.h>
#include <editusfm/focus.h>
#include <editusfm/load.h>
#include <editusfm/offset.h>
#include <editusfm/save.h>
#include <edit/index.h>
#include <edit/edit.h>
#include <edit/focus.h>
#include <edit/id.h>
#include <edit/load.h>
#include <edit/offset.h>
#include <edit/save.h>
#include <edit/styles.h>
#include <search/all.h>
#include <search/index.h>
#include <search/replace.h>
#include <search/getids.h>
#include <search/replacepre.h>
#include <search/replacego.h>
#include <search/search2.h>
#include <search/replace2.h>
#include <search/replacepre2.h>
#include <search/getids2.h>
#include <search/replacego2.h>
#include <search/similar.h>
#include <search/strongs.h>
#include <search/strong.h>
#include <search/originals.h>
#include <workbench/index.h>
#include <workbench/organize.h>
#include <workbench/settings.h>
#include <sendreceive/index.h>
#include <client/index.h>
#include <sync/setup.h>
#include <sync/settings.h>
#include <sync/bibles.h>
#include <sync/notes.h>
#include <sync/usfmresources.h>
#include <sync/changes.h>
#include <sync/files.h>
#include <sync/resources.h>
#include <sync/mail.h>
#include <resource/index.h>
#include <resource/organize.h>
#include <resource/get.h>
#include <resource/bible2resource.h>
#include <resource/manage.h>
#include <resource/print.h>
#include <resource/download.h>
#include <resource/select.h>
#include <resource/images.h>
#include <resource/image.h>
#include <resource/img.h>
#include <resource/imagefetch.h>
#include <resource/sword.h>
#include <resource/cache.h>
#include <resource/user9edit.h>
#include <resource/user1edit.h>
#include <resource/user9view.h>
#include <resource/user1view.h>
#include <mapping/index.h>
#include <mapping/map.h>
#include <notes/index.h>
#include <notes/poll.h>
#include <notes/notes.h>
#include <notes/create.h>
#include <notes/select.h>
#include <notes/note.h>
#include <notes/comment.h>
#include <notes/actions.h>
#include <notes/assign-1.h>
#include <notes/assign-n.h>
#include <notes/unassign-n.h>
#include <notes/status-1.h>
#include <notes/status-n.h>
#include <notes/verses.h>
#include <notes/severity-1.h>
#include <notes/severity-n.h>
#include <notes/bible-1.h>
#include <notes/bible-n.h>
#include <notes/bulk.h>
#include <notes/edit.h>
#include <notes/summary.h>
#include <notes/click.h>
#include <changes/changes.h>
#include <changes/change.h>
#include <changes/manage.h>
#include <index/listing.h>
#include <sprint/index.h>
#include <checks/index.h>
#include <checks/settings.h>
#include <checks/settingspatterns.h>
#include <checks/settingssentences.h>
#include <consistency/index.h>
#include <checks/suppress.h>
#include <consistency/poll.h>
#include <consistency/input.h>
#include <xrefs/index.h>
#include <xrefs/source.h>
#include <xrefs/target.h>
#include <xrefs/help.h>
#include <xrefs/extract.h>
#include <xrefs/interpret.h>
#include <xrefs/translate.h>
#include <xrefs/clear.h>
#include <xrefs/insert.h>
#include <xrefs/move.h>
#include <xrefs/next.h>
#include <webbible/search.h>
#include <editone/index.h>
#include <editone/load.h>
#include <editone/save.h>
#include <editone/verse.h>
#include <debug/index.h>
#include <paratext/index.h>
#include <personalize/index.h>
#include <menu/index.h>
#include <fonts/logic.h>
#include <lexicon/definition.h>
#include <database/logs.h>
#include <public/index.h>
#include <public/login.h>
#include <public/chapter.h>
#include <public/notes.h>
#include <public/new.h>
#include <public/create.h>
#include <public/note.h>
#include <public/comment.h>
#include <basic/index.h>
#include <editor/select.h>


// Internal function to check whether a request coming from the browser is considered secure enough.
// It returns true if the security is okay.
bool bootstrap_browser_request_security_okay (Webserver_Request * request)
{
  // If the request is made via https, the security is OK.
  if (request->secure) return true;

  // At this stage the request is made via plain http.
  // If https is not enforced for the browser, the security is OK.
  if (!config_globals_enforce_https_browser) return true;
  
  // Not secure enough:
  return false;
}


// This function is the first function to be called when a client requests a page or file.
// Based on the request from the client,
// it decides which other function to call to obtain the response.
void bootstrap_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string extension = filter_url_get_extension (request->get);
  string url = request->get.substr (1);
  
  // Serve graphics, stylesheets, JavaScript, fonts.
  if (   (extension == "ico")
      || (extension == "png")
      || (extension == "gif")
      || (extension == "jpg")
      || (extension == "css")
      || (extension == "js")
      || (Fonts_Logic::isFont (extension))
      || (extension == "sh")
      || (extension == "svg")
      ) http_serve_cache_file (request);
  else if ((url == resource_imagefetch_url ()) && resource_imagefetch_acl (request)) request->reply = resource_imagefetch (request);

  // Serve resource downloads.
  else if ((request->get.find (Database_Cache::fragment ()) != string::npos) && (extension == "sqlite")) http_serve_cache_file (request);
  
  // Serve initialization notice.
  else if (config_logic_version () != Database_Config_General::getInstalledDatabaseVersion ()) request->reply = setup_initialization_notice ();
  
  // Force setup.
  else if (config_logic_version () != Database_Config_General::getInstalledInterfaceVersion ()) request->reply = setup_index (request);

  // Home page and menu.
  else if ((url == index_index_url ()) && bootstrap_browser_request_security_okay (request) && index_index_acl (request)) request->reply = index_index (request);
  
  else if ((url == menu_index_url ()) && bootstrap_browser_request_security_okay (request) && menu_index_acl (request)) request->reply = menu_index (request);
  
  // Login and logout.
  else if ((url == session_login_url ()) && bootstrap_browser_request_security_okay (request) && session_login_acl (request)) request->reply = session_login (request);
  
  else if ((url == session_logout_url ()) && bootstrap_browser_request_security_okay (request) && session_logout_acl (request)) request->reply = session_logout (request);
  
  else if ((url == session_password_url ()) && bootstrap_browser_request_security_okay (request) && session_password_acl (request)) request->reply = session_password (request);
  
  else if ((url == session_signup_url ()) && bootstrap_browser_request_security_okay (request) && session_signup_acl (request)) request->reply = session_signup (request);
  
  else if ((url == session_switch_url ()) && bootstrap_browser_request_security_okay (request) && session_switch_acl (request)) request->reply = session_switch (request);
  
  // Bible menu.
  else if ((url == bible_manage_url ()) && bootstrap_browser_request_security_okay (request) && bible_manage_acl (request)) request->reply = bible_manage (request);
  
  else if ((url == bible_settings_url ()) && bootstrap_browser_request_security_okay (request) && bible_settings_acl (request)) request->reply = bible_settings (request);
  
  else if ((url == bible_book_url ()) && bootstrap_browser_request_security_okay (request) && bible_book_acl (request)) request->reply = bible_book (request);
  
  else if ((url == bible_chapter_url ()) && bootstrap_browser_request_security_okay (request) && bible_chapter_acl (request)) request->reply = bible_chapter (request);
  
  else if ((url == bible_import_url ()) && bootstrap_browser_request_security_okay (request) && bible_import_acl (request)) request->reply = bible_import (request);
  
  else if ((url == compare_index_url ()) && bootstrap_browser_request_security_okay (request) && compare_index_acl (request)) request->reply = compare_index (request);
  
  else if ((url == bible_abbreviations_url ()) && bootstrap_browser_request_security_okay (request) && bible_abbreviations_acl (request)) request->reply = bible_abbreviations (request);
  
  else if ((url == bible_order_url ()) && bootstrap_browser_request_security_okay (request) && bible_order_acl (request)) request->reply = bible_order (request);
  
  else if ((url == bible_css_url ()) && bootstrap_browser_request_security_okay (request) && bible_css_acl (request)) request->reply = bible_css (request);
  
  else if ((url == editverse_index_url ()) && bootstrap_browser_request_security_okay (request) && editverse_index_acl (request)) request->reply = editverse_index (request);
  
  else if ((url == editone_index_url ()) && bootstrap_browser_request_security_okay (request) && editone_index_acl (request)) request->reply = editone_index (request);
  
  else if ((url == editusfm_index_url ()) && bootstrap_browser_request_security_okay (request) && editusfm_index_acl (request)) request->reply = editusfm_index (request);
  
  else if ((url == edit_index_url ()) && bootstrap_browser_request_security_okay (request) && edit_index_acl (request)) request->reply = edit_index (request);
  
  else if ((url == search_index_url ()) && bootstrap_browser_request_security_okay (request) && search_index_acl (request)) request->reply = search_index (request);
  
  else if ((url == workbench_index_url ()) && bootstrap_browser_request_security_okay (request) && workbench_index_acl (request)) request->reply = workbench_index (request);
  
  else if ((url == workbench_organize_url ()) && bootstrap_browser_request_security_okay (request) && workbench_organize_acl (request)) request->reply = workbench_organize (request);
  
  else if ((url == bible_editing_url ()) && bootstrap_browser_request_security_okay (request) && bible_editing_acl (request)) request->reply = bible_editing (request);
  
  else if ((url == resource_bible2resource_url ()) && bootstrap_browser_request_security_okay (request) && resource_bible2resource_acl (request)) request->reply = resource_bible2resource (request);
  
  else if ((url == checks_index_url ()) && bootstrap_browser_request_security_okay (request) && checks_index_acl (request)) request->reply = checks_index (request);
  
  else if ((url == checks_settings_url ()) && bootstrap_browser_request_security_okay (request) && checks_settings_acl (request)) request->reply = checks_settings (request);
  
  else if ((url == consistency_index_url ()) && bootstrap_browser_request_security_okay (request) && consistency_index_acl (request)) request->reply = consistency_index (request);
  
  // Notes menu.
  else if ((url == notes_index_url ()) && bootstrap_browser_request_security_okay (request) && notes_index_acl (request)) request->reply = notes_index (request);
  
  else if ((url == notes_create_url ()) && bootstrap_browser_request_security_okay (request) && notes_create_acl (request)) request->reply = notes_create (request);
  
  else if ((url == notes_select_url ()) && bootstrap_browser_request_security_okay (request) && notes_select_acl (request)) request->reply = notes_select (request);
  
  else if ((url == notes_note_url ()) && bootstrap_browser_request_security_okay (request) && notes_note_acl (request)) request->reply = notes_note (request);
  
  else if ((url == notes_comment_url ()) && bootstrap_browser_request_security_okay (request) && notes_comment_acl (request)) request->reply = notes_comment (request);
  
  else if ((url == notes_actions_url ()) && bootstrap_browser_request_security_okay (request) && notes_actions_acl (request)) request->reply = notes_actions (request);
  
  else if ((url == notes_assign_1_url ()) && bootstrap_browser_request_security_okay (request) && notes_assign_1_acl (request)) request->reply = notes_assign_1 (request);
  
  else if ((url == notes_assign_n_url ()) && bootstrap_browser_request_security_okay (request) && notes_assign_n_acl (request)) request->reply = notes_assign_n (request);
  
  else if ((url == notes_unassign_n_url ()) && bootstrap_browser_request_security_okay (request) && notes_unassign_n_acl (request)) request->reply = notes_unassign_n (request);
  
  else if ((url == notes_status_1_url ()) && bootstrap_browser_request_security_okay (request) && notes_status_1_acl (request)) request->reply = notes_status_1 (request);
  
  else if ((url == notes_status_n_url ()) && bootstrap_browser_request_security_okay (request) && notes_status_n_acl (request)) request->reply = notes_status_n (request);
  
  else if ((url == notes_verses_url ()) && bootstrap_browser_request_security_okay (request) && notes_verses_acl (request)) request->reply = notes_verses (request);
  
  else if ((url == notes_severity_1_url ()) && bootstrap_browser_request_security_okay (request) && notes_severity_1_acl (request)) request->reply = notes_severity_1 (request);
  
  else if ((url == notes_severity_n_url ()) && bootstrap_browser_request_security_okay (request) && notes_severity_n_acl (request)) request->reply = notes_severity_n (request);
  
  else if ((url == notes_bible_1_url ()) && bootstrap_browser_request_security_okay (request) && notes_bible_1_acl (request)) request->reply = notes_bible_1 (request);
  
  else if ((url == notes_bible_n_url ()) && bootstrap_browser_request_security_okay (request) && notes_bible_n_acl (request)) request->reply = notes_bible_n (request);
  
  else if ((url == notes_bulk_url ()) && bootstrap_browser_request_security_okay (request) && notes_bulk_acl (request)) request->reply = notes_bulk (request);
  
  else if ((url == notes_edit_url ()) && bootstrap_browser_request_security_okay (request) && notes_edit_acl (request)) request->reply = notes_edit (request);
  
  else if ((url == notes_summary_url ()) && bootstrap_browser_request_security_okay (request) && notes_summary_acl (request)) request->reply = notes_summary (request);

  // Resources menu.
  else if ((url == resource_index_url ()) && bootstrap_browser_request_security_okay (request) && resource_index_acl (request)) request->reply = resource_index (request);
  
  else if ((url == resource_organize_url ()) && bootstrap_browser_request_security_okay (request) && resource_organize_acl (request)) request->reply = resource_organize (request);
  
  else if ((url == resource_print_url ()) && bootstrap_browser_request_security_okay (request) && resource_print_acl (request)) request->reply = resource_print (request);
  
  else if ((url == resource_manage_url ()) && bootstrap_browser_request_security_okay (request) && resource_manage_acl (request)) request->reply = resource_manage (request);
  
  else if ((url == resource_download_url ()) && bootstrap_browser_request_security_okay (request) && resource_download_acl (request)) request->reply = resource_download (request);
  
  else if ((url == resource_images_url ()) && bootstrap_browser_request_security_okay (request) && resource_images_acl (request)) request->reply = resource_images (request);
  
  else if ((url == resource_sword_url ()) && bootstrap_browser_request_security_okay (request) && resource_sword_acl (request)) request->reply = resource_sword (request);
  
  else if ((url == resource_select_url ()) && bootstrap_browser_request_security_okay (request) && resource_select_acl (request)) request->reply = resource_select (request);
  
  else if ((url == resource_cache_url ()) && bootstrap_browser_request_security_okay (request) && resource_cache_acl (request)) request->reply = resource_cache (request);

  else if ((url == resource_user9edit_url ()) && bootstrap_browser_request_security_okay (request) && resource_user9edit_acl (request)) request->reply = resource_user9edit (request);

  else if ((url == resource_user1edit_url ()) && bootstrap_browser_request_security_okay (request) && resource_user1edit_acl (request)) request->reply = resource_user1edit (request);

  else if ((url == resource_user9view_url ()) && bootstrap_browser_request_security_okay (request) && resource_user9view_acl (request)) request->reply = resource_user9view (request);
  
  else if ((url == resource_user1view_url ()) && bootstrap_browser_request_security_okay (request) && resource_user1view_acl (request)) request->reply = resource_user1view (request);
  
  // Changes menu.
  else if ((url == journal_index_url ()) && bootstrap_browser_request_security_okay (request) && journal_index_acl (request)) request->reply = journal_index (request);
  
  else if ((url == changes_changes_url ()) && bootstrap_browser_request_security_okay (request) && changes_changes_acl (request)) request->reply = changes_changes (request);
  
  else if ((url == changes_change_url ()) && bootstrap_browser_request_security_okay (request) && changes_change_acl (request)) request->reply = changes_change (request);
  
  else if ((url == changes_manage_url ()) && bootstrap_browser_request_security_okay (request) && changes_manage_acl (request)) request->reply = changes_manage (request);

  // Planning menu.
  else if ((url == sprint_index_url ()) && bootstrap_browser_request_security_okay (request) && sprint_index_acl (request)) request->reply = sprint_index (request);
  
  // Tools menu.
  else if ((url == sendreceive_index_url ()) && bootstrap_browser_request_security_okay (request) && sendreceive_index_acl (request)) request->reply = sendreceive_index (request);
  
  else if ((url == manage_exports_url ()) && bootstrap_browser_request_security_okay (request) && manage_exports_acl (request)) request->reply = manage_exports (request);
  
  else if ((url == manage_hyphenation_url ()) && bootstrap_browser_request_security_okay (request) && manage_hyphenation_acl (request)) request->reply = manage_hyphenation (request);
  
  else if ((url == xrefs_index_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_index_acl (request)) request->reply = xrefs_index (request);
  
  else if ((url == debug_index_url ()) && bootstrap_browser_request_security_okay (request) && debug_index_acl (request)) request->reply = debug_index (request);
  
  // Settings menu.
  else if ((url == personalize_index_url ()) && bootstrap_browser_request_security_okay (request) && personalize_index_acl (request)) request->reply = personalize_index (request);
  
  else if ((url == manage_users_url ()) && bootstrap_browser_request_security_okay (request) && manage_users_acl (request)) request->reply = manage_users (request);
  
  else if ((url == manage_index_url ()) && bootstrap_browser_request_security_okay (request) && manage_index_acl (request)) request->reply = manage_index (request);
  
  else if ((url == administration_language_url ()) && bootstrap_browser_request_security_okay (request) && administration_language_acl (request)) request->reply = administration_language (request);
  
  else if ((url == administration_timezone_url ()) && bootstrap_browser_request_security_okay (request) && administration_timezone_acl (request)) request->reply = administration_timezone (request);
  
  else if ((url == email_index_url ()) && bootstrap_browser_request_security_okay (request) && email_index_acl (request)) request->reply = email_index (request);
  
  else if ((url == styles_indext_url ()) && bootstrap_browser_request_security_okay (request) && styles_indext_acl (request)) request->reply = styles_indext (request);
  
  else if ((url == styles_indexm_url ()) && bootstrap_browser_request_security_okay (request) && styles_indexm_acl (request)) request->reply = styles_indexm (request);
  
  else if ((url == styles_sheetm_url ()) && bootstrap_browser_request_security_okay (request) && styles_sheetm_acl (request)) request->reply = styles_sheetm (request);
  
  else if ((url == styles_view_url ()) && bootstrap_browser_request_security_okay (request) && styles_view_acl (request)) request->reply = styles_view (request);
  
  else if ((url == versification_index_url ()) && bootstrap_browser_request_security_okay (request) && versification_index_acl (request)) request->reply = versification_index (request);
  
  else if ((url == versification_system_url ()) && bootstrap_browser_request_security_okay (request) && versification_system_acl (request)) request->reply = versification_system (request);
  
  else if ((url == collaboration_index_url ()) && bootstrap_browser_request_security_okay (request) && collaboration_index_acl (request)) request->reply = collaboration_index (request);
  
  else if ((url == client_index_url ()) && bootstrap_browser_request_security_okay (request) && client_index_acl (request)) request->reply = client_index (request);
  
  else if ((url == mapping_index_url ()) && bootstrap_browser_request_security_okay (request) && mapping_index_acl (request)) request->reply = mapping_index (request);
  
  else if ((url == mapping_map_url ()) && bootstrap_browser_request_security_okay (request) && mapping_map_acl (request)) request->reply = mapping_map (request);
  
  else if ((url == paratext_index_url ()) && bootstrap_browser_request_security_okay (request) && paratext_index_acl (request)) request->reply = paratext_index (request);
  
  else if ((url == basic_index_url ()) && bootstrap_browser_request_security_okay (request) && basic_index_acl (request)) request->reply = basic_index (request);
  
  // Help menu.
  else if ((help_index_url (url)) && bootstrap_browser_request_security_okay (request) && help_index_acl (request, url)) request->reply = help_index (request, url);

  // User menu.  
  else if ((url == user_notifications_url ()) && bootstrap_browser_request_security_okay (request) && user_notifications_acl (request)) request->reply = user_notifications (request);
  
  else if ((url == user_account_url ()) && bootstrap_browser_request_security_okay (request) && user_account_acl (request)) request->reply = user_account (request);

  // Public feedback menu.
  else if ((url == public_index_url ()) && bootstrap_browser_request_security_okay (request) && public_index_acl (request)) request->reply = public_index (request);
  
  else if ((url == public_login_url ()) && bootstrap_browser_request_security_okay (request) && public_login_acl (request)) request->reply = public_login (request);
  
  else if ((url == public_chapter_url ()) && bootstrap_browser_request_security_okay (request) && public_chapter_acl (request)) request->reply = public_chapter (request);
  
  else if ((url == public_notes_url ()) && bootstrap_browser_request_security_okay (request) && public_notes_acl (request)) request->reply = public_notes (request);
  
  else if ((url == public_new_url ()) && bootstrap_browser_request_security_okay (request) && public_new_acl (request)) request->reply = public_new (request);
  
  else if ((url == public_create_url ()) && bootstrap_browser_request_security_okay (request) && public_create_acl (request)) request->reply = public_create (request);
  
  else if ((url == public_note_url ()) && bootstrap_browser_request_security_okay (request) && public_note_acl (request)) request->reply = public_note (request);
  
  else if ((url == public_comment_url ()) && bootstrap_browser_request_security_okay (request) && public_comment_acl (request)) request->reply = public_comment (request);
  
  // Pages not in any menu.
  else if ((url == jobs_index_url ()) && bootstrap_browser_request_security_okay (request) && jobs_index_acl (request)) request->reply = jobs_index (request);
  
  else if ((url == search_all_url ()) && bootstrap_browser_request_security_okay (request) && search_all_acl (request)) request->reply = search_all (request);
  
  else if ((url == search_replace_url ()) && bootstrap_browser_request_security_okay (request) && search_replace_acl (request)) request->reply = search_replace (request);
  
  else if ((url == search_search2_url ()) && bootstrap_browser_request_security_okay (request) && search_search2_acl (request)) request->reply = search_search2 (request);
  
  else if ((url == search_replace2_url ()) && bootstrap_browser_request_security_okay (request) && search_replace2_acl (request)) request->reply = search_replace2 (request);
  
  else if ((url == search_similar_url ()) && bootstrap_browser_request_security_okay (request) && search_similar_acl (request)) request->reply = search_similar (request);
  
  else if ((url == search_strongs_url ()) && bootstrap_browser_request_security_okay (request) && search_strongs_acl (request)) request->reply = search_strongs (request);
  
  else if ((url == search_strong_url ()) && bootstrap_browser_request_security_okay (request) && search_strong_acl (request)) request->reply = search_strong (request);
  
  else if ((url == search_originals_url ()) && bootstrap_browser_request_security_okay (request) && search_originals_acl (request)) request->reply = search_originals (request);
  
  else if ((url == workbench_settings_url ()) && bootstrap_browser_request_security_okay (request) && workbench_settings_acl (request)) request->reply = workbench_settings (request);
  
  else if ((url == collaboration_flash_url ()) && bootstrap_browser_request_security_okay (request) && collaboration_flash_acl (request)) request->reply = collaboration_flash (request);
  
  else if ((url == collaboration_open_url ()) && bootstrap_browser_request_security_okay (request) && collaboration_open_acl (request)) request->reply = collaboration_open (request);
  
  else if ((url == collaboration_password_url ()) && bootstrap_browser_request_security_okay (request) && collaboration_password_acl (request)) request->reply = collaboration_password (request);
  
  else if ((url == collaboration_secure_url ()) && bootstrap_browser_request_security_okay (request) && collaboration_secure_acl (request)) request->reply = collaboration_secure (request);
  
  else if ((url == collaboration_direction_url ()) && bootstrap_browser_request_security_okay (request) && collaboration_direction_acl (request)) request->reply = collaboration_direction (request);
  
  else if ((url == checks_settingspatterns_url ()) && bootstrap_browser_request_security_okay (request) && checks_settingspatterns_acl (request)) request->reply = checks_settingspatterns (request);
  
  else if ((url == checks_settingssentences_url ()) && bootstrap_browser_request_security_okay (request) && checks_settingssentences_acl (request)) request->reply = checks_settingssentences (request);
  
  else if ((url == checks_suppress_url ()) && bootstrap_browser_request_security_okay (request) && checks_suppress_acl (request)) request->reply = checks_suppress (request);
  
  else if ((url == xrefs_help_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_help_acl (request)) request->reply = xrefs_help (request);
  
  else if ((url == xrefs_extract_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_extract_acl (request)) request->reply = xrefs_extract (request);
  
  else if ((url == xrefs_interpret_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_interpret_acl (request)) request->reply = xrefs_interpret (request);
  
  else if ((url == xrefs_translate_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_translate_acl (request)) request->reply = xrefs_translate (request);
  
  else if ((url == xrefs_clear_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_clear_acl (request)) request->reply = xrefs_clear (request);
  
  else if ((url == xrefs_insert_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_insert_acl (request)) request->reply = xrefs_insert (request);
  
  else if ((url == webbible_search_url ()) && bootstrap_browser_request_security_okay (request) && webbible_search_acl (request)) request->reply = webbible_search (request);
  
  else if ((url == manage_write_url ()) && bootstrap_browser_request_security_okay (request) && manage_write_acl (request)) request->reply = manage_write (request);
  
  else if ((url == manage_privileges_url ()) && bootstrap_browser_request_security_okay (request) && manage_privileges_acl (request)) request->reply = manage_privileges (request);
  
  else if ((url == resource_image_url ()) && bootstrap_browser_request_security_okay (request) && resource_image_acl (request)) request->reply = resource_image (request);
  
  else if ((url == resource_img_url ()) && bootstrap_browser_request_security_okay (request) && resource_img_acl (request)) request->reply = resource_img (request);
  
  else if ((url == editor_select_url ()) && bootstrap_browser_request_security_okay (request) && editor_select_acl (request)) request->reply = editor_select (request);
  
  // Downloads
  else if ((url == index_listing_url (url)) && bootstrap_browser_request_security_okay (request) && index_listing_acl (request, url)) request->reply = index_listing (request, url);
  
  // Client calls.
  else if (url == sync_setup_url ()) request->reply = sync_setup (request);
  else if (url == sync_settings_url ()) request->reply = sync_settings (request);
  else if (url == sync_bibles_url ()) request->reply = sync_bibles (request);
  else if (url == sync_notes_url ()) request->reply = sync_notes (request);
  else if (url == sync_usfmresources_url ()) request->reply = sync_usfmresources (request);
  else if (url == sync_changes_url ()) request->reply = sync_changes (request);
  else if (url == sync_files_url ()) request->reply = sync_files (request);
  else if (url == sync_resources_url ()) request->reply = sync_resources (request);
  else if (url == sync_mail_url ()) request->reply = sync_mail (request);
  
  // AJAX calls.
  else if ((url == navigation_update_url ()) && bootstrap_browser_request_security_okay (request) && navigation_update_acl (request)) request->reply = navigation_update (request);
  
  else if ((url == navigation_poll_url ()) && bootstrap_browser_request_security_okay (request) && navigation_poll_acl (request)) request->reply = navigation_poll (request);
  
  else if ((url == editverse_load_url ()) && bootstrap_browser_request_security_okay (request) && editverse_load_acl (request)) request->reply = editverse_load (request);
  
  else if ((url == editverse_save_url ()) && bootstrap_browser_request_security_okay (request) && editverse_save_acl (request)) request->reply = editverse_save (request);
  
  else if ((url == editone_load_url ()) && bootstrap_browser_request_security_okay (request) && editone_load_acl (request)) request->reply = editone_load (request);
  
  else if ((url == editone_save_url ()) && bootstrap_browser_request_security_okay (request) && editone_save_acl (request)) request->reply = editone_save (request);
  
  else if ((url == editone_verse_url ()) && bootstrap_browser_request_security_okay (request) && editone_verse_acl (request)) request->reply = editone_verse (request);
  
  else if ((url == editusfm_focus_url ()) && bootstrap_browser_request_security_okay (request) && editusfm_focus_acl (request)) request->reply = editusfm_focus (request);
  
  else if ((url == editusfm_load_url ()) && bootstrap_browser_request_security_okay (request) && editusfm_load_acl (request)) request->reply = editusfm_load (request);
  
  else if ((url == editusfm_offset_url ()) && bootstrap_browser_request_security_okay (request) && editusfm_offset_acl (request)) request->reply = editusfm_offset (request);
  
  else if ((url == editusfm_save_url ()) && bootstrap_browser_request_security_okay (request) && editusfm_save_acl (request)) request->reply = editusfm_save (request);
  
  else if ((url == edit_edit_url ()) && bootstrap_browser_request_security_okay (request) && edit_edit_acl (request)) request->reply = edit_edit (request);
  
  else if ((url == edit_focus_url ()) && bootstrap_browser_request_security_okay (request) && edit_focus_acl (request)) request->reply = edit_focus (request);
  
  else if ((url == edit_id_url ()) && bootstrap_browser_request_security_okay (request) && edit_id_acl (request)) request->reply = edit_id (request);
  
  else if ((url == edit_load_url ()) && bootstrap_browser_request_security_okay (request) && edit_load_acl (request)) request->reply = edit_load (request);
  
  else if ((url == edit_offset_url ()) && bootstrap_browser_request_security_okay (request) && edit_offset_acl (request)) request->reply = edit_offset (request);
  
  else if ((url == edit_save_url ()) && bootstrap_browser_request_security_okay (request) && edit_save_acl (request)) request->reply = edit_save (request);
  
  else if ((url == edit_styles_url ()) && bootstrap_browser_request_security_okay (request) && edit_styles_acl (request)) request->reply = edit_styles (request);
  
  else if ((url == search_getids_url ()) && bootstrap_browser_request_security_okay (request) && search_getids_acl (request)) request->reply = search_getids (request);
  
  else if ((url == search_replacepre_url ()) && bootstrap_browser_request_security_okay (request) && search_replacepre_acl (request)) request->reply = search_replacepre (request);
  
  else if ((url == search_replacego_url ()) && bootstrap_browser_request_security_okay (request) && search_replacego_acl (request)) request->reply = search_replacego (request);
  
  else if ((url == search_replacepre2_url ()) && bootstrap_browser_request_security_okay (request) && search_replacepre2_acl (request)) request->reply = search_replacepre2 (request);
  
  else if ((url == search_getids2_url ()) && bootstrap_browser_request_security_okay (request) && search_getids2_acl (request)) request->reply = search_getids2 (request);
  
  else if ((url == search_replacego2_url ()) && bootstrap_browser_request_security_okay (request) && search_replacego2_acl (request)) request->reply = search_replacego2 (request);
  
  else if ((url == resource_get_url ()) && bootstrap_browser_request_security_okay (request) && resource_get_acl (request)) request->reply = resource_get (request);
  
  else if ((url == notes_poll_url ()) && bootstrap_browser_request_security_okay (request) && notes_poll_acl (request)) request->reply = notes_poll (request);
  
  else if ((url == notes_notes_url ()) && bootstrap_browser_request_security_okay (request) && notes_notes_acl (request)) request->reply = notes_notes (request);
  
  else if ((url == notes_click_url ()) && bootstrap_browser_request_security_okay (request) && notes_click_acl (request)) request->reply = notes_click (request);
  
  else if ((url == consistency_poll_url ()) && bootstrap_browser_request_security_okay (request) && consistency_poll_acl (request)) request->reply = consistency_poll (request);
  
  else if ((url == consistency_input_url ()) && bootstrap_browser_request_security_okay (request) && consistency_input_acl (request)) request->reply = consistency_input (request);
  
  else if ((url == xrefs_source_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_source_acl (request)) request->reply = xrefs_source (request);
  
  else if ((url == xrefs_target_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_target_acl (request)) request->reply = xrefs_target (request);
  
  else if ((url == xrefs_move_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_move_acl (request)) request->reply = xrefs_move (request);
  
  else if ((url == xrefs_next_url ()) && bootstrap_browser_request_security_okay (request) && xrefs_next_acl (request)) request->reply = xrefs_next (request);
  
  else if ((url == lexicon_definition_url ()) && bootstrap_browser_request_security_okay (request) && lexicon_definition_acl (request)) request->reply = lexicon_definition (request);

  // Internal settings calls.
  else if ((url == administration_timeoffset_url ()) && administration_timeoffset_acl (request)) request->reply = administration_timeoffset (request);

  // Forward the browser to the default home page.
  else {
    redirect_browser (request, index_index_url ());
  }
}
