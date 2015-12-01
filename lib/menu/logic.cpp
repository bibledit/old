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


#include <menu/logic.h>
#include <menu/index.h>
#include <administration/language.h>
#include <administration/timezone.h>
#include <bible/manage.h>
#include <changes/changes.h>
#include <changes/manage.h>
#include <checks/index.h>
#include <checks/settings.h>
#include <client/index.h>
#include <client/logic.h>
#include <collaboration/index.h>
#include <config/globals.h>
#include <config/logic.h>
#include <consistency/index.h>
#include <database/config/general.h>
#include <debug/index.h>
#include <edit/index.h>
#include <editone/index.h>
#include <editusfm/index.h>
#include <editverse/index.h>
#include <email/index.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <index/index.h>
#include <index/listing.h>
#include <journal/index.h>
#include <locale/translate.h>
#include <manage/exports.h>
#include <manage/hyphenation.h>
#include <manage/index.h>
#include <manage/users.h>
#include <mapping/index.h>
#include <notes/index.h>
#include <notes/select.h>
#include <paratext/index.h>
#include <personalize/index.h>
#include <resource/admin.h>
#include <resource/images.h>
#include <resource/index.h>
#include <resource/manage.h>
#include <resource/print.h>
#include <resource/sword.h>
#include <search/index.h>
#include <search/replace.h>
#include <search/search2.h>
#include <search/replace2.h>
#include <search/all.h>
#include <search/similar.h>
#include <search/strongs.h>
#include <search/strong.h>
#include <search/originals.h>
#include <sendreceive/index.h>
#include <session/login.h>
#include <session/logout.h>
#include <sprint/index.h>
#include <styles/indexm.h>
#include <styles/indext.h>
#include <user/account.h>
#include <user/notifications.h>
#include <versification/index.h>
#include <workbench/index.h>
#include <workbench/logic.h>
#include <workbench/logic.h>
#include <workbench/logic.h>
#include <workbench/organize.h>
#include <workbench/organize.h>
#include <xrefs/index.h>
#include <public/index.h>
#include <public/logic.h>
#include <filter/url.h>


string menu_logic_href (string href)
{
  href = filter_string_str_replace ("?", "__q__", href);
  href = filter_string_str_replace ("&", "__a__", href);
  href = filter_string_str_replace ("=", "__i__", href);
  return href;
}


string menu_logic_click (string item)
{
  item = filter_string_str_replace ("__q__", "?", item);
  item = filter_string_str_replace ("__a__", "&", item);
  item = filter_string_str_replace ("__i__", "=", item);
  Database_Config_General::setLastMenuClick (item);
  return item;
}


string menu_logic_create_item (string href, string text, bool history)
{
  string item;
  item.append ("<span class=\"nowrap\">");
  item.append ("<a href=\"/");
  if (history) {
    item.append (menu_index_url ());
  } else {
    item.append (index_index_url ());
  }
  item.append ("?item=");
  item.append (menu_logic_href (href) + "\">" + text + "</a>");
  item.append ("</span>");
  return item;
}


string menu_logic_translate_menu ()
{
  return "translate";
}


string menu_logic_search_menu ()
{
  return "search";
}


string menu_logic_tools_menu ()
{
  return "tools";
}


string menu_logic_settings_menu ()
{
  return "settings";
}


// Returns the html for the main menu categories.
string menu_logic_main_categories (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  vector <string> html;

  if (workbench_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (workbench_index_url (), translate ("Desktop"), true));
  }

  if (!menu_logic_translate_category (webserver_request).empty ()) {
    html.push_back (menu_logic_create_item (menu_logic_translate_menu (), menu_logic_translate_text (), false));
  }
  
  if (!menu_logic_search_category (webserver_request).empty ()) {
    html.push_back (menu_logic_create_item (menu_logic_search_menu (), menu_logic_search_text (), false));
  }

  if (!menu_logic_tools_category (webserver_request).empty ()) {
    html.push_back (menu_logic_create_item (menu_logic_tools_menu (), menu_logic_tools_text (), false));
  }

  if (!menu_logic_settings_category (webserver_request).empty ()) {
    html.push_back (menu_logic_create_item (menu_logic_settings_menu (), menu_logic_settings_text (), false));
  }
  
  if (!menu_logic_help_category (webserver_request).empty ()) {
    html.push_back (menu_logic_create_item ("help/index", menu_logic_help_text (), true));
  }

  // When a user is not logged in, or a guest,
  // put the public feedback into the main menu, rather than in a sub menu.
  if (!config_logic_client_prepared ()) {
    if (menu_logic_public_or_guest (webserver_request)) {
      if (!public_logic_bibles (webserver_request).empty ()) {
        html.push_back (menu_logic_create_item (public_index_url (), menu_logic_public_feedback_text (), true));
      }
    }
  }

  // When a user is logged in, and is a guest, put the Logout into the main menu, rather than in a sub menu.
  if (request->session_logic ()->loggedIn ()) {
    if (request->session_logic ()->currentLevel () == Filter_Roles::guest ()) {
      if (session_logout_acl (webserver_request)) {
        html.push_back (menu_logic_create_item (session_logout_url (), menu_logic_logout_text (), true));
      }
    }
  }

  
  // When not logged in, display Login menu item.
  if (request->session_logic ()->currentUser ().empty ()) {
    html.push_back (menu_logic_create_item (session_login_url (), translate ("Login"), true));
  }

  return filter_string_implode (html, "\n");
}


/*
 This generates a start menu.
 
 It goes through all possible menu entries.
 It reads the access levels of those entries.
 It takes the menu entries the currently logged-in user has access to.
 
 It originally self-organized the entries such that the ones used clicked often came earlier in the menu.
 But menu entries moving around creates confusion.
 Therefore it was removed again.
*/


string menu_logic_desktop_category (void * webserver_request)
{
  vector <string> html;

  // Add the available configured desktops to the menu.
  // The user's role should be sufficiently high.
  if (workbench_organize_acl (webserver_request)) {
    vector <string> workbenches = workbench_get_names (webserver_request);
    for (size_t i = 0; i < workbenches.size(); i++) {
      string item = menu_logic_create_item (workbench_index_url () + "?bench=" + convert_to_string (i), workbenches[i], true);
      html.push_back (item);
    }
  }

  return filter_string_implode (html, "\n");
}


string menu_logic_translate_category (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  vector <string> html;
  
  if (edit_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (edit_index_url (), translate ("Visual chapter editor"), true));
  }
  
  if (editone_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (editone_index_url (), translate ("Visual verse editor"), true));
  }

  if (editusfm_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (editusfm_index_url (), translate ("USFM chapter editor"), true));
  }
    
  if (editverse_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (editverse_index_url (), translate ("USFM verse editor"), true));
  }
  
  if (notes_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (notes_index_url (), menu_logic_consultation_notes_text (), true));
  }

  if (resource_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (resource_index_url (), translate ("View resources"), true));
  }

  if (changes_changes_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (changes_changes_url (), translate ("Change notifications"), true));
  }

  if (index_listing_acl (webserver_request, "exports")) {
    html.push_back (menu_logic_create_item (index_listing_url ("exports"), translate ("Exports"), true));
  }
  
  // When a user is logged in, but not a guest,
  // put the public feedback into this sub menu, rather than in the main menu.
  if (!config_logic_client_prepared ()) {
    if (!request->session_logic ()->currentUser ().empty ()) {
      if (!menu_logic_public_or_guest (webserver_request)) {
        if (!public_logic_bibles (webserver_request).empty ()) {
          html.push_back (menu_logic_create_item (public_index_url (), menu_logic_public_feedback_text (), true));
        }
      }
    }
  }
  
  if (!html.empty ()) {
    html.insert (html.begin (), menu_logic_translate_text () + ": ");
  }

  return filter_string_implode (html, "\n");
}


string menu_logic_search_category (void * webserver_request)
{
  vector <string> html;

  if (search_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_index_url (), translate ("Search"), true));
  }
  
  if (search_replace_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_replace_url (), translate ("Replace"), true));
  }
  
  if (search_search2_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_search2_url (), translate ("Advanced search"), true));
  }
  
  if (search_replace2_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_replace2_url (), translate ("Advanced replace"), true));
  }
  
  if (search_all_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_all_url (), translate ("Search all Bibles and notes"), true));
  }

  if (search_similar_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_similar_url (), translate ("Search Bible for similar verses"), true));
  }

  if (search_strongs_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_strongs_url (), translate ("Search Bible for similar Strong's numbers"), true));
  }

  if (search_strong_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_strong_url (), translate ("Search Bible for Strong's number"), true));
  }

  if (search_originals_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (search_originals_url (), translate ("Search Bible for similar Hebrew or Greek words"), true));
  }
  
  if (!html.empty ()) {
    html.insert (html.begin (), menu_logic_search_text () + ": ");
  }
  
  return filter_string_implode (html, "\n");
}


string menu_logic_tools_category (void * webserver_request)
{
  vector <string> html;

  if (checks_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (checks_index_url (), translate ("Mechanical checks"), true));
  }

  if (consistency_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (consistency_index_url (), translate ("Consistency"), true));
  }

  if (!config_logic_client_prepared ()) {
    if (resource_print_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (resource_print_url (), translate ("Print resources"), true));
    }
  }

  // Downloading revisions only on server, not on client.
  if (!config_logic_client_prepared ()) {
    if (index_listing_acl (webserver_request, "revisions")) {
      html.push_back (menu_logic_create_item (index_listing_url ("revisions"), translate ("Download changes"), true));
    }
  }

  if (!config_logic_client_prepared ()) {
    if (sprint_index_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (sprint_index_url (), translate ("Planning"), true));
    }
  }

  if (sendreceive_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (sendreceive_index_url (), translate ("Send / receive"), true));
  }

  if (manage_hyphenation_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (manage_hyphenation_url (), translate ("Hyphenation"), true));
  }

  if (xrefs_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (xrefs_index_url (), translate ("Cross references"), true));
  }
  
  if (debug_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (debug_index_url (), translate ("Debug"), true));
  }

  if (manage_exports_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (manage_exports_url (), translate ("Export Bibles"), true));
  }

  if (journal_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (journal_index_url (), translate ("Journal"), true));
  }
  
  if (!html.empty ()) {
    html.insert (html.begin (), menu_logic_tools_text () + ": ");
  }
  
  return filter_string_implode (html, "\n");
}


string menu_logic_settings_category (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  bool client = client_logic_client_enabled ();
  bool demo = config_logic_demo_enabled ();

  vector <string> html;

  if (bible_manage_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (bible_manage_url (), menu_logic_bible_manage_text (), true));
  }
  
  if (workbench_organize_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (workbench_organize_url (), menu_logic_workbench_organize_text (), true));
  }

  if (checks_settings_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (checks_settings_url (), menu_logic_checks_settings_text (), true));
  }

  if (!config_logic_client_prepared ()) {
    if (resource_manage_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (resource_manage_url (), translate ("USFM resources"), true));
    }
  }

  if (!config_logic_client_prepared ()) {
    if (resource_admin_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (resource_admin_url (), menu_logic_resource_admin_text (), true));
    }
  }
  
  if (!config_logic_client_prepared ()) {
    if (resource_images_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (resource_images_url (), menu_logic_resource_images_text (), true));
    }
  }
  
  if (!config_logic_client_prepared ()) {
    if (resource_sword_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (resource_sword_url (), translate ("SWORD resources"), true));
    }
  }

  // Managing change notifications only on server, not on client.
  if (!config_logic_client_prepared ()) {
    if (changes_manage_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (changes_manage_url (), translate ("Change notifications"), true));
    }
  }
  
  if (personalize_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (personalize_index_url (), translate ("Personalize"), true));
  }

  if (!config_logic_client_prepared ()) {
    if (manage_users_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (manage_users_url (), menu_logic_manage_users_text (), true));
    }
  }

  if (manage_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (manage_index_url (), translate ("Indexes and Fonts"), true));
  }

  if (administration_language_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (administration_language_url (), translate ("Language"), true));
  }

  if (administration_timezone_acl (webserver_request)) {
    // Display menu to set the site's timezone only in case the calling program has not yet set this zone in the library.
    // So for example the app for iOS can set the timezone from the device, and in case this has been done,
    // then the user no longer can set it through Bibledit.
    if ((config_globals_timezone_offset_utc < MINIMUM_TIMEZONE)
        || (config_globals_timezone_offset_utc > MAXIMUM_TIMEZONE)) {
      html.push_back (menu_logic_create_item (administration_timezone_url (), translate ("Timezone"), true));
    }
  }

  if (!config_logic_client_prepared ()) {
    if (email_index_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (email_index_url (), translate ("Mail"), true));
    }
  }
  
  if (styles_indext_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (styles_indext_url (), translate ("Select stylesheet"), true));
  }
  
  if (versification_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (versification_index_url (), translate ("Versifications"), true));
  }
  
  if (mapping_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (mapping_index_url (), translate ("Verse mappings"), true));
  }
  
  if (collaboration_index_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (collaboration_index_url (), translate ("Collaboration"), true));
  }
  
  // If the installation is not prepared for Client mode, disable the client menu.
  // But keep the menu item in an open installation.
  bool client_menu = client_index_acl (webserver_request);
  if (!config_logic_client_prepared ()) client_menu = false;
  if (config_logic_demo_enabled ()) client_menu = true;
  if (client_menu) {
    if (client_index_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (client_index_url (), translate ("Cloud"), true));
    }
  }
  
  // Paratext can be enabled through ./configure --enable-paratext.
  if (config_logic_paratext_enabled ()) {
    if (paratext_index_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (paratext_index_url (), translate ("Paratext"), true));
    }
  }
  
  if (styles_indexm_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (styles_indexm_url (), translate ("Edit stylesheet"), true));
  }

  if (!(client || demo)) {
    // If logged in, but not as guest, put the Logout menu here.
    if (request->session_logic ()->loggedIn ()) {
      if (request->session_logic ()->currentLevel () != Filter_Roles::guest ()) {
        if (session_logout_acl (webserver_request)) {
          html.push_back (menu_logic_create_item (session_logout_url (), menu_logic_logout_text (), true));
        }
      }
    }
  }

  if (user_notifications_acl (webserver_request)) {
    html.push_back (menu_logic_create_item (user_notifications_url (), translate ("Notifications"), true));
  }

  if (!(client || demo)) {
    if (user_account_acl (webserver_request)) {
      html.push_back (menu_logic_create_item (user_account_url (), translate ("Account"), true));
    }
  }

  if (!html.empty ()) {
    html.insert (html.begin (), menu_logic_settings_text () + ": ");
  }
  
  return filter_string_implode (html, "\n");
}


string menu_logic_help_category (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  vector <string> html;

  if (!request->session_logic ()->currentUser ().empty ()) {
    html.push_back (menu_logic_create_item ("help/index", translate ("Help and About"), true));
  }

  if (!html.empty ()) {
    html.insert (html.begin (), menu_logic_help_text () + ": ");
  }
  
  return filter_string_implode (html, "\n");
}


// Returns true in case the user is a public user, that is, not logged-in,
// or when the user has the role of Guest.
bool menu_logic_public_or_guest (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  if (request->session_logic ()->currentUser ().empty ()) return true;
  if (request->session_logic ()->currentLevel () == Filter_Roles::guest ()) return true;
  return false;
}


// Returns the text that belongs to a certain menu item.
string menu_logic_menu_text (string menu_item) // Todo
{
  if (menu_item == menu_logic_translate_menu ()) {
    return menu_logic_translate_text ();
  }
  if (menu_item == notes_index_url ()) {
    return menu_logic_consultation_notes_text ();
  }
  if (menu_item == menu_logic_search_menu ()) {
    return menu_logic_search_text ();
  }
  if (menu_item == menu_logic_tools_menu ()) {
    return menu_logic_tools_text ();
  }
  if (menu_item == menu_logic_settings_menu ()) {
    return menu_logic_settings_text ();
  }
  return translate ("Menu");
}


// Returns the URL that belongs to $menu_item.
string menu_logic_menu_url (string menu_item)
{
  if (
      (menu_item == menu_logic_translate_menu ())
      ||
      (menu_item == menu_logic_search_menu ())
      ||
      (menu_item == menu_logic_tools_menu ())
      ||
      (menu_item == menu_logic_settings_menu ())
    ) {
    return filter_url_build_http_query (index_index_url (), "item", menu_item);
  }

  return menu_item;
}


string menu_logic_breadcrumbs (void * webserver_request, vector <string> crumbs) // Todo out.
{
  string result;
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  if (request->database_config_user ()->getDisplayBreadcrumbs ()) {
    result.append ("<p>");
    result.append ("<a href=\"/");
    result.append (index_index_url ());
    result.append ("\">");
    result.append (menu_logic_menu_text (""));
    result.append ("</a>");
    for (auto crumb : crumbs) {
      result.append (" » ");
      result.append ("<a href=\"/");
      result.append (menu_logic_menu_url (crumb));
      result.append ("\">");
      result.append (menu_logic_menu_text (crumb));
      result.append ("</a>");
    }
    result.append ("</p>");
  }
  return result;
}


string menu_logic_translate_text ()
{
  return translate ("Translate");
}


string menu_logic_search_text ()
{
  return translate ("Search");
}


string menu_logic_tools_text ()
{
  return translate ("Tools");
}


string menu_logic_settings_text ()
{
  return translate ("Settings");
}


string menu_logic_help_text ()
{
  return translate ("Help");
}


string menu_logic_public_feedback_text ()
{
  return translate ("Public feedback");
}


string menu_logic_logout_text ()
{
  return translate ("Logout");
}


string menu_logic_consultation_notes_text ()
{
  return translate ("Consultation notes");
}


string menu_logic_bible_manage_text ()
{
  return translate ("Bibles");
}


string menu_logic_workbench_organize_text ()
{
  return translate ("Desktops");
}


string menu_logic_checks_settings_text ()
{
  return translate ("Checks");
}


string menu_logic_resource_admin_text ()
{
  return translate ("External resources");
}


string menu_logic_resource_images_text ()
{
  return translate ("Image resources");
}


string menu_logic_manage_users_text ()
{
  return translate ("Users");
}
