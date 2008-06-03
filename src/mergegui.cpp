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


#include "utilities.h"
#include <glib.h>
#include "mergegui.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "resource_utils.h"
#include "dialogradiobutton.h"
#include "combobox.h"
#include "compareutils.h"
#include "projectutils.h"
#include "gtkwrappers.h"
#include "git.h"
#include "date_time_utils.h"
#include "unixwrappers.h"
#include "usfmtools.h"
#include "dialogmerge.h"
#include "merge_utils.h"


MergeGUI::MergeGUI (GtkWidget * notebook_vbox)
{
  // Save and initialize varables.
  my_notebook_vbox = notebook_vbox;
  active = false;
  load_gui_event_id = 0;  
  editors_changed_event_id = 0;
  
  // Build GUI.
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (notebook_vbox), vbox1, TRUE, TRUE, 0);

  label6 = gtk_label_new_with_mnemonic ("M_aster project");
  gtk_widget_show (label6);
  gtk_box_pack_start (GTK_BOX (vbox1), label6, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  combobox_master = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_master);
  gtk_box_pack_start (GTK_BOX (vbox1), combobox_master, FALSE, FALSE, 0);

  label7 = gtk_label_new_with_mnemonic ("E_dited project");
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (vbox1), label7, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  combobox_edited = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_edited);
  gtk_box_pack_start (GTK_BOX (vbox1), combobox_edited, FALSE, FALSE, 0);

  label_info = gtk_label_new ("");
  gtk_widget_show (label_info);
  gtk_box_pack_start (GTK_BOX (vbox1), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_info), 0, 0.5);

  editor = new Editor (vbox1, NULL, NULL, "");
  editor->title_bar_show (false);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

  button_previous = gtk_button_new ();
  gtk_widget_show (button_previous);
  gtk_box_pack_start (GTK_BOX (hbox1), button_previous, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_previous), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image6 = gtk_image_new_from_stock ("gtk-go-back", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image6);
  gtk_box_pack_start (GTK_BOX (hbox2), image6, FALSE, FALSE, 0);

  label9 = gtk_label_new_with_mnemonic ("_Previous");
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox2), label9, FALSE, FALSE, 0);

  button_merge = gtk_button_new ();
  gtk_widget_show (button_merge);
  gtk_box_pack_start (GTK_BOX (hbox1), button_merge, TRUE, TRUE, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_merge), alignment1);

  hbox4 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox4);

  image5 = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox4), image5, FALSE, FALSE, 0);

  label8 = gtk_label_new_with_mnemonic ("_Merge");
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox4), label8, FALSE, FALSE, 0);

  button_next = gtk_button_new ();
  gtk_widget_show (button_next);
  gtk_box_pack_start (GTK_BOX (hbox1), button_next, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_next), alignment3);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox3);

  label10 = gtk_label_new_with_mnemonic ("_Next");
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox3), label10, FALSE, FALSE, 0);

  image8 = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image8);
  gtk_box_pack_start (GTK_BOX (hbox3), image8, FALSE, FALSE, 0);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label6), combobox_master);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label7), combobox_edited);  
  
  g_signal_connect ((gpointer) combobox_master, "changed",
                    G_CALLBACK (on_combobox_master_changed),
                    gpointer(this));
  g_signal_connect ((gpointer) combobox_edited, "changed",
                    G_CALLBACK (on_combobox_edited_changed),
                    gpointer(this));
  g_signal_connect ((gpointer) button_previous, "clicked",
                    G_CALLBACK (on_button_previous_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_merge, "clicked",
                    G_CALLBACK (on_button_merge_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_next, "clicked",
                    G_CALLBACK (on_button_next_clicked),
                    gpointer(this));
                    
  // Create signalling buttons.
  editors_get_text_button = gtk_button_new ();
  new_reference_button = gtk_button_new ();
  save_editors_button = gtk_button_new ();
  reload_editors_button = gtk_button_new ();
  
  // Set buttons insensitive.
  gtk_widget_set_sensitive (button_previous, false);
  gtk_widget_set_sensitive (button_merge, false);
  gtk_widget_set_sensitive (button_next, false);
}


MergeGUI::~MergeGUI ()
{
  // Destroy signalling buttons.
  gtk_widget_destroy (editors_get_text_button);
  gtk_widget_destroy (new_reference_button);
  gtk_widget_destroy (save_editors_button);
  gtk_widget_destroy (reload_editors_button);
  // Destroy the editor.
  delete editor;
}


bool MergeGUI::has_focus ()
// Returns true if the merge GUI has focus.
{
  if (GTK_WIDGET_HAS_FOCUS (combobox_master)) return true;
  if (GTK_WIDGET_HAS_FOCUS (combobox_edited)) return true;
  if (editor->has_focus ()) return true;
  if (GTK_WIDGET_HAS_FOCUS (button_previous)) return true;
  if (GTK_WIDGET_HAS_FOCUS (button_merge)) return true;
  if (GTK_WIDGET_HAS_FOCUS (button_next)) return true;
  return false;
}


void MergeGUI::give_focus ()
// Focuses the merge GUI.
{
  gtk_widget_grab_focus (my_notebook_vbox);
  if (!has_focus ()) {
    editor->focus ();
  }
}


void MergeGUI::set_active (bool value)
// Tells the object whether it should be active.
{
  active = value;
  load_gui_delayer ();
}


void MergeGUI::set_master_project ()
{
  // If there is no focused editor, clear project.
  if (!focused_editor) {
    current_master_project.clear ();
  }

  // Load available projects.
  vector <ustring> projects;
  for (unsigned int i = 0; i < visible_editors.size (); i++) {
    projects.push_back (visible_editors[i]->project);
  }
  combobox_set_strings (combobox_master, projects);

  // Show previous project, if it is there.
  bool project_found = false;
  for (unsigned int i = 0; i < projects.size (); i++) {
    if (previous_master_project == projects[i]) {
      combobox_set_index (combobox_master, i);
      project_found = true;
    }
  }
  if (!project_found) {
    combobox_set_index (combobox_master, -1);
  }

  // Simulate editors changed, so it would load the differences.
  editors_changed ();
}


void MergeGUI::on_combobox_master_changed (GtkComboBox *combobox, gpointer user_data)
{
  ((MergeGUI *) user_data)->on_combobox_master ();
}


void MergeGUI::on_combobox_master ()
{
  // Get the new master project.
  current_master_project = combobox_get_active_string (combobox_master);
  if (!current_master_project.empty()) {
    previous_master_project = current_master_project;
    if (current_master_project == current_edited_project) {
      combobox_set_index (combobox_edited, -1);
    }
  }
  
  // Load master project and related data in the Editor object.
  editor->project = current_master_project;
  editor->create_or_update_formatting_data ();

  // Simulate editors changed, so it would load the differences.
  editors_changed ();
}


void MergeGUI::set_edited_project ()
{
  // If there is no focused editor, clear project.
  if (!focused_editor) {
    current_edited_project.clear ();
  }

  // Load available projects.
  vector <ustring> projects;
  for (unsigned int i = 0; i < visible_editors.size (); i++) {
    projects.push_back (visible_editors[i]->project);
  }
  combobox_set_strings (combobox_edited, projects);

  // Show previous project, if it is there.
  bool project_found = false;
  for (unsigned int i = 0; i < projects.size (); i++) {
    if (previous_edited_project == projects[i]) {
      combobox_set_index (combobox_edited, i);
      project_found = true;
    }
  }
  if (!project_found) {
    combobox_set_index (combobox_edited, -1);
  }

  // Simulate editors changed, so it would load the differences.
  editors_changed ();
}


void MergeGUI::on_combobox_edited_changed (GtkComboBox *combobox, gpointer user_data)
{
  ((MergeGUI *) user_data)->on_combobox_edited ();
}


void MergeGUI::on_combobox_edited ()
{
  current_edited_project = combobox_get_active_string (combobox_edited);
  if (!current_edited_project.empty()) {
    previous_edited_project = current_edited_project;
    if (current_edited_project == current_master_project) {
      combobox_set_index (combobox_master, -1);
    }
  }
  // Simulate editors changed, so it would load the differences.
  editors_changed ();
}


void MergeGUI::set_focused_editor (Editor * editor)
{
  focused_editor = editor;
  load_gui_delayer ();
}


void MergeGUI::set_visible_editors (const vector <Editor *>& editors)
{
  visible_editors = editors;
  load_gui_delayer ();
}


void MergeGUI::load_gui_delayer ()
{
  gw_destroy_source (load_gui_event_id);
  load_gui_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 10, GSourceFunc (on_load_gui_timeout), gpointer(this), NULL);
}


bool MergeGUI::on_load_gui_timeout (gpointer user_data)
{
  ((MergeGUI *) user_data)->load_gui ();
  return false;
}


void MergeGUI::load_gui ()
{
  // If the merger is not active, bail out.
  if (!active) return;
  
  // Set master and edited projects.
  set_master_project ();
  set_edited_project ();
  
  // Give info if there are not enough projects.
  if (visible_editors.size () < 2) {
    gtk_label_set_text (GTK_LABEL (label_info), "Not enough projects opened");
    gtk_widget_show (label_info);
  } else {
    gtk_widget_hide (label_info);
  }
  
  // Simulate editors changed, so it would load the differences.
  editors_changed ();
}


void MergeGUI::editors_changed ()
// This function is called if any of the Editors changed.
{
  gw_destroy_source (editors_changed_event_id);
  editors_changed_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 500, GSourceFunc (on_editors_changed_timeout), gpointer(this), NULL);
}


bool MergeGUI::on_editors_changed_timeout (gpointer user_data)
{
  ((MergeGUI *) user_data)->on_editors_changed ();
  return false;
}


void MergeGUI::on_editors_changed ()
// This function is called shortly after any of the Editors changed.
{
  // See whether to load new text.
  bool loadtext = true;
  if (!active) loadtext = false;
  if (current_master_project.empty ()) loadtext = false;;
  if (current_edited_project.empty ()) loadtext = false;;

  // Set buttons' sensitivity.
  gtk_widget_set_sensitive (button_previous, loadtext);
  gtk_widget_set_sensitive (button_merge, loadtext);
  gtk_widget_set_sensitive (button_next, loadtext);
  
  // Container with text to load.
  vector <ustring> lines;

  // If no loading, clear text and bail out.
  if (!loadtext) {
    editor->chapter_load (0, &lines);
    return;
  }
  
  // Signal to get the text from the editors.
  gtk_button_clicked (GTK_BUTTON (editors_get_text_button));

  // Make comparison.
  ParseLine parseline_main (main_project_data);
  ParseLine parseline_edited (edited_project_data);
  vector <ustring> comparison;
  compare_chapter (parseline_edited.lines, parseline_main.lines, comparison);
  
  // Load text.
  editor->chapter_load (0, &comparison);
  
  // Set the merge button sensitive if there's a difference.
  gtk_widget_set_sensitive (button_merge, main_project_data != edited_project_data);
}


void MergeGUI::on_button_previous_clicked (GtkButton *button, gpointer user_data)
{
  ((MergeGUI *) user_data)->on_button_next_previous (false);
}


void MergeGUI::on_button_next_clicked (GtkButton *button, gpointer user_data)
{
  ((MergeGUI *) user_data)->on_button_next_previous (true);
}


void MergeGUI::on_button_next_previous (bool next)
// This function looks for the next (or previous) chapter that differs
// in the two projects.
{
  // Variables that control the loop.
  bool change_found = false;
  bool more_chapters_available = true;
  unsigned int new_chapter = chapter;
  unsigned int new_book = book;
  do {
    // Look for next (or previous) chapter.
    vector <unsigned int> chapters = project_get_chapters (current_edited_project, new_book);
    if (chapters.empty ()) break;
    unsigned int index = 0;
    for (unsigned int i = 0; i < chapters.size (); i++) {
      if (new_chapter == chapters[i])
        index = i;
    }
    if (next) {
      if (index == (chapters.size () - 1)) {
        if (!cross_book_boundaries (true, new_book, new_chapter)) {
          more_chapters_available = false;
        }
      } else {
        new_chapter = chapters[++index];
      }
    } else {
      if (index == 0) {
        if (!cross_book_boundaries (false, new_book, new_chapter)) {
          more_chapters_available = false;
        }
      } else {
        new_chapter = chapters[--index];
      }
    }
    // See whether this chapter differs.
    vector <ustring> master_chapter_data = project_retrieve_chapter (current_master_project, new_book, new_chapter);
    vector <ustring> edited_chapter_data = project_retrieve_chapter (current_edited_project, new_book, new_chapter);
    if (master_chapter_data.size () != edited_chapter_data.size ()) change_found = true;
    for (unsigned int i = 0; i < master_chapter_data.size (); i++) {
      if (!change_found) {
        if (master_chapter_data[i] != edited_chapter_data[i]) 
          change_found = true;
      }
    }
  } while (!change_found && more_chapters_available);
  // Deal with whether a change was found.
  if (change_found) {
    book = new_book;
    chapter = new_chapter;
    gtk_button_clicked (GTK_BUTTON (new_reference_button));
  } else {
    gtkw_dialog_info (NULL, "No more differing chapters found");
  }
}


bool MergeGUI::cross_book_boundaries (bool next, unsigned int& cross_book, unsigned int& cross_chapter)
// This function looks for the next (or previous) chapter,
// crossing the book boundary.
// It returns true if it found another chapter in another book.
{
  // Index of the current book.
  int bookindex = -1;
  vector<unsigned int> allbooks = project_get_books (current_master_project);
  for (unsigned int i = 0; i < allbooks.size (); i++) {
    if (cross_book == allbooks[i])
      bookindex = i;
  }

  // If the requested book does not exist, bail out.
  if (bookindex < 0) {
    return false;
  }

  // Get the previous book, and the next book.
  int previousbookindex = bookindex - 1;
  previousbookindex = CLAMP (previousbookindex, 0, bookindex);
  unsigned int nextbookindex = bookindex + 1;
  nextbookindex = CLAMP (nextbookindex, 0, allbooks.size() - 1);

  // Get a list of all references in these three books.
  vector<unsigned int> books;
  vector<unsigned int> chapters;
  for (unsigned int i = previousbookindex; i <= nextbookindex; i++) {
    // Get the book metrics.
    vector<unsigned int> bookchapters = project_get_chapters (current_master_project, allbooks[i]);
    for (unsigned int i2 = 0; i2 < bookchapters.size(); i2++) {
      books.push_back (allbooks[i]);
      chapters.push_back (bookchapters[i2]);
    }
  }

  // Find the current reference in the list.
  unsigned int referenceindex = 0;
  for (unsigned int i = 0; i < books.size(); i++) {
    if (cross_chapter == chapters[i]) {
      if (cross_book == books[i]) {
        referenceindex = i;
        break;
      }
    }
  }
  
  // Get the next or previous value.
  if (next) {
    if (referenceindex == (chapters.size() - 1))
      return false;
    referenceindex++;
  } else {
    if (referenceindex == 0)
      return false;
    referenceindex--;
  }

  // Ok, give us the new book and chapter.
  cross_book = books[referenceindex];
  cross_chapter = chapters[referenceindex];
  return true;
}


void MergeGUI::on_button_merge_clicked (GtkButton *button, gpointer user_data)
{
  ((MergeGUI *) user_data)->on_button_merge ();
}


void MergeGUI::on_button_merge ()
{
  // Stop git tasks.
  git_command_pause (true);
  
  // Save all editors.
  gtk_button_clicked (GTK_BUTTON (save_editors_button));
  
  // Ask what to do.
  ustring book_chapter = books_id_to_english (book) + " " + convert_to_string (chapter);
  vector <ustring> labels;
  labels.push_back ("Merge " + book_chapter + " of project " + current_edited_project + " and " + current_master_project);
  labels.push_back ("Merge " + book_chapter + " of project " + current_edited_project + " and " + current_master_project + ", and review each change as compared to project " + current_master_project);
  labels.push_back ("Copy " + book_chapter + " of project " + current_master_project + " to project " + current_edited_project);
  labels.push_back ("Copy everything of project " + current_master_project + " to project " + current_edited_project);
  RadiobuttonDialog dialog ("Select action", "Select the type of merge or copy to be done", labels, 0);
  if (dialog.run () != GTK_RESPONSE_OK) return;
  // Take the selected action.
  switch (dialog.selection) {
    case 0:
    {
      merge_edited_into_master ();
      break;
    }
    case 1:
    {
      //merge_edited_into_master (); // Todo
      break;
    }
    case 2:
    {
      copy_master_to_edited_chapter (book, chapter, true);
      break;
    }
    case 3:
    {
      copy_master_to_edited_all ();
      break;
    }
  }

  // Reload the editors.
  gtk_button_clicked (GTK_BUTTON (reload_editors_button));

  // Start git tasks.
  git_command_pause (false);
}


void MergeGUI::merge_edited_into_master ()
// This merges the edited data into the master data, and does error checking.
{
  // Progress.
  ProgressWindow progresswindow ("Merging...", false);

  // Get the commits of the master project.
  progresswindow.set_fraction (0.3);
  progresswindow.set_text ("reading log");
  ustring masterchapterdirectory = project_data_directory_chapter (current_master_project, book, chapter);
  ustring masterpath = project_data_filename_chapter (current_master_project, book, chapter, false);
  masterpath = gw_path_get_basename (masterpath);
  vector <ustring> mastercommits;
  vector <unsigned int> masterseconds;
  git_log_read (masterchapterdirectory, mastercommits, masterseconds, masterpath);

  // Get the commits of the edited project.
  progresswindow.set_fraction (0.6);
  ustring editedchapterdirectory = project_data_directory_chapter (current_edited_project, book, chapter);
  ustring editedpath = project_data_filename_chapter (current_edited_project, book, chapter, false);
  editedpath = gw_path_get_basename (editedpath);
  vector <ustring> editedcommits;
  vector <unsigned int> editedseconds;
  git_log_read (editedchapterdirectory, editedcommits, editedseconds, editedpath);

  // Get the texts belonging to the commits of the master project.
  progresswindow.set_iterate (0, 1, mastercommits.size());
  progresswindow.set_text ("reading history");
  vector <ustring> mastertexts;
  for (unsigned int i = 0; i < mastercommits.size(); i++) {
    progresswindow.iterate ();
    vector <ustring> lines = git_retrieve_chapter_commit (current_master_project, book, chapter, mastercommits[i]);
    ustring line;
    for (unsigned int i = 0; i < lines.size (); i++) {
      line.append (lines[i]);
      line.append ("\n");
    }
    mastertexts.push_back (line);
  }
  
  // Get the texts belonging to the commits of the edited project.
  progresswindow.set_iterate (0, 1, editedcommits.size());
  vector <ustring> editedtexts;
  for (unsigned int i = 0; i < editedcommits.size(); i++) {
    progresswindow.iterate ();
    vector <ustring> lines = git_retrieve_chapter_commit (current_edited_project, book, chapter, editedcommits[i]);
    ustring line;
    for (unsigned int i = 0; i < lines.size (); i++) {
      line.append (lines[i]);
      line.append ("\n");
    }
    editedtexts.push_back (line);
  }
  
  // Compare them till a common ancestor has been found.
  progresswindow.set_iterate (0, 1, mastertexts.size());
  progresswindow.set_text ("finding ancestor");
  ustring common_ancestor;
  for (unsigned int m = 0; m < mastertexts.size (); m++) {
    progresswindow.iterate ();
    for (unsigned int e = 0; e < editedtexts.size (); e++) {
      if (common_ancestor.empty ()) {
        if (mastertexts[m] == editedtexts[e]) {
          common_ancestor = mastertexts[m];
        }
      }
    }
  }
  
  // If no common ancestor was found, give message and bail out.
  if (common_ancestor.empty ()) {
    gtkw_dialog_error (NULL, "Can't merge because a common ancestor was not found");
    return;
  }
  
  // Do the merge in a temporal directory.
  ustring workingdirectory = gw_build_filename (directories_get_temp (), "merge");
  unix_rmdir (workingdirectory);
  gw_mkdir_with_parents (workingdirectory);
  
  /*
  Merge works with file1, file2 and file3.
  
  merge [ options ] file1 file2 file3

  merge incorporates all changes that lead from file2 to file3 into file1.
  The result ordinarily goes into file1.
  merge is useful for combining separate changes to an original. 
  Suppose file2 is the original, and both file1 and file3 are modifications of file2. 
  Then merge combines both changes.
  */
  ustring file1 = gw_build_filename (workingdirectory, "file1");
  ustring file2 = gw_build_filename (workingdirectory, "file2");
  ustring file3 = gw_build_filename (workingdirectory, "file3");
  
  /*
  merge has problems when two consecutive lines are changed, 
  one line in one file and the other line in the other file. 
  Therefore data is going to be cut on the spaces, 
  so that there is one word per line. 
  Each new line is indicated too so as to facilitate joining the loose bits again.
  Another advantage of this is that the merge operation becomes finer grained.
  */
  
  // Write the data for the common ancestor.
  g_file_set_contents (file2.c_str(), split_data (common_ancestor).c_str(), -1, NULL);
  
  // Write the data for the main project.
  g_file_set_contents (file1.c_str(), split_data (main_project_data).c_str(), -1, NULL);
  
  // Write the data for the edited project.
  g_file_set_contents (file3.c_str(), split_data (edited_project_data).c_str(), -1, NULL);
 
  // Do the three-way merge.
  {
    GwSpawn spawn ("merge");
    spawn.workingdirectory (workingdirectory);
    spawn.arg (file1);
    spawn.arg (file2);
    spawn.arg (file3);
    spawn.run ();
  }    
  
  // Read the result of the merge.
  ustring merge_result;
  {
    gchar * contents;
    g_file_get_contents (file1.c_str(), &contents, NULL, NULL);
    if (contents) {
      merge_result = contents;
      g_free (contents);
    }
  }
  
  // Make conflicts human readable.
  merge_result = merge_conflicts_2_human_readable_text (merge_result);
  
  // Join the bits again.
  merge_result = join_data (merge_result);

  // If there are conflicts, resolve them.
  if (merge_result.find (merge_conflict_markup (1)) != string::npos) {
    MergeDialog dialog (merge_result);
    if (dialog.run () == GTK_RESPONSE_OK) {
      merge_result = dialog.reconciled_text;
    }
  }

  // If there are still conflicts, give a message and bail out.
  if (merge_result.find (merge_conflict_markup (1)) != string::npos) {
    gtkw_dialog_error (NULL, "The chapters were not merged");
    return;    
  }
  
  // Store the merge result in both chapters.  
  ParseLine parseline (merge_result);
  CategorizeChapterVerse ccv (parseline.lines);    
  project_store_chapter (current_master_project, book, ccv);
  project_store_chapter (current_edited_project, book, ccv);
  
  // Message ok.
  gtkw_dialog_info (NULL, "The chapters were successfully merged");
}


void MergeGUI::copy_master_to_edited_chapter (unsigned int bk, unsigned int ch, bool gui)
{
  vector <ustring> lines = project_retrieve_chapter (current_master_project, bk, ch);
  CategorizeChapterVerse ccv (lines);    
  project_store_chapter (current_edited_project, bk, ccv);
  if (gui) {
    ustring message = books_id_to_english (bk) + " " + convert_to_string (ch) + " was copied from project " + current_master_project + " to project " + current_edited_project;
    gtkw_dialog_info (NULL, message.c_str());
  }
}


void MergeGUI::copy_master_to_edited_all ()
{
  {
    vector <unsigned int> books = project_get_books (current_master_project);
    ProgressWindow progresswindow ("Copying...", false);
    progresswindow.set_iterate (0, 1, books.size());
    for (unsigned int bk = 0; bk < books.size(); bk++) {
      progresswindow.iterate ();
      vector <unsigned int> chapters = project_get_chapters (current_master_project, books[bk]);
      for (unsigned int ch = 0; ch < chapters.size(); ch++) {
        copy_master_to_edited_chapter (books[bk], chapters[ch], false);
      }
    }
  }
  ustring message = "All chapters of project " + current_master_project + " were copied to project " + current_edited_project;
  gtkw_dialog_info (NULL, message.c_str());
}


ustring MergeGUI::split_data (const ustring& data)
{
  ustring splitdata;
  ParseLine parseline (data);
  for (unsigned int l = 0; l < parseline.lines.size (); l++) {
    ustring s (parseline.lines[l]);
    ustring v = usfm_extract_marker (s);
    if (v == "v") {
      size_t position = s.find (" ");
      position = CLAMP (position, 0, s.length());
      s = s.substr (0, position);
      s.insert (0, " ");
      v = verse_indicator ();
      for (unsigned int i = 0; i < 10; i++) {
        v.append (s);
      }
      v.append ("\n");
      for (unsigned int i = 0; i < 5; i++) {
        splitdata.append (v);
      }
    }
    Parse parse (parseline.lines[l], false);
    for (unsigned int w = 0; w < parse.words.size (); w++) {
      splitdata.append (parse.words[w]);
      splitdata.append ("\n");
    }
    if (l != (parseline.lines.size () - 1))
      splitdata.append (new_line_indicator ());
  }
  return splitdata;
}


ustring MergeGUI::join_data (const ustring& data)
{
  ustring joineddata;
  Parse parse (data, false, new_line_indicator ());
  for (unsigned int p = 0; p < parse.words.size (); p++) {
    ParseLine parseline (parse.words[p]);
    for (unsigned int pl = 0; pl < parseline.lines.size (); pl++) {
      if (parseline.lines[pl].find (verse_indicator ()) == 0)
        continue;
      joineddata.append (parseline.lines[pl]);
      if (pl != (parseline.lines.size () - 1))
        joineddata.append (" ");
    }
    joineddata.append ("\n");
  }
  return joineddata;
}

  
gchar * MergeGUI::new_line_indicator ()
{
  return "__new__line__indicator__\n";
}


gchar * MergeGUI::verse_indicator ()
{
  return "___verse___indicator___";
}


ustring MergeGUI::merge_conflicts_2_human_readable_text (const ustring& data)
/*
This takes a merge conflict as produced by the merge program,
and converts it to human readable text.
Sample conflict:

<<<<<<< /home/joe/.bibledit_temp/merge/file1
wordone
=======
wordtwo
>>>>>>> /home/joe/.bibledit_temp/merge/file3

*/
{
  // Text to assemble.
  ustring text;

  // Cut the data into lines. That works easier.
  ParseLine parseline (data);
  
  // Process all the lines.
  for (unsigned int i = 0; i < parseline.lines.size (); i++) {
    if (parseline.lines[i].find ("<<<<<<<") == 0) {
      parseline.lines[i] = merge_conflict_markup (1);
    }
    else if (parseline.lines[i] == "=======") {
      parseline.lines[i] = merge_conflict_markup (2);
    }
    else if (parseline.lines[i].find (">>>>>>>") == 0) {
      parseline.lines[i] = merge_conflict_markup (3);
    }

    // Add the line.
    text.append (parseline.lines[i]);
    text.append ("\n");
  }
  
  // Give result.
  return text;
}
