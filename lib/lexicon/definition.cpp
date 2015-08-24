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


#include <lexicon/definition.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <lexicon/logic.h>


string lexicon_definition_url ()
{
  return "lexicon/definition";
}


bool lexicon_definition_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string lexicon_definition (void * webserver_request)
{
  // Retrieve the id: It may contain a Strong's number or a lemma.
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string id = request->query["id"];
  
  vector <string> renderings;
  
  if (!id.empty ()) {
    
    string letter = id.substr (0, 1);
    if (letter == HEBREW_ETCBE4_PREFIX) {
      
      // ETCBC4 database.
      renderings.push_back (lexicon_logic_render_etcb4_morphology (id));
      
    } else {

      string morphology = lexicon_logic_convert_item_to_morphology (id);
      if (!morphology.empty ()) {
        renderings.push_back (morphology);
      }
      
      // Whatever is the identifier, convert it to one or more Strong's numbers.
      vector <string> strongs = lexicon_logic_convert_item_to_strong (id);
      
      // Render Strong's definitions.
      for (auto& strong : strongs) {
        string rendering = lexicon_logic_render_definition (strong);
        if (!rendering.empty ()) renderings.push_back (rendering);
      }

    }
    
  }
  
  
  return filter_string_implode (renderings, "\n");
}
