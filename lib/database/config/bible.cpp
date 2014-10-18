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


#include <database/config/bible.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <libgen.h>
#include <sys/stat.h>
#include <cstring>
#include <filter/url.h>
#include <filter/string.h>


using namespace std;


Database_Config_Bible::Database_Config_Bible () // Todo
{
}


Database_Config_Bible::~Database_Config_Bible ()
{
}


// Functions for getting and setting values or lists of values.


string Database_Config_Bible::file (string bible, const char * key)
{
  return filter_url_create_root_path ("databases", "config", "bible", bible, key);
}


string Database_Config_Bible::getValue (string bible, const char * key, const char * default_value)
{
  string value;
  string filename = file (bible, key);
  if (filter_url_file_exists (filename)) value = filter_url_get_file_contents (filename);
  else value = default_value;
  return value;
}


void Database_Config_Bible::setValue (string bible, const char * key, string value)
{
  string filename = file (bible, key);
  string dirname = filter_url_dirname (filename);
  if (!filter_url_file_exists (dirname)) filter_url_mkdir (dirname); // Todo filter mkdir test it in the unit tests.
  filter_url_put_file_contents (filename, value);
}


// Named configuration functions.


string Database_Config_Bible::getRemoteRepositoryUrl (string bible)
{
  return getValue (bible, "remote-repo-url", "");
}
void Database_Config_Bible::setRemoteRepositoryUrl (string bible, string url)
{
  setValue (bible, "remote-repo-url", url);
}


string Database_Config_Bible::getViewableByAllUsers (string bible)
{
  return getValue (bible, "viewable-by-all-users", "");
}
void Database_Config_Bible::setViewableByAllUsers (string bible, string url)
{
  setValue (bible, "viewable-by-all-users", url);
}


string Database_Config_Bible::getCheckDoubleSpacesUsfm (string bible) 
{
  return getValue (bible, "double-spaces-usfm", "");
}
void Database_Config_Bible::setCheckDoubleSpacesUsfm (string bible, string value)
{
  setValue (bible, "double-spaces-usfm", value);
}


string Database_Config_Bible::getCheckFullStopInHeadings (string bible) 
{
  return getValue (bible, "full-stop-headings", "");
}
void Database_Config_Bible::setCheckFullStopInHeadings (string bible, string value) 
{
  setValue (bible, "full-stop-headings", value);
}


string Database_Config_Bible::getCheckSpaceBeforePunctuation (string bible) 
{
  return getValue (bible, "space-before-punctuation", "");
}
void Database_Config_Bible::setCheckSpaceBeforePunctuation (string bible, string value) 
{
  setValue (bible, "space-before-punctuation", value);
}


string Database_Config_Bible::getCheckSentenceStructure (string bible) 
{
  return getValue (bible, "sentence-structure", "");
}
void Database_Config_Bible::setCheckSentenceStructure (string bible, string value) 
{
  setValue (bible, "sentence-structure", value);
}


string Database_Config_Bible::getCheckParagraphStructure (string bible) 
{
  return getValue (bible, "paragraph-structure", "");
}
void Database_Config_Bible::setCheckParagraphStructure (string bible, string value)
{
  setValue (bible, "paragraph-structure", value);
}


string Database_Config_Bible::getCheckChaptesVersesVersification (string bible) 
{
  return getValue (bible, "check-chapters-verses-versification", "");
}
void Database_Config_Bible::setCheckChaptesVersesVersification (string bible, string value) 
{
  setValue (bible, "check-chapters-verses-versification", value);
}


string Database_Config_Bible::getCheckWellFormedUsfm (string bible)
{
  return getValue (bible, "check-well-formed-usfm", "");
}
void Database_Config_Bible::setCheckWellFormedUsfm (string bible, string value)
{
  setValue (bible, "check-well-formed-usfm", value);
}


string Database_Config_Bible::getCheckMissingPunctuationEndVerse (string bible) 
{
  return getValue (bible, "missing-punctuation-end-verse", "");
}
void Database_Config_Bible::setCheckMissingPunctuationEndVerse (string bible, string value) 
{
  setValue (bible, "missing-punctuation-end-verse", value);
}


string Database_Config_Bible::getCheckPatterns (string bible)
{
  return getValue (bible, "check_patterns", "");
}
void Database_Config_Bible::setCheckPatterns (string bible, string value) 
{
  setValue (bible, "check_patterns", value);
}


string Database_Config_Bible::getCheckingPatterns (string bible) 
{
  return getValue (bible, "checking-patterns", "");
}
void Database_Config_Bible::setCheckingPatterns (string bible, string value) 
{
  setValue (bible, "checking-patterns", value);
}


string Database_Config_Bible::getSentenceStructureCapitals (string bible)
{
  return getValue (bible, "sentence-structure-capitals", "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
}
void Database_Config_Bible::setSentenceStructureCapitals (string bible, string value) 
{
  setValue (bible, "sentence-structure-capitals", value);
}


string Database_Config_Bible::getSentenceStructureSmallLetters (string bible)
{
  return getValue (bible, "sentence-structure-small-letters", "a b c d e f g h i j k l m n o p q r s t u v w x y z");
}
void Database_Config_Bible::setSentenceStructureSmallLetters (string bible, string value) 
{
  setValue (bible, "sentence-structure-small-letters", value);
}


string Database_Config_Bible::getSentenceStructureEndPunctuation (string bible) 
{
  return getValue (bible, "sentence-structure-end-punctuation", ". ! ? :");
}
void Database_Config_Bible::setSentenceStructureEndPunctuation (string bible, string value) 
{
  setValue (bible, "sentence-structure-end-punctuation", value);
}


string Database_Config_Bible::getSentenceStructureMiddlePunctuation (string bible) 
{
  return getValue (bible, "sentence-structure-middle-punctuation", ", ;");
}
void Database_Config_Bible::setSentenceStructureMiddlePunctuation (string bible, string value) 
{
  setValue (bible, "sentence-structure-middle-punctuation", value);
}


string Database_Config_Bible::getSentenceStructureDisregards (string bible) 
{
  return getValue (bible, "sentence-structure-disregards", "( ) [ ] { } ' \" * - 0 1 2 3 4 5 6 7 8 9");
}
void Database_Config_Bible::setSentenceStructureDisregards (string bible, string value) 
{
  setValue (bible, "sentence-structure-disregards", value);
}


string Database_Config_Bible::getSentenceStructureNames (string bible) 
{
  return getValue (bible, "sentence-structure-names", "");
}
void Database_Config_Bible::setSentenceStructureNames (string bible, string value) 
{
  setValue (bible, "sentence-structure-names", value);
}


string Database_Config_Bible::getSprintTaskCompletionCategories (string bible) 
{
  return getValue (bible, "sprint-task-completion-categories", "Translate\nCheck\nHebrew/Greek\nDiscussions");
}
void Database_Config_Bible::setSprintTaskCompletionCategories (string bible, string value) 
{
  setValue (bible, "sprint-task-completion-categories", value);
}


string Database_Config_Bible::getRepeatSendReceive (string bible)
{
  return getValue (bible, "repeat-send-receive", "");
}
void Database_Config_Bible::setRepeatSendReceive (string bible, string value) 
{
  setValue (bible, "repeat-send-receive", value);
}


string Database_Config_Bible::getExportChapterDropCapsFrames (string bible)
{
  return getValue (bible, "export-chapter-drop-caps-frames", "0");
}
void Database_Config_Bible::setExportChapterDropCapsFrames (string bible, string value) 
{
  setValue (bible, "export-chapter-drop-caps-frames", value);
}


string Database_Config_Bible::getPageWidth (string bible) 
{
  return getValue (bible, "page-width", "210");
}
void Database_Config_Bible::setPageWidth  (string bible, string value) 
{
  setValue (bible, "page-width", value);
}


string Database_Config_Bible::getPageHeight (string bible) 
{
  return getValue (bible, "page-height", "297");
}
void Database_Config_Bible::setPageHeight  (string bible, string value) 
{
  setValue (bible, "page-height", value);
}


string Database_Config_Bible::getInnerMargin (string bible) 
{
  return getValue (bible, "inner-margin", "20");
}
void Database_Config_Bible::setInnerMargin  (string bible, string value) 
{
  setValue (bible, "inner-margin", value);
}


string Database_Config_Bible::getOuterMargin (string bible) 
{
  return getValue (bible, "outer-margin", "10");
}
void Database_Config_Bible::setOuterMargin  (string bible, string value) 
{
  setValue (bible, "outer-margin", value);
}


string Database_Config_Bible::getTopMargin (string bible)
{
  return getValue (bible, "top-margin", "10");
}
void Database_Config_Bible::setTopMargin  (string bible, string value) 
{
  setValue (bible, "top-margin", value);
}


string Database_Config_Bible::getBottomMargin (string bible) 
{
  return getValue (bible, "bottom-margin", "10");
}
void Database_Config_Bible::setBottomMargin  (string bible, string value) 
{
  setValue (bible, "bottom-margin", value);
}


string Database_Config_Bible::getDateInHeader (string bible) 
{
  return getValue (bible, "date-in-header", "");
}
void Database_Config_Bible::setDateInHeader  (string bible, string value) 
{
  setValue (bible, "date-in-header", value);
}


string Database_Config_Bible::getHyphenationFirstSet (string bible) 
{
  return getValue (bible, "hyphenation-first-set", "");
}
void Database_Config_Bible::setHyphenationFirstSet (string bible, string value) 
{
  setValue (bible, "hyphenation-first-set", value);
}


string Database_Config_Bible::getHyphenationSecondSet (string bible)
{
  return getValue (bible, "hyphenation-second-set", "");
}
void Database_Config_Bible::setHyphenationSecondSet (string bible, string value) 
{
  setValue (bible, "hyphenation-second-set", value);
}


string Database_Config_Bible::getExportStylesheet (string bible)
{
  return getValue (bible, "export-stylesheet", "Standard");
}
void Database_Config_Bible::setExportStylesheet (string bible, string value)
{
  setValue (bible, "export-stylesheet", value);
}


string Database_Config_Bible::getVersificationSystem (string bible)
{
  return getValue (bible, "versification-system", "English");
}
void Database_Config_Bible::setVersificationSystem (string bible, string value)
{
  setValue (bible, "versification-system", value);
}


string Database_Config_Bible::getVerseMapping (string bible)
{
  return getValue (bible, "verse-mapping", "English");
}
void Database_Config_Bible::setVerseMapping (string bible, string value)
{
  setValue (bible, "verse-mapping", value);
}


string Database_Config_Bible::getBookAbbreviations (string bible)
{
  return getValue (bible, "book-abbreviations", "");
}
void Database_Config_Bible::setBookAbbreviations (string bible, string value)
{
  setValue (bible, "book-abbreviations", value);
}


string Database_Config_Bible::getExportWebDuringNight (string bible)
{
  return getValue (bible, "export-web-during-night", "");
}
void Database_Config_Bible::setExportWebDuringNight (string bible, string value) 
{
  setValue (bible, "export-web-during-night", value);
}


string Database_Config_Bible::getExportHtmlDuringNight (string bible)
{
  return getValue (bible, "export-html-during-night", "");
}
void Database_Config_Bible::setExportHtmlDuringNight (string bible, string value) 
{
  setValue (bible, "export-html-during-night", value);
}


string Database_Config_Bible::getExportUsfmDuringNight (string bible)
{
  return getValue (bible, "export-usfm-during-night", "");
}
void Database_Config_Bible::setExportUsfmDuringNight (string bible, string value) 
{
  setValue (bible, "export-usfm-during-night", value);
}


string Database_Config_Bible::getExportTextDuringNight (string bible)
{
  return getValue (bible, "export-text-during-night", "");
}
void Database_Config_Bible::setExportTextDuringNight (string bible, string value) 
{
  setValue (bible, "export-text-during-night", value);
}


string Database_Config_Bible::getExportOdtDuringNight (string bible)
{
  return getValue (bible, "export-odt-during-night", "");
}
void Database_Config_Bible::setExportOdtDuringNight (string bible, string value) 
{
  setValue (bible, "export-odt-during-night", value);
}


string Database_Config_Bible::getGenerateInfoDuringNight (string bible)
{
  return getValue (bible, "generate-info-during-night", "");
}
void Database_Config_Bible::setGenerateInfoDuringNight (string bible, string value) 
{
  setValue (bible, "generate-info-during-night", value);
}


string Database_Config_Bible::getExportESwordDuringNight (string bible)
{
  return getValue (bible, "export-esword-during-night", "");
}
void Database_Config_Bible::setExportESwordDuringNight (string bible, string value) 
{
  setValue (bible, "export-esword-during-night", value);
}


string Database_Config_Bible::getExportOnlineBibleDuringNight (string bible)
{
  return getValue (bible, "export-onlinebible-during-night", "");
}
void Database_Config_Bible::setExportOnlineBibleDuringNight (string bible, string value) 
{
  setValue (bible, "export-onlinebible-during-night", value);
}


string Database_Config_Bible::getExportPassword (string bible)
{
  return getValue (bible, "export-password", "");
}
void Database_Config_Bible::setExportPassword (string bible, string value) 
{
  setValue (bible, "export-password", value);
}


string Database_Config_Bible::getSecureUsfmExport (string bible)
{
  return getValue (bible, "secure-usfm-export", "");
}
void Database_Config_Bible::setSecureUsfmExport (string bible, string value) 
{
  setValue (bible, "secure-usfm-export", value);
}


string Database_Config_Bible::getSecureOdtExport (string bible)
{
  return getValue (bible, "secure-odt-export", "");
}
void Database_Config_Bible::setSecureOdtExport (string bible, string value) 
{
  setValue (bible, "secure-odt-export", value);
}


string Database_Config_Bible::getBookOrder (string bible)
{
  return getValue (bible, "book-order", "");
}
void Database_Config_Bible::setBookOrder (string bible, string value) 
{
  setValue (bible, "book-order", value);
}


string Database_Config_Bible::getTextDirection (string bible)
{
  return getValue (bible, "text-direction", 0);
}
void Database_Config_Bible::setTextDirection (string bible, string value) 
{
  setValue (bible, "text-direction", value);
}


string Database_Config_Bible::getTextFont (string bible)
{
  return getValue (bible, "text-font", "");
}
void Database_Config_Bible::setTextFont (string bible, string value) 
{
  setValue (bible, "text-font", value);
}


