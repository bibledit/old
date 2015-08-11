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


#include <checks/settings.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/check.h>
#include <database/config/bible.h>
#include <client/logic.h>
#include <demo/logic.h>
#include <sendreceive/logic.h>
#include <config/logic.h>
#include <access/bible.h>
#include <dialog/list.h>
#include <checks/logic.h>


string checks_settings_url ()
{
  return "checks/settings";
}


bool checks_settings_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string checks_settings (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  string page;
  page = Assets_Page::header (translate ("Manage Checks"), webserver_request, "");
  Assets_View view = Assets_View ();
  
  
  if (request->query.count ("bible")) {
    string bible = request->query["bible"];
    if (bible == "") {
      Dialog_List dialog_list = Dialog_List ("settings", translate("Select which Bible to manage"), "", "");
      vector <string> bibles = access_bible_bibles (webserver_request);
      for (auto bible : bibles) {
        dialog_list.add_row (bible, "bible", bible);
      }
      page += dialog_list.run();
      return page;
    } else {
      request->database_config_user()->setBible (bible);
    }
  }
  string bible = access_bible_clamp (webserver_request, request->database_config_user()->getBible ());

  
  if (request->query.count ("run")) {
    checks_logic_start (bible);
    view.set_variable ("success", translate("Will run the checks. See the Journal for progress."));
  }
  
                        
  if (request->query.count ("doublespacesusfm")) {
    Database_Config_Bible::setCheckDoubleSpacesUsfm (bible, !Database_Config_Bible::getCheckDoubleSpacesUsfm (bible));
  }
  view.set_variable ("double_spaces_usfm", get_tick_box (Database_Config_Bible::getCheckDoubleSpacesUsfm (bible)));
   
  
  if (request->query.count ("fullstopheadings")) {
    Database_Config_Bible::setCheckFullStopInHeadings (bible, !Database_Config_Bible::getCheckFullStopInHeadings (bible));
  }
  view.set_variable ("full_stop_headings", get_tick_box (Database_Config_Bible::getCheckFullStopInHeadings (bible)));

  
  if (request->query.count ("spacebeforepunctuation")) {
    Database_Config_Bible::setCheckSpaceBeforePunctuation (bible, !Database_Config_Bible::getCheckSpaceBeforePunctuation (bible));
  }
  view.set_variable ("space_before_punctuation", get_tick_box (Database_Config_Bible::getCheckSpaceBeforePunctuation (bible)));
                      
                      
  if (request->query.count ("sentencestructure")) {
    Database_Config_Bible::setCheckSentenceStructure (bible, !Database_Config_Bible::getCheckSentenceStructure (bible));
  }
  view.set_variable ("sentence_structure", get_tick_box (Database_Config_Bible::getCheckSentenceStructure (bible)));
                      
                      
  if (request->query.count ("paragraphstructure")) {
    Database_Config_Bible::setCheckParagraphStructure (bible, !Database_Config_Bible::getCheckParagraphStructure (bible));
  }
  view.set_variable ("paragraph_structure", get_tick_box (Database_Config_Bible::getCheckParagraphStructure (bible)));
                      
                      
  if (request->query.count ("chaptersversesversification")) {
    Database_Config_Bible::setCheckChaptesVersesVersification (bible, !Database_Config_Bible::getCheckChaptesVersesVersification (bible));
  }
  view.set_variable ("chapters_verses_versification", get_tick_box (Database_Config_Bible::getCheckChaptesVersesVersification (bible)));
                      
                      
  if (request->query.count ("wellformedusfm")) {
    Database_Config_Bible::setCheckWellFormedUsfm (bible, !Database_Config_Bible::getCheckWellFormedUsfm (bible));
  }
  view.set_variable ("well_formed_usfm", get_tick_box (Database_Config_Bible::getCheckWellFormedUsfm (bible)));
                      
                      
  if (request->query.count ("punctuationatendverse")) {
    Database_Config_Bible::setCheckMissingPunctuationEndVerse (bible, !Database_Config_Bible::getCheckMissingPunctuationEndVerse (bible));
  }
  view.set_variable ("punctuation_end_verse", get_tick_box (Database_Config_Bible::getCheckMissingPunctuationEndVerse (bible)));
                      
                      
  if (request->query.count ("patterns")) {
    Database_Config_Bible::setCheckPatterns (bible, !Database_Config_Bible::getCheckPatterns (bible));
  }
  view.set_variable ("check_patterns", get_tick_box (Database_Config_Bible::getCheckPatterns (bible)));

                      
  view.set_variable ("bible", bible);

                      
  page += view.render ("checks", "settings");
  page += Assets_Page::footer ();
  return page;
}
