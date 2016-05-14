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


#include <sprint/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <filter/date.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <locale/logic.h>
#include <database/sprint.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <access/bible.h>
#include <dialog/list.h>
#include <sprint/burndown.h>
#include <menu/logic.h>


string sprint_index_url ()
{
  return "sprint/index";
}


bool sprint_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string sprint_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Sprint database_sprint;

  
  string page;
  Assets_Header header = Assets_Header (translate("Sprint"), request);
  header.addBreadCrumb (menu_logic_tools_menu (), menu_logic_tools_text ());
  page = header.run ();
  Assets_View view;
  
  
  if (request->query.count ("previoussprint")) {
    int month = request->database_config_user()->getSprintMonth ();
    int year = request->database_config_user()->getSprintYear ();
    filter_date_get_previous_month (month, year);
    request->database_config_user()->setSprintMonth (month);
    request->database_config_user()->setSprintYear (year);
  }
  
  
  if (request->query.count ("currentprint")) {
    request->database_config_user()->setSprintMonth (filter_date_numerical_month (filter_date_seconds_since_epoch ()));
    request->database_config_user()->setSprintYear (filter_date_numerical_year (filter_date_seconds_since_epoch ()));
  }
  
  
  if (request->query.count ("nextsprint")) {
    int month = request->database_config_user()->getSprintMonth ();
    int year = request->database_config_user()->getSprintYear ();
    filter_date_get_next_month (month, year);
    request->database_config_user()->setSprintMonth (month);
    request->database_config_user()->setSprintYear (year);
  }
  
  
  string bible = access_bible_clamp (webserver_request, request->database_config_user()->getBible ());
  int month = request->database_config_user()->getSprintMonth ();
  int year = request->database_config_user()->getSprintYear ();
  
  
  if (request->post.count ("add")) {
    string title = request->post ["add"];
    database_sprint.storeTask (bible, year, month, title);
    view.set_variable ("success", translate("New task added"));
    // Focus the entry for adding tasks only in case a new task was added.
    view.set_variable ("autofocus", "autofocus");
  }
  
  
  if (request->query.count ("mail")) {
    string mail = request->query ["mail"];
    sprint_burndown (bible, true);
    view.set_variable ("success", translate("The information was mailed to the subscribers"));
  }
  
  
  if (request->query.count ("bible")) {
    bible = request->query ["bible"];
    if (bible == "") {
      Dialog_List dialog_list = Dialog_List ("index", translate("Select which Bible to display the Sprint for"), "", "");
      vector <string> bibles = access_bible_bibles (request);
      for (auto & bible : bibles) {
        dialog_list.add_row (bible, "bible", bible);
      }
      page += dialog_list.run();
      return page;
    } else {
      request->database_config_user()->setBible (bible);
    }
  }
  
  
  bible = access_bible_clamp (webserver_request, request->database_config_user()->getBible ());
  
  
  int id = convert_to_int (request->query ["id"]);
  
  
  if (request->query.count ("remove")) {
    database_sprint.deleteTask (id);
    view.set_variable ("success", translate("The task was removed"));
  }
  
  
  if (request->query.count ("moveback")) {
    filter_date_get_previous_month (month, year);
    database_sprint.updateMonthYear (id, month, year);
    view.set_variable ("success", translate("The task was moved to the previous sprint"));
    request->database_config_user()->setSprintMonth (month);
    request->database_config_user()->setSprintYear (year);
  }
                        
                        
  if (request->query.count ("moveforward")) {
    filter_date_get_next_month (month, year);
    database_sprint.updateMonthYear (id, month, year);
    view.set_variable ("success", translate("The task was moved to the next sprint"));
    request->database_config_user()->setSprintMonth (month);
    request->database_config_user()->setSprintYear (year);
  }

  
  if (request->query.count ("complete")) {
    int complete = convert_to_int (request->query ["complete"]);
    database_sprint.updateComplete (id, complete);
  }
  
  
  if (request->post.count ("categories")) {
    string categories = request->post ["categories"];
    vector <string> categories2;
    categories = filter_string_trim (categories);
    vector <string> vcategories = filter_string_explode (categories, '\n');
    for (auto category : vcategories) {
      category = filter_string_trim (category);
      if (category != "") categories2.push_back (category);
    }
    categories = filter_string_implode (categories2, "\n");
    Database_Config_Bible::setSprintTaskCompletionCategories (bible, categories);
  }
  
  
  view.set_variable ("bible", bible);
  view.set_variable ("sprint", locale_logic_month (month) + " " + convert_to_string (year));

  
  string categorytext = Database_Config_Bible::getSprintTaskCompletionCategories (bible);
  view.set_variable ("categorytext", categorytext);
  vector <string> vcategories = filter_string_explode (categorytext, '\n');
  string categories;
  for (auto category : vcategories) {
    categories.append ("<td>" + category + "</td>\n");
  }
  view.set_variable ("categories", categories);
  
  
  string tasks;
  vector <int> vtasks = database_sprint.getTasks (bible, year, month);
  for (auto & id : vtasks) {
    string title = filter_string_sanitize_html (database_sprint.getTitle (id));
    int percentage = database_sprint.getComplete (id);
    tasks.append ("<tr id=\"a" + convert_to_string (id) + "\">\n");
    tasks.append ("<td><a href=\"?id=" + convert_to_string (id) + "&remove=\"> ✗ </a></td>\n");
    tasks.append ("<td></td>\n");
    tasks.append ("<td><a href=\"?id=" + convert_to_string (id) + "&moveback=\"> « </a></td>\n");
    tasks.append ("<td>" + title + "</td>\n");
    int category_count = vcategories.size();
    float category_percentage = 100 / category_count;
    for (unsigned int i2 = 0; i2 < vcategories.size (); i2++) {
      int low = round (i2 * category_percentage);
      int high = round ((i2 + 1) * category_percentage);
      string anchor;
      if (id > 3) {
        // When the monthly sprint has many items, and so the list will be long,
        // clicking an item down in the list scrolls the page back to the top.
        // And the clicked items scrolls out of view.
        // A solution is to navigate to an anchor after clicking an item.
        anchor = "#a" + convert_to_string (id - 3);
      }
      tasks.append ("<td>\n");
      if (percentage >= high) {
        tasks.append ("<a href=\"?id=" + convert_to_string (id) + "&complete=" + convert_to_string (low) + anchor + "\">" + get_tick_box (true) + "</a>\n");
      } else {
        tasks.append ("<a href=\"?id=" + convert_to_string (id) + "&complete=" + convert_to_string (high) + anchor + "\">" + get_tick_box (false) + "</a>\n");
      }
      tasks.append ("</td>\n");
    }
    
    tasks.append ("<td><a href=\"?id=" + convert_to_string (id) + "&moveforward=\"> » </a></td>\n");
    tasks.append ("</tr>\n");
  }
  view.set_variable ("tasks", tasks);

  
  view.set_variable ("chart", sprint_create_burndown_chart (bible, year, month));
                                                                                                                                                                                                                  

  page += view.render ("sprint", "index");
  page += Assets_Page::footer ();
  return page;
}
