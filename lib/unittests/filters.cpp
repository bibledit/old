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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <database/modifications.h>
#include <database/state.h>
#include <database/git.h>
#include <database/login.h>
#include <database/logs.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <filter/archive.h>
#include <filter/text.h>
#include <filter/css.h>
#include <filter/diff.h>
#include <filter/abbreviations.h>
#include <filter/git.h>
#include <filter/merge.h>
#include <filter/date.h>
#include <filter/shell.h>
#include <filter/memory.h>
#include <session/logic.h>
#include <text/text.h>
#include <esword/text.h>
#include <onlinebible/text.h>
#include <html/text.h>
#include <odf/text.h>
#include <styles/logic.h>
#include <styles/css.h>
#include <ipc/notes.h>
#include <client/logic.h>
#include <bible/logic.h>
#include <developer/logic.h>
#include <ldap/logic.h>


// Todo move into smaller units.

void test_filter_archive ()
{
}


void test_odf_text ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  string OdfTextTestDotOdt = "/tmp/OdfTextTest.odt";
  string Odt2TxtOutput = "/tmp/Odt2TxtOutput.txt";
  // Test Odf converter paragraphs.
  {
    Odf_Text odf_text ("phpunit");
    odf_text.createPageBreakStyle ();
    odf_text.newParagraph ();
    evaluate (__LINE__, __func__, styles_logic_standard_sheet (), odf_text.currentParagraphStyle);
    odf_text.addText ("Paragraph One");
    evaluate (__LINE__, __func__, "Paragraph One", odf_text.currentParagraphContent);
    odf_text.newParagraph ();
    evaluate (__LINE__, __func__, "", odf_text.currentParagraphContent);
    odf_text.addText ("Paragraph Two");
    evaluate (__LINE__, __func__, "Paragraph Two", odf_text.currentParagraphContent);
    odf_text.newHeading1 ("Heading One");
    evaluate (__LINE__, __func__, "", odf_text.currentParagraphContent);
    odf_text.newPageBreak ();
    odf_text.newParagraph ();
    odf_text.addText ("Paragraph Three");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "Paragraph One\n"
    "\n"
    "Paragraph Two\n"
    "\n"
    "Heading One\n"
    "===========\n"
    "\n"
    "Paragraph Three\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Automatic Paragraph
  {
    Odf_Text odf_text ("phpunit");
    odf_text.addText ("Should create new paragraph automatically");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
"Should create new paragraph automatically\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Basic Note
  {
    Odf_Text odf_text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("Text");
    odf_text.addNote ("†", "");
    odf_text.addNoteText ("Note");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "Text†\n"
    "\n"
    "Note\n"
    "\n"
    ".\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // TestBasicFormattedText
  {
    Database_Styles database_styles;
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData (styles_logic_standard_sheet (), "add");
    Odf_Text odf_text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("text");
    odf_text.openTextStyle (add, false, false);
    odf_text.addText ("add");
    odf_text.closeTextStyle (false, false);
    odf_text.addText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = "textaddnormal.";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Basic Formatted Note
  {
    Database_Styles database_styles;
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData (styles_logic_standard_sheet (), "add");
    Odf_Text odf_text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("Text");
    odf_text.addNote ("𐌰", "f");
    odf_text.openTextStyle (add, true, false);
    odf_text.addNoteText ("Add");
    odf_text.closeTextStyle (true, false);
    odf_text.addNoteText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
      "Text𐌰\n"
      "\n"
      "Addnormal\n"
      "\n"
      ".\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Embedded Formatted Text.
  {
    Database_Styles database_styles;
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData (styles_logic_standard_sheet (), "add");
    add.italic = ooitOn;
    add.bold = 0;
    add.underline = 0;
    add.smallcaps = 0;
    add.superscript = false;
    add.color = "#000000";
    Database_Styles_Item nd = database_styles.getMarkerData (styles_logic_standard_sheet (), "nd");
    nd.italic = 0;
    nd.bold = 0;
    nd.underline = 0;
    nd.smallcaps = ooitOn;
    nd.superscript = false;
    nd.color = "#000000";
    Odf_Text odf_text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("text");
    odf_text.openTextStyle (add, false, false);
    odf_text.addText ("add");
    odf_text.openTextStyle (nd, false, true);
    odf_text.addText ("nd");
    odf_text.closeTextStyle (false, false);
    odf_text.addText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = "textaddndnormal.";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Embedded Formatted Note.
  {
    Database_Styles database_styles;
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData (styles_logic_standard_sheet (), "add");
    add.italic = ooitOn;
    add.bold = 0;
    add.underline = 0;
    add.smallcaps = 0;
    add.superscript = false;
    add.color = "#000000";
    Database_Styles_Item nd = database_styles.getMarkerData (styles_logic_standard_sheet (), "nd");
    nd.italic = 0;
    nd.bold = 0;
    nd.underline = 0;
    nd.smallcaps = ooitOn;
    nd.superscript = false;
    nd.color = "#000000";
    Odf_Text odf_text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("text");
    odf_text.addNote ("𐌰", "f");
    odf_text.openTextStyle (add, true, false);
    odf_text.addNoteText ("add");
    odf_text.openTextStyle (nd, true, true);
    odf_text.addNoteText ("nd");
    odf_text.closeTextStyle (true, false);
    odf_text.addNoteText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "text𐌰\n"
    "\n"
    "addndnormal\n"
    "\n"
    ".\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test paragraph formatting.
  {
    Database_Styles database_styles;
    database_styles.create ();
    Database_Styles_Item d = database_styles.getMarkerData (styles_logic_standard_sheet (), "d");
    Odf_Text odf_text ("phpunit");
    odf_text.createParagraphStyle (d.marker, d.fontsize, d.italic, d.bold, d.underline, d.smallcaps, d.justification, d.spacebefore, d.spaceafter, d.leftmargin, d.rightmargin, d.firstlineindent, true, false);
    odf_text.newParagraph ("d");
    odf_text.addText ("Paragraph with d style");
    odf_text.newParagraph ("d");
    odf_text.addText ("Paragraph with d style at first, then Standard");
    odf_text.updateCurrentParagraphStyle (styles_logic_standard_sheet ());
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "Paragraph with d style\n"
    "\n"
    "Paragraph with d style at first, then Standard\n"
    "";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  refresh_sandbox (true);
}


void test_filter_text ()
{
  trace_unit_tests (__func__);
  
  Database_Styles database_styles;
  database_styles.create ();

  string TextTestOdt  = "/tmp/TextTest.odt";
  string TextTestHtml = "/tmp/TextTest.html";
  string TextTestTxt  = "/tmp/TextTest.txt";
  
  string bible = "phpunit";

  // The unittests depend on known settings.
  Database_Config_Bible::setExportChapterDropCapsFrames (bible, true);

  // Test extraction of all sorts of information from USFM code.
  // Test basic formatting into OpenDocument.
  {
    string usfm = ""
      "\\id GEN\n"
      "\\h Header\n"
      "\\h1 Header1\n"
      "\\h2 Header2\n"
      "\\h3 Header3\n"
      "\\toc1 The Book of Genesis\n"
      "\\toc2 Genesis\n"
      "\\toc3 Gen\n"
      "\\cl Chapter\n"
      "\\c 1\n"
      "\\cp Ⅰ\n"
      "\\p\n"
      "\\v 1 Text chapter 1\n"
      "\\c 2\n"
      "\\cp ②\n"
      "\\h Header4\n"
      "\\p\n"
      "\\v 2 Text chapter 2\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    // Check that it finds the running headers.
    int desiredRunningHeaders = 5;
    int actualRunningHeaders = (int)filter_text.runningHeaders.size();
    evaluate (__LINE__, __func__, desiredRunningHeaders, actualRunningHeaders);
    if (actualRunningHeaders == desiredRunningHeaders) {
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[0].verse);
      evaluate (__LINE__, __func__, "h", filter_text.runningHeaders[0].marker);
      evaluate (__LINE__, __func__, "Header", filter_text.runningHeaders[0].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[1].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[1].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[1].verse);
      evaluate (__LINE__, __func__, "h1", filter_text.runningHeaders[1].marker);
      evaluate (__LINE__, __func__, "Header1", filter_text.runningHeaders[1].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[2].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[2].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[2].verse);
      evaluate (__LINE__, __func__, "h2", filter_text.runningHeaders[2].marker);
      evaluate (__LINE__, __func__, "Header2", filter_text.runningHeaders[2].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[3].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[3].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[3].verse);
      evaluate (__LINE__, __func__, "h3", filter_text.runningHeaders[3].marker);
      evaluate (__LINE__, __func__, "Header3", filter_text.runningHeaders[3].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[4].book);
      evaluate (__LINE__, __func__, 2, filter_text.runningHeaders[4].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[4].verse);
      evaluate (__LINE__, __func__, "h", filter_text.runningHeaders[4].marker);
      evaluate (__LINE__, __func__, "Header4", filter_text.runningHeaders[4].value);
    }
    // Check on Table of Contents items.
    int desiredlongTOCs = 1;
    int actuallongTOCs = filter_text.longTOCs.size();
    evaluate (__LINE__, __func__, desiredlongTOCs, actuallongTOCs);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.longTOCs[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.longTOCs[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.longTOCs[0].verse);
      evaluate (__LINE__, __func__, "toc1", filter_text.longTOCs[0].marker);
      evaluate (__LINE__, __func__, "The Book of Genesis", filter_text.longTOCs[0].value);
    }
    int desiredshortTOCs = 1;
    int actualshortTOCs = filter_text.shortTOCs.size();
    evaluate (__LINE__, __func__, desiredshortTOCs, actualshortTOCs);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.shortTOCs[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.shortTOCs[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.shortTOCs[0].verse);
      evaluate (__LINE__, __func__, "toc2", filter_text.shortTOCs[0].marker);
      evaluate (__LINE__, __func__, "Genesis", filter_text.shortTOCs[0].value);
    }
    // Check book abbreviation.
    int desiredbookAbbreviations = 1;
    int actualbookAbbreviations = filter_text.bookAbbreviations.size();
    evaluate (__LINE__, __func__, desiredbookAbbreviations, actualbookAbbreviations);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.bookAbbreviations[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.bookAbbreviations[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.bookAbbreviations[0].verse);
      evaluate (__LINE__, __func__, "toc3", filter_text.bookAbbreviations[0].marker);
      evaluate (__LINE__, __func__, "Gen", filter_text.bookAbbreviations[0].value);
    }
    // Check chapter specials.
    int desiredchapterLabels = 1;
    int actualchapterLabels = filter_text.chapterLabels.size();
    evaluate (__LINE__, __func__, desiredchapterLabels, actualchapterLabels);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.chapterLabels[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.chapterLabels[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.chapterLabels[0].verse);
      evaluate (__LINE__, __func__, "cl", filter_text.chapterLabels[0].marker);
      evaluate (__LINE__, __func__, "Chapter", filter_text.chapterLabels[0].value);
    }
    int desiredpublishedChapterMarkers = 2;
    int actualpublishedChapterMarkers = filter_text.publishedChapterMarkers.size();
    evaluate (__LINE__, __func__, desiredpublishedChapterMarkers, actualpublishedChapterMarkers);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.publishedChapterMarkers[0].book);
      evaluate (__LINE__, __func__, 1, filter_text.publishedChapterMarkers[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.publishedChapterMarkers[0].verse);
      evaluate (__LINE__, __func__, "cp", filter_text.publishedChapterMarkers[0].marker);
      evaluate (__LINE__, __func__, "Ⅰ", filter_text.publishedChapterMarkers[0].value);
      evaluate (__LINE__, __func__, 1, filter_text.publishedChapterMarkers[1].book);
      evaluate (__LINE__, __func__, 2, filter_text.publishedChapterMarkers[1].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.publishedChapterMarkers[1].verse);
      evaluate (__LINE__, __func__, "cp", filter_text.publishedChapterMarkers[1].marker);
      evaluate (__LINE__, __func__, "②", filter_text.publishedChapterMarkers[1].value);
    }
    // OpenDocument output.
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
    "\n"
    "Header4 Ⅰ\n"
    "=========\n"
    "\n"
    "[-- Image: frame1 --]\n"
    "\n"
    "Ⅰ\n"
    "\n"
    "Text chapter 1\n"
    "\n"
    "Header4 ②\n"
    "=========\n"
    "\n"
    "[-- Image: frame2 --]\n"
    "\n"
    "②\n"
    "\n"
    "Text chapter 2\n"
    "\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (TextTestOdt);
  filter_url_unlink (TextTestHtml);
  // There are two books here. This normally gives one new page between these two books.
  // Test that basic USFM code gets transformed correctly.
  {
    string usfm =
      "\\id GEN\n"
      "\\ide XYZ\n"
      "\\c 1\n"
      "\\p Text Genesis 1\n"
      "\\c 2\n"
      "\\p Text Genesis 2\n"
      "\\id MAT\n"
      "\\c 1\n"
      "\\p Text Matthew 1\n"
      "\\c 2\n"
      "\\p Text Matthew 2\n"
      "\\rem Comment\n"
      "\\xxx Unknown markup\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "\n"
      "Genesis 1\n"
      "=========\n"
      "\n"
      "Text Genesis 1\n"
      "\n"
      "Genesis 2\n"
      "=========\n"
      "\n"
      "Text Genesis 2\n"
      "\n"
      "Matthew 1\n"
      "=========\n"
      "\n"
      "Text Matthew 1\n"
      "\n"
      "Matthew 2\n"
      "=========\n"
      "\n"
      "Text Matthew 2\n"
      "\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
    evaluate (__LINE__, __func__, {"Matthew 2:0 Unknown marker \\xxx, formatting error: Unknown markup"}, filter_text.fallout);
    evaluate (__LINE__, __func__, {"Genesis 0:0 Text encoding: \\ide XYZ", "Matthew 2:0 Comment: \\rem Comment"}, filter_text.info);
  }
  // Test transformation of verse numbers and text following.
  {
    string usfm = 
      "\\id GEN\n"
      "\\v 1 Verse One.\n"
      "\\p Paragraph One.\n"
      "\\v 2 Verse Two.\n"
      "\\p\n"
      "\\v 3 Verse Three.\n"
      "\\v 4 Verse Four.\n"
      "\\v 5 Verse Five.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "\n"
      "1" + en_space () + "Verse One.\n"
      "\n"
      "Paragraph One. 2" + en_space () + "Verse Two.\n"
      "\n"
      "3" + en_space () + "Verse Three. 4" + en_space () + "Verse Four. 5" + en_space () + "Verse Five.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Test footnotes and cross references.
  {
    string usfm = 
      "\\id GEN\n"
      "\\v 1 Text 1\\x + \\xt Isa. 1.1.\\x*\\x - \\xt Isa. 2.2.\\x*\\x + \\xt Isa. 3.3.\\x*, text 2\\f + \\fk Word1: \\fl Heb. \\fq Explanation1.\\f*\\f + \\fk Word2: \\fl Heb. \\fq Explanation2.\\f*, text3.\\f + \\fk Test: \\fl Heb. \\fq Note at the very end.\\f*\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "\n"
      "1" + en_space () + "Text 1a\n"
      "\n"
      "Isa. 1.1.\n"
      "\n"
      "Isa. 2.2.\n"
      "\n"
      "b\n"
      "\n"
      "Isa. 3.3.\n"
      "\n"
      ", text 21\n"
      "\n"
      "Word1: Heb. Explanation1.\n"
      "\n"
      "2\n"
      "\n"
      "Word2: Heb. Explanation2.\n"
      "\n"
      ", text3.3\n"
      "\n"
      "Test: Heb. Note at the very end.\n"
      "\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Test clear text export.
  {
    string usfm = 
      "\\id GEN\n"
      "\\h Genesis\n"
      "\\toc2 Genesis\n"
      "\\mt2 The book of\n"
      "\\mt Genesis\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 In the be\\x + \\xt Isa. 1.1.\\x*\\x - \\xt Isa. 2.2.\\x*ginning, God created\\f + \\fk Word1: \\fl Heb. \\fq Explanation1.\\f*\\f + \\fk Word2: \\fl Heb. \\fq Explanation2.\\f* the heavens and the earth.\\f + \\fk Test: \\fl Heb. \\fq Note at the very end.\\f*\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.text_text = new Text_Text ();
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    string output = filter_text.text_text->get ();
    string notes = filter_text.text_text->getnote ();
    string standard =
      "The book of\n"
      "Genesis\n"
      "1\n"
      "1 In the beginning, God created the heavens and the earth.";
    evaluate (__LINE__, __func__, standard, output);
    string standardnotes =
      "Isa. 1.1.\n"
      "Isa. 2.2.\n"
      "Word1: Heb. Explanation1.\n"
      "Word2: Heb. Explanation2.\n"
      "Test: Heb. Note at the very end.";
    evaluate (__LINE__, __func__, standardnotes, notes);
  }
  // Test clear text export.
  {
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 Chapter 1, verse one.\n"
      "\\v 2 Verse two.\n"
      "\\c 2\n"
      "\\p\n"
      "\\v 1 Chapter 2, verse one.\n"
      "\\v 2 Verse two.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.text_text = new Text_Text ();
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    string output = filter_text.text_text->get ();
    string notes = filter_text.text_text->getnote ();
    string standard =
      "1\n"
      "1 Chapter 1, verse one. 2 Verse two.\n"
      "2\n"
      "1 Chapter 2, verse one. 2 Verse two.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (output));
    evaluate (__LINE__, __func__, "", notes);
  }
  // Test verses headings.
  {
    string usfm =
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 Verse one.\n"
      "\\v 2 Verse two.\n"
      "\\s Heading one\n"
      "\\p\n"
      "\\v 3 Verse three\n"
      "\\p\n"
      "\\s Heading two\n"
      "\\p\n"
      "\\v 4 Verse four.\n"
      "\\v 5 Verse five.\n"
      "\\c 2\n"
      "\\s Heading three\n"
      "\\p\n"
      "\\v 1 Verse one.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse (false);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    map <int, string> output = filter_text.verses_headings;
    map <int, string> standard = { {0, "Heading three"}, {2, "Heading one"}, {3, "Heading two"} };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test verses headings.
  {
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 Verse one.\n"
      "\\s \\s Usuku lweN\\nd kosi\\nd* luyeza masinyane\n"
      "\\p\n"
      "\\v 2 Verse two\n"
      "\\p\n"
      "\\s Heading \\add two\\add*\n"
      "\\p\n"
      "\\v 3 Verse three\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse (false);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    map <int, string> output = filter_text.verses_headings;
    map <int, string> standard = { {1, "Usuku lweNkosi luyeza masinyane"}, {2, "Heading two"} };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test verses text.
  {
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 2 Verse\\x + \\xt Isa. 1.1.\\x* two.\n"
      "\\v 3 Verse three\\x + \\xt Isa. 1.1.\\x*.\n"
      "\\s Heading one\n"
      "\\p\n"
      "\\v 4 Verse four.\n"
      "\\p\n"
      "\\s Heading two\n"
      "\\p\n"
      "\\v 5 Verse five.\n"
      "\\v 6 Verse six.\n"
      "\\c 2\n"
      "\\s Heading three\n"
      "\\p\n"
      "\\v 1 Verse one\\x + \\xt Isa. 1.1.\\x*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse (false);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    map <int, string> output = filter_text.getVersesText ();
    map <int, string> standard = {
      {1, "Verse one."},
      {2, "Verse two."},
      {3, "Verse three."},
      {4, "Verse four."},
      {5, "Verse five."},
      {6, "Verse six."},
    };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test verses text.
  {
    string usfm = 
      "\\c 15\n"
      "\\s Heading\n"
      "\\p\n"
      "\\v 1 He said:\n"
      "\\p I will sing to the Lord.\n"
      "\\v 2 The Lord is my strength.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse (false);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    map <int, string> output = filter_text.getVersesText ();
    map <int, string> standard = {
      {1, "He said: I will sing to the Lord."},
      {2, "The Lord is my strength." }
    };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test paragraph starting markers.
  {
    string usfm = 
      "\\c 1\n"
      "\\s Heading\n"
      "\\p\n"
      "\\v 1 He said:\n"
      "\\q1 I will sing to the Lord.\n"
      "\\v 2 The Lord is my strength.\n"
      "\\q2 I trust in Him.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse (false);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    evaluate (__LINE__, __func__, {"p", "q1", "q2"}, filter_text.paragraph_starting_markers);
  }
  // Test improved paragraph detection.
  {
    string path = filter_url_create_root_path ("unittests", "tests", "ShonaNumbers23.usfm");
    string usfm = filter_url_file_get_contents (path);
    Filter_Text filter_text = Filter_Text ("");
    filter_text.initializeHeadingsAndTextPerVerse (false);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    for (size_t i = 0; i < filter_text.verses_paragraphs.size (); i++) {
      map <int, string> verses_paragraph = filter_text.verses_paragraphs [i];
      map <int, string> standard;
      if (i == 0) {
        standard = {
          make_pair (1, "Zvino Bharamu wakati kuna Bharaki: Ndivakire pano aritari nomwe, ugondigadzirire pano nzombe nomwe nemakondohwe manomwe."),
          make_pair (2, "Bharaki ndokuita Bharamu sezvakataura; Bharaki naBharamu ndokupa nzombe diki negondohwe paaritari imwe neimwe."),
          make_pair (3, "Zvino Bharamu wakati kuna Bharaki: Mira pachipo chako chinopiswa, asi ini ndichaenda; zvimwe Jehovha achauya kuzosangana neni; zvino chinhu chaanondiratidza ndichakuudza. Zvino wakaenda pakakwirira."),
          make_pair (4, "Zvino Mwari wakasangana naBharamu, iye ndokuti kwaari: Ndagadzira aritari nomwe uye ndapa nzombe diki negondohwe paaritari imwe neimwe. "),
          make_pair (5, "Jehovha ndokuisa shoko mumuromo waBharamu, ndokuti: Dzokera kuna Bharaki ugotaura seizvi."),
          make_pair (6, "Zvino wakadzokera kwaari, uye tarira, amire pachipo chake chinopiswa, iye nemachinda ese aMoabhu."),
          make_pair (7, "Zvino wakasimudza mufananidzo wake ndokuti: Bharaki mambo waMoabhu wakandivinga kubva Siriya, pamakomo ekumabvazuva achiti: Uya nditukire Jakove, uye uya unyombe Israeri."),
          make_pair (8, "Ini ndingatuka sei Mwari waasina kutuka; uye ndinganyomba sei Jehovha waasina kunyomba?"),
          make_pair (9, "Nokuti kubva panhongonya yemabwe ndinomuona, uye kubva pazvikomo ndinomutarisa; tarira, vanhu vachagara vega, uye havangaverengwi pakati pendudzi."),
          make_pair (10, "Ndiani angaverenga guruva raJakove, nekuverenga chechina chaIsraeri? Mweya wangu ngaufe rufu rwevakarurama, uye magumo angu ngaave seake!"),
          make_pair (11, "Zvino Bharaki wakati kuna Bharamu: Wandiitei? Ndakutora kuti utuke vavengi vangu; zvino tarira, wavaropafadza nekuvaropafadza."),
          make_pair (12, "Zvino wakapindura ndokuti: Handifaniri kuchenjerera kutaura izvo Jehovha zvaanoisa mumuromo mangu here?"),
          make_pair (13, "Zvino Bharaki wakati kwaari: Uya hako neni kune imwe nzvimbo paungavaona kubvapo; uchaona kuguma kwavo chete, uye haungavaoni vese; ugonditukira ivo kubva ipapo.")
        };
      }
      if (i == 1) {
        standard = {
          make_pair (14, "Zvino wakamutora akamuisa mumunda weZofimu, panhongonya yePisiga, ndokuvaka aritari nomwe, ndokupa nzombe diki negondohwe paaritari imwe neimwe."),
          make_pair (15, "Zvino wakati kuna Bharaki: Mira pano pachipo chako chinopiswa, zvino ini ndichasangana naJehovha uko."),
          make_pair (16, "Zvino Jehovha wakasangana naBharamu, ndokuisa shoko mumuromo make, ndokuti: Dzokera kuna Bharaki ugotaura seizvi."),
          make_pair (17, "Zvino asvika kwaari, tarira, amire pachipo chake chinopiswa, nemachinda aMoabhu anaye. Bharaki ndokuti kwaari: Jehovha wataurei?"),
          make_pair (18, "Zvino wakasimudza mufananidzo wake ndokuti: Simuka Bharaki, ugonzwa; urereke nzeve kwandiri, iwe mwanakomana waZipori:"),
          make_pair (19, "Mwari haasi munhu kuti areve nhema, kana mwanakomana wemunhu kuti ashanduke. Iye wakareva, haangaiti kanhi? Kana wakareva, haangasimbisi kanhi?"),
          make_pair (20, "Tarira, ndagamuchira kuti ndiropafadze; zvino waropafadza, uye handigoni kuzvidzosa."),
          make_pair (21, "Haana kutarisira chakaipa kuna Jakove; uye haana kuona kutsauka kuna Israeri; Jehovha Mwari wake anaye, nekudanidzira kwamambo kuri pakati pavo."),
          make_pair (22, "Mwari wakavabudisa Egipita, ane sesimba renyati."),
          make_pair (23, "Zvirokwazvo hakuna un'anga hunopikisa Jakove, kana kuvuka kunopikisa Israeri. Ikozvino zvicharehwa pamusoro paJakove nepamusoro paIsraeri kuti Mwari wakazviita."),
          make_pair (24, "Tarirai, vanhu vachasimuka seshumbakadzi, vazvisimudze seshumba. Havangarari pasi kusvika vadya chakajimbiwa, uye vanwa ropa revakaurawa.")
        };
      }
      if (i == 2) {
        standard = {
          make_pair (25, "Zvino Bharaki wakati kuna Bharamu: Zvino usatongovatuka kana kutongovaropafadza."),
          make_pair (26, "Asi Bharamu wakapindura, ndokuti kuna Bharaki: Handina kukuudza here ndichiti: Zvese Jehovha zvaanotaura, ndizvo zvandichaita?")
        };
      }
      if (i == 3) {
        standard = {
          make_pair (27, "Zvino Bharaki wakati kuna Bharamu: Uya hako, ndichakuendesa kune imwe nzvimbo; zvimwe zvingarurama pameso aMwari kuti unditukire ivo kubva ipapo."),
          make_pair (28, "Zvino Bharaki wakatora ndokuisa Bharamu panhongonya yePeori, pakatarisa pasi pachiso cherenje."),
          make_pair (29, "Zvino Bharamu wakati kuna Bharaki: Ndivakire pano aritari nomwe, undigadzirire pano nzombe diki nomwe nemakondohwe manomwe."),
          make_pair (30, "Bharaki ndokuita Bharamu sezvaakataura, ndokupa nzombe diki negondohwe paaritari imwe neimwe.")
        };
      }
      evaluate (__LINE__, __func__, standard, verses_paragraph);
    }
    evaluate (__LINE__, __func__, {"p", "p", "p", "p"}, filter_text.paragraph_starting_markers);
  }
  // Test embedded text.
  {
    string usfm = 
      "\\c 1\n"
      "\\p\n"
      "\\v 1 He said: I will sing \\add to the \\+nd Lord\\+nd*\\add*.\n"
      "\\v 2 The \\nd Lord\\nd* is my strength.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse (false);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    map <int, string> output = filter_text.getVersesText ();
    map <int, string> standard = {
      {1, "He said: I will sing to the Lord."},
      {2, "The Lord is my strength."}
    };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test embedded html.
  {
    // Open character style, and embedded character style, and close both normally.
    string usfm =
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd*\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.html_text_standard = new Html_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    string html = filter_text.html_text_standard->getInnerHtml ();
    string standard = 
      "<p class=\"p\">"
      "<span class=\"v\">1</span>"
      "<span> I will sing </span>"
      "<span class=\"add\">to the </span>"
      "<span class=\"add nd\">Lord</span>"
      "<span>.</span>"
      "</p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test embedded html.
  {
    // Open character style, open embedded character style, close embedded one, then close the outer one.
    string usfm = 
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd* God\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.html_text_standard = new Html_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    string html = filter_text.html_text_standard->getInnerHtml ();
    string standard = 
      "<p class=\"p\">"
      "<span class=\"v\">1</span>"
      "<span> I will sing </span>"
      "<span class=\"add\">to the </span>"
      "<span class=\"add nd\">Lord</span>"
      "<span class=\"add\"> God</span>"
      "<span>.</span>"
      "</p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test embedded html.
  {
    // Open character style, open embedded character style, then closing the outer one closes the embedded one also.
    string usfm = 
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.html_text_standard = new Html_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    string html = filter_text.html_text_standard->getInnerHtml ();
    string standard = 
      "<p class=\"p\">"
      "<span class=\"v\">1</span>"
      "<span> I will sing </span>"
      "<span class=\"add\">to the </span>"
      "<span class=\"add nd\">Lord</span>"
      "<span>.</span>"
      "</p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test embedded OpenDocument.
  {
    // Open character style, and embedded character style, and close both normally.
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd*\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "Genesis 1\n"
      "=========\n"
      "\n"
      "1 I will sing to the Lord.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Exercise bits in document to generate text and note citations.
  {
    string usfm = 
      "\\id GEN\n"
      "\\v 1 Text 1\\x + \\xt Isa. 1.1.\\x* text\\f + \\fk Word: \\fl Heb. \\fq Explanation1.\\f* text\\fe + \\fk Word: \\fl Heb. \\fq Explanation1.\\fe*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_text_and_note_citations = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    filter_text.odf_text_text_and_note_citations->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    string standard = ""
      "1 Text 1a text1 text1.";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Test embedded OpenDocument.
  {
    // Open character style, open embedded character style, close embedded one, then close the outer one.
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd* God\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "Genesis 1\n"
      "=========\n"
      "\n"
      "1 I will sing to the Lord God.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Test embedded OpenDocument.
  {
    // Open character style, open embedded character style, then closing the outer one closes the embedded one also.
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run (styles_logic_standard_sheet ());
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "Genesis 1\n"
      "=========\n"
      "\n"
      "1 I will sing to the Lord.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (TextTestOdt);
  filter_url_unlink (TextTestHtml);
  filter_url_unlink (TextTestTxt);
  refresh_sandbox (true);
}


void test_filter_url ()
{
  trace_unit_tests (__func__);
  
  {
    // Test writing to and reading from files, and whether a file exists.
    string filename = "/tmp/בוקר טוב";
    string contents = "בוקר טוב בוקר טוב";
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filename));
    filter_url_file_put_contents (filename, contents);
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filename));
    evaluate (__LINE__, __func__, contents, filter_url_file_get_contents (filename));
    filter_url_unlink (filename);
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filename));
  }
  
  {
    // Test function to check existence of directory.
    string folder = "/tmp/בוקר טוב";
    evaluate (__LINE__, __func__, false, file_or_dir_exists (folder));
    filter_url_mkdir (folder);
    evaluate (__LINE__, __func__, true, file_or_dir_exists (folder));
    filter_url_rmdir (folder);
    evaluate (__LINE__, __func__, false, file_or_dir_exists (folder));
  }
  
  {
    // Test unique filename.
    string filename = "/tmp/unique";
    filter_url_file_put_contents (filename, "");
    string filename1 = filter_url_unique_path (filename);
    filter_url_file_put_contents (filename1, "");
    evaluate (__LINE__, __func__, "/tmp/unique.1", filename1);
    string filename2 = filter_url_unique_path (filename);
    filter_url_file_put_contents (filename2, "");
    evaluate (__LINE__, __func__, "/tmp/unique.2", filename2);
    filter_url_unlink (filename);
    filter_url_unlink (filename1);
    filter_url_unlink (filename2);
  }
  
  {
    // Html export filenames.
    evaluate (__LINE__, __func__, "index.html", filter_url_html_file_name_bible ());
    evaluate (__LINE__, __func__, "path/index.html", filter_url_html_file_name_bible ("path"));
    evaluate (__LINE__, __func__, "path/01-Genesis.html", filter_url_html_file_name_bible ("path", 1));
    evaluate (__LINE__, __func__, "01-Genesis.html", filter_url_html_file_name_bible ("", 1));
    evaluate (__LINE__, __func__, "path/11-1Kings.html", filter_url_html_file_name_bible ("path", 11));
    evaluate (__LINE__, __func__, "path/22-SongofSolomon-000.html", filter_url_html_file_name_bible ("path", 22, 0));
    evaluate (__LINE__, __func__, "path/33-Micah-333.html", filter_url_html_file_name_bible ("path", 33, 333));
    evaluate (__LINE__, __func__, "33-Micah-333.html", filter_url_html_file_name_bible ("", 33, 333));
  }

  {
    // mkdir and rmdir including parents.
    string directory = filter_url_create_path (testing_directory, "a", "b");
    filter_url_mkdir (directory);
    string path = filter_url_create_path (directory, "c");
    string contents = "unittest";
    filter_url_file_put_contents (path, contents);
    evaluate (__LINE__, __func__, contents, filter_url_file_get_contents (path));
    
    path = filter_url_create_path (testing_directory, "a");
    evaluate (__LINE__, __func__, true, file_or_dir_exists (path));
    evaluate (__LINE__, __func__, true, filter_url_is_dir (path));
    
    filter_url_rmdir (path);
    evaluate (__LINE__, __func__, false, file_or_dir_exists (path));
    evaluate (__LINE__, __func__, false, filter_url_is_dir (path));
  }

  {
    // Test filter_url_escape_shell_argument.
    evaluate (__LINE__, __func__, "'argument'", filter_url_escape_shell_argument ("argument"));
    evaluate (__LINE__, __func__, "'argu\\'ment'", filter_url_escape_shell_argument ("argu'ment"));
  }

  {
    // Test URL decoder.
    evaluate (__LINE__, __func__, "Store settings", filter_url_urldecode ("Store+settings"));
    evaluate (__LINE__, __func__, "test@mail", filter_url_urldecode ("test%40mail"));
    evaluate (__LINE__, __func__, "ᨀab\\d@a", filter_url_urldecode ("%E1%A8%80ab%5Cd%40a"));
    // Test URL encoder.
    evaluate (__LINE__, __func__, "Store%20settings", filter_url_urlencode ("Store settings"));
    evaluate (__LINE__, __func__, "test%40mail", filter_url_urlencode ("test@mail"));
    evaluate (__LINE__, __func__, "%E1%A8%80ab%5Cd%40a", filter_url_urlencode ("ᨀab\\d@a"));
    evaluate (__LINE__, __func__, "foo%3Dbar%26baz%3D", filter_url_urlencode ("foo=bar&baz="));
    evaluate (__LINE__, __func__, "%D7%91%D6%BC%D6%B0%D7%A8%D6%B5%D7%90%D7%A9%D7%81%D6%B4%D6%96%D7%99%D7%AA", filter_url_urlencode ("בְּרֵאשִׁ֖ית"));
  }

  {
    // Test dirname and basename functions.
    evaluate (__LINE__, __func__, ".", filter_url_dirname (""));
    evaluate (__LINE__, __func__, ".", filter_url_dirname ("/"));
    evaluate (__LINE__, __func__, ".", filter_url_dirname ("dir/"));
    evaluate (__LINE__, __func__, ".", filter_url_dirname ("/dir"));
    evaluate (__LINE__, __func__, "foo", filter_url_dirname ("foo/bar"));
    evaluate (__LINE__, __func__, "/foo", filter_url_dirname ("/foo/bar"));
    evaluate (__LINE__, __func__, "/foo", filter_url_dirname ("/foo/bar/"));
    evaluate (__LINE__, __func__, "a.txt", filter_url_basename ("/a.txt"));
    evaluate (__LINE__, __func__, "txt", filter_url_basename ("/txt/"));
    evaluate (__LINE__, __func__, "foo.bar", filter_url_basename ("/path/to/foo.bar"));
    evaluate (__LINE__, __func__, "foo.bar", filter_url_basename ("foo.bar"));
  }

  {
    // Test http GET and POST
    string result, error;
    result = filter_url_http_get ("http://localhost/none", error, false);
#ifndef HAVE_CLIENT
    evaluate (__LINE__, __func__, "Couldn't connect to server", error);
#endif
    evaluate (__LINE__, __func__, "", result);
    map <string, string> values = {make_pair ("a", "value1"), make_pair ("b", "value2")};
    result = filter_url_http_post ("http://localhost/none", values, error, false, false);
#ifndef HAVE_CLIENT
    evaluate (__LINE__, __func__, "Couldn't connect to server", error);
#endif
    evaluate (__LINE__, __func__, "", result);
  }
  
  {
    // Test low-level http(s) client error for unknown host.
    string result, error;
    result = filter_url_http_request_mbed ("http://unknownhost", error, {}, "", false);
    evaluate (__LINE__, __func__, "", result);
    evaluate (__LINE__, __func__, "unknownhost: nodename nor servname provided, or not known", error);
  }

  {
    // Test low-level http(s) client error for closed port.
    string result, error;
    result = filter_url_http_request_mbed ("http://bibledit.org:8082/non-existing", error, {}, "", false);
    evaluate (__LINE__, __func__, "", result);
    evaluate (__LINE__, __func__, "bibledit.org:8082: Connection refused | bibledit.org:8082: No route to host", error);
  }

  {
    // Test low-level http(s) client result.
    string result, error;
    result = filter_url_http_request_mbed ("http://bibledit.org", error, {}, "", false);
    evaluate (__LINE__, __func__, true, result.find ("Bibledit") != string::npos);
    evaluate (__LINE__, __func__, true, result.find ("Cloud") != string::npos);
    evaluate (__LINE__, __func__, true, result.find ("Install") != string::npos);
    evaluate (__LINE__, __func__, true, result.find ("Demo") != string::npos);
    evaluate (__LINE__, __func__, "", error);
  }

  {
    // Test removing credentials from a URL.
    string url = "https://username:password@github.com/username/repository.git";
    url = filter_url_remove_username_password (url);
    evaluate (__LINE__, __func__, "https://github.com/username/repository.git", url);
  }

  {
    // Test recursively copying a directory.
    string input = filter_url_create_root_path ("unittests");
    string output = "/tmp/test_copy_directory";
    filter_url_rmdir (output);
    filter_url_dir_cp (input, output);
    string path = filter_url_create_path (output, "tests", "basic.css");
    evaluate (__LINE__, __func__, true, file_or_dir_exists (path));
  }

  {
    // Secure communications.
    filter_url_ssl_tls_initialize ();
    
    string url;
    string error;
    string result;
    
    url = filter_url_set_scheme (" localhost ", false);
    evaluate (__LINE__, __func__, "http://localhost", url);
    url = filter_url_set_scheme ("httpx://localhost", false);
    evaluate (__LINE__, __func__, "http://localhost", url);
    url = filter_url_set_scheme ("http://localhost", true);
    evaluate (__LINE__, __func__, "https://localhost", url);
    
    result = filter_url_http_request_mbed ("http://www.google.nl", error, {}, "", false);
    evaluate (__LINE__, __func__, true, result.find ("google") != string::npos);
    evaluate (__LINE__, __func__, true, result.find ("search") != string::npos);
    evaluate (__LINE__, __func__, true, result.find ("background") != string::npos);
    evaluate (__LINE__, __func__, "", error);
    
    result = filter_url_http_request_mbed ("https://www.google.nl", error, {}, "", false);
    evaluate (__LINE__, __func__, true, result.find ("google") != string::npos);
    evaluate (__LINE__, __func__, true, result.find ("search") != string::npos);
    evaluate (__LINE__, __func__, true, result.find ("background") != string::npos);
    evaluate (__LINE__, __func__, "", error);
    
    result = filter_url_http_request_mbed ("https://bibledit.org:8081", error, {}, "", false);
    evaluate (__LINE__, __func__, "", result);
    evaluate (__LINE__, __func__, "Response code: 302 Found", error);
    
    filter_url_ssl_tls_finalize ();
  }
  
  {
    // Testing is_dir.
    string path = filter_url_create_root_path ("git");
    evaluate (__LINE__, __func__, true, filter_url_is_dir (path));
    path = filter_url_create_root_path ("setup", "index.html");
    evaluate (__LINE__, __func__, false, filter_url_is_dir (path));
    
    string directory = filter_url_create_root_path (filter_url_temp_dir ());
    string file1 = filter_url_create_path (directory, "1");
    string file2 = filter_url_create_path (directory, "2");
    filter_url_file_put_contents (file1, "x");
    filter_url_file_put_contents (file2, "x");
    
    // Testing checking for and setting write permissions.
    evaluate (__LINE__, __func__, true, filter_url_get_write_permission (directory));
    evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file1));
    evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file2));
    
    chmod (directory.c_str(), S_IRUSR);
    chmod (file1.c_str(), S_IRUSR);
    chmod (file2.c_str(), S_IRUSR);
    
    evaluate (__LINE__, __func__, false, filter_url_get_write_permission (directory));
    evaluate (__LINE__, __func__, false, filter_url_get_write_permission (file1));
    evaluate (__LINE__, __func__, false, filter_url_get_write_permission (file2));
    
    filter_url_set_write_permission (directory);
    filter_url_set_write_permission (file1);
    filter_url_set_write_permission (file2);
    
    evaluate (__LINE__, __func__, true, filter_url_get_write_permission (directory));
    evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file1));
    evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file2));
  }
  
  refresh_sandbox (true);
}


void test_filter_string_rand ()
{
  trace_unit_tests (__func__);
  
  int floor = 100000;
  int ceiling = 999999;
  int r1 = filter_string_rand (floor, ceiling);
  if ((r1 < floor) || (r1 > ceiling)) evaluate (__LINE__, __func__, "Random generator out of bounds", convert_to_string (r1));
  int r2 = filter_string_rand (floor, ceiling);
  if ((r2 < floor) || (r2 > ceiling)) evaluate (__LINE__, __func__, "Random generator out of bounds", convert_to_string (r2));
  if (r1 == r2) evaluate (__LINE__, __func__, "Random generator should generate different values", convert_to_string (r1) + " " + convert_to_string (r2));
}


void test_filter_passage ()
{
  trace_unit_tests (__func__);
  {
    Passage passage;
    evaluate (__LINE__, __func__, "", passage.bible);
    evaluate (__LINE__, __func__, 0, passage.book);
    evaluate (__LINE__, __func__, 0, passage.chapter);
    evaluate (__LINE__, __func__, "", passage.verse);
    passage = Passage ("bible", 1, 2, "3");
    evaluate (__LINE__, __func__, "bible", passage.bible);
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "3", passage.verse);
    evaluate (__LINE__, __func__, true, passage.equal (passage));
    Passage passage2 = Passage ("bible", 1, 2, "4");
    evaluate (__LINE__, __func__, false, passage.equal (passage2));
  }
  // Encoding and decoding passages.
  {
    Passage input = Passage ("עברית", 1, 2, "3");
    string encoded = input.encode ();
    Passage output = Passage::decode (encoded);
    evaluate (__LINE__, __func__, true, input.equal (output));

    input = Passage ("ελληνικά", 5, 4, "0");
    encoded = input.encode ();
    output = Passage::decode (encoded);
    evaluate (__LINE__, __func__, true, input.equal (output));
    
    input = Passage ("Sample .!_ Bible", 99, 999, "9999");
    encoded = input.encode ();
    output = Passage::decode (encoded);
    evaluate (__LINE__, __func__, true, input.equal (output));
  }
  // PassageDisplay
  {
    evaluate (__LINE__, __func__, "Genesis 10:2", filter_passage_display (1, 10, "2"));
    evaluate (__LINE__, __func__, "Exodus 11:2", filter_passage_display (2, 11, "2"));
  }
  // PassagesDisplayInline
  {
    evaluate (__LINE__, __func__, "Genesis 10:2", filter_passage_display_inline ({Passage ("", 1, 10, "2")}));
    evaluate (__LINE__, __func__, "", filter_passage_display_inline ({}));
    evaluate (__LINE__, __func__, "Genesis 10:2 | Exodus 777:777", filter_passage_display_inline ({ Passage ("", 1, 10, "2"), Passage ("", 2, 777, "777")}));
    evaluate (__LINE__, __func__, "Unknown 10:2 | Exodus 777:777", filter_passage_display_inline ({ Passage ("", -1, 10, "2"), Passage ("", 2, 777, "777")}));
  }
  // PassagesDisplayMultiline.
  {
    evaluate (__LINE__, __func__, "Genesis 10:2\n", filter_passage_display_multiline ({Passage ("", 1, 10, "2")}));
    evaluate (__LINE__, __func__, "", filter_passage_display_inline ({}));
    evaluate (__LINE__, __func__, "Genesis 10:2\nExodus 777:777\n", filter_passage_display_multiline ({ Passage ("", 1, 10, "2"), Passage ("", 2, 777, "777")}));
    evaluate (__LINE__, __func__, "Unknown 10:2\nExodus 777:777\n", filter_passage_display_multiline ({ Passage ("", -1, 10, "2"), Passage ("", 2, 777, "777")}));
  }
  // PassageInteger ()
  {
    Passage standard = Passage ("", 6, 4, "2");
    int numeric = filter_passage_to_integer (standard);
    Passage passage = filter_integer_to_passage (numeric);
    evaluate (__LINE__, __func__, true, standard.equal (passage));
  }
  // InterpretBookEnglish
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("Genesis"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 Corinthians"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Song of Solomon"));
    evaluate (__LINE__, __func__, 60, filter_passage_interpret_book ("I Peter"));
    evaluate (__LINE__, __func__, 63, filter_passage_interpret_book ("II John"));
    evaluate (__LINE__, __func__, 64, filter_passage_interpret_book ("III John"));
  }
  // InterpretBookUSFM
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("GEN"));
    evaluate (__LINE__, __func__, 67, filter_passage_interpret_book ("FRT"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1CO"));
  }
  // InterpretBookOSIS
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("Gen"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1Cor"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Song"));
  }
  // InterpretBookBibleWorks
  {
    evaluate (__LINE__, __func__, 1,  filter_passage_interpret_book ("Gen"));
    evaluate (__LINE__, __func__, 2,  filter_passage_interpret_book ("Exod"));
    evaluate (__LINE__, __func__, 3,  filter_passage_interpret_book ("Lev"));
    evaluate (__LINE__, __func__, 4,  filter_passage_interpret_book ("Num"));
    evaluate (__LINE__, __func__, 5,  filter_passage_interpret_book ("Deut"));
    evaluate (__LINE__, __func__, 6,  filter_passage_interpret_book ("Jos"));
    evaluate (__LINE__, __func__, 7,  filter_passage_interpret_book ("Jdg"));
    evaluate (__LINE__, __func__, 8,  filter_passage_interpret_book ("Ruth"));
    evaluate (__LINE__, __func__, 9,  filter_passage_interpret_book ("1 Sam"));
    evaluate (__LINE__, __func__, 10, filter_passage_interpret_book ("2 Sam"));
    evaluate (__LINE__, __func__, 11, filter_passage_interpret_book ("1 Ki"));
    evaluate (__LINE__, __func__, 12, filter_passage_interpret_book ("2 Ki"));
    evaluate (__LINE__, __func__, 13, filter_passage_interpret_book ("1 Chr"));
    evaluate (__LINE__, __func__, 14, filter_passage_interpret_book ("2 Chr"));
    evaluate (__LINE__, __func__, 15, filter_passage_interpret_book ("Ezr"));
    evaluate (__LINE__, __func__, 16, filter_passage_interpret_book ("Neh"));
    evaluate (__LINE__, __func__, 17, filter_passage_interpret_book ("Est"));
    evaluate (__LINE__, __func__, 18, filter_passage_interpret_book ("Job"));
    evaluate (__LINE__, __func__, 19, filter_passage_interpret_book ("Ps"));
    evaluate (__LINE__, __func__, 20, filter_passage_interpret_book ("Prov"));
    evaluate (__LINE__, __func__, 21, filter_passage_interpret_book ("Eccl"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Cant"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Sol"));
    evaluate (__LINE__, __func__, 23, filter_passage_interpret_book ("Isa"));
    evaluate (__LINE__, __func__, 24, filter_passage_interpret_book ("Jer"));
    evaluate (__LINE__, __func__, 25, filter_passage_interpret_book ("Lam"));
    evaluate (__LINE__, __func__, 26, filter_passage_interpret_book ("Ezek"));
    evaluate (__LINE__, __func__, 27, filter_passage_interpret_book ("Dan"));
    evaluate (__LINE__, __func__, 28, filter_passage_interpret_book ("Hos"));
    evaluate (__LINE__, __func__, 29, filter_passage_interpret_book ("Joel"));
    evaluate (__LINE__, __func__, 30, filter_passage_interpret_book ("Amos"));
    evaluate (__LINE__, __func__, 31, filter_passage_interpret_book ("Obad"));
    evaluate (__LINE__, __func__, 32, filter_passage_interpret_book ("Jon"));
    evaluate (__LINE__, __func__, 33, filter_passage_interpret_book ("Mic"));
    evaluate (__LINE__, __func__, 34, filter_passage_interpret_book ("Nah"));
    evaluate (__LINE__, __func__, 35, filter_passage_interpret_book ("Hab"));
    evaluate (__LINE__, __func__, 36, filter_passage_interpret_book ("Zeph"));
    evaluate (__LINE__, __func__, 37, filter_passage_interpret_book ("Hag"));
    evaluate (__LINE__, __func__, 38, filter_passage_interpret_book ("Zech"));
    evaluate (__LINE__, __func__, 39, filter_passage_interpret_book ("Mal"));
    evaluate (__LINE__, __func__, 40, filter_passage_interpret_book ("Matt"));
    evaluate (__LINE__, __func__, 41, filter_passage_interpret_book ("Mk"));
    evaluate (__LINE__, __func__, 42, filter_passage_interpret_book ("Lk"));
    evaluate (__LINE__, __func__, 43, filter_passage_interpret_book ("Jn"));
    evaluate (__LINE__, __func__, 44, filter_passage_interpret_book ("Acts"));
    evaluate (__LINE__, __func__, 45, filter_passage_interpret_book ("Rom"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 Co"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1Co"));
    evaluate (__LINE__, __func__, 47, filter_passage_interpret_book ("2 Co"));
    evaluate (__LINE__, __func__, 48, filter_passage_interpret_book ("Gal"));
    evaluate (__LINE__, __func__, 49, filter_passage_interpret_book ("Eph"));
    evaluate (__LINE__, __func__, 50, filter_passage_interpret_book ("Phil"));
    evaluate (__LINE__, __func__, 51, filter_passage_interpret_book ("Col"));
    evaluate (__LINE__, __func__, 52, filter_passage_interpret_book ("1 Thess"));
    evaluate (__LINE__, __func__, 53, filter_passage_interpret_book ("2 Thess"));
    evaluate (__LINE__, __func__, 54, filter_passage_interpret_book ("1 Tim"));
    evaluate (__LINE__, __func__, 55, filter_passage_interpret_book ("2 Tim"));
    evaluate (__LINE__, __func__, 56, filter_passage_interpret_book ("Tit"));
    evaluate (__LINE__, __func__, 57, filter_passage_interpret_book ("Phlm"));
    evaluate (__LINE__, __func__, 58, filter_passage_interpret_book ("Heb"));
    evaluate (__LINE__, __func__, 59, filter_passage_interpret_book ("Jas"));
    evaluate (__LINE__, __func__, 60, filter_passage_interpret_book ("1 Pet"));
    evaluate (__LINE__, __func__, 61, filter_passage_interpret_book ("2 Pet"));
    evaluate (__LINE__, __func__, 62, filter_passage_interpret_book ("1 Jn"));
    evaluate (__LINE__, __func__, 63, filter_passage_interpret_book ("2 Jn"));
    evaluate (__LINE__, __func__, 64, filter_passage_interpret_book ("3 Jn"));
    evaluate (__LINE__, __func__, 65, filter_passage_interpret_book ("Jude"));
    evaluate (__LINE__, __func__, 66, filter_passage_interpret_book ("Rev"));
  }
  // InterpretBookOnlineBibleAbbreviations.
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("Ge"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1Co"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("So"));
  }  
  // InterpretBookPartialNames.
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("G"));
    evaluate (__LINE__, __func__, 37, filter_passage_interpret_book ("g"));
    evaluate (__LINE__, __func__, 3, filter_passage_interpret_book ("ge"));
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("gene"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 Cori"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 cori"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 corint"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1cor"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("song"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("song of"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("song of sol"));
    evaluate (__LINE__, __func__, 11, filter_passage_interpret_book ("1ki"));
  }
  // Explode Passage
  {
    Passage passage = filter_passage_explode_passage ("Genesis 2:2");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
    
    passage = filter_passage_explode_passage ("1 Corinth. 2:2");
    evaluate (__LINE__, __func__, 46, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
    
    passage = filter_passage_explode_passage ("Song of Sol. 2:2");
    evaluate (__LINE__, __func__, 22, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
   
    passage = filter_passage_explode_passage ("Revelation 2:2");
    evaluate (__LINE__, __func__, 66, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
  }
  // InterpretPassage
  {
    Passage currentPassage = Passage ("", 2, 4, "6");

    Passage standard = Passage ("", 1, 2, "3");
    Passage output = filter_passage_interpret_passage (currentPassage, " Genesis 2 3");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 46, 2, "3");
    output = filter_passage_interpret_passage (currentPassage, "1  Corinthians 2:3");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 22, 2, "3");
    output = filter_passage_interpret_passage (currentPassage, "Song of Solomon 2.3");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = currentPassage;
    output = filter_passage_interpret_passage (currentPassage, "");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 1, 1, "1");
    output = filter_passage_interpret_passage (currentPassage, "Genesis Exodus");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 2, 1, "1");
    output = filter_passage_interpret_passage (currentPassage, "Exodus");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 2, 4, "11");
    output = filter_passage_interpret_passage (currentPassage, "11");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 2, 11, "14");
    output = filter_passage_interpret_passage (currentPassage, "11 14");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 22, 2, "1");
    output = filter_passage_interpret_passage (currentPassage, "Song of Solomon 2");
    evaluate (__LINE__, __func__, true, standard.equal (output));
  }
  // Sequence And Range None.
  {
    vector <string> standard = {"Exod. 30:4"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 30:4");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Sequence
  {
    vector <string> standard = {"Exod. 37:5", "14", "28"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 37:5, 14, 28");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Range
  {
    vector <string> standard = {"Exod. 37:5", "14", "15", "16"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 37:5, 14 - 16");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Sequence And Range One
  {
    vector <string> standard = {"Exod. 37:4", "5", "14", "15", "27", "28", "30", "40"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 37:4 - 5, 14 - 15, 27 - 28, 30, 40");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Sequence And Range Two
  {
    vector <string> standard = {"Exod. 25:13", "14", "27", "28"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 25:13-14, 27-28");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test ordered books.
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles;
    Database_State::create ();
    
    string bible = "php unit";

    // No ordering.
    database_bibles.createBible (bible);
    database_bibles.storeChapter (bible, 1, 1, "data");
    database_bibles.storeChapter (bible, 2, 1, "data");
    database_bibles.storeChapter (bible, 3, 1, "data");
    database_bibles.storeChapter (bible, 4, 1, "data");
    database_bibles.storeChapter (bible, 5, 1, "data");
    vector <int> books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 2, 3, 4, 5}, books);

    // Existing books re-ordered.
    Database_Config_Bible::setBookOrder (bible, "1 3 2 5 4");
    books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 3, 2, 5, 4}, books);

    // Some books ordered, and Bible has extra books: These are to be added to the end.
    Database_Config_Bible::setBookOrder (bible, "1 3 2");
    books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 3, 2, 4, 5}, books);

    // More books ordered than in Bible: Remove the extra ones.
    Database_Config_Bible::setBookOrder (bible, "1 3 2 5 4 6");
    books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 3, 2, 5, 4}, books);
  }
}


void test_filter_string_text2html ()
{
  trace_unit_tests (__func__);
  
  {
    string html = 
      "<p>author</p>\n"
      "<p>Text 1<div>Text 2</div><div>Text 3</div></p>";
    string plain = 
      "author\n"
      "Text 1\n"
      "Text 2\n"
      "Text 3";
    evaluate (__LINE__, __func__, plain, filter_string_html2text (html));
  }
  {
    string html =
      "<p>writer (15 Nov):</p>\n"
      "<p>Second note.<div>Second matter.</div><div>A second round is needed.</div></p>\n"
      "<p>Here is <b>bold</b>, and here is <i>italics</i>.<div>Here is <sup>superscript</sup>&nbsp;and here is <sub>subscript</sub>.</div><div style=\"text-align: center;\">Centered</div><div style=\"text-align: left;\">Left justified</div><div style=\"text-align: left;\"><ol><li>Numbered list.</li></ol>No numbered text.</div><div style=\"text-align: left;\">Link to <a href=\"http://google.nl\">http://google.nl</a>.</div><div style=\"text-align: left;\">Here follows an image:&nbsp;<img src=\"http://localhost/image\">.<br></div><h1>Header 1</h1><div>Normal text again below the header.</div></p>\n";
    string plain =
      "writer (15 Nov):\n"
      "Second note.\n"
      "Second matter.\n"
      "A second round is needed.\n"
      "Here is bold, and here is italics.\n"
      "Here is superscript and here is subscript.\n"
      "Centered\n"
      "Left justified\n"
      "Numbered list.\n"
      "No numbered text.\n"
      "Link to http://google.nl.\n"
      "Here follows an image: .\n"
      "Header 1\n"
      "Normal text again below the header.\n";
    evaluate (__LINE__, __func__, filter_string_trim (plain), filter_string_trim (filter_string_html2text (html)));
  }
  {
    string html =
      "test notes four\n"
      "\n"
      "Logbook:\n"
      "\n";
    string plain =
      "test notes fourLogbook:";
    evaluate (__LINE__, __func__, filter_string_trim (plain), filter_string_trim (filter_string_html2text (html)));
  }
  {
    string html =
"Line one.<BR>\n"
"\n"
"Line two.<BR>\n"
"\n"
"Line three.<BR>\n";
    string plain =
"Line one.\n"
"Line two.\n"
"Line three.\n";
    evaluate (__LINE__, __func__, filter_string_trim (plain), filter_string_trim (filter_string_html2text (html)));
  }
}


void test_email ()
{
  trace_unit_tests (__func__);
  
  evaluate (__LINE__, __func__, true, filter_url_email_is_valid ("user@web.site"));
  evaluate (__LINE__, __func__, false, filter_url_email_is_valid ("user@website"));
  evaluate (__LINE__, __func__, false, filter_url_email_is_valid (" user@web.site"));
  evaluate (__LINE__, __func__, false, filter_url_email_is_valid ("user @ web.site"));

  evaluate (__LINE__, __func__, "foo@bar.eu", filter_string_extract_email ("Foo Bar <foo@bar.eu>"));
  evaluate (__LINE__, __func__, "foo@bar.eu", filter_string_extract_email ("<foo@bar.eu>"));
  evaluate (__LINE__, __func__, "foo@bar.eu", filter_string_extract_email ("foo@bar.eu"));

  string body = "body";
  evaluate (__LINE__, __func__, body, filter_string_extract_body (body));

  body = 
    "\n"
    "test\n"
    "\n"
    "On Wed, 2011-03-02 at 08:26 +0100, Bibledit wrote:\n"
    "\n"
    "> test notes three\n"
    "\n"
    "\n"
    "> test\n"
    "\n"
    "On Wed, 2011-03-02 at 08:26 +0100, Bibledit wrote:\n"
    "\n"
    ">    test notes three \n";
  evaluate (__LINE__, __func__, "test", filter_string_extract_body (body, "2011", "Bibledit"));
}


void test_replace ()
{
  trace_unit_tests (__func__);
  
  evaluate (__LINE__, __func__, "ABXEFG", substr_replace ("ABCDEFG", "X", 2, 2));
  evaluate (__LINE__, __func__, "ABX", substr_replace ("ABCDEFG", "X", 2, 5));
  evaluate (__LINE__, __func__, "ABXG", substr_replace ("ABCDEFG", "X", 2, 4));
}


void test_styles_css ()
{
  trace_unit_tests (__func__);
  
  Webserver_Request request;
  Database_State database_state;
  database_state.create ();
  // Basic.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "basic.css"));
    evaluate (__LINE__, __func__, standard, css);
  }
  // Export.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.exports ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "exports.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
  // Editor.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.editor ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "editor.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
}


void test_filter_custom_css ()
{
  trace_unit_tests (__func__);
  
  // Direction
  {
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionUnspecified (100));
    evaluate (__LINE__, __func__, "", Filter_Css::directionUnspecified (101));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionLeftToRight (101));
    evaluate (__LINE__, __func__, "", Filter_Css::directionLeftToRight (102));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionRightToLeft (102));
    evaluate (__LINE__, __func__, "", Filter_Css::directionRightToLeft (103));
    
    evaluate (__LINE__, __func__, 0, Filter_Css::directionValue (""));
    evaluate (__LINE__, __func__, 1, Filter_Css::directionValue ("ltr"));
    evaluate (__LINE__, __func__, 2, Filter_Css::directionValue ("rtl"));
  }
  // Writing Mode
  {
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeUnspecified (102));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeUnspecified (112));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeTopBottomLeftRight (112));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeTopBottomLeftRight (122));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeTopBottomRightLeft (122));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeTopBottomRightLeft (132));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeBottomTopLeftRight (132));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeBottomTopLeftRight (142));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeBottomTopRightLeft (142));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeBottomTopRightLeft (152));
  }
  // Css
  {
    string css = Filter_Css::getCss ("class", "", 0);
    
    string standard =
    ".class\n"
    "{\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 101);
    standard =
    ".class\n"
    "{\n"
    "direction: ltr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 102);
    standard =
    ".class\n"
    "{\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 110);
    standard =
    ".class\n"
    "{\n"
    "writing-mode: tb-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("CLass", "", 130);
    standard =
    ".CLass\n"
    "{\n"
    "writing-mode: bt-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("CLass", "", 1322);
    standard =
    ".CLass\n"
    "{\n"
    "direction: rtl;\n"
    "writing-mode: tb-rl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("Class", "sherif", 0);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("Class", "sherif", 102);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("classs", "../font.ttf", 0);
    standard =
    "@font-face\n"
    "{\n"
    "font-family: classs;\n"
    "src: url(../font.ttf);\n"
    "}\n"
    ".classs\n"
    "{\n"
    "font-family: classs;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
  }
  // Class.
  {
    string clss = Filter_Css::getClass ("ആഈഘലറ");
    evaluate (__LINE__, __func__, "customf86528", clss);
  }
}


void test_filter_diff ()
{
  trace_unit_tests (__func__);
  
  // Diff 1
  {
    string output = filter_diff_diff ("Old text", "New text");
    string standard = "<span style=\"text-decoration: line-through;\">Old</span> <span style=\"font-weight: bold;\">New</span> text";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Diff 2
  {
    string output = filter_diff_diff ("this is really old text", "and this is new text");
    string standard = "<span style=\"font-weight: bold;\">and</span> this is <span style=\"text-decoration: line-through;\">really</span> <span style=\"text-decoration: line-through;\">old</span> <span style=\"font-weight: bold;\">new</span> text";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Diff with new lines in the text.
  {
    string oldtext =
    "Genesis 1.1 1 In the beginning God created the heavens and the earth.\n"
    "Genesis 1.2 2 And the earth was without form, and void; and darkness was upon the face of the deep. And the Spirit of God moved upon the face of the waters.\n"
    "Genesis 1.3 3 And God said, Let there be light: and there was light.\n";
    
    string newtext =
    "Genesis 1.1 1 In the beginning God created the heaven and the earth.\n"
    "Genesis 1.2 2 And the earth was without form and void and darkness was upon the face of the deep. And the Spirit of God moved upon the face of the waters.\n"
    "Genesis 1.3 3 And God said: \"Let there be light\". And there was light.\n";
    
    string output = filter_diff_diff (oldtext, newtext);
    
    string standard = filter_url_file_get_contents (filter_url_create_root_path ("unittests", "tests", "diff.txt"));
    evaluate (__LINE__, __func__, standard, output);
  }
  // Character similarity 1.
  {
    int similarity = filter_diff_character_similarity ("Old text", "New text");
    evaluate (__LINE__, __func__, 45, similarity);

    similarity = filter_diff_character_similarity ("New text", "New text");
    evaluate (__LINE__, __func__, 100, similarity);

    similarity = filter_diff_character_similarity ("ABCDEFGH", "IJKLMNOPQRST");
    evaluate (__LINE__, __func__, 0, similarity);
  }
  // Similarity with text that used to crash the routine but was fixed.
  {
    // Invalid UTF8 results in 0% similarity.
    string path = filter_url_create_root_path ("unittests", "tests");
    string oldtext = filter_url_file_get_contents (filter_url_create_path (path, "invalid-utf8-old.txt"));
    string newtext = filter_url_file_get_contents (filter_url_create_path (path, "invalid-utf8-new.txt"));
    int similarity = filter_diff_character_similarity (oldtext, newtext);
    evaluate (__LINE__, __func__, 0, similarity);
  }
  // Similarity.
  {
    string first =
    "\\c 29\n"
    "\\s Kugadzwa kwevapristi\n"
    "\\p\n"
    "\\v 1 Zvino ichi ndicho chinhu chauchavaitira kuvaita vatsvene, kuti vandishumire pabasa reupristi: Tora nzombe imwe diki; nemakondohwe maviri asina chaanopomerwa,\n"
    "\\v 2 nechingwa chisina kuviriswa, nemakeke asina kuviriswa akakanyiwa nemafuta, nezvingwa zvitete zvisina kuviriswa zvine mafuta; uchazviita neupfu hwakatsetseka hwegorosi.\n"
    "\\v 3 Zvino uchazviisa mudengu rimwe, ugopinda nazvo zviri mudengu, pamwe nenzombe diki nemakondohwe maviri.\n"
    "\\v 4 NaAroni nevanakomana vake uchauya navo kumukova wetende rekusangana, ugovashambidza nemvura;\n"
    "\\v 5 uchatorawo hanzu ugopfekedza Aroni nguvo refu, nejazi reefodhi, neefodhi, nechidzitiro chechipfuva; uye umusunge chiuno nebhanhire reefodhi rakarukwa neumhizha;\n"
    "\\v 6 uisewo ngundu pamusoro wake, nekuisa korona tsvene pangundu.\n"
    "\\v 7 Zvino uchatora mafuta ekuzodza, ugodira pamusoro wake, ugomuzodza.\n"
    "\\v 8 Nevanakomana vake uchavaswededza pedo ugovapfekedza majazi.\n"
    "\\v 9 Uchavasungawo zviuno nebhanhire, Aroni nevanakomana vake, uvasungirire nguwani; uye basa reupristi richava ravo chive chimiso nekusingaperi; uye uchaeresa Aroni nevanakomana vake.\n"
    "\\v 10 Zvino uchauisa nzombe pamberi petende rekusangana, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wenzombe.\n"
    "\\v 11 Zvino uchabaya nzombe pamberi paJehovha pamukova wetende rekusangana.\n"
    "\\v 12 Zvino uchatora ropa renzombe, ugoisa panyanga dzearitari nemunwe wako, ndokudururira ropa rese mujinga memuzasi mearitari:\n"
    "\\v 13 Zvino uchatora mafuta ese anofukidza zvemukati, nebutiro riri pamusoro pechiropa, neitsvo mbiri, nemafuta ari padziri uzvipise pamusoro pearitari.\n"
    "\\v 14 Asi nyama yenzombe, nedehwe rayo, nechizvizvi chayo, uchapisa nemoto kunze kwemisasa; chipiriso chezvivi.\n"
    "\\p\n"
    "\\v 15 Uchatorawo gondohwe rimwe, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wegondohwe,\n"
    "\\v 16 zvino uchauraya gondohwe ndokutora ropa raro ndokusasa kupoteredza aritari.\n"
    "\\v 17 Zvino uchaguranya gondohwe ndokusuka zvemukati zvaro nemakumbo aro ugoisa panhindi dzaro nepamusoro waro.\n"
    "\\v 18 Zvino uchapisa gondohwe rese paaritari; chipiriso chinopiswa kuna Jehovha, hwema hunonhuhwira, chipiriso chinoitirwa Jehovha, nemoto.\n"
    "\\v 19 Zvino uchatora gondohwe rechipiri, uye Aroni nevanakomana vake vagoisa maoko avo pamusoro pemusoro wegondohwe.\n"
    "\\v 20 Ipapo uchauraya gondohwe ndokutora paropa raro, ndokuisa pamucheto wezasi wenzeve yerudyi yaAroni, nepamicheto yezasi yenzeve dzerudyi dzevanakomana vake, nepagunwe reruoko rwerudyi, nepagumwe guru retsoka yavo yerudyi, nekusasawo ropa paaritari kupoteredza.\n"
    "\\v 21 Zvino uchatora paropa riri paaritari nepamafuta ekuzodza ugosasa pamusoro paAroni napanguvo dzake nepamusoro pevanakomana vake, nepanguvo dzevanakomana vake pamwe naye, agoitwa mutsvene iye nenguvo dzake nevanakomana vake nenguvo dzevanakomana vake pamwe naye.\n"
    "\\v 22 Zvino utorewo mamwe mafuta egondohwe, nebemhe raro, namafuta anofukidza ura, namafuta ari pamusoro pechiropa, netsvo mbiri, namafuta ari padziri, nebandauko rorudyi (nokuti igondohwe rokugadza naro),\n"
    "\\v 23 nebundu rimwe rechingwa, nechingwa chimwe chidiki chinamafuta, nechingwa chimwe chitete chinobva mudengu rezvingwa zvisina kuviriswa, zviri pamberi paJehovha.\n"
    "\\v 24 Zvino uzviise zvese pamaoko aAroni napamaoko avanakomana vake, vagozvizunguzira, chive chipo chinozunguzirwa pamberi paJehovha.\n"
    "\\v 25 Zvino uzvibvise pamaoko avo, uzvipise paaritari pamusoro pechipiriso chinopiswa, kuti chive chipo chinonhuhwira pamberi paJehovha; chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 26 Utorewo chityu chegondohwe rokugadza naro raAroni, urizunguzire pamberi paJehovha, kuti chive chipo chinozunguzirwa; ndiwo mugove wako.\n"
    "\\v 27 Zvino chityu chinozunguzirwa uchiite chitsvene, nebandauko rechipiriso chinosimudzwa, chinozunguzirwa nekusimudzwa, zvegondohwe rokugadza naro, kuti zvive zvaAroni nezvavanakomana vake.\n"
    "\\v 28 Izvozvo zvinofanira kupiwa Aroni navanakomana vake navana vaIsraeri nokusingaperi nokuti chipiriso chinosimudzwa, chinobva kuvana vaIsraeri pazvibairo zvezvipiriso zvavo zvokuyananisa, chive chipiriso chavo chinosimudzirwa Jehovha.\n"
    "\\p\n"
    "\\v 29 Nguvo dzaAroni dzichava dzavanakomana vake vanomutevera, kuti vazodzwe vakadzipfeka, vagadzwe vakadzipfeka.\n"
    "\\v 30 Mwanakomana wake anopinda paupristi panzvimbo yake, anofanira kudzipfeka mazuva manomwe, kana achipinda mutende rekusangana kushumira panzvimbo tsvene.\n"
    "\\p\n"
    "\\v 31 Zvino unofanira kutora gondohwe rokugadza naro, ubike nyama yaro panzvimbo tsvene.\n"
    "\\v 32 Aroni navanakomana vake ngavadye nyama yegondohwe, nechingwa chiri mudengu pamukova wetende rokusangana.\n"
    "\\v 33 Vanofanira kudya izvozvo zvaiyananiswa nazvo, pakugadzwa nokuitwa vatsvene kwavo; asi mweni ngaarege kuzvidya, nokuti zvitsvene.\n"
    "\\v 34 Kana imwe nyama yokugadza nayo ikasara, kana chimwe chingwa, kusvikira mangwana, unofanira kupisa zvasara nomoto; hazvifaniri kudyiwa, nokuti zvitsvene.\n"
    "\\p\n"
    "\\v 35 Unofanira kuitira Aroni navanakomana vake saizvozvo, zvese sezvandakuraira; uite mazuva manomwe uchivagadza.\n"
    "\\v 36 Zuva rimwe nerimwe unofanira kubaira nzombe yechipiriso chezvivi, chokuyananisa nacho; unofanira kunatsa aritari, kana uchiiyananisira; uizodze kuti uiite tsvene.\n"
    "\\v 37 Unofanira kuita mazuva manomwe pakuyananisira aritari, nokuiita tsvene; kuti aritari ive tsvene kwazvo, uye zvese zvinobata paaritari zvinofanira kuva zvitsvene.\n"
    "\\s Zvibairo zvezuva rimwe nerimwe\n"
    "\\p\n"
    "\\v 38 Zvino ndizvo zvaunofanira kubaira paaritari: Zuva rimwe nerimwe nguva dzese makwayana maviri.\n"
    "\\v 39 Rimwe gwayana unofanira kuribaira mangwanani, rimwe gwayana unofanira kuribaira madekwani.\n"
    "\\v 40 Pamwe chete negwayana rimwe unofanira kuisa chipiriso choupfu hwakatsetseka hwakasvika pachegumi cheefa hwakakanyiwa nechechina chehini yamafuta akasviniwa nechechina chehini yewaini, chive chipiriso chinodururwa.\n"
    "\\v 41 Rimwe gwayana uribaire madekwani, uriitire sezvawakaitira chipiriso choupfu chamangwanani, nezvawakaitira chipiriso charo chinomwiwa, kuti chive chipiriso chinonhuhwira, chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 42 Chinofanira kuva chipiriso chinopiswa nguva dzese pamarudzi enyu ese, pamukova wetende rekusangana pamberi paJehovha, pandichasangana nemi, nditaurepo newe.\n"
    "\\v 43 Ndipo pandichasangana nevana vaIsraeri, uye tende richaitwa dzvene nekubwinya kwangu.\n"
    "\\v 44 Tende rokusangana nearitari ndichazviita zvitsvene; uye Aroni navanakomana vake ndichavaita vatsvene, kuti vandishumire pabasa roupristi.\n"
    "\\v 45 Ndichagara pakati pavana vaIsraeri, kuti ndive Mwari wavo.\n"
    "\\v 46 Vachaziva kuti ndini Jehovha wavo, wakavabudisa panyika kuti ndigare pakati pavo; ndini Jehovha Mwari wavo.\n";
    
    string second =
    "\\c 29\n"
    "\\s Kugadzwa kwevapristi\n"
    "\\p\n"
    "\\v 1 Zvino ichi ndicho chinhu chauchavaitira kuvaita vatsvene, kuti vandishumire pabasa reupristi: Tora nzombe imwe diki; nemakondohwe maviri asina chaanopomerwa,\n"
    "\\v 2 nechingwa chisina kuviriswa, nemakeke asina kuviriswa akakanyiwa nemafuta, nezvingwa zvitete zvisina kuviriswa zvine mafuta; uchazviita neupfu hwakatsetseka hwegorosi.\n"
    "\\v 3 Zvino uchazviisa mudengu rimwe, ugopinda nazvo zviri mudengu, pamwe nenzombe diki nemakondohwe maviri.\n"
    "\\v 4 NaAroni nevanakomana vake uchauya navo kumukova wetende rekusangana, ugovashambidza nemvura;\n"
    "\\v 5 uchatorawo hanzu ugopfekedza Aroni nguvo refu, nejazi reefodhi, neefodhi, nechidzitiro chechipfuva; uye umusunge chiuno nebhanhire reefodhi rakarukwa neumhizha;\n"
    "\\v 6 uisewo ngundu pamusoro wake, nekuisa korona tsvene pangundu.\n"
    "\\v 7 Zvino uchatora mafuta ekuzodza, ugodira pamusoro wake, ugomuzodza.\n"
    "\\v 8 Nevanakomana vake uchavaswededza pedo ugovapfekedza majazi.\n"
    "\\v 9 Uchavasungawo zviuno nebhanhire, Aroni nevanakomana vake, uvasungirire nguwani; uye basa reupristi richava ravo chive chimiso nekusingaperi; uye uchaeresa Aroni nevanakomana vake.\n"
    "\\v 10 Zvino uchauisa nzombe pamberi petende rekusangana, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wenzombe.\n"
    "\\v 11 Zvino uchabaya nzombe pamberi paJehovha pamukova wetende rekusangana.\n"
    "\\v 12 Zvino uchatora ropa renzombe, ugoisa panyanga dzearitari nemunwe wako, ndokudururira ropa rese mujinga memuzasi mearitari:\n"
    "\\v 13 Zvino uchatora mafuta ese anofukidza zvemukati, nebutiro riri pamusoro pechiropa, neitsvo mbiri, nemafuta ari padziri uzvipise pamusoro pearitari.\n"
    "\\v 14 Asi nyama yenzombe, nedehwe rayo, nechizvizvi chayo, uchapisa nemoto kunze kwemisasa; chipiriso chezvivi.\n"
    "\\p\n"
    "\\v 15 Uchatorawo gondohwe rimwe, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wegondohwe,\n"
    "\\v 16 zvino uchauraya gondohwe ndokutora ropa raro ndokusasa kupoteredza aritari.\n"
    "\\v 17 Zvino uchaguranya gondohwe ndokusuka zvemukati zvaro nemakumbo aro ugoisa panhindi dzaro nepamusoro waro.\n"
    "\\v 18 Zvino uchapisa gondohwe rese paaritari; chipiriso chinopiswa kuna Jehovha, hwema hunonhuhwira, chipiriso chinoitirwa Jehovha, nemoto.\n"
    "\\v 19 Zvino uchatora gondohwe rechipiri, uye Aroni nevanakomana vake vagoisa maoko avo pamusoro pemusoro wegondohwe.\n"
    "\\v 20 Ipapo uchauraya gondohwe ndokutora paropa raro, ndokuisa pamucheto wezasi wenzeve yerudyi yaAroni, nepamicheto yezasi yenzeve dzerudyi dzevanakomana vake, nepagunwe reruoko rwerudyi, nepagumwe guru retsoka yavo yerudyi, nekusasawo ropa paaritari kupoteredza.\n"
    "\\v 21 Zvino uchatora paropa riri paaritari nepamafuta ekuzodza ugosasa pamusoro paAroni napanguvo dzake nepamusoro pevanakomana vake, nepanguvo dzevanakomana vake pamwe naye, agoitwa mutsvene iye nenguvo dzake nevanakomana vake nenguvo dzevanakomana vake pamwe naye.\n"
    "\\v 22 Zvino utorewo mamwe mafuta egondohwe, nebemhe raro, namafuta anofukidza ura, namafuta ari pamusoro pechiropa, netsvo mbiri, namafuta ari padziri, nebandauko rorudyi (nokuti igondohwe rokugadza naro),\n"
    "\\v 23 nebundu rimwe rechingwa, nechingwa chimwe chidiki chinamafuta, nechingwa chimwe chitete chinobva mudengu rezvingwa zvisina kuviriswa, zviri pamberi paJehovha.\n"
    "\\v 24 Zvino uzviise zvese pamaoko aAroni napamaoko avanakomana vake, vagozvizunguzira, chive chipo chinozunguzirwa pamberi paJehovha.\n"
    "\\v 25 Zvino uzvibvise pamaoko avo, uzvipise paaritari pamusoro pechipiriso chinopiswa, kuti chive chipo chinonhuhwira pamberi paJehovha; chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 26 Utorewo chityu chegondohwe rokugadza naro raAroni, urizunguzire pamberi paJehovha, kuti chive chipo chinozunguzirwa; ndiwo mugove wako.\n"
    "\\v 27 Zvino chityu chinozunguzirwa uchiite chitsvene, nebandauko rechipiriso chinosimudzwa, chinozunguzirwa nekusimudzwa, zvegondohwe rokugadza naro, kuti zvive zvaAroni nezvavanakomana vake.\n"
    "\\v 28 Izvozvo zvinofanira kupiwa Aroni navanakomana vake navana vaIsraeri nokusingaperi nokuti chipiriso chinosimudzwa, chinobva kuvana vaIsraeri pazvibairo zvezvipiriso zvavo zvokuyananisa, chive chipiriso chavo chinosimudzirwa Jehovha.\n"
    "\\p\n"
    "\\v 29 Nguvo dzaAroni dzichava dzavanakomana vake vanomutevera, kuti vazodzwe vakadzipfeka, vagadzwe vakadzipfeka.\n"
    "\\v 30 Mwanakomana wake anopinda paupristi panzvimbo yake, anofanira kudzipfeka mazuva manomwe, kana achipinda mutende rekusangana kushumira panzvimbo tsvene.\n"
    "\\p\n"
    "\\v 31 Zvino unofanira kutora gondohwe rokugadza naro, ubike nyama yaro panzvimbo tsvene.\n"
    "\\v 32 Aroni navanakomana vake ngavadye nyama yegondohwe, nechingwa chiri mudengu pamukova wetende rokusangana.\n"
    "\\v 33 Vanofanira kudya izvozvo zvaiyananiswa nazvo, pakugadzwa nokuitwa vatsvene kwavo; asi mweni ngaarege kuzvidya, nokuti zvitsvene.\n"
    "\\v 34 Kana imwe nyama yokugadza nayo ikasara, kana chimwe chingwa, kusvikira mangwana, unofanira kupisa zvasara nomoto; hazvifaniri kudyiwa, nokuti zvitsvene.\n"
    "\\p\n"
    "\\v 35 Unofanira kuitira Aroni navanakomana vake saizvozvo, zvese sezvandakuraira; uite mazuva manomwe uchivagadza.\n"
    "\\v 36 Zuva rimwe nerimwe unofanira kubaira nzombe yechipiriso chezvivi, chokuyananisa nacho; unofanira kunatsa aritari, kana uchiiyananisira; uizodze kuti uiite tsvene.\n"
    "\\v 37 Unofanira kuita mazuva manomwe pakuyananisira aritari, nokuiita tsvene; kuti aritari ive tsvene kwazvo, uye zvese zvinobata paaritari zvinofanira kuva zvitsvene.\n"
    "\\s Zvibairo zvezuva rimwe nerimwe\n"
    "\\p\n"
    "\\v 41 Rimwe gwayana uribaire madekwani, uriitire sezvawakaitira chipiriso choupfu chamangwanani, nezvawakaitira chipiriso charo chinomwiwa, kuti chive chipiriso chinonhuhwira, chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 42 Chinofanira kuva chipiriso chinopiswa nguva dzese pamarudzi enyu ese, pamukova wetende rekusangana pamberi paJehovha, pandichasangana nemi, nditaurepo newe.\n"
    "\\v 43 Ndipo pandichasangana nevana vaIsraeri, uye tende richaitwa dzvene nekubwinya kwangu.\n"
    "\\v 44 Tende rokusangana nearitari ndichazviita zvitsvene; uye Aroni navanakomana vake ndichavaita vatsvene, kuti vandishumire pabasa roupristi.\n"
    "\\v 45 Ndichagara pakati pavana vaIsraeri, kuti ndive Mwari wavo.\n"
    "\\v 46 Vachaziva kuti ndini Jehovha wavo, wakavabudisa panyika kuti ndigare pakati pavo; ndini Jehovha Mwari wavo.\n";
    int similarity = filter_diff_character_similarity (first, second);
    evaluate (__LINE__, __func__, 94, similarity);

    similarity = filter_diff_word_similarity (first, second);
    evaluate (__LINE__, __func__, 94, similarity);

    similarity = filter_diff_word_similarity ("one two three", "three two one");
    evaluate (__LINE__, __func__, 20, similarity);

    similarity = filter_diff_word_similarity ("one two three", "one two three four");
    evaluate (__LINE__, __func__, 75, similarity);
  }
  {
    refresh_sandbox (true);
    Webserver_Request request;
    Database_Modifications database_modifications;
    Database_State::create ();

    client_logic_enable_client (false);
    database_modifications.truncateTeams ();
    
    string temporary_folder = filter_url_tempfile ();
    filter_url_mkdir (temporary_folder);
    
    request.database_bibles()->createBible ("phpunit");
    bible_logic_store_chapter ("phpunit", 1, 2, "old chapter text");
    database_modifications.truncateTeams ();
    bible_logic_store_chapter ("phpunit", 1, 2, "new chapter text");

    filter_diff_produce_verse_level ("phpunit", temporary_folder);

    string path, standard, output;

    path = filter_url_create_path ("unittests", "tests", "verses_old.usfm");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_old.usfm");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
    
    path = filter_url_create_path ("unittests", "tests", "verses_new.usfm");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_new.usfm");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);

    path = filter_url_create_path ("unittests", "tests", "verses_old.txt");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_old.txt");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
    
    path = filter_url_create_path ("unittests", "tests", "verses_new.txt");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_new.txt");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
    
    string oldfile = filter_url_create_path (temporary_folder, "verses_old.usfm");
    string newfile = filter_url_create_path (temporary_folder, "verses_new.usfm");
    string outputfile = filter_url_create_path (temporary_folder, "changed_verses.html");
    filter_diff_run_file (oldfile, newfile, outputfile);

    path = filter_url_create_path ("unittests", "tests", "changed_verses.html");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "changed_verses.html");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
  }
}


void test_filter_abbreviations ()
{
  trace_unit_tests (__func__);
  
  // Read ()
  {
    string input =
    "Psalms = Ps.\n"
    "\n"
    "Exodus = Exod.\n"
    "\n"
    "Exodu = Exod.\n"
    "\n"
    "Psalms = Psa.\n"
    "\n"
    "Joshua =\n"
    "\n";
    vector <pair <int, string> > output = filter_abbreviations_read (input);
    vector <pair <int, string> > standard = { make_pair (19, "Ps."), make_pair (2, "Exod."), make_pair (19, "Psa.") };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Display ()
  {
    string input =
    "Psalms = Ps.\n"
    "\n"
    "Exodus = Exod.\n"
    "\n";
    string output = filter_abbreviations_display (input);
    string standard =
    "Genesis = \n"
    "Exodus = Exod.\n"
    "Leviticus = \n"
    "Numbers = \n"
    "Deuteronomy = \n"
    "Joshua = \n"
    "Judges = \n"
    "Ruth = \n"
    "1 Samuel = \n"
    "2 Samuel = \n"
    "1 Kings = \n"
    "2 Kings = \n"
    "1 Chronicles = \n"
    "2 Chronicles = \n"
    "Ezra = \n"
    "Nehemiah = \n"
    "Esther = \n"
    "Job = \n"
    "Psalms = Ps.\n"
    "Proverbs = \n"
    "Ecclesiastes = \n"
    "Song of Solomon = \n"
    "Isaiah = \n"
    "Jeremiah = \n"
    "Lamentations = \n"
    "Ezekiel = \n"
    "Daniel = \n"
    "Hosea = \n"
    "Joel = \n"
    "Amos = \n"
    "Obadiah = \n"
    "Jonah = \n"
    "Micah = \n"
    "Nahum = \n"
    "Habakkuk = \n"
    "Zephaniah = \n"
    "Haggai = \n"
    "Zechariah = \n"
    "Malachi = \n"
    "Matthew = \n"
    "Mark = \n"
    "Luke = \n"
    "John = \n"
    "Acts = \n"
    "Romans = \n"
    "1 Corinthians = \n"
    "2 Corinthians = \n"
    "Galatians = \n"
    "Ephesians = \n"
    "Philippians = \n"
    "Colossians = \n"
    "1 Thessalonians = \n"
    "2 Thessalonians = \n"
    "1 Timothy = \n"
    "2 Timothy = \n"
    "Titus = \n"
    "Philemon = \n"
    "Hebrews = \n"
    "James = \n"
    "1 Peter = \n"
    "2 Peter = \n"
    "1 John = \n"
    "2 John = \n"
    "3 John = \n"
    "Jude = \n"
    "Revelation = \n"
    "Front Matter = \n"
    "Back Matter = \n"
    "Other Material = ";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Read 
  {
    vector <pair <int, string> > input =
      { make_pair (1, "One"), make_pair (2, "Two"), make_pair (2, "Two."), make_pair (3, "3") };
    vector <pair <int, string> > output = filter_abbreviations_sort (input);
    vector <pair <int, string> > standard =
      { make_pair (2, "Two."), make_pair (1, "One"), make_pair (2, "Two"), make_pair (3, "3") };
    evaluate (__LINE__, __func__, standard, output);
  }
}


void test_filter_markup ()
{
  trace_unit_tests (__func__);
  
  {
    string text =
    "Zvino uchagadzira makumbo uye Makumbo uye maKumbo uye MAKUMBO emuakasia*, ndokuaputira negoridhe.\n"
    "Zvino uchaisa makumbo muzvindori panhivi dzeareka, kuti areka itakurwe nawo.\n"
    "Zvindori zvichava pamupendero kuti zvive nzvimbo dzemakumbo kutakura tafura.\n"
    "Zvino uchaita makumbo nematanda neMatanda nemaTANDA emuAkasia, ugoiputira negoridhe, kuti tafura itakurwe nawo.\n";
    vector <string> words = { "makumbo", "akasia", "matanda" };
    string result = filter_string_markup_words (words, text);
    string standard =
    "Zvino uchagadzira <mark>makumbo</mark> uye <mark>Makumbo</mark> uye <mark>maKumbo</mark> uye <mark>MAKUMBO</mark> emu<mark>akasia</mark>*, ndokuaputira negoridhe.\n"
    "Zvino uchaisa <mark>makumbo</mark> muzvindori panhivi dzeareka, kuti areka itakurwe nawo.\n"
    "Zvindori zvichava pamupendero kuti zvive nzvimbo dze<mark>makumbo</mark> kutakura tafura.\n"
    "Zvino uchaita <mark>makumbo</mark> ne<mark>matanda</mark> ne<mark>Matanda</mark> ne<mark>maTANDA</mark> emu<mark>Akasia</mark>, ugoiputira negoridhe, kuti tafura itakurwe nawo.\n";
    evaluate (__LINE__, __func__, standard, result);
  }
  {
    string text =
    "Zvino uchagadzira makumbo uye Makumbo uye maKumbo uye MAKUMBO emuakasia*, ndokuaputira negoridhe.\n"
    "Zvino uchaisa makumbo muzvindori panhivi dzeareka, kuti areka itakurwe nawo.\n"
    "Zvindori zvichava pamupendero kuti zvive nzvimbo dzemakumbo kutakura tafura.\n"
    "Zvino uchaita makumbo nematanda neMatanda nemaTANDA emuAkasia, ugoiputira negoridhe, kuti tafura itakurwe nawo.\n";
    vector <string> words;
    string result = filter_string_markup_words (words, text);
    evaluate (__LINE__, __func__, text, result);
  }
}


void test_filter_git_setup (Webserver_Request * request, string bible, string newbible,
                            string psalms_0_data, string psalms_11_data, string song_of_solomon_2_data)
{
#ifdef HAVE_CLIENT
  (void) request;
  (void) bible;
  (void) newbible;
  (void) psalms_0_data;
  (void) psalms_11_data;
  (void) song_of_solomon_2_data;
#endif
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  
  Database_State::create ();
  Database_Git::create ();
  Database_Login::create ();

  string repository = filter_git_directory (bible);
  string newrepository = filter_git_directory (newbible);

  filter_url_mkdir (repository);
  filter_url_mkdir (newrepository);
  
  request->remote_address = "127.0.0.1";
  request->user_agent = "unittest";
  request->database_users ()->create ();
  request->session_logic ()->setUsername ("unittest");
  request->database_bibles()->createBible (bible);

  bool result;
  result = filter_git_init (repository);
  evaluate (__LINE__, __func__, true, result);
  result = filter_git_init (newrepository);
  evaluate (__LINE__, __func__, true, result);
  
  filter_url_mkdir (filter_url_create_path (repository, "Psalms", "0"));
  filter_url_mkdir (filter_url_create_path (repository, "Psalms", "11"));
  filter_url_mkdir (filter_url_create_path (repository, "Song of Solomon", "2"));
  
  filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), psalms_0_data);
  filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "11", "data"), psalms_11_data);
  filter_url_file_put_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);

  filter_url_mkdir (filter_url_create_path (newrepository, "Psalms", "0"));
  filter_url_mkdir (filter_url_create_path (newrepository, "Psalms", "11"));
  filter_url_mkdir (filter_url_create_path (newrepository, "Song of Solomon", "2"));
  
  filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "0", "data"), psalms_0_data);
  filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "11", "data"), psalms_11_data);
  filter_url_file_put_contents (filter_url_create_path (newrepository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);
#endif
}


void test_filter_git ()
{
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  
  string bible = "localrepo";
  string newbible = "newlocalrepo";
  string repository = filter_git_directory (bible);
  string newrepository = filter_git_directory (newbible);
  string remoterepository = filter_git_directory ("remoterepo");
  string clonedrepository = filter_git_directory ("clonedrepo");
  Webserver_Request request;
  
  string psalms_0_data =
    "\\id PSA\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 UGWALO\n"
    "\\mt LWEZIHLABELELO";

  string psalms_11_data =
  "\\c 11\n"
  "\\s IN\\sc KOSI\\sc* iyisiphephelo sabaqotho\n"
  "\\d Kumqondisi wokuhlabelela. EsikaDavida\n"
  "\\p\n"
  "\\v 1 Ngithembela eN\\sc KOSI\\sc*ni\\x + Hlab. 25.2.\\x*. Lingatsho njani emphefumulweni wami: Balekela entabeni yenu \\add njeng\\add*enyoni\\x + 1 Sam. 23.14,19. 26.19,20.\\x*.\n"
  "\\v 2 Ngoba, khangela, ababi bayagobisa idandili\\x + Hlab. 7.12. Hlab. 64.4.\\x*, balungisa umtshoko wabo entanjeni\\x + Hlab. 7.12. 21.12.\\x*, ukuze batshoke emnyameni abaqotho ngenhliziyo\\x + Hlab. 7.10.\\x*.\n"
  "\\v 3 Nxa izisekelo zidilizwa\\x + Isa. 19.10. Hlab. 82.5. Hlab. 75.3.\\x*, angenzani olungileyo\\x + Jobe 22.13.\\x*?\n"
  "\\p\n"
  "\\v 4 IN\\sc KOSI\\x + Hab. 2.20.\\x*\\sc* isethempelini layo elingcwele\\x + Hlab. 5.7. Hlab. 150.1.\\x*; iN\\sc KOSI\\sc*, isihlalo sayo sobukhosi sisemazulwini\\x + Hlab. 2.4. 103.19. 115.3. 123.1. Isa. 66.1. Mat. 5.34. 23.22. Seb. 7.49. Isam. 4.2.\\x*; amehlo ayo ayakhangela\\x + Jobe 24.23. Hlab. 33.13. 34.15. 66.7. Hlab. 14.2. 102.19. 113.5,6.\\x*, inkophe zayo ziyahlola, abantwana babantu.\n"
  "\\v 5 IN\\sc KOSI\\sc* iyamhlola olungileyo, kodwa omubi lothanda ubudlwangudlwangu, umphefumulo wayo uyamzonda\\x + Gen. 22.1.\\x*.\n"
  "\\v 6 Uzanisa phezu kwababi imijibila, umlilo, lesolufa*\\x + Jobe 18.15.\\x*, lomoya otshisayo\\x + Hlab. 119.53. Lilo 5.10.\\x*, kuzakuba yisabelo senkezo yabo\\x + Hlab. 75.8. Jobe 21.20. Hlab. 16.5.\\x*.\n"
  "\\v 7 Ngoba ilungile iN\\sc KOSI\\sc*, iyathanda ukulunga\\x + Hlab. 33.5. 45.7. Hlab. 37.28. 146.8.\\x*; ubuso bayo buyabona oqotho\\x + Hlab. 33.18. Hlab. 17.2.\\x*.";

  string song_of_solomon_2_data =
  "\\c 2\n"
  "\\p\n"
  "\\v 1 Ngilirozi\\x + Isa. 35.1.\\x* leSharoni\\x + Josh. 12.18.\\x*, umduze wezigodi\\x + 2.16. 4.5. 5.13. 6.2,3. 7.2. 2 Lan. 4.5. Hos. 14.5. Hlab. 45.\\x*.\n"
  "\\p\n"
  "\\v 2 Njengomduze phakathi kwameva\\x + 2.16. 4.5. 5.13. 6.2,3. 7.2. 2 Lan. 4.5. Hos. 14.5. Hlab. 45.\\x*, unjalo umngane wami phakathi kwamadodakazi\\x + 1.15.\\x*.\n"
  "\\p\n"
  "\\v 3 Njengesihlahla sama-aphula phakathi kwezihlahla zeganga, sinjalo isithandwa sami phakathi kwamadodana\\x + Zaga 25.11.\\x*. Ngahlala emthunzini waso ngathokoza kakhulu\\x + Isa. 25.4. 32.2.\\x*, lesithelo saso simnandi ekunambitheni kwami\\x + Isam. 22.2.\\x*.\n"
  "\\v 4 Sangisa endlini yewayini, lesiboniso saso phezu kwami siluthando\\x + 1.4.\\x*.\n"
  "\\v 5 Ngisekelani\\x + Gen. 27.37.\\x* ngeziphiso zewayini\\x + 2 Sam. 6.19. 1 Lan. 16.3. Hos. 3.1.\\x*, lingiqinise ngama-aphula\\x + Zaga 25.11.\\x*, ngoba ngigul\\add isw\\add*a \\add lu\\add*thando\\x + 5.8.\\x*.\n"
  "\\v 6 Isandla saso sokhohlo singaphansi kwekhanda lami\\x + 8.3. Dute. 33.27.\\x*, lesokunene saso siyangigona\\x + 8.3. Dute. 33.27.\\x*.\n"
  "\\v 7 Ngiyalifungisa\\x + 3.5. 8.4. 5.8,9.\\x*, madodakazi eJerusalema\\x + 1.5.\\x*, ngemiziki\\x + 2.9,17.\\x*\\x + Zaga 6.5.\\x* kumbe ngezimpala zeganga\\x + 2.9,17.\\x*\\x + Zaga 5.19.\\x*, ukuze lingaphazamisi lingavusi uthando luze luthande.\n"
  "\\p\n"
  "\\v 8 Ilizwi lesithandwa sami! Khangela sona siyeza, siseqa phezu kwezintaba, siqolotsha phezu kwamaqaqa\\x + Isa. 52.7.\\x*.\n"
  "\\v 9 Isithandwa sami sinjengomziki\\x + 2.7,17. 8.14. 2 Sam. 22.34.\\x* kumbe njengethole lendluzele\\x + 2.7,17. 8.14. 2 Sam. 22.34.\\x*\\x + 4.5. 7.3.\\x*. Khangela simi ngemva komduli wethu, silunguza emawindini, sizibonakalisa\\f + \\fk zibonakalisa: \\fl Heb. \\fq hluma.\\f* eminxibeni yewindi\\x + Isa. 14.16.\\x*.\n"
  "\\v 10 Isithandwa sami saphendula sathi kimi\\x + Hlu. 18.14.\\x*: Vuka wena\\x + 2.13.\\x*, mngane wami, omuhle wami, a\\add si\\add*hambe wena\\x + 1.15.\\x*.\n"
  "\\v 11 Ngoba khangela, ubusika sebudlulile, izulu seliphelile, lihambile.\n"
  "\\v 12 Amaluba ayabonakala emhlabeni, isikhathi sokuhlabelela \\add se\\add*sifikile, lelizwi lejuba liyezwakala elizweni lakithi\\x + Hlab. 74.19. Jer. 8.7.\\x*.\n"
  "\\v 13 Isihlahla somkhiwa sivuthisa imikhiwa yaso eluhlaza, lamavini \\add ale\\add*mpoko\\x + 2.15. 7.12.\\x* aletha iphunga elimnandi\\x + Tshu. 7.1.\\x*. Vuka wena, mngane wami, omuhle wami wena, \\add si\\add*hambe\\x + 2.10.\\x*.\n"
  "\\p\n"
  "\\v 14 Juba lami\\x + 5.2. 6.9. 1.15. Mat. 10.16.\\x*, \\add elis\\add*engoxweni yedwala\\x + Jer. 48.28.\\x*\\x + Jer. 49.16. Obad. 3.\\x*, ekusithekeni kweliwa\\x + Hez. 38.20.\\x*, ngitshengisa ubuso bakho, ngizwise ilizwi lakho\\x + 8.13.\\x*, ngoba ilizwi lakho limnandi, lobuso bakho buyabukeka\\x + 1.5.\\x*.\n"
  "\\v 15 Sibambeleni amakhanka, amakhanka amancinyane, ona izivini\\x + Hez. 13.4. Luka 13.32.\\x*, ngoba izivini zethu \\add zile\\add*zimpoko\\x + 2.15. 7.12.\\x*.\n"
  "\\v 16 Isithandwa sami ngesami, lami ngingowaso\\x + 6.3. 7.10.\\x*, eselusa phakathi kwemiduze\\x + 2.1. 4.5. 6.3.\\x*.\n"
  "\\v 17 Kuze kube semadabukakusa, lamathunzi abaleke\\x + 4.6.\\x*, phenduka, sithandwa sami, ube njengomziki kumbe njengethole lendluzele\\x + 8.14. 2.9.\\x* phezu kwezintaba zeBhetheri\\x + 2 Sam. 2.29.\\x*.";

  // Sync Bible To Git 1
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));
    
    request.database_bibles()->storeChapter (bible, 2, 1, song_of_solomon_2_data);
    filter_git_sync_bible_to_git (&request, bible, repository);

    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));

    // Remove generated journal entries.
    refresh_sandbox (false);
  }

  // Sync Bible To Git 2
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);

    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));
    
    request.database_bibles()->storeChapter (bible, 19, 1, song_of_solomon_2_data);
    filter_git_sync_bible_to_git (&request, bible, repository);

    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "1", "data")));
    
    string data = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "1", "data"));
    evaluate (__LINE__, __func__, song_of_solomon_2_data, data);
    
    // Remove generated journal entries.
    refresh_sandbox (false);
  }
  
  // Sync Bible To Git 3
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));

    request.database_bibles()->storeChapter (bible, 19, 1, song_of_solomon_2_data);
    request.database_bibles()->storeChapter (bible, 22, 2, psalms_11_data);
    request.database_bibles()->storeChapter (bible, 19, 11, song_of_solomon_2_data);
    filter_git_sync_bible_to_git (&request, bible, repository);
    
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "1", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));

    string data = filter_url_file_get_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"));
    evaluate (__LINE__, __func__, psalms_11_data, data);
    
    data = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "11", "data"));
    evaluate (__LINE__, __func__, song_of_solomon_2_data, data);
    
    data = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "1", "data"));
    evaluate (__LINE__, __func__, song_of_solomon_2_data, data);
    
    // Remove generated journal entries.
    refresh_sandbox (false);
  }
  
  // Sync Git To Bible Add Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // The Bible has been created, but has no data yet.
    // Run the filter, and check that all three chapters are now in the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19, 22}, books);
    // Check that the data matches.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, song_of_solomon_2_data, usfm);
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git To Bible Delete Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    // Remove one book and one chapter from the git repository,
    // and check that after running the filter, the database is updated accordingly.
    filter_url_rmdir (repository + "/Song of Solomon");
    filter_url_rmdir (repository + "/Psalms/0");
    filter_git_sync_git_to_bible (&request, repository, bible);
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19}, books);
    // Check that the data matches.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "", usfm);
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git To Bible Update Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    // Update some chapters in the git repository,
    // and check that after running the filter, the database is updated accordingly.
    filter_url_file_put_contents (repository + "/Psalms/11/data", "\\c 11");
    filter_url_file_put_contents (repository + "/Song of Solomon/2/data", "\\c 2");
    filter_git_sync_git_to_bible (&request, repository, bible);
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, "\\c 11", usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "\\c 2", usfm);
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git Chapter To Bible Add Chapters ()
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // The Bible has been created, but has no data yet.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "", usfm);
    
    // Run the filter for each chapter, and check that all three chapters make it into the database.
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 0);
    usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 11);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    
    filter_git_sync_git_chapter_to_bible (repository, bible, 22, 2);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, song_of_solomon_2_data, usfm);
    
    // Check the two books are there.
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19, 22}, books);

    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git Chapter To Bible Delete Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    
    // Remove one book and one chapter from the git repository,
    filter_url_rmdir (repository + "/Song of Solomon");
    filter_url_rmdir (repository + "/Psalms/0");
    
    // Run updates on the three chapters.
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 0);
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 11);
    filter_git_sync_git_chapter_to_bible (repository, bible, 22, 2);
    
    // There should still be two books, although one book would have no chapters.
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19, 22}, books);
    
    // Check that the chapter data matches.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "", usfm);

    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git Chapter To Bible Update Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the Bible database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    
    // Update some chapters in the git repository.
    filter_url_file_put_contents (repository + "/Psalms/11/data", "\\c 11");
    filter_url_file_put_contents (repository + "/Song of Solomon/2/data", "\\c 2");
    
    // Run updates on the two chapters.
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 11);
    filter_git_sync_git_chapter_to_bible (repository, bible, 22, 2);
    
    // Check that the database is updated accordingly.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, "\\c 11", usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "\\c 2", usfm);
    
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }

  // Setting values in the configuration.
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    filter_git_config_set_bool (repository, "foo.bar", false);
    filter_git_config_set_int (repository, "bar.baz", 11);
    string path = filter_url_create_path (repository, ".git", "config");
    string contents = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, true, contents.find ("[foo]") != string::npos);
    evaluate (__LINE__, __func__, true, contents.find ("[bar]") != string::npos);
    evaluate (__LINE__, __func__, true, contents.find ("bar = false") != string::npos);
    evaluate (__LINE__, __func__, true, contents.find ("baz = 11") != string::npos);
    refresh_sandbox (false);
  }

  // Test of basic git operations in combination with a remote repository.
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    string error;
    bool success;
    string remoteurl = "file://" + remoterepository;
    vector <string> messages;
    
    // Create bare remote reository.
    filter_url_mkdir (remoterepository);
    filter_git_init (remoterepository, true);
    
    // Test read access to the remote repository.
    success = filter_git_remote_read (remoterepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Test cloning the repository.
    success = filter_git_remote_clone (remoteurl, clonedrepository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Store some Bible data in the cloned repository.
    filter_url_mkdir (filter_url_create_path (clonedrepository, "Psalms", "0"));
    filter_url_file_put_contents (filter_url_create_path (clonedrepository, "Psalms", "0", "data"), psalms_0_data);
    filter_url_mkdir (filter_url_create_path (clonedrepository, "Psalms", "11"));
    filter_url_file_put_contents (filter_url_create_path (clonedrepository, "Psalms", "11", "data"), psalms_11_data);
    filter_url_mkdir (filter_url_create_path (clonedrepository, "Song of Solomon", "2"));
    filter_url_file_put_contents (filter_url_create_path (clonedrepository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);

    // Add the Bible data to the git index.
    success = filter_git_add_remove_all (clonedrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Commit the index to the repository.
    success = filter_git_commit (clonedrepository, "username", "unittest", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Remove some Bible data from the cloned repository.
    filter_url_rmdir (filter_url_create_path (clonedrepository, "Psalms"));
    success = filter_git_add_remove_all (clonedrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Commit the index to the repository.
    success = filter_git_commit (clonedrepository, "username", "unittest", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Push to the remote repository.
    success = filter_git_push (clonedrepository, messages);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, 2, (int)messages.size());

    // Pull from remote repository.
    success = filter_git_pull (clonedrepository, messages);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, {"Already up-to-date."}, messages);

    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Get Git Passage
  {
    Passage passage = filter_git_get_passage ("From https://github.com/joe/test");
    evaluate (__LINE__, __func__, 0, passage.book);

    passage = filter_git_get_passage ("   443579b..90dcb57  master     -> origin/master");
    evaluate (__LINE__, __func__, 0, passage.book);

    passage = filter_git_get_passage ("Updating 443579b..90dcb57");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage ("Fast-forward");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage (" Genesis/3/data | 2 +-");
    Passage standard = Passage ("", 1, 3, "");
    evaluate (__LINE__, __func__, true, standard.equal (passage));
    
    passage = filter_git_get_passage (" 1 file changed, 1 insertion(+), 1 deletion(-)");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage (" delete mode 100644 Leviticus/1/data");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage (" Revelation/3/data | 2 +-");
    standard = Passage ("", 66, 3, "");
    evaluate (__LINE__, __func__, true, standard.equal (passage));
    
    passage = filter_git_get_passage ("	modified:   Exodus/3/data");
    standard = Passage ("", 2, 3, "");
    evaluate (__LINE__, __func__, true, standard.equal (passage));
  }
  
  // Exercise the "git status" filter.
  {
    // Refresh the repository, and store three chapters in it.
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);

    vector <string> paths;

    // There should be three modified paths.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "Psalms/") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "Song of Solomon/") != paths.end());

    // Add the files to the index.
    string error;
    vector <string> messages;
    filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, "", error);

    // There should still be three paths.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "new file:   Psalms/0/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "new file:   Psalms/11/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "new file:   Song of Solomon/2/data") != paths.end());
    
    // Commit the index.
    filter_git_commit (repository, "user", "unittest", messages, error);
    evaluate (__LINE__, __func__, "", error);

    // There should be no modified paths now.
    paths = filter_git_status (repository);
    evaluate (__LINE__, __func__, {"On branch master", "nothing to commit, working tree clean"}, paths);

    // Remove both Psalms chapters.
    filter_url_rmdir (filter_url_create_path (repository, "Psalms"));

    // There should be two modified paths now.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/0/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/11/data") != paths.end());

    // Add / remove the files to the index.
    filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, "", error);
    
    // There should still be two paths now.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/0/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/11/data") != paths.end());
    
    // Commit the index.
    filter_git_commit (repository, "user", "unittest", messages, error);
    evaluate (__LINE__, __func__, "", error);
    
    // There should be no modified paths now.
    paths = filter_git_status (repository);
    evaluate (__LINE__, __func__, {"On branch master", "nothing to commit, working tree clean"}, paths);

    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Test git's internal conflict resolution.
  {
    refresh_sandbox (true);
    string error;
    bool success;
    vector <string> messages;

    // Create remote repository.
    filter_url_mkdir (remoterepository);
    filter_git_init (remoterepository, true);
    string remoteurl = "file://" + remoterepository;

    // Clone the remote repository.
    success = filter_git_remote_clone (remoteurl, repository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Store three chapters in the local repository and push it to the remote repository.
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "0"));
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "11"));
    filter_url_mkdir (filter_url_create_path (repository, "Song of Solomon", "2"));
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), psalms_0_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "11", "data"), psalms_11_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (repository, messages, true);
    evaluate (__LINE__, __func__, true, success);

    // Clone the remote repository to a new local repository.
    success = filter_git_remote_clone (remoteurl, newrepository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Set the stage for a conflict that git itself cannot merge:
    // Change something in the new repository, push it to the remote.
    string newcontents =
    "\\id PSA\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 THE BOOK\n"
    "\\mt OF PSALMS\n";
    filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "0", "data"), newcontents);
    success = filter_git_add_remove_all (newrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (newrepository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (newrepository, messages, true);
    // Change something in the repository, and pull from remote:
    // Git fails to merge by itself.
    string contents =
    "\\id PSALM\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 UGWALO\n"
    "\\mt LWEZIHLABELELO\n";
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), contents);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_pull (repository, messages);
    evaluate (__LINE__, __func__, false, success);
    success = find (messages.begin(), messages.end(), "Auto-merging Psalms/0/data") != messages.end();
    evaluate (__LINE__, __func__, true, success);
    success = find (messages.begin(), messages.end(), "CONFLICT (content): Merge conflict in Psalms/0/data") != messages.end();
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_push (repository, messages);
    evaluate (__LINE__, __func__, false, success);
    vector <string> paths = { "Psalms/0/data" };
    success = filter_git_resolve_conflicts (repository, paths, error);
    evaluate (__LINE__, __func__, "", error);
    // Check the merge result.
    string standard =
    "\\id PSALM\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 THE BOOK\n"
    "\\mt OF PSALMS";
    contents = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "0", "data"));
    evaluate (__LINE__, __func__, standard, contents);
    
    // Remove journal entries.
    refresh_sandbox (false);
  }

  {
    refresh_sandbox (true);
    string error;
    bool success;
    vector <string> messages;
    
    // Create remote repository.
    filter_url_mkdir (remoterepository);
    filter_git_init (remoterepository, true);
    string remoteurl = "file://" + remoterepository;
    
    // Clone the remote repository.
    success = filter_git_remote_clone (remoteurl, repository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Store three chapters in the local repository and push it to the remote repository.
    psalms_0_data =
    "Line one one one\n"
    "Line two two two\n"
    "Line three three three\n";
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "0"));
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "11"));
    filter_url_mkdir (filter_url_create_path (repository, "Song of Solomon", "2"));
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), psalms_0_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "11", "data"), psalms_11_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (repository, messages, true);
    evaluate (__LINE__, __func__, true, success);
    
    // Clone the remote repository to a new local repository.
    success = filter_git_remote_clone (remoteurl, newrepository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Set the stage for a conflict that git itself can merge:
    // Change something in the new repository, push it to the remote.
    string newcontents =
    "Line 1 one one\n"
    "Line two two two\n"
    "Line three three three\n";
    filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "0", "data"), newcontents);
    success = filter_git_add_remove_all (newrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (newrepository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (newrepository, messages, true);
    // Change something in the repository, and pull from remote:
    // Git fails to merge by itself.
    string contents =
    "Line one one 1 one\n"
    "Line two 2 two 2 two\n"
    "Line three 3 three 3 three\n";
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), contents);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    // Pulling changes should result in a merge conflict.
    success = filter_git_pull (repository, messages);
    evaluate (__LINE__, __func__, false, success);

    // Resolve the conflict.
    success = filter_git_resolve_conflicts (repository, messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    evaluate (__LINE__, __func__, {"Psalms/0/data"}, messages);

    // Verify the resolved contents on correctness.
    contents = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "0", "data"));
    string standard =
    "Line 1 one 1 one\n"
    "Line two 2 two 2 two\n"
    "Line three 3 three 3 three";
    evaluate (__LINE__, __func__, standard, contents);

    // The status still displays the file as in conflict.
    // messages = filter_git_status (repository);
    // evaluate (__LINE__, __func__, {"Psalms/0/data"}, messages);

    // Commit and push the result.
    success = filter_git_commit (repository, "", "message", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, 4, (int)messages.size());
    success = filter_git_push (repository, messages);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, 2, (int)messages.size());
    
    // Status up-to-date.
    messages = filter_git_status (repository);
    evaluate (__LINE__, __func__, 3, (int)messages.size ());

    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Test one user saving Bible data in an uninterrupted sequence, that it leads to correct records in git.
  {
    refresh_sandbox (true);

    string error;
    bool success;
    vector <string> messages;

    test_filter_git_setup (&request, bible, newbible, "Psalm 0\n", "Psalm 11\n", "Song of Solomon 2\n");
    
    string repository = filter_git_directory (bible);

    // Commit the data to the repository.
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_commit (repository, "", "initial commit", messages, error);
    evaluate (__LINE__, __func__, true, success);

    int psalms = 19;
    string user1 = "user1";
    string user2 = "user2";
    string oldusfm1, newusfm1;
    string out_err;

    // Create records of user saving data.
    oldusfm1 = "Psalm 11\n";
    newusfm1 = oldusfm1 + "Praise";
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    oldusfm1 = newusfm1;
    newusfm1.append (" Jesus");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    oldusfm1 = newusfm1;
    newusfm1.append (" forever.\n");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    filter_git_sync_modifications_to_git (bible, repository);

    // Check the diff.
    filter_shell_run ("cd " + repository + " && git log -p", out_err);
    evaluate (__LINE__, __func__, true, out_err.find ("+Praise Jesus forever.") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("Author: user1 <bibledit@bibledit.org>") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("User modification") != string::npos);
    
    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Test one user saving Bible data, but this time the sequence is interrupted by undefined other users.
  {
    refresh_sandbox (true);
    
    string error;
    bool success;
    vector <string> messages;
    
    test_filter_git_setup (&request, bible, newbible, "Psalm 0\n", "Psalm 11\n", "Song of Solomon 2\n");
    
    string repository = filter_git_directory (bible);
    
    // Commit the data to the repository.
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_commit (repository, "", "initial commit", messages, error);
    evaluate (__LINE__, __func__, true, success);
    
    int psalms = 19;
    string user1 = "user1";
    string user2 = "user2";
    string oldusfm1, newusfm1;
    string out_err;

    // Create records of two users saving data.
    oldusfm1 = "Psalm 11\n";
    newusfm1 = oldusfm1 + "Praise";
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    oldusfm1 = newusfm1;
    newusfm1.append (" Jesus");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    Database_Git::store_chapter (user2, bible, psalms, 11, oldusfm1 + " xx\n", newusfm1 + " xxx\n");
    oldusfm1 = newusfm1;
    newusfm1.append (" forever.\n");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    filter_git_sync_modifications_to_git (bible, repository);
    filter_shell_run ("cd " + repository + " && git log -p", out_err);
    evaluate (__LINE__, __func__, true, out_err.find ("+Praise Jesus forever.") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("Author: user1 <bibledit@bibledit.org>") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("Author: user2 <bibledit@bibledit.org>") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("User modification") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("System-generated to clearly display user modification in next commit") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("+Praise Jesus xxx") != string::npos);
    
    // Remove journal entries.
    refresh_sandbox (false);
  }
#endif
}


void test_filter_merge ()
{
  trace_unit_tests (__func__);
  
  // Test Line Merge Simple Modifications.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n";
    string userModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka kukaJesu\n";
    string serverModificationData =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka kukaJesu";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Line Merge Equal Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n";
    string userModificationData =
    "\\c 28\n"
    "\\s Ukuvuka kukaJesu\n"
    "\\s Ukuvuka kukaJesu\n";
    string serverModificationData =
    "\\c 28\n"
    "\\s Ukuvuka kukaJesu\n"
    "\\s Ukuvuka kukaJesu\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 28\n"
    "\\s Ukuvuka kukaJesu\n"
    "\\s Ukuvuka kukaJesu";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Line Merge Multiple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\pp\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Marko 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya osukwini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesematho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65-66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\pp\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Marko 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya osukwini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesematho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65-66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Word Merge Simple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Word Merge Conflicting Modifications.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 1, conflicts.size ());
  }
  
  // Test Word Merge Multiple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakaliswa kwaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, njalo isembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakaliswa kwaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Hlu. 13.6.\\x*, njalo isembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Test Grapheme Merge Simple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela besabe baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela besabe baba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Test Conflict Take Server.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba; baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba; baba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 1, conflicts.size ());
  }

  // Realistic merge example
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string userModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string serverModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth verse.\n"
    "\\v 5\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first verse.\n"
    "\\v 2 And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth verse.\n"
    "\\v 5";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Merge situation taken from real life.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string path;
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_base.usfm");
    string mergeBaseData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_modification.usfm");
    string userModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_server.usfm");
    string serverModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_result.usfm");
    string standard = filter_url_file_get_contents (path);

    string output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());

    conflicts.clear ();
    output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, true, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Testing the clever merge routine on chapter 0.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\id GEN\n"
    "\\p Some text one.\n";
    string userModificationData =
    "\\id GEN\n"
    "\\p Some text two.\n";
    string serverModificationData =
    "\\id GEN\n"
    "\\p Some text one.\n";
    string output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    string standard =
    "\\id GEN\n"
    "\\p Some text two.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
    
    conflicts.clear ();
    output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, true, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Testing switching from separate verses into a combined verse.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string userModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string serverModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
   
    conflicts.clear ();
    output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }

  // Testing switching from a combined verse to separate verses.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string userModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string serverModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
    
    conflicts.clear ();
    output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Merge situation taken from real life.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string path;
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_base.usfm");
    string mergeBaseData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_modification.usfm");
    string userModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_server.usfm");
    string serverModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_result.usfm");
    string standard = filter_url_file_get_contents (path);
    
    string output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 3, conflicts.size ());
    
    conflicts.clear ();
    output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, true, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 3, conflicts.size ());
  }
}


void test_filter_tidy ()
{
  trace_unit_tests (__func__);
  
  string folder = filter_url_create_root_path ("unittests", "tests");
  string html = filter_url_file_get_contents (filter_url_create_path (folder, "/biblehub-john-1-1.html"));
  vector <string> tidy = filter_string_explode (html_tidy (html), '\n');
  evaluate (__LINE__, __func__, 752, (int)tidy.size());
}


void test_ipc_notes ()
{
  trace_unit_tests (__func__);
  
  // Initialize.
  refresh_sandbox (true);
  Webserver_Request request;
  request.database_users ()->create ();
  request.session_logic ()->setUsername ("phpunit");

  // There should be no note identifier.
  int identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);

  // Test opening note.
  Ipc_Notes::open (&request, 123456789);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 123456789, identifier);
  
  // Test trimming.
  request.database_ipc()->trim ();
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 123456789, identifier);

  // Test deleting note once.
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);

  // Test deleting two notes.
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);
}


void test_filter_date ()
{
  trace_unit_tests (__func__);
  
  // First Business Day Of Month
  {
    // Sunday the 1st.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (1, 0));
    // Monday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 1));
    // Tuesday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 2));
    // Wednesday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 3));
    // Thirsday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 4));
    // Friday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 5));
    // Saturday the 1st.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (1, 6));
    // Sunday the 2nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (2, 0));
    // Monday the 2nd.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (2, 1));
    // Tuesday the 2nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (2, 2));
    // Sunday the 3nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (3, 0));
    // Monday the 3nd.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (3, 1));
    // Tuesday the 3nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (3, 2));
    // Sunday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 0));
    // Monday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 1));
    // Tuesday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 2));
  }
  // Last Business Day Of Month
  {
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2013, 9));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2013, 10));
    evaluate (__LINE__, __func__, 29, filter_date_get_last_business_day_of_month (2013, 11));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2013, 12));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2014, 1));
    evaluate (__LINE__, __func__, 28, filter_date_get_last_business_day_of_month (2014, 2));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2014, 3));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 4));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 5));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 6));
  }
  // Is Business Day
  {
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 1));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 2));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 3));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 4));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 5));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 6));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 7));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 8));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 30));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2015, 3, 1));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2015, 2, 32));
    
  }
  // Seonds since Unix epoch.
  {
    int year, month, day, seconds;

    year = 2011;
    month = 2;
    day = 5;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));

    year = 2015;
    month = 3;
    day = 15;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));
    
    year = 2030;
    month = 12;
    day = 31;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));
  }
}


void test_filter_string ()
{
  trace_unit_tests (__func__);
  
  {
    string input = "<span>Praise the LORD&#xB6;, all &amp; you nations</span>";
    string output = convert_xml_character_entities_to_characters (input);
    string standard = filter_string_str_replace ("&#xB6;", "¶", input);
    evaluate (__LINE__, __func__, standard, output);

    input = "<span>Praise the LORD &#x5D0; all you nations</span>";
    output = convert_xml_character_entities_to_characters (input);
    standard = filter_string_str_replace ("&#x5D0;", "א", input);
    evaluate (__LINE__, __func__, standard, output);
    
    input = "Username";
    output = encrypt_decrypt ("key", input);
    output = encrypt_decrypt ("key", output);
    evaluate (__LINE__, __func__, input, output);

    input = "בְּרֵאשִׁ֖ית בָּרָ֣א אֱלֹהִ֑ים אֵ֥ת הַשָּׁמַ֖יִם וְאֵ֥ת הָאָֽרֶץ";
    output = encrypt_decrypt ("בְּרֵאשִׁ֖ית", input);
    output = encrypt_decrypt ("בְּרֵאשִׁ֖ית", output);
    evaluate (__LINE__, __func__, input, output);
  }

  {
    string one = get_new_random_string ();
    this_thread::sleep_for (chrono::milliseconds (10));
    string two = get_new_random_string ();
    evaluate (__LINE__, __func__, 32, one.length ());
    evaluate (__LINE__, __func__, true, one != two);
  }

  {
    evaluate (__LINE__, __func__, 4, (int)unicode_string_length ("test"));
    evaluate (__LINE__, __func__, 4, (int)unicode_string_length ("ᨁᨃᨅᨕ"));
  }

  {
    string hebrew = "אָבּגּדּהּ";
    evaluate (__LINE__, __func__, "st1234", unicode_string_substr ("test1234", 2));
    evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2));
    evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 5));
    evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 6));
    evaluate (__LINE__, __func__, "test", unicode_string_substr ("test123456", 0, 4));
    evaluate (__LINE__, __func__, "12", unicode_string_substr ("test123456", 4, 2));
    evaluate (__LINE__, __func__, "גּדּ", unicode_string_substr (hebrew, 2, 2));
    evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2, 10));
  }
  
  {
    string hebrew = "אָבּגּדּהּ";
    string needle = "דּ";
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos ("012345", "3"));
    evaluate (__LINE__, __func__, 5, (int)unicode_string_strpos ("012345", "5"));
    evaluate (__LINE__, __func__, 0, (int)unicode_string_strpos ("012345", "0"));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos ("012345", "6"));
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos (hebrew, needle));
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos (hebrew, needle, 3));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos (hebrew, needle, 4));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos ("", "3"));
  }
  
  {
    evaluate (__LINE__, __func__, 2, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "c"));
    evaluate (__LINE__, __func__, 2, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "cD"));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "ce"));
  }
  
  {
    evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("test1234"));
    evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("TEST1234"));
    evaluate (__LINE__, __func__, "θεος", unicode_string_casefold ("Θεος"));
    evaluate (__LINE__, __func__, "α α β β", unicode_string_casefold ("Α α Β β"));
    evaluate (__LINE__, __func__, "אָבּגּדּהּ", unicode_string_casefold ("אָבּגּדּהּ"));
  }

  {
    evaluate (__LINE__, __func__, "TEST1234", unicode_string_uppercase ("test1234"));
    evaluate (__LINE__, __func__, "TEST1234", unicode_string_uppercase ("TEST1234"));
    evaluate (__LINE__, __func__, "ΘΕΟΣ", unicode_string_uppercase ("Θεος"));
    evaluate (__LINE__, __func__, "Α Α Β Β", unicode_string_uppercase ("Α α Β β"));
    evaluate (__LINE__, __func__, "אָבּגּדּהּ", unicode_string_uppercase ("אָבּגּדּהּ"));
  }

  {
    evaluate (__LINE__, __func__, "ABCDEFG", unicode_string_transliterate ("ABCDEFG"));
    evaluate (__LINE__, __func__, "Ιησου Χριστου", unicode_string_transliterate ("Ἰησοῦ Χριστοῦ"));
    evaluate (__LINE__, __func__, "אבגדה", unicode_string_transliterate ("אָבּגּדּהּ"));
  }
  
  {
    vector <string> needles;
    needles = filter_string_search_needles ("ABC", "one abc two ABc three aBc four");
    evaluate (__LINE__, __func__, { "abc", "ABc", "aBc" }, needles);
    needles = filter_string_search_needles ("abc", "one abc two ABc three aBc four");
    evaluate (__LINE__, __func__, { "abc", "ABc", "aBc" }, needles);
  }
  
  {
    evaluate (__LINE__, __func__, false, unicode_string_is_punctuation ("A"));
    evaluate (__LINE__, __func__, false, unicode_string_is_punctuation ("z"));
    evaluate (__LINE__, __func__, true, unicode_string_is_punctuation ("."));
    evaluate (__LINE__, __func__, true, unicode_string_is_punctuation (","));
  }
  
  {
    evaluate (__LINE__, __func__, false, convert_to_bool ("0"));
    evaluate (__LINE__, __func__, false, convert_to_bool ("false"));
    evaluate (__LINE__, __func__, false, convert_to_bool ("FALSE"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("1"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("true"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("TRUE"));
  }
  
  {
    evaluate (__LINE__, __func__, 21109, unicode_string_convert_to_codepoint ("創"));
    evaluate (__LINE__, __func__, 97, unicode_string_convert_to_codepoint ("a"));
  }
  
  {
    // Check that the function to desanitize html no longer corrupts UTF-8.
    string html = "<p>“Behold”, from “הִנֵּה”, means look at</p>";
    string desanitized = filter_string_desanitize_html (html);
    evaluate (__LINE__, __func__, html, desanitized);
  }
}


void test_filter_shell ()
{
  trace_unit_tests (__func__);
  
  evaluate (__LINE__, __func__, true, filter_shell_is_present ("zip"));
  evaluate (__LINE__, __func__, false, filter_shell_is_present ("xxxxx"));
  
  string output;
  int result;

  result = filter_shell_vfork (output, "", "ls", "-l");
  evaluate (__LINE__, __func__, 0, result);
  if (output.find ("unittest") == string::npos) {
    evaluate (__LINE__, __func__, "Supposed to list files", output);
  }

  result = filter_shell_vfork (output, "/", "ls", "-l");
  evaluate (__LINE__, __func__, 0, result);
  if (output.find ("tmp") == string::npos) {
    evaluate (__LINE__, __func__, "Supposed to list folder /", output);
  }
}


void test_filter_memory ()
{
  trace_unit_tests (__func__);
  
  int available = filter_memory_percentage_available ();
  (void) available;
}


void test_filter_ldap ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);

  // Copy the default LDAP server configuration into place.
  string ldap_txt = filter_url_create_root_path ("config", "ldap.txt");
  string ldap_conf = filter_url_create_root_path ("config", "ldap.conf");
  filter_url_file_cp (ldap_txt, ldap_conf);
  // Initialize LDAP configuration.
  ldap_logic_initialize ();
  
  // Authenticate a user and check the results.
  string user = "boyle";
  string password = "password";
  bool okay;
  bool access;
  string email;
  int role;
  okay = ldap_logic_fetch (user, password, access, email, role, false);
  evaluate (__LINE__, __func__, true, okay);
  evaluate (__LINE__, __func__, true, access);
  evaluate (__LINE__, __func__, "boyle@ldap.forumsys.com", email);
  evaluate (__LINE__, __func__, Filter_Roles::guest (), role);

  // Check there is one journal entry as a result of authenticating a user.
  string last = "0";
  vector <string> logs = Database_Logs::get (last);
  evaluate (__LINE__, __func__, 1, logs.size ());

  // Clear LDAP settings.
  ldap_logic_clear ();
  refresh_sandbox (false);
}


void test_filter_dev ()
{
  trace_unit_tests (__func__);
}
