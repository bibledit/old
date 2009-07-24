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
#include <glib.h>
#include "windowreferences.h"
#include "help.h"
#include "window.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "references.h"
#include "keyboard.h"
#include "dialogentry3.h"
#include "gtkwrappers.h"
#include "referenceutils.h"
#include <sqlite3.h>
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "bible.h"


WindowReferences::WindowReferences(GtkAccelGroup * accelerator_group, bool startup, GtkWidget * parent_box):
WindowBase(widReferences, "References", startup, 0, parent_box), reference(0, 0, "")
// Window for showing the quick references.  
{
  lower_boundary = 0;
  active_entry = -1;
    
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_container_add(GTK_CONTAINER(window_vbox), scrolledwindow);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  htmlview = gtk_html_new();
  gtk_widget_show(htmlview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), htmlview);
  gtk_html_allow_selection(GTK_HTML(htmlview), true);

  g_signal_connect((gpointer) htmlview, "link-clicked", G_CALLBACK(on_html_link_clicked), gpointer(this));

  // Signal button.
  signal_button = gtk_button_new();

  // Main focused widget.
  last_focused_widget = htmlview;
  gtk_widget_grab_focus (last_focused_widget);

  // Load previously saved references.
  load ();
  html_link_clicked ("");
}


WindowReferences::~WindowReferences()
{
  // Save references.
  save ();
  // Destroy signal button.
  gtk_widget_destroy(signal_button);
}


void WindowReferences::set (vector <Reference>& refs, const ustring& language, vector <ustring> * comments_in)
// Sets the references in the window.
// refs: the references to be loaded.
// language: the language in which to display the references.
{
  mylanguage = language;
  references.clear();    
  comments.clear();
  active_entry = -1;
  lower_boundary = 0;
  for (unsigned int i = 0; i < refs.size(); i++) {
    references.push_back (refs[i]);
    if (comments_in)
      comments.push_back (comments_in->at (i));
    else
      comments.push_back ("");
  }  
  html_link_clicked ("");
}


vector <Reference> WindowReferences::get ()
// Gets the references from the window.
{
  return references;
}


void WindowReferences::open()
{
  // Settings.
  extern Settings *settings;
  // Ask for a file.
  ustring filename = gtkw_file_chooser_open(window_vbox, "Open File", settings->genconfig.references_file_get());
  if (filename.empty())
    return;
  // Allow for up to three words to search for in these references.
  ustring searchword1, searchword2, searchword3;
  vector < ustring > import_references_searchwords = settings->session.import_references_searchwords;
  for (unsigned int i = 0; i < import_references_searchwords.size(); i++) {
    if (i == 0)
      searchword1 = import_references_searchwords[i];
    if (i == 1)
      searchword2 = import_references_searchwords[i];
    if (i == 2)
      searchword3 = import_references_searchwords[i];
  }
  Entry3Dialog dialog2("Search for", true, "Optionally enter _1st searchword", searchword1, "Optionally enter _2nd searchword", searchword2, "Optionally enter _3rd searchword", searchword3);
  int result = dialog2.run();
  if (result == GTK_RESPONSE_OK) {
    searchword1 = dialog2.entered_value1;
    searchword2 = dialog2.entered_value2;
    searchword3 = dialog2.entered_value3;
    import_references_searchwords.clear();
    if (!searchword1.empty())
      import_references_searchwords.push_back(searchword1);
    if (!searchword2.empty())
      import_references_searchwords.push_back(searchword2);
    if (!searchword3.empty())
      import_references_searchwords.push_back(searchword3);
    settings->session.import_references_searchwords = import_references_searchwords;
    settings->genconfig.references_file_set(filename);
    load(settings->genconfig.references_file_get());
    if (import_references_searchwords.size() > 0) {
      settings->session.highlights.clear();
      for (unsigned int i = 0; i < import_references_searchwords.size(); i++) {
        SessionHighlights sessionhighlights(import_references_searchwords[i], false, false, false, false, atRaw, false, false, false, false, false, false, false, false);
        settings->session.highlights.push_back(sessionhighlights);
      }
    }
  }
}


void WindowReferences::load ()
// Loads references from database.
{
  // Bail out if there are no references.
  if (!g_file_test(references_database_filename().c_str(), G_FILE_TEST_IS_REGULAR))
    return;
    
  // Language.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  ustring language = projectconfig->language_get();
  
  // Database variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Open database.
    rc = sqlite3_open(references_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    // Read the references.
    {
      SqliteReader sqlitereader(0);
      char *sql;
      sql = g_strdup_printf("select book, chapter, verse, comment from refs;");
      rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
      g_free(sql);
      if (rc != SQLITE_OK) {
        throw runtime_error(error);
      }
      for (unsigned int i = 0; i < sqlitereader.ustring0.size(); i++) {
        Reference reference(convert_to_int(sqlitereader.ustring0[i]), convert_to_int(sqlitereader.ustring1[i]), sqlitereader.ustring2[i]);
        references.push_back(reference);
        comments.push_back(sqlitereader.ustring3[i]);
      }
    }
    // Read the searchwords.
    {
      SqliteReader sqlitereader(0);
      char *sql;
      sql = g_strdup_printf("select word, casesensitive, glob, matchbegin, matchend, areatype, areaid, areaintro, areaheading, areachapter, areastudy, areanotes, areaxref, areaverse from highlights;");
      rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
      g_free(sql);
      if (rc != SQLITE_OK) {
        throw runtime_error(error);
      }
      extern Settings *settings;
      for (unsigned int i = 0; i < sqlitereader.ustring0.size(); i++) {
        SessionHighlights sessionhighlights(sqlitereader.ustring0[i],
                                            convert_to_bool(sqlitereader.ustring1[i]),
                                            convert_to_bool(sqlitereader.ustring2[i]),
                                            convert_to_bool(sqlitereader.ustring3[i]),
                                            convert_to_bool(sqlitereader.ustring4[i]), (AreaType) convert_to_int(sqlitereader.ustring5[i]), convert_to_bool(sqlitereader.ustring6[i]), convert_to_bool(sqlitereader.ustring7[i]), convert_to_bool(sqlitereader.ustring8[i]), convert_to_bool(sqlitereader.ustring9[i]), convert_to_bool(sqlitereader.ustring10[i]), convert_to_bool(sqlitereader.ustring11[i]), convert_to_bool(sqlitereader.ustring12[i]), convert_to_bool(sqlitereader.ustring13[i]));
        settings->session.highlights.push_back(sessionhighlights);
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close connection.  
  sqlite3_close(db);
}


void WindowReferences::load (const ustring & filename)
// Loads references from a file.
{
  references.clear();
  comments.clear();
  try {
    ReadText rt(filename, true);
    // Pick out the references and leave the rest.
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      unsigned int book, chapter;
      ustring verse;
      if (reference_discover(0, 0, "", rt.lines[i], book, chapter, verse)) {
        Reference reference(book, chapter, verse);
        references.push_back(reference);
        comments.push_back ("");
      }
    }
    sort_references(references);
  }
  catch(exception & ex) {
    cerr << "Loading references: " << ex.what() << endl;
  }
  // Load these.
  html_link_clicked ("");
}


void WindowReferences::save ()
{
  // Remove existing database.
  unlink(references_database_filename().c_str());
  // Some database variables.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Open the database.
    rc = sqlite3_open(references_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    // Create table for the references.
    char *sql;
    sql = g_strdup_printf("create table refs (book integer, chapter integer, verse text, comment text);");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    // Set it to store references fast.
    sql = g_strdup_printf("PRAGMA synchronous=OFF;");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    // Store the references and the comments.
    for (unsigned int i = 0; i < references.size(); i++) {
      sql = g_strdup_printf("insert into refs values (%d, %d, '%s', '%s')", references[i].book, references[i].chapter, references[i].verse.c_str(), double_apostrophy(comments[i]).c_str());
      rc = sqlite3_exec(db, sql, NULL, NULL, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
    }
    // Create table for the searchwords.
    sql = g_strdup_printf("create table highlights (word text, casesensitive integer, glob integer, matchbegin integer, matchend integer, areatype integer, areaid integer, areaintro integer, areaheading integer, areachapter integer, areastudy integer, areanotes integer, areaxref integer, areaverse integer);");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    // Store the searchwords and related data.
    extern Settings *settings;
    for (unsigned int i = 0; i < settings->session.highlights.size(); i++) {
      sql = g_strdup_printf("insert into highlights values ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                            double_apostrophy(settings->session.highlights[i].word).c_str(),
                            (int)settings->session.highlights[i].casesensitive,
                            (int)settings->session.highlights[i].globbing,
                            (int)settings->session.highlights[i].matchbegin,
                            (int)settings->session.highlights[i].matchend, (int)settings->session.highlights[i].areatype, (int)settings->session.highlights[i].id, (int)settings->session.highlights[i].intro, (int)settings->session.highlights[i].heading, (int)settings->session.highlights[i].chapter, (int)settings->session.highlights[i].study, (int)settings->session.highlights[i].notes, (int)settings->session.highlights[i].xref, (int)settings->session.highlights[i].verse);
      rc = sqlite3_exec(db, sql, NULL, NULL, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close db.
  sqlite3_close(db);
}


void WindowReferences::save(const ustring& filename)
{
  vector <ustring> lines;
  for (unsigned int i = 0; i < references.size(); i++) {
    lines.push_back(references[i].human_readable(""));
  }
  try {
    write_lines(filename, lines);
  }
  catch(exception & ex) {
    cerr << "Saving references: " << ex.what() << endl;
  }
}


void WindowReferences::clear()
{
  dismiss (false, true);
  html_link_clicked ("");
}


gboolean WindowReferences::on_html_link_clicked(GtkHTML * html, const gchar * url, gpointer user_data)
{
  ((WindowReferences *) user_data)->html_link_clicked(url);
  return true;
}


void WindowReferences::html_link_clicked (const gchar * url)
// Callback for clicking a link.
{
  // Store scrolling position for the now active url.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  scrolling_position[active_url] = gtk_adjustment_get_value (adjustment);

  // New url.
  active_url = url;

  // Start writing a html page.
  HtmlWriter2 htmlwriter ("");
  bool display_another_page = true;

  if (active_url.find ("goto ") == 0) {
    // Signal that a reference was clicked.
    ustring ref (active_url);
    ref.erase (0, 5);
    active_entry = convert_to_int (ref);
    reference.assign (references[active_entry]);
    gtk_button_clicked(GTK_BUTTON(signal_button));
    display_another_page = false;
  }

  else if (active_url.find ("prev") == 0) {
    // Go to the previous page.
    if (lower_boundary) {
      lower_boundary -= 25;
    }
    html_write_references (htmlwriter);
  }

  else if (active_url.find ("next") == 0) {
    // Go to the next page.
    if (lower_boundary < references.size() - 25) {
      lower_boundary += 25;
    }
    html_write_references (htmlwriter);
  }

  else if (active_url.find ("action") == 0) {
    // Go to the action page.
    html_write_action_page (htmlwriter);
  }

  else if (active_url.find ("dismiss") == 0) {
    // Dismiss one reference or a page full of them.
    bool cursor = active_url.find ("cursor") != string::npos;
    bool all = active_url.find ("all") != string::npos;
    dismiss (cursor, all);
    html_write_references (htmlwriter);
  }

  else if (active_url.find ("hide") == 0) {
    // Hide the active reference from now on.
    vector <ustring> hidden_references = references_hidden_ones_load();
    hidden_references.push_back (hide_string (active_entry));
    references_hidden_ones_save(hidden_references);
    dismiss (true, false);
    html_write_references (htmlwriter);
  }

  else {
    // Load the references.
    html_write_references (htmlwriter);
  }
  
  htmlwriter.finish();
  if (display_another_page) {
    // Load the page.
    GtkHTMLStream *stream = gtk_html_begin(GTK_HTML(htmlview));
    gtk_html_write(GTK_HTML(htmlview), stream, htmlwriter.html.c_str(), -1);
    gtk_html_end(GTK_HTML(htmlview), stream, GTK_HTML_STREAM_OK);
    // Scroll to the position that possibly was stored while this url was last active.
    gtk_adjustment_set_value (adjustment, scrolling_position[active_url]);
  }
}


void WindowReferences::html_write_references (HtmlWriter2& htmlwriter)
{
  // Retrieve the reference boundaries, as we're only displaying a selection.
  upper_boundary = lower_boundary + 25;
  upper_boundary = CLAMP (upper_boundary, 0, references.size());

  // Write action bar.
  html_write_action_bar (htmlwriter, true);

  // References page.
  for (unsigned int i = lower_boundary; i < upper_boundary; i++) {
    htmlwriter.paragraph_open();
    ustring url = "goto " + convert_to_string (i);
    htmlwriter.hyperlink_add (url, references[i].human_readable (mylanguage));
    if (!comments[i].empty()) {
      htmlwriter.text_add (" ");
      htmlwriter.text_add (comments[i]);
    }
    htmlwriter.paragraph_close();
  }
  
  // Write action bar.
  html_write_action_bar (htmlwriter, false);
}


void WindowReferences::html_write_action_bar (HtmlWriter2& htmlwriter, bool topbar)
{
  // If there are no references, don't write an action bar at the bottom, only at the top.
  if (!topbar) {
    if (references.empty()) {
      return;
    }
  }

  htmlwriter.paragraph_open ();

  if (references.empty()) {
    htmlwriter.text_add ("no references");
  }

  if (!references.empty()) {
    if (lower_boundary) {
      htmlwriter.hyperlink_add ("prev", "[prev]");
      htmlwriter.text_add (" ");
    }
    htmlwriter.text_add ("Items " + convert_to_string (lower_boundary + 1) + " - " + convert_to_string (upper_boundary) + " of " + convert_to_string (references.size()));
    if (upper_boundary < references.size()) {
      htmlwriter.text_add (" ");
      htmlwriter.hyperlink_add ("next", "[next]");
    }
  }

  htmlwriter.text_add (" ");
  htmlwriter.hyperlink_add ("actions", "[actions]");

  htmlwriter.paragraph_close ();
}


void WindowReferences::html_write_action_page (HtmlWriter2& htmlwriter)
{
  // Write the link for going back to the references.
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("", "[back]");
  htmlwriter.paragraph_close ();
  // If any references has been clicked, offer the option to dismiss it.
  if (active_entry >= 0) {
    htmlwriter.paragraph_open ();
    htmlwriter.hyperlink_add ("dismiss cursor", "Dismiss " + references[active_entry].human_readable (mylanguage));
    htmlwriter.paragraph_close ();
  }
  // If the page has any references, offer the option to dismiss the whole page.
  if (upper_boundary > lower_boundary) {
    htmlwriter.paragraph_open ();
    htmlwriter.hyperlink_add ("dismiss page", "Dismiss the whole page of " + convert_to_string (upper_boundary - lower_boundary) + " references");
    htmlwriter.paragraph_close ();
  }  
  // If there are any references at all, offer the option to dismiss the whole lot.
  if (!references.empty()) {
    htmlwriter.paragraph_open ();
    htmlwriter.hyperlink_add ("dismiss all", "Dismiss the whole lot of " + convert_to_string (references.size()) + " references");
    htmlwriter.paragraph_close ();
  }  
  // If a reference has been clicked, offer the option to hide it from now on.
  if (active_entry >= 0) {
    htmlwriter.paragraph_open ();
    htmlwriter.hyperlink_add ("hide", "Hide \"" + hide_string (active_entry) + "\" from now on");
    htmlwriter.paragraph_close ();
  }
}


ustring WindowReferences::references_database_filename()
// Gives the filename of the database to save the references to.
{
  return gw_build_filename(directories_get_temp(), "references.sqlite3");
}


void WindowReferences::dismiss (bool cursor, bool all)
// Dismiss the reference that was selected last (if cursor = true),
// or the whole page of them,
// or the whole lot (if all = true).
{
  unsigned int low = lower_boundary;
  unsigned int high = upper_boundary;
  if (cursor) {
    low = active_entry;
    high = low + 1;
  }
  if (all) {
    low = 0;
    high = references.size();
  }
  vector <Reference> temporal_references = references;
  vector <ustring> temporal_comments = comments;
  references.clear();
  comments.clear();
  for (unsigned int i = 0; i < temporal_references.size(); i++) {
    if ((i < low) || (i >= high)) {
      references.push_back (temporal_references[i]);
      comments.push_back (temporal_comments[i]);
    }
  }
  active_entry = -1;
  if (all) {
    extern Settings *settings;
    settings->session.highlights.clear();
  }
}


ustring WindowReferences::hide_string (unsigned int index)
// Generates the string that is used in the hiding mechanisms.
{
  ustring hs;
  hs.append (references[index].human_readable (mylanguage));
  if (!comments[index].empty()) {
    hs.append (" ");
    hs.append (comments[index]);
  }
  return hs; 
}


/*


Todo various tasks.


New references window, where text becomes better visible, e.g. it shows the original language (KJV in this case),
and the target language, all in one html file. 


We may have to introduce <next 25> or <previous 25> hyperlinks. This would keep the system fast in case that many references will be loaded.
Also to have a link [delete page] / [delete entries].


The reference area has a link for settings, and one can set there how many references appear on one page, and which versions are included
in the display.


All actions related to references can be removed from the menu, and put into the html page itself as links.


We should highlight the currently selected reference, so it is easily visible.


Each time references are loaded, the lower boundary needs to be reset to zero.


There is a link to delete the page. If the user clicks on the link at the top, it attempt to load the previous lot, if available.
If the user clicks at the link at the bottom, it attempts to load the next lot, if available.


Once we have moved all to the new system, all old methods can go out.


There's no File / References menu anymore, but all settings and actions are done in the html window itself,
also preferences if there are any. We need to think of export and import / open, these should go there as well.
Also "Dismiss all references before the current one", and the referenes hiding management, so as to unhide these.
All of that goes into a html page.


To remove all menu items under File / References, and to create a new one under the View menu.


Remove all of the references.h/cpp data at the end.



Dismiss page:
- If the user came to action page through the top bar, it shows the previous page after dismissing a page.
- If the user came to the action page through the bottom bar, it shows the next page after dismissing a page.


  if (guifeatures.references_management()) {
Implement the above.


When references are hidden, we should check upon loading these, that the hidden ones are removed, and not displayed,
even if these are offered.

The preferences where the hidden references are managed should also be made accessible from the html page.


The references management gui should be implemented, so that no management is available if this feature has been disabled.




*/
