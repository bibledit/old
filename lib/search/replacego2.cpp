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


#include <search/replacego2.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <filter/usfm.h>
#include <filter/text.h>
#include <text/text.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <bible/logic.h>


string search_replacego2_url ()
{
  return "search/replacego2";
}


bool search_replacego2_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string search_replacego2 (void * webserver_request) // Todo fix it similar to search_replacego.
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  string siteUrl = Database_Config_General::getSiteURL ();
  
  
  // Get the action variables from the query.
  string id = request->query ["id"];
  string searchfor = request->query ["q"];
  string replacewith = request->query ["r"];
  bool casesensitive = (request->query ["c"] == "true");
  bool searchplain = (request->query ["p"] == "true");
  
  
  // Get Bible and passage for this identifier.
  vector <string> details = filter_string_explode (id, '_');
  int bibleID = -1;
  if (details.size() > 0) bibleID = convert_to_int (details [0]);
  string bible = request->database_bibles()->getName (bibleID);
  int book = 0;
  if (details.size () > 1) book = convert_to_int (details [1]);
  int chapter = -1;
  if (details.size () > 2) chapter = convert_to_int (details [2]);
  string verse;
  if (details.size () > 3) verse = details [3];
  
  
  string stylesheet = Database_Config_Bible::getExportStylesheet (bible);
  
  
  // As a standard to compare against, get the plain text from the search database,
  // do the replacements, count the replacements, and then get the desired new plain text.
  // This only applies when searching/replacing in the plain text, not when doing it in the USFM.
  int standardReplacementCount = 0;
  string standardPlainText = request->database_search()->getBibleVerseText (bible, book, chapter, convert_to_int (verse));
  if (searchplain) {
    if (casesensitive) {
      standardPlainText = filter_string_str_replace (searchfor, replacewith, standardPlainText, &standardReplacementCount);
    } else {
      vector <string> needles = filter_string_search_needles (searchfor, standardPlainText);
      for (auto & needle : needles) {
        standardPlainText = filter_string_str_replace (needle, replacewith, standardPlainText, &standardReplacementCount);
      }
    }
  }
  
  
  // Get the old USFM into a map of verse => USFM fragment.
  map <int, string> usfmArray;
  string usfmString = request->database_bibles()->getChapter (bible, book, chapter);
  vector <int> verses = usfm_get_verse_numbers (usfmString);
  sort (verses.begin(), verses.end());
  for (auto & vs : verses) {
    usfmArray [vs] = usfm_get_verse_text (usfmString, vs);
  }
  
  
  // Do the replacing in the correct verse of the raw USFM, and count how many replacements were made.
  string usfm = usfmArray [convert_to_int (verse)];
  int usfmReplacementCount = 0;
  if (casesensitive) {
    usfm = filter_string_str_replace (searchfor, replacewith, usfm, &usfmReplacementCount);
  } else {
    vector <string> needles = filter_string_search_needles (searchfor, usfm);
    for (auto & needle : needles) {
      usfm = filter_string_str_replace (needle, replacewith, usfm, &usfmReplacementCount);
    }
  }
  usfmArray [convert_to_int (verse)] = usfm;

  
  // Create the updated chapter USFM as a string.
  string updatedUsfm;
  for (auto & element : usfmArray) {
    if (!updatedUsfm.empty ()) updatedUsfm.append ("\n");
    updatedUsfm.append (element.second);
  }
  
  
  // Text filter for getting the new plain text from the new USFM.
  // This is for search/replace in plain text, not in USFM.
  Filter_Text filter_text = Filter_Text (bible);
  filter_text.text_text = new Text_Text ();
  filter_text.initializeHeadingsAndTextPerVerse (false);
  filter_text.addUsfmCode (updatedUsfm);
  filter_text.run (stylesheet);

  
  // Get the updated plain text of the correct verse of the updated USFM.
  // This is for search/replace in plain text, not in USFM.
  string updatedPlainText;
  map <int, string> texts = filter_text.getVersesText ();
  for (auto & element : texts) {
    int vs = element.first;
    string text = element.second;
    if (convert_to_string (vs) == verse) updatedPlainText.append (text + "\n");
  }
  map <int, string> headings = filter_text.verses_headings;
  for (auto & element : headings) {
    int vs = element.first;
    string heading = element.second;
    if (convert_to_string (vs) == verse) updatedPlainText.append (heading + "\n");
  }

  
  // Check that the standard and real number of replacements, and the standard and new texts, are the same.
  // This is for search/replace in plain text, not in USFM.
  bool replacementOkay = true;
  if (searchplain) {
    if (usfmReplacementCount != standardReplacementCount) replacementOkay = false;
    if (filter_string_trim (updatedPlainText) != filter_string_trim (standardPlainText)) replacementOkay = false;
  }
  
  
  // Generate success or failure icon.
  string icon;
  if (replacementOkay) {
    icon = "<span class=\"success\">✔</span>";
  } else {
    icon = "<span class=\"error\">✗</span>";
  }
  
  
  // Store the new chapter in the database on success.
  if (replacementOkay) {
    Bible_Logic::storeChapter (bible, book, chapter, updatedUsfm);
  }
  
  
  // Mark the new plain text.
  if (replacewith != "") {
    if (searchplain) {
      updatedPlainText = filter_string_markup_words ({replacewith}, updatedPlainText);
    }
  }
  
  
  // Clickable passage.
  string link = filter_passage_link_for_opening_editor_at (book, chapter, verse);
  
  
  // Success or failure message.
  string msg;
  if (replacementOkay) {
    msg = updatedPlainText;
  } else {
    msg = "<span class=\"error\">" + translate("This text could not be automatically replaced. Click the passage to do it manually.") + "</span>";
  }

  
  // Create output.
  string output = "<p>" + icon + " " + link + " " + msg + "</p>\n";
  
  
  // Output to browser.
  return output;
}
