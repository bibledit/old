/*
** Copyright (©) 2003-2009 Teus Benschop.
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
#include "dialogsword.h"
#include "directories.h"
#include "unixwrappers.h"
#include "gtkwrappers.h"
#include "shell.h"
#include "swordnote.h"
#include "xmlutils.h"
#include "languages.h"
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

void export_to_usfm(GtkWidget * parent, bool zipped)
// Export the whole project to USFM files.
{
  extern Settings *settings;
  ustring location = settings->session.export_usfm_location;
  if (location.empty())
    location = g_get_home_dir();
  // Get zipped output in the right directory, and not in its parent.
  if (zipped)
    location = gw_build_filename(location, "dummy");
  if (zipped)
    location = gtkw_file_chooser_save(parent, "", location);
  else
    location = gtkw_file_chooser_select_folder(parent, "", location);
  if (!location.empty()) {
    settings->session.export_usfm_location = location;
    export_to_usfm(settings->genconfig.project_get(), location, zipped);
  }
}

void export_to_usfm(const ustring & project, ustring location, bool zip)
{
  // (Temporal) output directory.
  ustring tempdir = gw_build_filename(directories_get_temp(), "usfm-export");
  unix_rmdir(tempdir);
  gw_mkdir_with_parents(tempdir);
  if (!zip)
    gw_mkdir_with_parents(location);

  // Get books.
  vector < unsigned int >books = project_get_books(project);

  // Progress information.
  ProgressWindow progresswindow("Exporting project", false);
  progresswindow.set_iterate(0, 1, books.size());

  // Export all books to usfm.
  for (unsigned int i = 0; i < books.size(); i++) {
    // Progress info.
    progresswindow.iterate();
    vector < ustring > lines = project_retrieve_book(project, books[i]);
    ustring filename = books_id_to_english(books[i]) + ".usfm";
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
    ustring command = "cd" + shell_quote_space(tempdir) + "; zip -r zip.zip *.usfm; mv zip.zip" + shell_quote_space(location);
    if (system(command.c_str())) ;
  }
}

void export_to_bibleworks(GtkWidget * parent)
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
  // Configuration
  extern Settings *settings;
  // Get the filename to which the data is to be saved.
  ustring filename = gtkw_file_chooser_save(parent, "", settings->genconfig.export_to_bibleworks_filename_get());
  if (!filename.empty()) {
    // Store (new) filename.
    settings->genconfig.export_to_bibleworks_filename_set(filename);
    // Progress bar.
    ProgressWindow progresswindow("Export", true);
    // Start process.
    try {
      Usfm usfm(stylesheet_get_actual ());
      // Write to outputfile.
      WriteText wt(filename);
      // Get all the books and go through them.
      vector < unsigned int >scripture_books = project_get_books(settings->genconfig.project_get());
      progresswindow.set_iterate(0, 1, scripture_books.size());
      progresswindow.set_text("Exporting");
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
        vector < unsigned int >chapters = project_get_chapters(settings->genconfig.project_get(), scripture_books[bk]);
        for (unsigned int ch = 0; ch < chapters.size(); ch++) {
          // Do not export chapter 0.
          if (chapters[ch] == 0)
            continue;
          // Get the chapter and handle it.
          vector < ustring > lines;
          lines = project_retrieve_chapter(settings->genconfig.project_get(), scripture_books[bk], chapters[ch]);
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
      cerr << "Converting to BibleWorks: " << ex.what() << endl;
    }
  }
}

void export_translation_notes(const ustring & filename, ExportNotesFormat format, const vector < unsigned int >&ids_to_display, bool export_all, GtkWidget * parent)
{
  ExportTranslationNotes etn(filename, format, ids_to_display, export_all, parent);
}

void export_to_sword_interactive()
// Exports a whole project to a SWORD module.
{
  // Dialog for information entry.
  {
    SwordDialog sworddialog(0);
    if (sworddialog.run() != GTK_RESPONSE_OK)
      return;
  }
  extern Settings *settings;
  export_to_sword_script(settings->genconfig.project_get(), "", true);
}

void export_to_sword_script(const ustring & project, ustring directory, bool gui)
/*
Exports a whole project to a SWORD module.
At the time of writing this, the information on how to create a module for 
sword was found at http://www.crosswire.org/sword/develop/swordmodule/
Here's how we do the conversion
- A .conf file is created and put in ~/.sword/mods.d/
- We use the "osis" format as this offers most options and seems best supported.
- Create the module with program osis2mod.
- The text will be stored in ~/.sword/modules/texts/bibledit/<name>/
*/
{
  // Directory to store module.
  if (directory.empty())
    directory = g_get_home_dir();

  // Configuration
  extern Settings *settings;

  // Progress information.
  ProgressWindow *progresswindow = NULL;
  if (gui)
    progresswindow = new ProgressWindow("Exporting project", true);

  // Open the project that contains the data.
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // The temporal directories for the data.
  ustring base_directory = gw_build_filename(directories_get_temp(), "sword");
  unix_rmdir(base_directory);
  gw_mkdir_with_parents(base_directory);
  ustring absolute_conf_directory = gw_build_filename(base_directory, "mods.d");
  gw_mkdir_with_parents(absolute_conf_directory);
  ustring relative_text_directory = gw_build_filename("modules", "texts", "bibledit", lowerCase(settings->genconfig.project_get() + projectconfig->sword_name_get()));
  ustring absolute_text_directory = gw_build_filename(base_directory, relative_text_directory);
  gw_mkdir_with_parents(absolute_text_directory);

  // Create the configuration file.
  vector < ustring > lines;
  ustring line;
  lines.push_back("[" + settings->genconfig.project_get() + projectconfig->sword_name_get() + "]");
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
  lines.push_back("Lang=" + language_encode_sword(projectconfig->sword_language_get()));
  lines.push_back("Version=" + projectconfig->sword_version_get());
  lines.push_back("Description=" + settings->genconfig.project_get() + projectconfig->sword_description_get());
  lines.push_back("About=" + settings->genconfig.project_get() + projectconfig->sword_about_get());
  lines.push_back("LCSH=" + projectconfig->sword_lcsh_get());
  lines.push_back("DistributionLicense=" + projectconfig->sword_license_get());
  if (projectconfig->right_to_left_get())
    lines.push_back("Direction=RtoL");
  write_lines(gw_build_filename(absolute_conf_directory, lowerCase(settings->genconfig.project_get() + projectconfig->sword_name_get()) + ".conf"), lines);
  lines.clear();

  // Osis file name.
  ustring osisfile = gw_build_filename(g_get_home_dir (), "osis-from-usfm.txt");
  unlink(osisfile.c_str());
  
  // OSIS to USFM converter.
  {
    Usfm2Osis usfm2osis (osisfile);

    // Stylesheet.
    usfm2osis.set_stylesheet (stylesheet_get_actual ());

    // Write header.
    usfm2osis.header (settings->genconfig.project_get() + projectconfig->sword_name_get(), settings->genconfig.project_get() + projectconfig->sword_description_get());

    // Get all the books and go through them.
    vector <unsigned int> books = project_get_books(settings->genconfig.project_get());
    if (gui) {
      progresswindow->set_iterate(0, 1, books.size());
    }
    for (unsigned int bk = 0; bk < books.size(); bk++) {

      // Progress information.
      if (gui) {
        progresswindow->iterate();
        if (progresswindow->cancel) {
          delete progresswindow;
          return;
        }
      }

      // Skip the book if it is not known in the Osis standard.
      if (books_id_to_osis(books[bk]).empty())
        continue;

      // Open book in Osis converter.
      usfm2osis.open_book (books[bk]);

      // Let the Osis converter transform the book contents.
      vector <ustring> contents = project_retrieve_book (settings->genconfig.project_get(), books[bk]);
      usfm2osis.load_book (contents);

      // Close book in the Osis converter.
      usfm2osis.close_book();

    }
  }
  
  // Hide progress.
  if (progresswindow)
    delete progresswindow;

  // Check for converter.
  if (!gw_find_program_in_path("osis2mod")) {
    ustring message = "The SWORD compiler osis2mod was not found.";
    if (gui)
      gtkw_dialog_error(NULL, message);
    return;
  }

  // Convert the inputfile using the sword api utility.
  {
    GwSpawn spawn("osis2mod");
    spawn.arg(absolute_text_directory);
    spawn.arg(osisfile);
    spawn.progress("Compiling", false);
    spawn.run();
  }

  // Install it.
  unix_cp_r(gw_build_filename(base_directory, "mods.d"), settings->genconfig.export_to_sword_install_path_get());
  unix_cp_r(gw_build_filename(base_directory, "modules"), settings->genconfig.export_to_sword_install_path_get());

  // Compress and save the module.
  ustring command;
  ustring zipfile = temporary_file(settings->genconfig.project_get() + projectconfig->sword_name_get()) + ".zip";
  unlink(zipfile.c_str());
  command = "cd" + shell_quote_space(base_directory) + "; ";
  command.append("zip -r" + shell_quote_space(zipfile) + "*");
  if (system(command.c_str())) ;
  unix_mv(zipfile, settings->genconfig.export_to_sword_module_path_get());
}

void export_to_opendocument(GtkWidget * parent)
{
  // Configurations.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Book selection.
  vector < unsigned int >books = project_get_books(settings->genconfig.project_get());
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
  ustring filename = gw_build_filename(g_get_home_dir(), "bibledit-export");
  filename = gtkw_file_chooser_save(parent, "", filename);
  if (filename.empty())
    return;

  // If more books are selected, whether to save to multiple files.
  bool singlefile = true;
  if (selectedbooks.size() > 1) {
    vector < ustring > labels;
    labels.push_back("Single file");
    labels.push_back("Multiple files");
    RadiobuttonDialog dialog("Save method", "Multiple books have been selected.\nShould these be saved to a single file or to multiple files?", labels, 0);
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
    singlefile = dialog.selection == 0;
  }
  // Export.
  if (singlefile) {
    OpenDocument odt(settings->genconfig.project_get(), filename, true, &selectedbooks);
  } else {
    vector < unsigned int >books(selectedbooks.begin(), selectedbooks.end());
    for (unsigned int i = 0; i < books.size(); i++) {
      set < unsigned int >selectedbook;
      selectedbook.insert(books[i]);
      ustring combinedfilename = filename + "-" + books_id_to_english(books[i]);
      OpenDocument odt(settings->genconfig.project_get(), combinedfilename, true, &selectedbook);
    }
  }
}
