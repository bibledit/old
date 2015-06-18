/*
** Copyright (©) 2003-2013 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/

#include "libraries.h"
#include "utilities.h"
#include "export_utils.h"
#include "bible.h"
#include "usfm.h"
#include "usfmtools.h"
#include "notes_utils.h"
#include "date_time_utils.h"
#include "export_translation_notes.h"
#include "constants.h"
#include "gwrappers.h"
#include "directories.h"
#include "unixwrappers.h"
#include "gtkwrappers.h"
#include "shell.h"
#include "swordnote.h"
#include "xmlutils.h"
#include "mapping.h"
#include "projectutils.h"
#include "books.h"
#include "progresswindow.h"
#include "opendocument.h"
#include "dialogselectbooks.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "textreplacement.h"
#include "tiny_utilities.h"
#include "clean.h"
#include "dialogradiobutton.h"
#include "usfm2osis.h"
#include "osis.h"
#include <libxml/xmlwriter.h>
#include "categorize.h"
#include "snapshots.h"
#include "java.h"
#include <glib/gi18n.h>

void export_to_usfm (const ustring& project, ustring location, bool zip)
{
  // (Temporal) output directory.
  ustring tempdir = gw_build_filename(Directories->get_temp(), "usfm-export");
  unix_rmdir(tempdir);
  gw_mkdir_with_parents(tempdir);
  if (!zip)
    gw_mkdir_with_parents(location);

  // Configuration.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Book selection.
  vector <unsigned int> books = project_get_books(project);
  {
    set <unsigned int> selectedbooks (books.begin(), books.end());
    SelectBooksDialog dialog(false);
    dialog.language(projectconfig->language_get());
    dialog.selection_set(selectedbooks);
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
    selectedbooks = dialog.selectionset;
    books.assign (selectedbooks.begin(), selectedbooks.end());
  }

  // Progress information.
  ProgressWindow progresswindow(_("Exporting project"), false);
  progresswindow.set_iterate(0, 1, books.size());

  // Export all books to usfm.
  for (unsigned int i = 0; i < books.size(); i++) {
    // Progress info.
    progresswindow.iterate();
    vector <ustring> lines = project_retrieve_book(project, books[i]);
    char padded [3];
    sprintf (padded, "%02d", books[i]);
    ustring padded2 (padded);
    ustring filename = padded2  + " " + books_id_to_english(books[i]) + ".usfm";
    replace_text (filename, " ", "_");
    if (zip)
      filename = gw_build_filename(tempdir, filename);
    else
      filename = gw_build_filename(location, filename);
    write_lines(filename, lines);
  }

  // Zip them?
  if (zip) {
    if (!g_str_has_suffix(location.c_str(), ".zip"))
      location.append(".zip");
    unlink(location.c_str());
#ifdef WIN32
    ustring command = "cd" + shell_quote_space(tempdir) + " && zip -r zip.zip *.usfm && move zip.zip" + shell_quote_space(location);
#else
    ustring command = "cd" + shell_quote_space(tempdir) + "; zip -r zip.zip *.usfm; mv zip.zip" + shell_quote_space(location);
#endif
    // This one does not work with GwSpawn because of the wildcards used.
    if (int ret = system(command.c_str())) {
      cerr << "System call for zip and move returned " << ret << endl;
      cerr << "-1 indicates error; return status of the command otherwise" << endl;
    } 
  }
}

void export_to_bibleworks(const ustring& project, const ustring& filename)
/*
Exports a whole project to a file that is fit for being imported by the 
BibleWorks Version Database Compiler.
This done in this manner:
- If an \id line is found, use its ID to get the name of the book in BibleWorks.
- If an chapter tag if found, use its number.
- As soon as an ID is found, put the current chapter at one.
- If a verse is found, add its number.
- Text of tags known not to be printable is thrown out.
- Any other tag's text gets added to the text of the verse.
Yes, this is a bit rough, I know...
*/
{
  if (!filename.empty()) {
    // Progress bar.
    ProgressWindow progresswindow(_("Export"), true);
    // Start process.
    try {
      Usfm usfm(stylesheet_get_actual ());
      // Write to outputfile.
      WriteText wt(filename);
      // Get all the books and go through them.
      vector < unsigned int >scripture_books = project_get_books(project);
      progresswindow.set_iterate(0, 1, scripture_books.size());
      progresswindow.set_text(_("Exporting"));
      for (unsigned int bk = 0; bk < scripture_books.size(); bk++) {
        // Progress information.
        progresswindow.iterate();
        if (progresswindow.cancel)
          return;
        // Get the abbreviation to be used.
        ustring abbreviation = books_id_to_bibleworks(scripture_books[bk]);
        // If the book does not exist in BibleWorks, skip it.
        if (abbreviation.empty())
          continue;
        // Handle each chapter in the book.
        vector < unsigned int >chapters = project_get_chapters(project, scripture_books[bk]);
        for (unsigned int ch = 0; ch < chapters.size(); ch++) {
          // Do not export chapter 0.
          if (chapters[ch] == 0)
            continue;
          // Get the chapter and handle it.
          vector < ustring > lines;
          lines = project_retrieve_chapter(project, scripture_books[bk], chapters[ch]);
          text_replacement(lines);
          CategorizeChapterVerse ccv(lines);
          for (unsigned int vs = 0; vs < ccv.verse.size(); vs++) {
            // Do not export verse 0.
            if (ccv.verse[vs] == "0")
              continue;
            // Bibledit does not handle combined verses like 1-4a., etc. So make
            // a simple verse.
            ustring verse = number_in_string(ccv.verse[vs]);
            // Remove the verse number from the line itself, except when the verse
            // was not a simple one, e.g. it was something like verse 1-3.
            size_t position = ccv.line[vs].find(" ");
            if (position != string::npos)
              ccv.line[vs].erase(0, ++position);
            if (verse == ccv.verse[vs]) {
              position = ccv.line[vs].find(" ");
              if (position != string::npos)
                ccv.line[vs].erase(0, ++position);
            }
            CategorizeLine cat_line(ccv.line[vs]);
            // Output verse.
            wt.text(abbreviation);
            wt.text(" ");
            wt.text(convert_to_string(chapters[ch]));
            wt.text(":");
            wt.text(verse);
            wt.text(" ");
            wt.text(cat_line.verse);
            wt.text("\n");
          }
        }
      }
    }
    catch(exception & ex) {
      cerr << _("Converting to BibleWorks: ") << ex.what() << endl;
    }
  }
}

void export_translation_notes(const ustring & filename, const vector < unsigned int >&ids_to_display, bool export_all)
{
  ExportTranslationNotes etn(filename, ids_to_display, export_all);
}


void export_to_osis_recommended (const ustring& project, const ustring& filename)
{
  ProgressWindow progresswindow(_("Exporting project"), true);

  unlink(filename.c_str());
  
  Usfm2Osis usfm2osis (filename);

  usfm2osis.set_stylesheet (stylesheet_get_actual ());

  usfm2osis.header (project, project);

  vector <unsigned int> books = project_get_books(project);
  progresswindow.set_iterate(0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    progresswindow.iterate();
    if (progresswindow.cancel) {
      return;
    }

    // Skip the book if it is not known in the Osis standard.
    if (books_id_to_osis(books[bk]).empty())
      continue;

    usfm2osis.open_book (books[bk]);

    vector <ustring> contents = project_retrieve_book (project, books[bk]);
    usfm2osis.load_book (contents);

    usfm2osis.close_book();
  }
}


void export_to_osis_old (const ustring& project, const ustring& filename)
{
  ProgressWindow progresswindow (_("Exporting project"), true);

  try {

    // Prepare for notes and inline text.
    Usfm usfm(stylesheet_get_actual ());
    SwordNote swordnote(usfm, true);
    UsfmInlineMarkers usfm_inline_markers(usfm);

    // Write to OSIS file.
    unlink(filename.c_str());
    WriteText wt(filename);

    // Write out xml headers.
    OsisRoot osisroot(&wt, project, project);

    // Get all the books and go through them.
    vector < unsigned int >scripture_books = project_get_books(project);
    progresswindow.set_iterate(0, 1, scripture_books.size());
    for (unsigned int bk = 0; bk < scripture_books.size(); bk++) {
      progresswindow.iterate();
      if (progresswindow.cancel) {
        return;
      }

      // Only proceed if book exists in the Osis encoding.
      if (books_id_to_osis(scripture_books[bk]).empty())
        continue;

      // Signal "new book" to notes system.
      swordnote.new_book();

      // Open book in osis code.
      OsisBook osisbook(&wt, books_id_to_english(scripture_books[bk]));

      // Go through the book and collect verses and other data.
      vector < unsigned int >chapters;
      vector < ustring > verses;
      vector < ustring > lines;
      {
        ustring swordverse = "0";
        int swordchapter = 0;
        vector < ustring > bookcontents = project_retrieve_book(project, scripture_books[bk]);
        CleanUsfm clean_usfm(bookcontents);
        text_replacement(clean_usfm.lines);
        for (unsigned int i2 = 0; i2 < clean_usfm.lines.size(); i2++) {
          ustring line = clean_usfm.lines[i2];
          ustring marker = usfm_extract_marker(line);
          if (usfm.is_chapter_number(marker)) {
            swordchapter = convert_to_int(number_in_string(line));
            swordverse = "0";
          } else if (usfm.is_verse_number(marker)) {
            // Extract verse number.
            size_t position = line.find(" ");
            position = CLAMP(position, 0, line.length());
            swordverse = line.substr(0, position);
            // Erase verse number from the line.
            position++;
            line.erase(0, position);
            // Store data.
            chapters.push_back(swordchapter);
            verses.push_back(swordverse);
            lines.push_back(line);
          } else {
            // Store data.
            chapters.push_back(swordchapter);
            verses.push_back(swordverse);
            // Store line with marker in it. Needed for formatting.
            lines.push_back(clean_usfm.lines[i2]);
          }
        }
      }
      // Default to opening chapter 0 and verse 0.
      unsigned int current_chapter = 0;
      ustring current_verse = "0";
      OsisChapter *osischapter = new OsisChapter(&wt, osisbook.book, current_chapter);
      OsisVerse *osisverse = new OsisVerse(&wt, osisbook.book, osischapter->chapter, current_verse);
      OsisParagraph *osisparagraph = NULL;
      // Go through all the lines of the remapped book.
      for (unsigned int i2 = 0; i2 < chapters.size(); i2++) {
        // Deal with a new chapter.
        if (chapters[i2] != current_chapter) {
          // Close verse, chapter.
          delete osisverse;
          delete osischapter;
          // Store new chapter.
          current_chapter = chapters[i2];
          // Reopen chapter and verse.
          osischapter = new OsisChapter(&wt, osisbook.book, current_chapter);
          current_verse = "0";
          osisverse = new OsisVerse(&wt, osisbook.book, osischapter->chapter, current_verse);
          // Signal new chapter to notes system.
          swordnote.new_chapter();
        }
        // Handle new verse.
        bool newverse = false;
        if (current_verse != verses[i2]) {
          // Close osis verse.
          delete osisverse;
          // Store new verse.
          current_verse = verses[i2];
          // Reopen osis verse.
          osisverse = new OsisVerse(&wt, osisbook.book, osischapter->chapter, current_verse);
          // Mark a new verse.
          newverse = true;
        }
        // Get the raw line.
        ustring line = lines[i2];
        // Change certain characters to xml entities.
        xml_handle_entities(line, NULL);
        // Deal with notes.
        swordnote.transform(line);
        // Deal with inline text.
        usfm_handle_inline_text(line, &usfm_inline_markers, NULL, imSword, NULL);
        // Get the style belonging to the marker.
        ustring marker = usfm_extract_marker(line);
        if (usfm.is_identifier(marker)) {
          // Do nothing with an identifier.
        } else if (usfm.is_starting_paragraph(marker)) {
          // Find out what type of paragraph it is.
          ParagraphType paragraphtype = usfm.paragraph_get_subtype(marker);
          bool close_paragraph = false;
          bool new_paragraph = false;
          ustring title_type;
          switch (paragraphtype) {
          case ptMainTitle:
            close_paragraph = true;
            title_type = "main";
            break;
          case ptSubTitle:
            close_paragraph = true;
            title_type = "sub";
            break;
          case ptSectionHeading:
            close_paragraph = true;
            title_type = "part";
            break;
          case ptNormalParagraph:
            close_paragraph = true;
            new_paragraph = true;
            break;
          }
          // Close if need be.
          if (close_paragraph) {
            if (osisparagraph)
              delete osisparagraph;
            osisparagraph = NULL;
          }
          // Open new one if needed.
          if (new_paragraph) {
            osisparagraph = new OsisParagraph(&wt);
          }
          // Handle titles also.
          if (!title_type.empty()) {
            wt.text("<title type=\"" + title_type + "\">");
          }
          // Output text.
          wt.text(line);
          // Close title if need be.
          if (!title_type.empty()) {
            wt.text("</title>");
          }
        } else {
          // Fallback for verse marker (removed before), unknown marker, or no marker, or incomplete marker.
          if (!newverse)
            wt.text(" ");
          wt.text(line);
        }
      }
      // Close paragraph, verse and chapter.      
      if (osisparagraph)
        delete osisparagraph;
      delete osisverse;
      delete osischapter;
    }
  }
  catch(exception & ex) {
    cerr << _("Export: ") << ex.what() << endl;
  }
}


void export_to_osis_for_go_bible_creator (const ustring& project, const ustring& filename)
// Exports a Bible to a stripped down OSIS file fit for the Go Bible Creator.
{
  // Progress.
  ProgressWindow progresswindow (_("Exporting"), false);

  // XML writer.
  xmlBufferPtr xmlbuffer = xmlBufferCreate();
  xmlTextWriterPtr xmlwriter = xmlNewTextWriterMemory(xmlbuffer, 0);

  // Setup and open document.
  xmlTextWriterStartDocument(xmlwriter, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent(xmlwriter, 1);
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "osis");
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "osisText");
  xmlTextWriterStartElement(xmlwriter, BAD_CAST "div");

  // Go through the books.
  vector <unsigned int> books = project_get_books (project);
  progresswindow.set_iterate (0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    progresswindow.iterate ();

    // Get and write book.
    unsigned int book = books[bk];
    ustring osis_book_id = books_id_to_osis (book);
    if (osis_book_id.empty()) {
      continue;
    }
    xmlTextWriterStartElement(xmlwriter, BAD_CAST "div");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "type", "book");
    xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", "%s", osis_book_id.c_str());

    // Go through the chapters.
    vector <unsigned int> chapters = project_get_chapters (project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {

      // Get and write chapter.
      unsigned int chapter = chapters[ch];
      if (chapter == 0) {
        continue;
      }
      ustring chapter_osis_id = osis_book_id + "." + convert_to_string (chapter);
      xmlTextWriterStartElement(xmlwriter, BAD_CAST "chapter");
      xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", "%s", chapter_osis_id.c_str());

      // Go through the verses.
      vector <ustring> verses = project_get_verses (project, book, chapter);
      for (unsigned int vs = 0; vs < verses.size(); vs++) {

        // Get verse, and skip 0.
        ustring verse = verses[vs];
        if (verse == "0") {
          continue;
        }

        // Write the verse number.
        ustring verse_osis_id = chapter_osis_id + "." + verse;
        xmlTextWriterStartElement(xmlwriter, BAD_CAST "verse");
        xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "sID", "%s", verse_osis_id.c_str());
        xmlTextWriterWriteFormatAttribute(xmlwriter, BAD_CAST "osisID", "%s", verse_osis_id.c_str());
 
        // Get verse text, remove verse number, and write it.
        ustring line = project_retrieve_verse (project, book, chapter, verse);
        line = usfm_get_verse_text_only (line);
        xmlTextWriterWriteFormatString(xmlwriter, "%s", line.c_str());

        // Close verse.
        xmlTextWriterEndElement(xmlwriter);
      }

      // Close chapter.
      xmlTextWriterEndElement(xmlwriter);
    }
    
    // Close book.
    xmlTextWriterEndElement(xmlwriter);
    
  }

  // Close document and write it to disk.
  xmlTextWriterEndDocument(xmlwriter);
  xmlTextWriterFlush(xmlwriter);
  g_file_set_contents(filename.c_str(), (const gchar *)xmlbuffer->content, -1, NULL);

  // Free memory.
  if (xmlwriter)
    xmlFreeTextWriter(xmlwriter);
  if (xmlbuffer)
    xmlBufferFree(xmlbuffer);
}


void export_to_sword (const ustring& project, ustring directory)
/*
Exports a whole project to a SWORD module.
At the time of writing this, the information on how to create a module for 
sword was found at http://www.crosswire.org/sword/develop/swordmodule/
Here's how we do the conversion
- A .conf file is created and put in ~/.sword/mods.d/
- We use the "osis" format as this offers most options and seems best supported.
- Create the module with program osis2mod.
- The text will be stored in ~/.sword/modules/texts/bibledit/<name>/
project: Which Bible to do.
directory: Where to put the module.
*/
{
  // Check for converter.
  if (!gw_find_program_in_path("osis2mod")) {
    gtkw_dialog_error(NULL, _("The SWORD compiler osis2mod was not found."));
    return;
  }

  // Directory to store module.
  if (directory.empty())
    directory = g_get_home_dir();

  // Configuration
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Progress information.
  ProgressWindow *progresswindow = new ProgressWindow(_("Exporting project"), true);

  // The temporal directories for the data.
  ustring base_directory = gw_build_filename(Directories->get_temp(), "sword");
  unix_rmdir(base_directory);
  gw_mkdir_with_parents(base_directory);
  ustring absolute_conf_directory = gw_build_filename(base_directory, "mods.d");
  gw_mkdir_with_parents(absolute_conf_directory);
  ustring relative_text_directory = gw_build_filename("modules", "texts", "bibledit", lowerCase(projectconfig->sword_name_get()));
  ustring absolute_text_directory = gw_build_filename(base_directory, relative_text_directory);
  gw_mkdir_with_parents(absolute_text_directory);

  // Create the configuration file.
  vector <ustring> lines;
  ustring line;
  lines.push_back("[" + projectconfig->sword_name_get() + "]");
  line = "DataPath=.";
  line.append(G_DIR_SEPARATOR_S + relative_text_directory + G_DIR_SEPARATOR_S);
  lines.push_back(line);
  lines.push_back("ModDrv=RawText");
  lines.push_back("SourceType=OSIS");
  lines.push_back("Encoding=UTF-8");
  lines.push_back("BlockType=BOOK");
  lines.push_back("GlobalOptionFilter=OSISStrongs");
  lines.push_back("GlobalOptionFilter=OSISMorph");
  lines.push_back("GlobalOptionFilter=OSISFootnotes");
  lines.push_back("GlobalOptionFilter=OSISHeadings");
  lines.push_back("GlobalOptionFilter=OSISRedLetterWords");
  lines.push_back("MinimumVersion=1.5.6");
  lines.push_back("Lang=" + projectconfig->sword_language_get());
  lines.push_back("Version=" + projectconfig->sword_version_get());
  lines.push_back("Description=" + projectconfig->sword_description_get());
  lines.push_back("About=" + projectconfig->sword_about_get());
  lines.push_back("LCSH=Bible--Translation");
  lines.push_back("DistributionLicense=" + projectconfig->sword_license_get());
  if (projectconfig->right_to_left_get())
    lines.push_back("Direction=RtoL");
  write_lines(gw_build_filename(absolute_conf_directory, lowerCase(projectconfig->sword_name_get()) + ".conf"), lines);
  lines.clear();

  // Osis file name.
  ustring osisfile = gw_build_filename(Directories->get_temp(), "osis-from-usfm.xml");
  unlink(osisfile.c_str());
  
  // OSIS to USFM converter.
  {
    Usfm2Osis usfm2osis (osisfile);

    // Stylesheet.
    usfm2osis.set_stylesheet (stylesheet_get_actual ());

    // Write header.
    usfm2osis.header (projectconfig->sword_name_get(), projectconfig->sword_description_get());

    // Get all the books and go through them.
    vector <unsigned int> books = project_get_books(project);
    progresswindow->set_iterate(0, 1, books.size());
    for (unsigned int bk = 0; bk < books.size(); bk++) {

      // Progress information.
      progresswindow->iterate();
      if (progresswindow->cancel) {
        delete progresswindow;
        return;
      }

      // Skip the book if it is not known in the Osis standard.
      if (books_id_to_osis(books[bk]).empty())
        continue;

      // Open book in Osis converter.
      usfm2osis.open_book (books[bk]);

      // Let the Osis converter transform the book contents.
      vector <ustring> contents = project_retrieve_book (project, books[bk]);
      usfm2osis.load_book (contents);

      // Close book in the Osis converter.
      usfm2osis.close_book();

    }
  }
  
  // Hide progress.
  delete progresswindow;

  // Convert the inputfile using the sword api utility.
  {
    GwSpawn spawn("osis2mod");
    spawn.arg(absolute_text_directory);
    spawn.arg(osisfile);
    spawn.progress("Compiling", false);
    spawn.describe ();
    spawn.run();
  }

  // Install it.
  unix_cp_r(gw_build_filename(base_directory, "mods.d"), settings->genconfig.export_to_sword_install_path_get());
  unix_cp_r(gw_build_filename(base_directory, "modules"), settings->genconfig.export_to_sword_install_path_get());

  // Compress and save the module.
  ustring command;
  ustring zipfile = temporary_file(projectconfig->sword_name_get()) + ".zip";
  unlink(zipfile.c_str());
  command = "cd" + shell_quote_space(base_directory) + " && ";
  command.append("zip -r" + shell_quote_space(zipfile) + "*");
  if (system(command.c_str())) ; // This one does not work with GwSpawn because of the wildcards used.
  unix_mv(zipfile, directory);
}


void export_to_opendocument(const ustring& project, const ustring& filename)
{
  // Configurations.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Book selection.
  vector < unsigned int >books = project_get_books(project);
  set < unsigned int >selectedbooks(books.begin(), books.end());
  {
    SelectBooksDialog dialog(false);
    dialog.language(projectconfig->language_get());
    dialog.selection_set(selectedbooks);
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
    selectedbooks = dialog.selectionset;
  }

  // Filename to save to.
  if (filename.empty())
    return;

  // If more books are selected, whether to save to multiple files.
  bool singlefile = true;
  if (selectedbooks.size() > 1) {
    vector < ustring > labels;
    labels.push_back(_("Single file"));
    labels.push_back(_("Multiple files"));
    RadiobuttonDialog dialog(_("Save method"), _("Multiple books have been selected.\nShould these be saved to a single file or to multiple files?"), labels, 0, false);
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
    singlefile = dialog.selection == 0;
  }
  // Keep note of unformatted markers.
  vector <ustring> unformatted_markers;
  // Export.
  if (singlefile) {
    OpenDocument odt(project, filename, &selectedbooks);
    odt.note_unformatted_markers (unformatted_markers);
  } else {
    vector < unsigned int >books(selectedbooks.begin(), selectedbooks.end());
    for (unsigned int i = 0; i < books.size(); i++) {
      set < unsigned int >selectedbook;
      selectedbook.insert(books[i]);
      ustring combinedfilename = filename + "-" + books_id_to_english(books[i]);
      OpenDocument odt(project, combinedfilename, &selectedbook);
      odt.note_unformatted_markers (unformatted_markers);
    }
  }
  if (!unformatted_markers.empty()) {
    // Give warning in case of unformatted markers.
    ustring message = _("The export has completed,\nbut the following markers could not be formatted properly:");
    for (unsigned int i = 0; i < unformatted_markers.size(); i++) {
      message.append ("\n" + unformatted_markers[i]);
    }
    gtkw_dialog_warning (NULL, message);
  }
}


void export_to_usfm_changes (const ustring& project, int time_from, ustring comment)
{
  // Working directory and zipped filename.
  ustring workingdirectory = gw_build_filename(Directories->get_temp(), "bible_backup");
  gw_mkdir_with_parents(workingdirectory);
  int currentseconds = date_time_seconds_get_current();
  int currentdays = date_time_julian_day_get_current();
  guint currentyear, currentmonth, currentday;
  int currenthour, currentminute, currentsecond;
  date_time_normal_get_year_month_day(currentdays, currentyear, currentmonth, currentday);
  date_time_normal_get_hour_minute_second(currentseconds, currenthour, currentminute, currentsecond);
  ustring currentsuffix = "-" + convert_to_string(currentyear) + "-" + convert_to_string(currentmonth) + "-" + convert_to_string(currentday) + "-" + convert_to_string(currenthour) + "-" + convert_to_string(currentminute) + "-" + convert_to_string(currentsecond);
  ustring filename;
  {
    guint fromyear, frommonth, fromday;
    int fromhour, fromminute, fromsecond;
    date_time_normal_get_year_month_day(date_time_seconds_to_julian(time_from), fromyear, frommonth, fromday);
    date_time_normal_get_hour_minute_second(time_from, fromhour, fromminute, fromsecond);
    ustring fromsuffix = "-from-" + convert_to_string(fromyear) + "-" + convert_to_string(frommonth) + "-" + convert_to_string(fromday) + "-" + convert_to_string(fromhour) + "-" + convert_to_string(fromminute) + "-" + convert_to_string(fromsecond);
    filename = "incremental-backup" + fromsuffix + "-to" + currentsuffix;
  }
  filename.append("-of-project-");
  filename.append(project);
  ustring username = g_get_real_name ();
  replace_text (username, " ", "-");
  filename.append ("-backed-up-by-");
  filename.append (username);
  if (!comment.empty()) {
    filename.append("-comment-");
    replace_text (comment, " ", "-");
    filename.append (comment);
  }
  filename.append(".zip");
  replace_text(filename, " ", "_");
  filename = gw_build_filename(workingdirectory, filename);

  {
    // Handle incremental backups.
    // Get the books/chapters changed since a given time.
    // Get them from git, because git also records changes made by others.
    // Locally produced tracking mechanisms would not so easily 
    // provide that information.
    vector < unsigned int >books;
    vector < unsigned int >chapters;
    snapshots_get_chapters_changed_since(project, time_from, books, chapters);
    if (books.empty()) {
      gtkw_dialog_info(NULL, _("There was nothing to backup"));
      return;
    }
    quick_sort(books, chapters, 0, books.size());
    // Storage for the data of the chapters in each book.
    map < unsigned int, vector < ustring > >lines;
    // Always include chapter 0 as that has the id line.
    set < unsigned int >books_chapter_zero_done;
    for (unsigned int i = 0; i < books.size(); i++) {
      if (books_chapter_zero_done.find(books[i]) == books_chapter_zero_done.end()) {
        vector < ustring > data = project_retrieve_chapter(project, books[i], 0);
        for (unsigned int i2 = 0; i2 < data.size(); i2++) {
          lines[books[i]].push_back(data[i2]);
        }
        books_chapter_zero_done.insert(books[i]);
      }
    }
    // Go through the books/chapters with changes, and add the text to the map.
    // Skip chapter 0. Has been done already.
    for (unsigned int i = 0; i < books.size(); i++) {
      if (chapters[i] != 0) {
        vector < ustring > data = project_retrieve_chapter(project, books[i], chapters[i]);
        for (unsigned int i2 = 0; i2 < data.size(); i2++) {
          lines[books[i]].push_back(data[i2]);
        }
      }
    }
    // Go through all books, see which ones have data, save to file.
    vector < unsigned int >all_books = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < all_books.size(); i++) {
      if (!lines[books[i]].empty()) {
        ustring usfmfilename = gw_build_filename(workingdirectory, books_id_to_english(books[i]) + ".usfm");
        write_lines(usfmfilename, lines[books[i]]);
      }
    }
    // Zip them.
#ifdef WIN32
    ustring command = "cd" + shell_quote_space(workingdirectory) + " && zip -r " + gw_path_get_basename(filename) + " *.usfm && del *.usfm";
#else
    ustring command = "cd" + shell_quote_space(workingdirectory) + "; zip -r " + gw_path_get_basename(filename) + " *.usfm; rm *.usfm";
#endif
    if (system(command.c_str())) ; // This one does not work with GwSpawn because of the wildcards used.
  }

  // Open web page with information: backup_is_ready.html.
  ustring filesize = convert_to_string(file_get_size(filename));
  ReadText rt(gw_build_filename(Directories->get_package_data(), "backup_is_ready.html"), true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    replace_text(rt.lines[i], "100", filesize);
    replace_text(rt.lines[i], "location", filename);
    replace_text(rt.lines[i], "tempdir", workingdirectory);
  }
  ustring htmlfile = gw_build_filename(workingdirectory, "backup.html");
  write_lines(htmlfile, rt.lines);
  unix_cp(gw_build_filename(Directories->get_package_data(), "bibledit.css"), gw_build_filename(workingdirectory, "bibledit.css"));
  gtkw_show_uri (htmlfile, false);
}


void export_to_go_bible (const ustring& project, const ustring& foldername)
// Export the project to a full GoBible.
{
  // Working area directory.
  ustring workingdirectory = gw_build_filename (Directories->get_temp(), "gobible");
  unix_rmdir (workingdirectory);
  gw_mkdir_with_parents (workingdirectory);
  
  // Check whether Java is installed. If not, bail out.
  if (!java_runtime_present (true)) {
    return;
  }

  // For some reason spaces in the project name confuse the GoBibleCreator.
  ustring no_space_project (project);
  replace_text (no_space_project, " ", "_");
  
  // Create a temporary OSIS file fit for the Go Bible Creator.
  ustring xmlfile = gw_build_filename (workingdirectory, no_space_project + ".xml");
  export_to_osis_for_go_bible_creator (project, xmlfile);

  // Create a Collections.txt file.
  ustring collections_txt_file = gw_build_filename (workingdirectory, "Collections.txt");
  {
    GwSpawn spawn ("gobiblecreator");
    spawn.workingdirectory (workingdirectory);
    spawn.arg (xmlfile);
    spawn.describe();
    spawn.run ();
    ReadText rt (collections_txt_file, true, false);
    vector <ustring> lines;
    lines.push_back ("Source-Text: " + no_space_project + ".xml");
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      lines.push_back (rt.lines[i]);
    }
    write_lines (collections_txt_file, lines);
  }

  // Create the Go Bible.
  {
    GwSpawn spawn ("gobiblecreator");
    spawn.workingdirectory (workingdirectory);
    spawn.arg (collections_txt_file);
    spawn.describe ();
    spawn.run ();  
  }

  // Check whether the Go Bible is there.
  ustring jarfile = gw_build_filename (workingdirectory, no_space_project + ".jar");
  ustring jadfile = gw_build_filename (workingdirectory, no_space_project + ".jad");
  bool jarpresent = g_file_test (jarfile.c_str(), G_FILE_TEST_IS_REGULAR);
  bool jadpresent = g_file_test (jarfile.c_str(), G_FILE_TEST_IS_REGULAR);
  if (jarpresent && jadpresent) {
    unix_mv (jarfile, foldername);
    unix_mv (jadfile, foldername);
  } else {
    gtkw_dialog_error (NULL, _("There was an error producing the Go Bible.\nPlease check the system log for more information."));
  }  
}



