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


#include <rss/logic.h>
#include <filter/url.h>
#include <database/config/general.h>
#include <pugixml/pugixml.hpp>
#include <locale/translate.h>


using namespace pugi;


string rss_logic_xml_path ()
{
  return filter_url_create_root_path ("rss", "feed.xml");
}


void rss_logic_update_xml (vector <string> titles, vector <string> descriptions)
{
  string path = rss_logic_xml_path ();
  int size = Database_Config_General::getMaxRssFeedItems ();
  if (size == 0) {
    if (file_or_dir_exists (path)) filter_url_unlink (path);
    return;
  }
  bool document_updated = false;
  xml_document document;
  document.load_file (path.c_str());
  xml_node rss_node = document.first_child ();
  if (strcmp (rss_node.name (), "rss") != 0) {
    rss_node = document.append_child ("rss");
    rss_node.append_attribute ("version") = "2.0";
    xml_node channel = rss_node.append_child ("channel");
    xml_node node = channel.append_child ("title");
    node.text () = "Bibledit";
    node = channel.append_child ("link");
    node.text () = Database_Config_General::getSiteURL().c_str();
    node = channel.append_child ("description");
    node.text () = translate ("Recent changes in the Bible text").c_str ();
    document_updated = true;
  }
  xml_node channel = rss_node.child ("channel");
  for (size_t i = 0; i < titles.size(); i++) {
    xml_node item = channel.append_child ("item");
    item.append_child ("title").text () = titles [i].c_str();
    item.append_child ("link");
    item.append_child ("description").text () = descriptions [i].c_str();
    document_updated = true;
  }
  int count = distance (channel.children ().begin (), channel.children ().end ());
  count -= 3;
  count -= size;
  while (count > 0) {
    xml_node node = channel.child ("item");
    channel.remove_child (node);
    document_updated = true;
    count--;
  }
  if (document_updated) {
    xml_node decl = document.prepend_child (node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";
    stringstream output;
    document.print (output, "", format_raw);
    filter_url_file_put_contents (path, output.str ());
  }
}
