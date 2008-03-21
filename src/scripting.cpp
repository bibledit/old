/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "libraries.h"
#include <config.h>
#include "exception.h"
#include <gtk/gtk.h>
#include "utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include "stylesheetutils.h"
#include "shell.h"
#include "projectutils.h"
#include "scripting.h"
#include "import.h"
#include "categorize.h"
#include "progresswindow.h"
#include "books.h"
#include "settings.h"
#include "notes_utils.h"
#include "export_utils.h"
#include "checks.h"
#include "check_validate_usfm.h"
#include "check_count_words.h"
#include "opendocument.h"
#include "check_count_usfms.h"
#include "check_count_characters.h"
#include "check_chapters_verses.h"
#include "check_compare_usfm.h"
#include "check_unwanted_patterns.h"
#include "check_unwanted_words.h"
#include "check_repetition.h"
#include "check_capitalization.h"
#include "check_matching_pairs.h"
#include "mapping.h"
#include "versification.h"
#include "originreferences.h"
#include "upgrade.h"
#include "unixwrappers.h"
#include "keyterms.h"
#include <popt.h>
#include "git.h"


void scripting_usage (char option_parse_result, poptContext optCon);
gchar * scripting_get_option (const char *argv[]);
ustring scripting_get_from_gchar (gchar * argument);
void scripting_ensure_project (const ustring& project);
void initialize_gui (bool gui, const int argc, const char *argv[]);
int export_stylesheet (const int argc, const char *argv[]);
int upgrade_data (const int argc, const char *argv[]);
int import_book (const int argc, const char *argv[]);
int export_book (const int argc, const char *argv[]);
int shutdown_actions (const int argc, const char *argv[]);
int export_project_usfm (const int argc, const char *argv[]);
int export_project_sword (const int argc, const char *argv[]);
int export_project_opendocument (const int argc, const char *argv[]);
int create_project (const int argc, const char *argv[]);
int delete_project (const int argc, const char *argv[]);
int validate_markers (const int argc, const char *argv[]);
int count_words (const int argc, const char *argv[]);
int count_usfms (const int argc, const char *argv[]);
int count_characters (const int argc, const char *argv[]);
int check_chapters_verses (const int argc, const char *argv[]);
int compare_usfms (const int argc, const char *argv[]);
int unwanted_patterns (const int argc, const char *argv[]);
int unwanted_words (const int argc, const char *argv[]);
int repeating_words (const int argc, const char *argv[]);
int check_capitalization (const int argc, const char *argv[]);
int check_matching_pairs (const int argc, const char *argv[]);
int import_verse_system (const int argc, const char *argv[]);
int map_ (const int argc, const char *argv[]);
int test_ (const int argc, const char *argv[]);
int export_keyterms (const int argc, const char *argv[]);
int create_keyterms (const int argc, const char *argv[]);


int scripting (const int argc, const char *argv[])
{
  // Action flags.
  bool version = false;
  bool upgradedata = false;
  bool importbook = false;
  bool exportbook = false;
  bool exportprojectusfm = false;
  bool exportprojectsword = false;
  bool exportprojectopendocument = false;
  bool exportstylesheet = false;
  bool shutdownactions = false;
  bool createproject = false;
  bool deleteproject = false;
  bool validatemarkers = false;
  bool countwords = false;
  bool countusfms = false;
  bool countcharacters = false;
  bool checkchaptersverses = false;
  bool compareusfms = false;
  bool unwantedpatterns = false;
  bool unwantedwords = false;
  bool repeatingwords = false;
  bool checkcapitalization = false;
  bool checkmatchingpairs = false;
  bool map = false;
  bool test = false;
  bool exportkeyterms = false;
  bool createkeyterms = false;
  
  // At first we used Glib's commandline option parser.
  // But this part of the library was so unstable that it broke proper 
  // bibledit operation at times.
  // Therefore we switched to the popt library. This has been doing well so far.
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { "version",                 '\0', POPT_ARG_NONE, &version,                   0, "Version number",                             NULL },
    { "upgrade-data",            '\0', POPT_ARG_NONE, &upgradedata,               0, "Upgrade all relevant data",                  NULL },
    { "import-book",             '\0', POPT_ARG_NONE, &importbook,                0, "Import a book",                              NULL },
    { "export-book",             '\0', POPT_ARG_NONE, &exportbook,                0, "Export a book",                              NULL },
    { "export-project-usfm",     '\0', POPT_ARG_NONE, &exportprojectusfm,         0, "Export a project to USFM",                   NULL },
    { "export-project-sword",    '\0', POPT_ARG_NONE, &exportprojectsword,        0, "Export a project to SWORD",                  NULL },
    { "export-project-odt",      '\0', POPT_ARG_NONE, &exportprojectopendocument, 0, "Export a project to OpenDocument text",      NULL },
    { "export-stylesheet",       '\0', POPT_ARG_NONE, &exportstylesheet,          0, "Export a stylesheet",                        NULL },
    { "shutdown-actions",        '\0', POPT_ARG_NONE, &shutdownactions,           0, "Called on Bibledit shutdown",                NULL },
    { "create-project",          '\0', POPT_ARG_NONE, &createproject,             0, "Create a project",                           NULL },
    { "delete-project",          '\0', POPT_ARG_NONE, &deleteproject,             0, "Delete a project",                           NULL },
    { "validate-markers",        '\0', POPT_ARG_NONE, &validatemarkers,           0, "Validate USFM markers",                      NULL },
    { "count-words",             '\0', POPT_ARG_NONE, &countwords,                0, "Count words",                                NULL },
    { "count-usfms",             '\0', POPT_ARG_NONE, &countusfms,                0, "Count USFMs",                                NULL },
    { "count-characters",        '\0', POPT_ARG_NONE, &countcharacters,           0, "Count characters",                           NULL },
    { "check-chapters-verses",   '\0', POPT_ARG_NONE, &checkchaptersverses,       0, "Check chapters and verses",                  NULL },
    { "compare-usfms",           '\0', POPT_ARG_NONE, &compareusfms,              0, "Compare USFMs",                              NULL },
    { "check-unwanted-patterns", '\0', POPT_ARG_NONE, &unwantedpatterns,          0, "Look for unwanted patterns",                 NULL },
    { "check-unwanted-words",    '\0', POPT_ARG_NONE, &unwantedwords,             0, "Look for unwanted words",                    NULL },
    { "check-repeating-words",   '\0', POPT_ARG_NONE, &repeatingwords,            0, "Look for repeating words",                   NULL },
    { "check-capitalization",    '\0', POPT_ARG_NONE, &checkcapitalization,       0, "Check capitalization",                       NULL },
    { "check-matching-pairs",    '\0', POPT_ARG_NONE, &checkmatchingpairs,        0, "Check matching pairs of punctuation",        NULL },
    { "map",                     '\0', POPT_ARG_NONE, &map,                       0, "Map a verse to another versification",       NULL },
    { "test",                    '\0', POPT_ARG_NONE, &test,                      0, "Test Bibledit",                              NULL },
    { "export-keyterms",         '\0', POPT_ARG_NONE, &exportkeyterms,            0, "Export the keyterms",                        NULL },
    { "create-keyterms",         '\0', POPT_ARG_NONE, &createkeyterms,            0, "Create a keyterms database",                 NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  poptGetNextOpt (optCon);
  
  // Take action depending on options.
  if (version) {
    cout << VERSION << endl;
    return 0;
  } else if (upgradedata) {
    return upgrade_data (argc, argv);
  } else if (importbook) {
    return import_book (argc, argv);
  } else if (exportbook) {
    export_book (argc, argv);
  } else if (exportprojectusfm) {
    export_project_usfm (argc, argv);
  } else if (exportprojectsword) {
    export_project_sword (argc, argv);
  } else if (exportprojectopendocument) {
    export_project_opendocument (argc, argv);
  } else if (exportstylesheet) {
    export_stylesheet (argc, argv);
  } else if (shutdownactions) {
    shutdown_actions (argc, argv);
  } else if (createproject) {
    create_project (argc, argv);
  } else if (deleteproject) {
    delete_project (argc, argv);
  } else if (validatemarkers) {
    validate_markers (argc, argv);
  } else if (countwords) {
    count_words (argc, argv);
  } else if (countusfms) {
    count_usfms (argc, argv);
  } else if (countcharacters) {
    count_characters (argc, argv);
  } else if (checkchaptersverses) {
    check_chapters_verses (argc, argv);
  } else if (compareusfms) {
    compare_usfms (argc, argv);
  } else if (unwantedpatterns) {
    unwanted_patterns (argc, argv);
  } else if (unwantedwords) {
    unwanted_words (argc, argv);
  } else if (repeatingwords) {
    repeating_words (argc, argv);
  } else if (checkcapitalization) {
    check_capitalization (argc, argv);
  } else if (checkmatchingpairs) {
    check_matching_pairs (argc, argv);
  } else if (map) {
    map_ (argc, argv);
  } else if (test) {
    test_ (argc, argv);
  } else if (exportkeyterms) {
    export_keyterms (argc, argv);
  } else if (createkeyterms) {
    create_keyterms (argc, argv);
  } else {
    // If no action was taken, print the help.
    scripting_usage ('\0', optCon);
  }
  
  // Free memory.
  poptFreeContext (optCon);
  
  // Ready.
  return 0;
}


void scripting_usage (char option_parse_result, poptContext optCon)
{
  if (option_parse_result < -1) {
    fprintf (stderr, "%s: %s\n\n", poptBadOption (optCon, POPT_BADOPTION_NOALIAS), poptStrerror(option_parse_result));
  }
  poptPrintHelp (optCon, stdout, 0);
  poptFreeContext (optCon);
  exit (EXIT_FAILURE);
}


gchar * scripting_get_option (const char *argv[])
// Returns a pointer to the first option passed to the program, 
// without the two preceding dashes (--).
{
  gchar * option = (gchar *) argv[1];
  option++;
  option++;
  return option;
}


ustring scripting_get_from_gchar (gchar * argument)
{
  ustring value;
  if (argument) { 
    value = argument; 
    g_free (argument);
  }
  return value;
}


void scripting_ensure_project (const ustring& project)
{
  if (!project_exists (project)) {
    cerr << "Project " << project << " does not exist" << endl;
    exit (EXIT_FAILURE);
  }
}


void initialize_gui (bool gui, const int argc, const char *argv[])
{
  if (gui)
    gtk_init ((int *) &argc, (char ***) &argv);
}


int export_stylesheet (const int argc, const char *argv[])
// Exports a stylesheet.
{
  // Parse options.
  ustring stylesheet;
  ustring file;
  gchar * g_stylesheet = NULL;
  gchar * g_file = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "stylesheet", '\0', POPT_ARG_STRING, &g_stylesheet,  0, "Name of the stylesheet to be exported", NULL },
    { "file", '\0', POPT_ARG_STRING, &g_file,  0, "Name of the file to write to", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  stylesheet = scripting_get_from_gchar (g_stylesheet);
  file = scripting_get_from_gchar (g_file);
  
  // Usage.
  if (stylesheet.empty() || file.empty()) scripting_usage (0, optCon);

  // Check stylesheet.
  if (!stylesheet_exists (stylesheet)) {
    cerr << "Stylesheet " << stylesheet << " not found." << endl;
    return 1;
  }

  // Do the export.
  stylesheet_export (stylesheet, file);
  
  return 0;
}


int upgrade_data (const int argc, const char *argv[])
// Upgrades older data to the currently used formats.
{
  // Parse options.
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  poptFreeContext (optCon);
  initialize_gui (gui, argc, argv);

  // Upgrade data.
  upgrade (gui);
  
  return 0;
}


int import_book (const int argc, const char *argv[])
// Imports a USFM file into Bibledit's database, with good error checking.
{
  // Parse options.
  ustring project;
  ustring file;
  ustring book;
  ustring encoding;  
  gchar * g_project = NULL;
  gchar * g_file = NULL;
  gchar * g_book = NULL;
  gchar * g_encoding = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project to import the book into", NULL },
    { "file", '\0', POPT_ARG_STRING, &g_file,  0, "Name of the USFM file which should be imported", NULL },
    { "book", '\0', POPT_ARG_STRING, &g_book,  0, "Optional: the name of the book, e.g. Matthew", NULL },
    { "encoding", '\0', POPT_ARG_STRING, &g_encoding,  0, "Optional: the encoding of the file, e.g. iso-8859-15", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  project = scripting_get_from_gchar (g_project);
  file = scripting_get_from_gchar (g_file);
  book = scripting_get_from_gchar (g_book);
  encoding = scripting_get_from_gchar (g_encoding);

  // Usage.
  if (project.empty() || file.empty()) scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Check project.
  scripting_ensure_project (project);
  
  // Read the book.
  ImportBookRead ibr (file, encoding);
  ibr.usfm ();
  if (ibr.lines.size() == 0) {
    cerr << "Could not read file " << file << "." << endl;
    return 1;
  }
  
  // Handle bookname.
  if (book.empty()) {
    book = ibr.bookname;
  }
  if (book.empty()) {
    cerr << "Could not determine the bookname." << endl;
    return 1;
  }
  
  // Each line gets associated chapter and verse information.
  CategorizeChapterVerse ccv (ibr.lines);

  // Store it in the databases.
  project_store_book (project, books_english_to_id (book), ccv);

  // Success.  
  return 0;
}


int export_book (const int argc, const char *argv[])
// Exports a USFM file from Bibledit's database, with good error checking.
{
  // Parse options.
  ustring project;
  ustring book;
  ustring file;
  gchar * g_project = NULL;
  gchar * g_book = NULL;
  gchar * g_file = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project to be exported", NULL },
    { "book", '\0', POPT_ARG_STRING, &g_book,  0, "Name of the book, e.g. Matthew", NULL },
    { "file", '\0', POPT_ARG_STRING, &g_file,  0, "Name of the file to which should be exported", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  project = scripting_get_from_gchar (g_project);
  book = scripting_get_from_gchar (g_book);
  file = scripting_get_from_gchar (g_file);
  
  // Usage.
  if (project.empty() || file.empty() || book.empty()) 
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Book is there?
  if (!project_book_exists (project, books_english_to_id (book))) {
    cerr << "Book " << book << " does not exist." << endl;
    return 1;
  }

  // Get and write book.
  vector <ustring> lines = project_retrieve_book (project, books_english_to_id (book));
  write_lines (file, lines);

  // Success.  
  return 0;
}


int shutdown_actions (const int argc, const char *argv[])
// Takes certain actions when Bibledit shuts down.
{
  // Parse options.
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  poptFreeContext (optCon);

  // Open a configuration to read parameters from.
  extern Settings * settings;
    
  unsigned int startuptime = settings->genconfig.startup_time_get ();
  // Set about to vacuum the sqlite databases.
  // Vacuuming a database is done only when it got changed. Saves time.
  // Project related databases.
  vector <ustring> projects = projects_get_all ();
  for (unsigned int i = 0; i < projects.size(); i++) {
    project_vacuum (projects[i], startuptime);
  }
  // Stylesheets.
  vector <ustring> stylesheets;
  stylesheet_get_ones_available (stylesheets);
  for (unsigned int i = 0; i < stylesheets.size(); i++) {
    stylesheet_vacuum (stylesheets[i], startuptime);
  }
  // Notes.
  notes_vacuum (startuptime);
  // Git system.
  git_cleanup ();

  // Success.  
  return 0;
}


int export_project_usfm (const int argc, const char *argv[])
// Exports a project to USFM files, one per book
// Does good error checking.
{
  // Parse options.
  ustring project;
  ustring location;
  bool zip = false;
  bool gui = false;
  gchar * g_project = NULL;
  gchar * g_location = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project to be exported", NULL },
    { "location", '\0', POPT_ARG_STRING, &g_location,  0, "Where to store the output", NULL },
    { "zip", '\0', POPT_ARG_NONE, &zip, 0, "Whether to zip the files", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (gui, argc, argv);
  project = scripting_get_from_gchar (g_project);
  location = scripting_get_from_gchar (g_location);

  // Usage.
  if (project.empty() || location.empty()) 
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Check project.
  scripting_ensure_project (project);

  // (Temporal) output directory.
  ustring tempdir = gw_build_filename (directories_get_temp(), "usfm-export");
  unix_rmdir (tempdir);
  gw_mkdir_with_parents (tempdir);
  if (!zip) gw_mkdir_with_parents (location);
    
  // Get books.
  vector <unsigned int> books = project_get_books (project);

  // Progress information.
  ProgressWindow * progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow ("Exporting project", true);
    progresswindow->set_iterate (0, 1, books.size());
  }

  // Export all books to usfm.
  for (unsigned int i = 0; i < books.size(); i++) {
    // Progress info.
    if (gui) {
      progresswindow->iterate ();
      if (progresswindow->cancel)
        return 1;
    }
    vector <ustring> lines = project_retrieve_book (project, books[i]);
    ustring filename = books_id_to_english (books[i]) + ".usfm";
    if (zip) filename = gw_build_filename (tempdir, filename);
    else filename = gw_build_filename (location, filename);
    write_lines (filename, lines);
  }

  // Zip them?
  if (zip) {
    if (!g_str_has_suffix (location.c_str(), ".zip")) location.append (".zip");
    unlink (location.c_str());
    ustring command = "cd" + shell_quote_space (tempdir) + "; zip -r zip.zip *.usfm; mv zip.zip" + shell_quote_space (location); // todo fix on Windows.
    system (command.c_str());
  }
  
  // Destroy window.
  if (progresswindow)
    delete progresswindow;

  // Success.  
  return 0;
}


int export_project_sword (const int argc, const char *argv[])
// Exports a project to a file fit for compiling by the Sword compiler.
// Does good error checking.
{
  // Parse options.
  ustring project;
  bool gui = false;
  ustring directory;
  gchar * g_project = NULL;
  gchar * g_directory = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project to be exported", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    { "directory", '\0', POPT_ARG_STRING, &g_directory,  0, "Optional: where to store the module", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (gui, argc, argv);
  project = scripting_get_from_gchar (g_project);
  directory = scripting_get_from_gchar (g_directory);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Check project.
  scripting_ensure_project (project);

  // Take the action.
  export_to_sword_script (project, directory, gui);
  
  // Success.  
  return 0;
}


int export_project_opendocument (const int argc, const char *argv[])
// Exports a project to an OpenDocument text file.
{
  // Parse options.
  ustring project;
  bool gui = false;
  ustring filename;
  gchar * g_project = NULL;
  gchar * g_filename = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project to be exported", NULL },
    { "file", '\0', POPT_ARG_STRING, &g_filename,  0, "Where to save the file", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (gui, argc, argv);
  project = scripting_get_from_gchar (g_project);
  filename = scripting_get_from_gchar (g_filename);

  // Usage.
  if ((project.empty()) || (filename.empty()))
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);
    
  // Check project.
  scripting_ensure_project (project);

  // Take the action.
  OpenDocument odt (project, filename, gui, NULL);
  
  // Success.  
  return 0;
}


int create_project (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project to create", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  project = scripting_get_from_gchar (g_project);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);
    
  // Execute task.
  if (project_exists (project)) {
    cerr << "Project already exists" << endl;
    return 1;
  }
  project_create (project);
  return 0;  
}


int delete_project (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project to delete", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  project = scripting_get_from_gchar (g_project);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Execute task.
  project_delete (project);
  return 0;  
}


int validate_markers (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  bool gui = false;
  bool checksheet = false;
  gchar * g_project = NULL;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    { "check-stylesheet", '\0', POPT_ARG_NONE, &checksheet,  0, "Optional: Are the markers in the stylesheet?", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (gui, argc, argv);
  project = scripting_get_from_gchar (g_project);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckValidateUsfm check (project, books, gui, checksheet);
  checks_output_references_comments (check.references, check.comments);

  return 0;  
}


int count_words (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  bool sortword = false;
  bool sortcount = false;
  int excludecount = 0;
  ustring extracharacters;
  gchar * g_extracharacters = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "sort-word", '\0', POPT_ARG_NONE, &sortword, 0, "Optional: Sort on word", NULL },
    { "sort-count", '\0', POPT_ARG_NONE, &sortcount, 0, "Optional: Sort on count", NULL },
    { "exclude-count", '\0', POPT_ARG_INT, &excludecount, 0, "Optional: Exclude words with a count of n or higher", NULL },
    { "extra-characters", '\0', POPT_ARG_STRING, &g_extracharacters,  0, "Optional: Extra characters that are part of a word", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);
  extracharacters = scripting_get_from_gchar (g_extracharacters);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Sort on only one thing.
  if (sortword && sortcount) {
    cout << "Cant't sort on more than one key" << endl;
    return 1;
  }
  
  // Execute task.
  vector <unsigned int> books;
  CheckCountWords check (project, books, extracharacters, sortword, sortcount, excludecount, gui);
  checks_output_two_columns (check.words, check.counts);

  return 0;  
}


int count_usfms (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  bool sortmarker = false;
  bool sortcount = false;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "sort-marker", '\0', POPT_ARG_NONE, &sortmarker, 0, "Optional: Sort on marker", NULL },
    { "sort-count", '\0', POPT_ARG_NONE, &sortcount, 0, "Optional: Sort on count", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckSortType sorttype = cstSort0;
  if (sortmarker) sorttype = cstSort1;
  if (sortcount) sorttype = cstSort2;
  CheckCountUsfms check (project, books, sorttype, gui);
  checks_output_two_columns (check.markers, check.counts);

  return 0;  
}


int count_characters (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  bool sortcharacter = false;
  bool sortcount = false;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "sort-character", '\0', POPT_ARG_NONE, &sortcharacter, 0, "Optional: Sort on character", NULL },
    { "sort-count", '\0', POPT_ARG_NONE, &sortcount, 0, "Optional: Sort on count", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckCountCharacters check (project, books, sortcharacter, sortcount, gui);
  checks_output_two_columns (check.characters, check.counts);
  return 0;  
}


int check_chapters_verses (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckChaptersVerses check (project, books, gui);
  checks_output_references_comments (check.references, check.comments);
  return 0;  
}


int compare_usfms (const int argc, const char *argv[])
{
  // Parse options.
  ustring project1;
  gchar * g_project1 = NULL;
  ustring project2;
  gchar * g_project2 = NULL;
  bool allmarkers = false;
  ustring includeonly;
  gchar * g_includeonly = NULL;
  ustring ignore;
  gchar * g_ignore = NULL;
  bool ignoreverse0 = false;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project1", '\0', POPT_ARG_STRING, &g_project1,  0, "Name of the first project", NULL },
    { "project2", '\0', POPT_ARG_STRING, &g_project2,  0, "Name of the second project", NULL },
    { "all-markers", '\0', POPT_ARG_NONE, &allmarkers, 0, "Optional: Check all the markers", NULL },
    { "include-only", '\0', POPT_ARG_STRING, &g_includeonly,  0, "Optional: Only include markers given", NULL },
    { "ignore", '\0', POPT_ARG_STRING, &g_ignore,  0, "Optional: Markers to ignore", NULL },
    { "ignore-verse-0", '\0', POPT_ARG_NONE, &ignoreverse0, 0, "Optional: Ignore verse 0", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project1 = scripting_get_from_gchar (g_project1);
  project2 = scripting_get_from_gchar (g_project2);
  includeonly = scripting_get_from_gchar (g_includeonly);
  ignore = scripting_get_from_gchar (g_ignore);

  // Usage.
  if ((project1.empty()) || (project2.empty()))
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project1);
  scripting_ensure_project (project2);

  // Execute task.
  vector <unsigned int> books;
  CheckCompareUsfms check (project1, project2, books, gui, allmarkers, includeonly, ignore, ignoreverse0);
  checks_output_references_comments (check.references, check.comments);
  return 0;  
}


int unwanted_patterns (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  ustring file;
  gchar * g_file = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "file", '\0', POPT_ARG_STRING, &g_file,  0, "Optional: File with patterns", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);
  file = scripting_get_from_gchar (g_file);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckUnwantedPatterns check (project, books, file, gui);
  checks_output_references_comments (check.references, check.comments);
  return 0;  
}


int unwanted_words (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  ustring file;
  gchar * g_file = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "file", '\0', POPT_ARG_STRING, &g_file,  0, "Optional: File with words", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);
  file = scripting_get_from_gchar (g_file);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckUnwantedWords check (project, books, file, gui);
  checks_output_references_comments (check.references, check.comments);
  return 0;  
}


int repeating_words (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  bool ignorecase = false;
  ustring onlythese;
  gchar * g_onlythese = NULL;
  ustring ignorethese;
  gchar * g_ignorethese = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "ignore-case", '\0', POPT_ARG_NONE, &ignorecase, 0, "Optional: Whether to ignore case", NULL },
    { "only-these", '\0', POPT_ARG_STRING, &g_onlythese,  0, "Optional: File with words to only include", NULL },
    { "ignore-these", '\0', POPT_ARG_STRING, g_ignorethese,  0, "Optional: File with words to ignore", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);
  onlythese = scripting_get_from_gchar (g_onlythese);
  ignorethese = scripting_get_from_gchar (g_ignorethese);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckRepetition check (project, books, ignorecase, onlythese, ignorethese, gui);
  checks_output_references_comments (check.references, check.comments);
  return 0;  
}


int check_capitalization (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  ustring punctuation_followed_by_capitals;
  gchar * g_punctuation_followed_by_capitals = NULL;
  ustring ignore_lower_case_following;
  gchar * g_ignore_lower_case_following = NULL;
  ustring abbreviations;
  gchar * g_abbreviations = NULL;
  bool anyprefixes = false;
  ustring uncapitalizedprefixes;
  gchar * g_uncapitalizedprefixes = NULL;
  ustring capitalizedsuffixes;
  gchar * g_capitalizedsuffixes = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "pfbc", '\0', POPT_ARG_STRING, &g_punctuation_followed_by_capitals,  0, "Optional: Punctuation followed by capitals", NULL },
    { "ilcf", '\0', POPT_ARG_STRING, &g_ignore_lower_case_following,  0, "Optional: Ignore lower case following these", NULL },
    { "abbreviations", '\0', POPT_ARG_STRING, &g_abbreviations,  0, "Optional: File with abbreviations", NULL },
    { "any-prefixes", '\0', POPT_ARG_NONE, &anyprefixes, 0, "Optional: Allow any prefixes", NULL },
    { "uncapitalized-prefixes", '\0', POPT_ARG_STRING, &g_uncapitalizedprefixes,  0, "Optional: File with uncapitalized prefixes", NULL },
    { "capitalized-suffixes", '\0', POPT_ARG_STRING, &g_capitalizedsuffixes,  0, "Optional: File with capitalized suffixes", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);
  punctuation_followed_by_capitals = scripting_get_from_gchar (g_punctuation_followed_by_capitals);
  ignore_lower_case_following = scripting_get_from_gchar (g_ignore_lower_case_following);
  abbreviations = scripting_get_from_gchar (g_abbreviations);
  uncapitalizedprefixes = scripting_get_from_gchar (g_uncapitalizedprefixes);
  capitalizedsuffixes = scripting_get_from_gchar (g_capitalizedsuffixes);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckCapitalization check (project, books, 
                             punctuation_followed_by_capitals,
                             ignore_lower_case_following,
                             abbreviations,
                             anyprefixes,
                             uncapitalizedprefixes,
                             capitalizedsuffixes,
                             gui);
  checks_output_references_comments (check.references, check.comments);
  return 0;  
}


int check_matching_pairs (const int argc, const char *argv[])
{
  // Parse options.
  ustring project;
  gchar * g_project = NULL;
  ustring ignore;
  gchar * g_ignore = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "project", '\0', POPT_ARG_STRING, &g_project,  0, "Name of project", NULL },
    { "ignore", '\0', POPT_ARG_STRING, &g_ignore,  0, "Optional: Pairs to ignore", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  project = scripting_get_from_gchar (g_project);
  ignore = scripting_get_from_gchar (g_ignore);

  // Usage.
  if (project.empty())
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  // Project is there?
  scripting_ensure_project (project);

  // Execute task.
  vector <unsigned int> books;
  CheckMatchingPairs check (project, books, ignore, gui);
  checks_output_references_comments (check.references, check.comments);
  return 0;  
}


int map_ (const int argc, const char *argv[])
{
  // Parse options.
  ustring name;
  gchar * g_name = NULL;
  ustring book;
  gchar * g_book = NULL;
  int chapter = 0;
  ustring verse;
  gchar * g_verse = NULL;
  bool to_original = false;
  bool to_me = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "name", '\0', POPT_ARG_STRING, &g_name,  0, "Versification system", NULL },
    { "book", '\0', POPT_ARG_STRING, &g_book,  0, "Book", NULL },
    { "chapter", '\0', POPT_ARG_INT, &chapter, 0, "Chapter", NULL },
    { "verse", '\0', POPT_ARG_STRING, &g_verse,  0, "Verse", NULL },
    { "to-original", '\0', POPT_ARG_NONE, &to_original, 0, "Map data to original versification", NULL },
    { "to-me", '\0', POPT_ARG_NONE, &to_me, 0, "Map data to my versification", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (true, argc, argv);
  name = scripting_get_from_gchar (g_name);
  book = scripting_get_from_gchar (g_book);
  verse = scripting_get_from_gchar (g_verse);

  if ((name.empty()) || book.empty() || (chapter == 0) || (verse.empty()) || (to_original == to_me))
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  Mapping mapping (name, books_english_to_id (book));
  vector <int> chapters;
  vector <int> verses;
  if (to_original) mapping.me_to_original (chapter, verse, chapters, verses);
  if (to_me) mapping.original_to_me (chapter, verse, chapters, verses);
  for (unsigned int i = 0; i < chapters.size(); i++) {
    cout << book << " " << chapters[i] << ":" << verses[i] << endl;
  }    
    
  return 0;  
}


int test_ (const int argc, const char *argv[])
{
  return 0;  
}


int export_keyterms (const int argc, const char *argv[])
{
  // Parse options.
  ustring directory;
  gchar * g_directory = NULL;
  bool gui = false;
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    { "directory", '\0', POPT_ARG_STRING, &g_directory,  0, "Directory for output", NULL },
    { "gui", '\0', POPT_ARG_NONE, &gui, 0, "Show graphical progressbar", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  initialize_gui (gui, argc, argv);
  directory = scripting_get_from_gchar (g_directory);

  if (directory.empty()) 
    scripting_usage (option_parse_result, optCon);
  poptFreeContext (optCon);

  keyterms_export (directory, gui);
  
  return 0;  
}


int create_keyterms (const int argc, const char *argv[])
{
  // Parse options.
  char option_parse_result;
  poptContext optCon;
  struct poptOption optionsTable[] = {
    POPT_AUTOHELP
    { scripting_get_option (argv), '\0', POPT_ARG_NONE, NULL, 0, " ", NULL },
    POPT_TABLEEND
  };
  optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
  option_parse_result = poptGetNextOpt (optCon);
  if (option_parse_result < -1) {
    scripting_usage (option_parse_result, optCon);
  }
  poptFreeContext (optCon);

  /*
  At some point in time templates in the form of sqlite databases were 
  distributed with Bibledit. But it was discovered that this could give problems 
  when a database was produced on the developer's computer using a newer version 
  of sqlite. Users still used older versions of sqlite, and the file format 
  was not always compatible, so that older versions of sqlite could not read the 
  templates produced with a newer version.

  The first solution was to distribute templates in text files, and to let 
  Bibledit produces databases out of them. But the disadvantage of this approach
  was that Bibledit on startup had to check and possibly create new databases.
  In particular with the keyterms that took too long.
  The new solution is to distribute an empty sqlite database with Bibledit,
  move the creation of the database to a script, the script uses that older 
  type of database, and the so created databases with keytersm are distributed
  with Bibledit.
  */

  keyterms_create_database ();
  vector <ustring> paths = keyterms_get_raw_files ();
  for (unsigned int i = 0; i < paths.size(); i++) {
    keyterms_import_textfile (paths[i]);
  }
  keyterms_import_otkey_db ();
  keyterms_import_ktref_db ();
  keyterms_import_ktbh_txt ();
  
  return 0;  
}
