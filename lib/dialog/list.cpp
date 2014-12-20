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


#include <dialog/list.h>
#include <assets/view.h>
#include <assets/page.h>
#include <locale/translate.h>


// Entry dialog constructor
// $url: The base URL of the page.
// $question: The question to be asked.
// $info_top : Information.
// $info_bottom: Information.
Dialog_List::Dialog_List (string url, string question, string info_top, string info_bottom) // Todo porting this one.
{
  Assets_View * view = new Assets_View ();
  base_url = url;
  view->set_variable ("base_url", base_url);
  view->set_variable ("question", question);
  if (info_top == "")
    info_top = gettext("Here are the various options:");
  view->set_variable ("info_top", info_top);
  if (info_bottom == "")
    info_bottom = gettext("Please pick one.");
  view->set_variable ("info_bottom", info_bottom);
  assets_view = view;
}


Dialog_List::~Dialog_List ()
{
  Assets_View * view = (Assets_View *) assets_view;
  delete view;
}


// $query   : A map with query variables, e.g. ("search" => "bibledit").
//            If any $query is passed, if Cancel is clicked in this dialog, it should go go back
//            to the original caller page  with the $query added. Same for Ok, it would post
//            the value entered, but also add the $query to the url.
void Dialog_List::set_query (map <string, string> query)
{
  if (query.empty ()) {}; // Temporal
  /* Todo
   $base_url = $_SERVER['PHP_SELF'];
   if (is_array ($query)) {
   $base_url .= "?" . http_build_query ($query);
   }
   */
}


void Dialog_List::add_row (string text, string query)
{
  if (!list_block.empty ()) list_block.append ("\n");
  if (!horizontal) list_block.append ("<li>");
  list_block.append ("<a href=\"" + base_url + query + "\">" + text + "</a>");
  if (!horizontal) list_block.append ("</li>");
}


string Dialog_List::run ()
{
  Assets_View * view = (Assets_View *) assets_view;
  view->set_variable ("list_block", list_block);
  string page = view->render ("dialog", "listhorizontal");
  page += Assets_Page::footer ();
  return page;
}
/*
 

 
 $this->view->view->horizontal = $horizontal;
 }
 
 
 public function add_row ($text_line, $get_parameter)
 {
 $this->text_lines[]     = $text_line;
 $this->get_parameters[] = $get_parameter;
 }
 
 
*/