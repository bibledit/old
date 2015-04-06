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


#include <xrefs/insert.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/abbreviations.h>
#include <tasks/logic.h>
#include <webserver/request.h>
#include <journal/index.h>
#include <database/volatile.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <access/bible.h>
#include <locale/translate.h>
#include <ipc/focus.h>
#include <bible/logic.h>


string xrefs_insert_url ()
{
  return "xrefs/insert";
}


bool xrefs_insert_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string xrefs_insert (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  Database_Volatile database_volatile;
  
  
  string sourceBible = request->database_config_user()->getSourceXrefBible ();
  string targetBible = request->database_config_user()->getTargetXrefBible ();
  int book = Ipc_Focus::getBook (webserver_request);
  int chapter = Ipc_Focus::getChapter (webserver_request);
  
  
  // Retrieve all abbreviations for the source Bible, sort them, longest first. Todo test sorting.
  // The replace routines replaces the longer strings first,
  // to be sure that no partial book abbreviations will be replaced before a fuller abbeviation.
  string s_abbrevs = Database_Config_Bible::getBookAbbreviations (sourceBible);
  vector <pair <int, string> > m_sourceAbbreviations = filter_abbreviations_read (s_abbrevs);
  vector <string> v_sourceAbbreviations;
  vector <unsigned int> lengths;
  for (auto element : m_sourceAbbreviations) {
    string abbrev = element.second;
    v_sourceAbbreviations.push_back (abbrev);
    int length = unicode_string_length (abbrev);
    lengths.push_back (length);
  }
  quick_sort (lengths, v_sourceAbbreviations, 0, lengths.size ());
  reverse (v_sourceAbbreviations.begin (), v_sourceAbbreviations.end ());

  
  s_abbrevs = Database_Config_Bible::getBookAbbreviations (targetBible);
  vector <pair <int, string> > targetAbbreviations = filter_abbreviations_read (s_abbrevs);


  /* Todo
  // Create array with book abbreviations to find, and one with their matching replacements.
  vector <string> find;
  vector <string> replace;
  for (auto sourceAbbreviation : v_sourceAbbreviations) {
    find.push_back (sourceAbbreviation);
    int book = 0;
    for (auto & element : targetAbbreviations) {
      
    }
    
    m_sourceAbbreviations [sourceAbbreviation];
    string target = sourceAbbreviation;
    for (auto element : targetAbbreviations) {
      if (element.second == bk) target = element.first;
    }
    replace.push_back (target);
  }
  
  
  // Storage identifier, based on the user's name.
  int identifier = filter_string_user_identifier (webserver_request);
  
  
  // Retrieve all notes from the database.
  string s_allxrefs = database_volatile.getValue (identifier, "sourcexrefs");
  vector <string> allxrefs = filter_string_explode (s_allxrefs, '\n');
  
  
  // Replace the abbreviations in the cross references.
  for (unsigned int i = 0; i < allxrefs.size () - 2; i += 3) {
    for (unsigned int i2 = 0; i2 < find.size (); i2++) {
      allxrefs [i + 2] = filter_string_str_replace (find [i2], replace [i2], allxrefs [i + 2]);
    }
  }
  
  
  // Get the target USFM into a map of verse => USFM fragment.
  map <int, string> usfmMap;
  string usfmString = request->database_bibles()->getChapter (targetBible, book, chapter);
  vector <int> verses = usfm_get_verse_numbers (usfmString);
  verses = array_unique (verses);
  for (auto verse : verses) {
    usfmMap [verse] = usfm_get_verse_text (usfmString, verse);
  }
  

  // Go through each verse, through each note within that verse,
  // look at source location, define target location, and insert the xref.
  for (int verse : verses) {
    
    
    // Gather array of cross references for this verse, if any.
    xrefs = array ();
    reset (allxrefs);
    for (allxrefs as xref) {
      if (xref ['verse'] == verse) {
        xrefs [] = array (xref ['offset'], xref ['text']);
      }
    }
    if (empty (xrefs)) continue;
    
    
    // Get the USFM for the current verse in the target Bible, if any.
    if (!isset (usfmArray [verse])) continue;
    usfm = usfmArray [verse];
    
    
    // Get the length of the text of the verse in the source Bible without the xrefs.
    // Get the ratio for positioning the xrefs by comparing the lengths of source and target verse text.
    sourceUsfm = request->database_bibles()->getChapter (sourceBible, book, chapter);
    sourceUsfm = usfm_get_verse_text (sourceUsfm, verse);
    sourceUsfm = Filter_Usfm::removeNotes (sourceUsfm, array ("x"));
    sourceLength = unicode_string_length (sourceUsfm);
    targetLength = unicode_string_length (usfm);
    ratio = targetLength / sourceLength;
    
    
    // Insert the notes.
    usfm = Filter_Usfm::insertNotes (usfm, xrefs, ratio);
    usfmArray [verse] = usfm;
    
    
  }

  
  string usfm;
  for (auto element : usfmMap) {
    if (!usfm.empty ()) usfm.append ("\n");
    usfm.append (element.second);
  }
  Bible_Logic::storeChapter (targetBible, book, chapter, usfm);
   */

  
  string page;
  Assets_Header header = Assets_Header (translate("Cross references"), webserver_request);
  page = header.run ();
  Assets_View view = Assets_View ();
  page += view.render ("xrefs", "insert");
  page += Assets_Page::footer ();
  return page;
}
