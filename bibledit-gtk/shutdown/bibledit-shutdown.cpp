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

  // Remove the maintenance database.
  unlink (maintenance_db_filename);

  // Done.  
  return 0;
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


bool on_timeout (gpointer data)
{
  // Open maintenance database.
  sqlite3 *db;
  sqlite3_open(maintenance_db_filename, &db);
  sqlite3_busy_timeout(db, 2000);

  // First do the shell commands, as these do contain the commands that we'd like to run.
  // If these shell commands were done later on, and the user would cancel the maintenance
  // routine, then some important shell commands would not be done.
  bool something_was_done = handle_shell_command (db);

  // Tasks related to the git repository.
  if (!something_was_done) {
    something_was_done = handle_git_repositories (db);
  }

  // Tasks related to the snapshots. Done before databases.
  if (!something_was_done) {
    something_was_done = handle_snapshots (db);
  }
 
  // Tasks related to the databases.
  if (!something_was_done) {
    something_was_done = handle_databases (db);
  }

  // Earlier tasks here could have added shell commands, therefore check whether there are any to be done.
  if (!something_was_done) {
    something_was_done = handle_shell_command (db);
  }

  if (something_was_done) {
    // There could be more tasks to be done, let's go for another round.
    g_timeout_add(10, GSourceFunc(on_timeout), NULL);
  } else {
    // All maintenance tasks have been done.
    gtk_main_quit ();
  }

  // Close database.
  sqlite3_close(db);

  // Done.
  return false;
}


bool handle_shell_command (sqlite3 *db)
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
  while (gtk_events_pending()) gtk_main_iteration();
}


bool handle_git_repositories (sqlite3 *db)
// Looks at the git repositories mentioned in the database.
// If one needs to be optimized, it does that, and returns true.
{
  // Get the git repository directories from the database.
  vector <string> repositories_in_database;
  {
    SqliteReader reader(0);
    sqlite3_exec(db, "select distinct directory from gitrepos;", reader.callback, &reader, NULL);
    repositories_in_database = reader.string0;
  }

  // Go through the repositories.
  for (unsigned int repo = 0; repo < repositories_in_database.size(); repo++) {
    
    // If the repository was committed, it should get optimized.
    // Store shell commands in the database for later execution.
    printf ("Optimize git repository at %s\n", repositories_in_database[repo].c_str());
    fflush (stdout);
    char *sql;
    // Prune all unreachable objects from the object database.
    sql = g_strdup_printf("insert into commands values ('%s', 'git prune');", double_apostrophy (repositories_in_database[repo]).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    // Cleanup unnecessary files and optimize the local repository.
    sql = g_strdup_printf("insert into commands values ('%s', 'git gc');", double_apostrophy (repositories_in_database[repo]).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    // Remove extra objects that are already in pack files.
    sql = g_strdup_printf("insert into commands values ('%s', 'git prune-packed');", double_apostrophy (repositories_in_database[repo]).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    // Pack unpacked objects in the repository.
    sql = g_strdup_printf("insert into commands values ('%s', 'git repack');", double_apostrophy (repositories_in_database[repo]).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
    // Remove any mention of this repository from the database.
    sql = g_strdup_printf("delete from gitrepos where directory = '%s';", double_apostrophy (repositories_in_database[repo]).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
    // Feedback.
    feedback ();
    // Return true since something was done.
    return true;
   
  }

  // Return false since nothing was done.
  return false;  
}


bool handle_snapshots (sqlite3 *db)
// Looks at the snapshot databases mentioned in the maintenance database.
// If one needs to be optimized, it does that, and returns true.
{
  // Get the databases.
  vector <string> databases_in_database;
  {
    SqliteReader reader(0);
    sqlite3_exec(db, "select distinct filename from snapshots;", reader.callback, &reader, NULL);
    databases_in_database = reader.string0;
  }
  
  // Go through the databases.
  for (unsigned int database = 0; database < databases_in_database.size(); database++) {

    // Optimize it.
    printf ("Optimize snapshot at %s\n", databases_in_database[database].c_str());
    fflush (stdout);
    trim_snapshots (databases_in_database[database]);
    // Remove this database from the maintenance database.
    gchar * sql;
    sql = g_strdup_printf("delete from snapshots where filename = '%s';", double_apostrophy (databases_in_database[database]).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
    // Feedback.
    feedback ();
    // Return true since something was done.
    return true;

  }
  // Return false since nothing was done.
  return false;  
}


bool handle_databases (sqlite3 *db)
// Looks at the databases mentioned in the maintenance database.
// If one needs to be optimized, it does that, and returns true.
{
  // Get the databases.
  vector <string> databases_in_database;
  {
    SqliteReader reader(0);
    sqlite3_exec(db, "select distinct filename from databases;", reader.callback, &reader, NULL);
    databases_in_database = reader.string0;
  }
  // Go through the databases.
  for (unsigned int database = 0; database < databases_in_database.size(); database++) {
    // If the database was edited, it should get optimized.
    printf ("Optimize database at %s\n", databases_in_database[database].c_str());
    fflush (stdout);
    if (!databases_in_database[database].empty()) {
      sqlite3 *db2;
      sqlite3_open(databases_in_database[database].c_str(), &db2);
      sqlite3_busy_timeout(db2, 2000);
      sqlite3_exec(db2, "vacuum;", NULL, NULL, NULL);
      sqlite3_close(db2);
    }
    // Remove this database from the maintenance database.
    gchar * sql;
    sql = g_strdup_printf("delete from databases where filename = '%s';", double_apostrophy (databases_in_database[database]).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
    // Feedback.
    feedback ();
    // Return true since something was done.
    return true;
  }
  // Return false since nothing was done.
  return false;  
}
