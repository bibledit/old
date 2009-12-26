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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#include "bibledit-shutdown.h"


SqliteReader::SqliteReader(int dummy)
{
}


SqliteReader::~SqliteReader()
{
}


int SqliteReader::callback(void *userdata, int argc, char **argv, char **column_names)
{
  ((SqliteReader *) userdata)->string0.push_back(argv[0]);
  if (argc == 1)
    return 0;
  ((SqliteReader *) userdata)->string1.push_back(argv[1]);
  if (argc == 2)
    return 0;
  ((SqliteReader *) userdata)->string2.push_back(argv[2]);
  if (argc == 3)
    return 0;
  ((SqliteReader *) userdata)->string3.push_back(argv[3]);
  if (argc == 4)
    return 0;
  return 0;
}


int main (int argc, char *argv[])
{
  // Init variables.
  action_count = 0;
  
#ifndef WIN32
  // Do not run as root.
  if (getuid () == 0)
    return 1;
#endif

  // Redirect stdout and stderr to file.
  if (argc > 2) {
    // When a file is opened it is always allocated the lowest available file 
    // descriptor. Therefore the following commands cause stdout to be 
    // redirected to the logfile.
    close(1);
    creat (argv[2], 0666); 
    // The dup() routine makes a duplicate file descriptor for an already opened 
    // file using the first available file descriptor. Therefore the following 
    // commands cause stderr to be redirected to the file stdout writes to.
    close(2);
    if (dup(1));
  }    

  // Database to read commands from.
  if (argc > 1) maintenance_db_filename = argv[1];
  if (maintenance_db_filename == NULL) 
    return 1;
  if (!g_file_test(maintenance_db_filename, G_FILE_TEST_IS_REGULAR))
    return 1;

  // Initialize Gtk.
  gtk_init (&argc, &argv);

  // Display the window.      
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Bibledit");
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_NORMAL);
  g_signal_connect ((gpointer) window, "delete_event", G_CALLBACK (gtk_main_quit), NULL);

  GtkWidget * vbox = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 30);

  label = gtk_label_new ("Tidying up");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  label = gtk_label_new ("");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  GtkWidget * button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  g_signal_connect ((gpointer) button, "clicked", G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show_all (window);

  // Wait shortly, then process the data.
  g_timeout_add(2000, GSourceFunc(on_timeout), NULL);

  // Main loop.
  gtk_main();

/*
  // Trim the snapshots.
  vector <string> snapshot_databases = get_snapshot_databases (filename);
  for (unsigned int i = 0; i < snapshot_databases.size(); i++) {
    trim_snapshots (snapshot_databases[i]);
  }

  // Vacuum the databases.
  vector <string> vacuum_databases = get_vacuum_databases(filename);
  for (unsigned int i = 0; i < vacuum_databases.size(); i++) {
    vacuum_database (vacuum_databases[i]);
  }

  // Run the shutdown commands.
  vector <string> paths;
  vector <string> commands;
  get_commands  (filename, paths, commands);
  for (unsigned int i = 0; i < paths.size(); i++) {
    string command = "cd '" + paths[i] + "' ; " + commands[i];
    if (system (command.c_str()));
  }

  // Remove the command database.
  unlink (maintenance_db_filename);

*/  

  // Done.  
  return 0;
}


vector <string> get_vacuum_databases (const char * filename)
{
  vector <string> databases;
  /*
  sqlite3 *cmddb;
  sqlite3_open(filename, &cmddb);
  sqlite3_busy_timeout(cmddb, 2000);
  {
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select filename from vacuum;");
    sqlite3_exec(cmddb, sql, reader.callback, &reader, NULL);
    g_free(sql);
    databases = reader.string0;
  }
  {
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select database from snapshots;");
    sqlite3_exec(cmddb, sql, reader.callback, &reader, NULL);
    g_free(sql);
    for (unsigned int i = 0; i < reader.string0.size(); i++) {
      databases.push_back (reader.string0[i]);
    }
  }
  sqlite3_close(cmddb);
  */
  return databases;  
}


void vacuum_database (string filename)
{
  /*
  if (!filename.empty()) {
    sqlite3 *db;
    int rc;
    char *error = NULL;
    try {
      rc = sqlite3_open(filename.c_str(), &db);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      sqlite3_busy_timeout(db, 2000);
      rc = sqlite3_exec(db, "vacuum;", NULL, NULL, &error);
      if (rc != SQLITE_OK) {
        throw runtime_error(error);
      }
    }
    catch(exception & ex) {
      g_critical("%s", ex.what());
    }
    sqlite3_close(db);
  }
  */
}


vector <string> get_snapshot_databases (const char * filename)
{
  vector <string> databases;
  /*
  sqlite3 *db;
  sqlite3_open(filename, &db);
  sqlite3_busy_timeout(db, 2000);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select database from snapshots;");
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  databases = reader.string0;
  sqlite3_close(db);
  */
  return databases;  
}


void trim_snapshots (string filename)
{
  // Bail out if the database (no longer) exists.
  if (!g_file_test (filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    return;
  }

  // Current time (note that the time zone is not considered).
  int current_second = time(0);

    // Open snapshots database.
  sqlite3 *db;
  sqlite3_open(filename.c_str(), &db);
  char *sql;

  // We're may have to write a lot, therefore write fast.  
  sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL);

  // Go through all books and chapters.
  vector <unsigned int> books;
  {
    SqliteReader reader(0);
    sql = g_strdup_printf("select distinct book from snapshots;");
    sqlite3_exec(db, sql, reader.callback, &reader, NULL);
    g_free(sql);
    for (unsigned int i = 0; i < reader.string0.size(); i++) {
      books.push_back (convert_to_int (reader.string0[i]));
    }
  }
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    unsigned int book = books[bk];
    vector <unsigned int> chapters;
    {
      SqliteReader reader(0);
      sql = g_strdup_printf("select distinct chapter from snapshots where book = %d;", book);
      sqlite3_exec(db, sql, reader.callback, &reader, NULL);
      g_free(sql);
      for (unsigned int i = 0; i < reader.string0.size(); i++) {
        chapters.push_back (convert_to_int (reader.string0[i]));
      }
    }
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      unsigned int chapter = chapters[ch];

      // Read the persistent snapshots and remove their non-persistent duplicates.
      {
        vector <unsigned int> persistent_seconds;
        SqliteReader reader(0);
        sql = g_strdup_printf("select seconds from snapshots where book = %d and chapter = %d and persistent = 1;", book, chapter);
        sqlite3_exec(db, sql, reader.callback, &reader, NULL);
        g_free(sql);
        for (unsigned int i = 0; i < reader.string0.size(); i++) {
          persistent_seconds.push_back (convert_to_int (reader.string0[i]));
        }
        for (unsigned int i = 0; i < persistent_seconds.size(); i++) {
          sql = g_strdup_printf("delete from snapshots where book = %d and chapter = %d and seconds = %d and persistent = 0;", book, chapter, persistent_seconds[i]);
          sqlite3_exec(db, sql, NULL, NULL, NULL);
          g_free(sql);
        }
      }

      {
        // List the non-persistent snapshots, youngest one first.
        vector <unsigned int> snapshots;
        SqliteReader reader(0);
        sql = g_strdup_printf("select seconds from snapshots where book = %d and chapter = %d and persistent = 0 order by seconds desc;", book, chapter);
        sqlite3_exec(db, sql, reader.callback, &reader, NULL);
        g_free(sql);
        for (unsigned int i = 0; i < reader.string0.size(); i++) {
          snapshots.push_back (convert_to_int (reader.string0[i]));
        }

        // Divide the snapshots in age groups.
        vector <unsigned int> two_days_to_one_week;
        vector <unsigned int> one_week_to_one_month;
        vector <unsigned int> one_month_to_six_months;
        vector <unsigned int> six_months_and_more;
        for (unsigned int i = 0; i < snapshots.size(); i++) {
          int snapshot_age = current_second  - snapshots[i];
          // From 0 to 2 days old: Keep everything.
          if (snapshot_age <= 172800) {
          }
          // From 2 days to 1 week.
          else if ((snapshot_age >= 172800) && (snapshot_age <= 604800)) {
            two_days_to_one_week.push_back (snapshots[i]);
          }
          // From 1 week to 1 month.
          else if ((snapshot_age >= 604800) && (snapshot_age <= 2592000)) {
            one_week_to_one_month.push_back (snapshots[i]);
          }
          // From 1 month to 6 months.
          else if ((snapshot_age >= 2592000) && (snapshot_age <= 15552000)) {
            one_month_to_six_months.push_back (snapshots[i]);
          }
          // Older than 6 months.
          else if (snapshot_age >= 15552000) {
            six_months_and_more.push_back (snapshots[i]);
          }
        }

        // Trim each group.
        // From 2 days to 1 week: Keep one snapshot per hour.
        trim_snapshots_by_group (db, two_days_to_one_week, book, chapter, 3600);
        // From 1 week to 1 month: Keep one snapshot per day.
        trim_snapshots_by_group (db, one_week_to_one_month, book, chapter, 86400);
        // From 1 month to 6 months: Keep one snapshot every week.
        trim_snapshots_by_group (db, one_month_to_six_months, book, chapter, 604800);
        // Older than 6 months: Keep one snapshot every month.
        trim_snapshots_by_group (db, six_months_and_more, book, chapter, 2592000);
       
      }
    }
  }

  // Close the database.
  sqlite3_close(db);
}


unsigned int convert_to_int(const string& str)
{
  unsigned int i = 0;
  istringstream r(str);
  r >> i;
  return i;
}


void trim_snapshots_by_group (sqlite3 *db, const vector <unsigned int>& group, unsigned int book, unsigned int chapter, int spacing)
// Trims a group of snapshots.
// db: the database.
// group: the group to trim.
// spacing: the seconds of space that should be between the snapshots.
{
  // Bail out if there are less than 3 snapshots.
  if (group.size () < 3) {
    return;
  }

  // The second of the previous snapshot.
  unsigned int previous_second = group[0];
  
  // Go through the snapshots, but leave the first and last one in peace.
  for (unsigned int i = 1; i < (group.size() - 1); i++) {

    // Trim the snapshots.
    if (group[i] > (previous_second - spacing)) {
      char * sql;
      sql = g_strdup_printf("delete from snapshots where book = %d and chapter = %d and seconds = %d and persistent = 0;", book, chapter, group[i]);
      sqlite3_exec(db, sql, NULL, NULL, NULL);
      g_free(sql);
    } else {
      previous_second = group[i];
    }
  }
}


void get_commands (const char * filename, vector <string>& paths, vector <string>& commands)
{
  /*
  set <string> commandset;
  sqlite3 *db;
  sqlite3_open(filename, &db);
  sqlite3_busy_timeout(db, 2000);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select directory, command from commands;");
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  for (unsigned int i = 0; i < reader.string0.size(); i++) {
    string path = reader.string0[i];
    string command = reader.string1[i];
    if (commandset.find (path + command) == commandset.end ()) {
      paths.push_back (path);
      commands.push_back (command);
    }
  }
  sqlite3_close(db);
  */
}


bool on_timeout (gpointer data) // Todo
{
  bool something_was_done = false;

  sqlite3 *db;
  sqlite3_open(maintenance_db_filename, &db);
  sqlite3_busy_timeout(db, 2000);

  something_was_done = handle_shell_command (db);

  if (something_was_done) {
    g_timeout_add(10, GSourceFunc(on_timeout), NULL);
  } else {
    gtk_main_quit ();
  }

  sqlite3_close(db);

  return false;
}


bool handle_shell_command (sqlite3 *db) // Todo
// Looks in the database whether there's a shell command to run.
// It it ran one, it returns true.
{
  // Read the commands to run.
  SqliteReader reader(0);
  sqlite3_exec(db, "select * from commands;", reader.callback, &reader, NULL);
  // Return false if there's nothing to run.
  if (reader.string0.empty()) {
    return false;
  }

  // Get parameters for the shell command to run.
  string workingdirectory = reader.string0[0];
  string shellcommand = reader.string1[0];

  // Remove this command, and any duplicate, from the database.
  char *sql;
  sql = g_strdup_printf("delete from commands where workingdirectory = '%s' and shellcommand = '%s';", double_apostrophy (workingdirectory).c_str(), double_apostrophy (shellcommand).c_str());
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);

  // Run and log the command.
  string command = "cd '" + workingdirectory + "' ; " + shellcommand;
  printf ("Run command %s\n", command.c_str());
  fflush (stdout);
  int exitcode = system (command.c_str());
  printf ("Exit code %d\n", exitcode);
  fflush (stdout);

  // Feedback.
  feedback ();
  
  // Return true since it ran a shell command.
  return true;
}


string double_apostrophy(const string & line)
// SQLite needs any apostrophy in the data to be prefixed by another one.
// This function does that.
{
  string returnvalue;
  returnvalue = line;
  size_t offset = returnvalue.find("'");
  while (offset != string::npos) {
    returnvalue.insert(offset, "'");
    offset++;
    offset++;
    offset = returnvalue.find("'", offset);
  }
  return returnvalue;
}


void feedback ()
{
  action_count++;
  gchar *text = g_strdup_printf("%d", action_count);
  gtk_label_set_text (GTK_LABEL (label), text);
  g_free(text);
}


// Todo since the db usually persists, it needs to vacuum it at the end.
