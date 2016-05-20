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
#include <glib.h>
#include "windowmerge.h"
#include "help.h"
#include "floatingwindow.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "resource_utils.h"
#include "dialogradiobutton.h"
#include "combobox.h"
#include "compareutils.h"
#include "gtkwrappers.h"
#include "git.h"
#include "date_time_utils.h"
#include "unixwrappers.h"
#include "usfmtools.h"
#include "dialogmerge.h"
#include "merge_utils.h"
#include "shell.h"
#include "snapshots.h"
#include <glib/gi18n.h>

WindowMerge::WindowMerge(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup):
  FloatingWindow(parent_layout, widMerge, _("Merge"), startup)
// Window for merging changes.  
{
  // Save and initialize variables.
  load_gui_event_id = 0;
  editors_changed_event_id = 0;

  // Build GUI.
  notebook1 = gtk_notebook_new();
  gtk_widget_show(notebook1);
  gtk_container_add(GTK_CONTAINER(vbox_client), notebook1);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook1), FALSE);

  // Build merge GUI.
  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_container_add(GTK_CONTAINER(notebook1), vbox1);

  label6 = gtk_label_new_with_mnemonic(_("M_aster project"));
  gtk_widget_show(label6);
  gtk_box_pack_start(GTK_BOX(vbox1), label6, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  combobox_master = gtk_combo_box_new_text();
  gtk_widget_show(combobox_master);
  gtk_box_pack_start(GTK_BOX(vbox1), combobox_master, FALSE, FALSE, 0);

  connect_focus_signals (combobox_master);
  
  label7 = gtk_label_new_with_mnemonic(_("E_dited project"));
  gtk_widget_show(label7);
  gtk_box_pack_start(GTK_BOX(vbox1), label7, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label7), 0, 0.5);

  combobox_edited = gtk_combo_box_new_text();
  gtk_widget_show(combobox_edited);
  gtk_box_pack_start(GTK_BOX(vbox1), combobox_edited, FALSE, FALSE, 0);

  connect_focus_signals (combobox_edited);

  label_info = gtk_label_new("");
  gtk_widget_show(label_info);
  gtk_box_pack_start(GTK_BOX(vbox1), label_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_info), 0, 0.5);

  display_changes_gui = new DisplayChangesGui (vbox1);

  connect_focus_signals (display_changes_gui->textview);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, FALSE, 0);

  button_previous = gtk_button_new();
  gtk_widget_show(button_previous);
  gtk_box_pack_start(GTK_BOX(hbox1), button_previous, FALSE, FALSE, 0);

  connect_focus_signals (button_previous);
  
  alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment2);
  gtk_container_add(GTK_CONTAINER(button_previous), alignment2);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox2);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox2);

  image6 = gtk_image_new_from_stock("gtk-go-back", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image6);
  gtk_box_pack_start(GTK_BOX(hbox2), image6, FALSE, FALSE, 0);

  label9 = gtk_label_new_with_mnemonic(_("_Previous"));
  gtk_widget_show(label9);
  gtk_box_pack_start(GTK_BOX(hbox2), label9, FALSE, FALSE, 0);

  button_merge = gtk_button_new();
  gtk_widget_show(button_merge);
  gtk_box_pack_start(GTK_BOX(hbox1), button_merge, TRUE, TRUE, 0);

  connect_focus_signals (button_merge);
  
  alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment1);
  gtk_container_add(GTK_CONTAINER(button_merge), alignment1);

  hbox4 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox4);
  gtk_container_add(GTK_CONTAINER(alignment1), hbox4);

  image5 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image5);
  gtk_box_pack_start(GTK_BOX(hbox4), image5, FALSE, FALSE, 0);

  label8 = gtk_label_new_with_mnemonic(_("_Merge"));
  gtk_widget_show(label8);
  gtk_box_pack_start(GTK_BOX(hbox4), label8, FALSE, FALSE, 0);

  button_next = gtk_button_new();
  gtk_widget_show(button_next);
  gtk_box_pack_start(GTK_BOX(hbox1), button_next, FALSE, FALSE, 0);

  connect_focus_signals (button_next);
  
  alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment3);
  gtk_container_add(GTK_CONTAINER(button_next), alignment3);

  hbox3 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox3);
  gtk_container_add(GTK_CONTAINER(alignment3), hbox3);

  label10 = gtk_label_new_with_mnemonic(_("_Next"));
  gtk_widget_show(label10);
  gtk_box_pack_start(GTK_BOX(hbox3), label10, FALSE, FALSE, 0);

  image8 = gtk_image_new_from_stock("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image8);
  gtk_box_pack_start(GTK_BOX(hbox3), image8, FALSE, FALSE, 0);

  // Build approve GUI.
  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox2);
  gtk_container_add(GTK_CONTAINER(notebook1), vbox2);

  label_approve = gtk_label_new("");
  gtk_widget_show(label_approve);
  gtk_box_pack_start(GTK_BOX(vbox2), label_approve, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_approve), 0, 0.5);

  scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow2);
  gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_SHADOW_IN);

  textview_approval = gtk_text_view_new();
  gtk_widget_show(textview_approval);
  gtk_container_add(GTK_CONTAINER(scrolledwindow2), textview_approval);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_approval), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_approval), GTK_WRAP_WORD);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview_approval), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview_approval), FALSE);

  connect_focus_signals (textview_approval);
  
  approve_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_approval));

  button_ready = gtk_button_new();
  gtk_widget_show(button_ready);
  gtk_box_pack_start(GTK_BOX(vbox2), button_ready, FALSE, FALSE, 0);

  connect_focus_signals (button_ready);
  
  alignment6 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment6);
  gtk_container_add(GTK_CONTAINER(button_ready), alignment6);

  hbox7 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox7);
  gtk_container_add(GTK_CONTAINER(alignment6), hbox7);

  image11 = gtk_image_new_from_stock("gtk-ok", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image11);
  gtk_box_pack_start(GTK_BOX(hbox7), image11, FALSE, FALSE, 0);

  label19 = gtk_label_new_with_mnemonic(_("_Save"));
  gtk_widget_show(label19);
  gtk_box_pack_start(GTK_BOX(hbox7), label19, FALSE, FALSE, 0);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label6), combobox_master);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label7), combobox_edited);

  g_signal_connect((gpointer) combobox_master, "changed", G_CALLBACK(on_combobox_master_changed), gpointer(this));
  g_signal_connect((gpointer) combobox_edited, "changed", G_CALLBACK(on_combobox_edited_changed), gpointer(this));
  g_signal_connect((gpointer) button_previous, "clicked", G_CALLBACK(on_button_previous_clicked), gpointer(this));
  g_signal_connect((gpointer) button_merge, "clicked", G_CALLBACK(on_button_merge_clicked), gpointer(this));
  g_signal_connect((gpointer) button_next, "clicked", G_CALLBACK(on_button_next_clicked), gpointer(this));
  g_signal_connect((gpointer) button_ready, "clicked", G_CALLBACK(on_button_ready_clicked), gpointer(this));

  // Create signalling buttons.
  editors_get_text_button = gtk_button_new();
  new_reference_button = gtk_button_new();
  save_editors_button = gtk_button_new();
  reload_editors_button = gtk_button_new();

  // Set buttons insensitive.
  gtk_widget_set_sensitive(button_previous, false);
  gtk_widget_set_sensitive(button_merge, false);
  gtk_widget_set_sensitive(button_next, false);

  // Main focused widget.
  last_focused_widget = combobox_master;
  gtk_widget_grab_focus (last_focused_widget);
}


WindowMerge::~WindowMerge()
{
  // Destroy changes GUI.
  delete display_changes_gui;
  // Destroy signalling buttons.
  gtk_widget_destroy(editors_get_text_button);
  gtk_widget_destroy(new_reference_button);
  gtk_widget_destroy(save_editors_button);
  gtk_widget_destroy(reload_editors_button);
}


void WindowMerge::set_master_project()
{
  // If there is no focused editor, clear project.
  if (!editor_was_focused) {
    current_master_project.clear();
  }
  // Load available projects.
  combobox_set_strings(combobox_master, open_projects);

  // Show previous project, if it is there.
  bool project_found = false;
  for (unsigned int i = 0; i < open_projects.size(); i++) {
    if (previous_master_project == open_projects[i]) {
      combobox_set_index(combobox_master, i);
      project_found = true;
    }
  }
  if (!project_found) {
    combobox_set_index(combobox_master, -1);
  }
  // Simulate editors changed, so it would load the differences.
  editors_changed();
}


void WindowMerge::on_combobox_master_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((WindowMerge *) user_data)->on_combobox_master();
}


void WindowMerge::on_combobox_master()
{
  // Get the new master project.
  current_master_project = combobox_get_active_string(combobox_master);
  if (!current_master_project.empty()) {
    previous_master_project = current_master_project;
    if (current_master_project == current_edited_project) {
      combobox_set_index(combobox_edited, -1);
    }
  }
  // Simulate editors changed, so it would load the differences.
  editors_changed();
}


void WindowMerge::set_edited_project()
{
  // If there is no focused editor, clear project.
  if (!editor_was_focused) {
    current_edited_project.clear();
  }
  // Load available projects.
  combobox_set_strings(combobox_edited, open_projects);

  // Show previous project, if it is there.
  bool project_found = false;
  for (unsigned int i = 0; i < open_projects.size(); i++) {
    if (previous_edited_project == open_projects[i]) {
      combobox_set_index(combobox_edited, i);
      project_found = true;
    }
  }
  if (!project_found) {
    combobox_set_index(combobox_edited, -1);
  }
  // Simulate editors changed, so it would load the differences.
  editors_changed();
}


void WindowMerge::on_combobox_edited_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((WindowMerge *) user_data)->on_combobox_edited();
}


void WindowMerge::on_combobox_edited()
{
  current_edited_project = combobox_get_active_string(combobox_edited);
  if (!current_edited_project.empty()) {
    previous_edited_project = current_edited_project;
    if (current_edited_project == current_master_project) {
      combobox_set_index(combobox_master, -1);
    }
  }
  // Simulate editors changed, so it would load the differences.
  editors_changed();
}


void WindowMerge::set_focused_editor()
{
  editor_was_focused = true;
  load_gui_delayer();
}


void WindowMerge::set_open_projects(const vector < ustring>& projects)
{
  open_projects = projects;
  load_gui_delayer();
}


void WindowMerge::load_gui_delayer()
{
  gw_destroy_source(load_gui_event_id);
  load_gui_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 10, GSourceFunc(on_load_gui_timeout), gpointer(this), NULL);
}


bool WindowMerge::on_load_gui_timeout(gpointer user_data)
{
  ((WindowMerge *) user_data)->load_gui();
  return false;
}


void WindowMerge::load_gui()
{
  // Set master and edited projects.
  set_master_project();
  set_edited_project();

  // Give info if there are not enough projects.
  if (open_projects.size() < 2) {
    gtk_label_set_text(GTK_LABEL(label_info), _("Not enough projects opened"));
    gtk_widget_show(label_info);
  } else {
    gtk_widget_hide(label_info);
  }

  // Simulate editors changed, so it would load the differences.
  editors_changed();
}


void WindowMerge::editors_changed()
// This function is called if any of the Editors changed.
{
  gw_destroy_source(editors_changed_event_id);
  editors_changed_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(on_editors_changed_timeout), gpointer(this), NULL);
}


bool WindowMerge::on_editors_changed_timeout(gpointer user_data)
{
  ((WindowMerge *) user_data)->on_editors_changed();
  return false;
}


void WindowMerge::on_editors_changed()
// This function is called shortly after any of the Editors changed.
{
  // See whether to load new text.
  bool loadtext = true;
  if (current_master_project.empty())
    loadtext = false;;
  if (current_edited_project.empty())
    loadtext = false;;

  // Set buttons' sensitivity.
  gtk_widget_set_sensitive(button_previous, loadtext);
  gtk_widget_set_sensitive(button_merge, loadtext);
  gtk_widget_set_sensitive(button_next, loadtext);

  // If no loading, clear text and bail out.
  if (!loadtext) {
    display_changes_gui->clear ();
    return;
  }
  // Signal to get the text from the editors.
  gtk_button_clicked(GTK_BUTTON(editors_get_text_button));

  // Show the comparison.
  show_comparison();
}


void WindowMerge::on_button_previous_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowMerge *) user_data)->on_button_next_previous(false);
}


void WindowMerge::on_button_next_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowMerge *) user_data)->on_button_next_previous(true);
}


void WindowMerge::on_button_next_previous(bool next)
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
    vector < unsigned int >chapters = project_get_chapters(current_edited_project, new_book);
    if (chapters.empty())
      break;
    unsigned int index = 0;
    for (unsigned int i = 0; i < chapters.size(); i++) {
      if (new_chapter == chapters[i])
        index = i;
    }
    if (next) {
      if (index == (chapters.size() - 1)) {
        if (!cross_book_boundaries(true, new_book, new_chapter)) {
          more_chapters_available = false;
        }
      } else {
        new_chapter = chapters[++index];
      }
    } else {
      if (index == 0) {
        if (!cross_book_boundaries(false, new_book, new_chapter)) {
          more_chapters_available = false;
        }
      } else {
        new_chapter = chapters[--index];
      }
    }
    // See whether this chapter differs.
    vector < ustring > master_chapter_data = project_retrieve_chapter(current_master_project, new_book, new_chapter);
    vector < ustring > edited_chapter_data = project_retrieve_chapter(current_edited_project, new_book, new_chapter);
    if (master_chapter_data.size() != edited_chapter_data.size())
      change_found = true;
    for (unsigned int i = 0; i < master_chapter_data.size(); i++) {
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
    gtk_button_clicked(GTK_BUTTON(new_reference_button));
  } else {
    gtkw_dialog_info(NULL, _("No more differing chapters found"));
  }
}


bool WindowMerge::cross_book_boundaries(bool next, unsigned int &cross_book, unsigned int &cross_chapter)
// This function looks for the next (or previous) chapter,
// crossing the book boundary.
// It returns true if it found another chapter in another book.
{
  // Index of the current book.
  int bookindex = -1;
  vector < unsigned int >allbooks = project_get_books(current_master_project);
  for (unsigned int i = 0; i < allbooks.size(); i++) {
    if (cross_book == allbooks[i])
      bookindex = i;
  }

  // If the requested book does not exist, bail out.
  if (bookindex < 0) {
    return false;
  }
  // Get the previous book, and the next book.
  int previousbookindex = bookindex - 1;
  previousbookindex = clamp(previousbookindex, 0, bookindex);
  unsigned int nextbookindex = bookindex + 1;
  nextbookindex = CLAMP(nextbookindex, 0, allbooks.size() - 1);

  // Get a list of all references in these three books.
  vector < unsigned int >books;
  vector < unsigned int >chapters;
  for (unsigned int i = previousbookindex; i <= nextbookindex; i++) {
    // Get the book metrics.
    vector < unsigned int >bookchapters = project_get_chapters(current_master_project, allbooks[i]);
    for (unsigned int i2 = 0; i2 < bookchapters.size(); i2++) {
      books.push_back(allbooks[i]);
      chapters.push_back(bookchapters[i2]);
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


void WindowMerge::on_button_merge_clicked(GtkButton * button, gpointer user_data)
{
  ((WindowMerge *) user_data)->on_button_merge();
}


void WindowMerge::on_button_merge()
{
  // Settings.
  extern Settings * settings;
  
  // Save all editors.
  gtk_button_clicked(GTK_BUTTON(save_editors_button));

  // Ask what to do.
  ustring book_chapter = books_id_to_english(book) + " " + convert_to_string(chapter);
  vector < ustring > labels;
  labels.push_back(_("Merge ") + book_chapter + _(" of project ") + current_edited_project + _(" and ") + current_master_project);
  labels.push_back(_("Merge ") + book_chapter + _(" of project ") + current_edited_project + _(" and ") + current_master_project + ",\n" + _("and approve of each change as compared to project ") + current_master_project);
  labels.push_back(_("Copy ") + book_chapter + _(" of project ") + current_master_project + _(" to project ") + current_edited_project);
  labels.push_back(_("Copy everything of project ") + current_master_project + _(" to project ") + current_edited_project);
  RadiobuttonDialog dialog(_("Select action"), _("Select the type of merge or copy to be done"), labels, settings->session.merge_action, false);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  // Store action taken.
  settings->session.merge_action = dialog.selection;
  // Take the selected action.
  switch (dialog.selection) {
  case 0:
    {
      merge_edited_into_master(false);
      break;
    }
  case 1:
    {
      merge_edited_into_master(true);
      break;
    }
  case 2:
    {
      copy_master_to_edited_chapter(book, chapter, true);
      break;
    }
  case 3:
    {
      copy_master_to_edited_all();
      break;
    }
  }

  // Reload the editors.
  gtk_button_clicked(GTK_BUTTON(reload_editors_button));
}


void WindowMerge::merge_edited_into_master(bool approve)
// This merges the edited data into the master data, and does error checking.
{
  // Bail out if there's nothing to merge.
  if (main_project_data == edited_project_data) {
    gtkw_dialog_info(NULL, _("Both the chapters already are the same"));
    return;
  }

  // Get the available snapshots of the master and edited projects.
  vector <unsigned int> masterseconds = snapshots_get_seconds (current_master_project, book, chapter);
  vector <unsigned int> editedseconds = snapshots_get_seconds (current_edited_project, book, chapter);

  // We need to look for the common ancestor.
  // It needs a fast routine that goes through the history as little as possible.

  // Make a combined set of the times and flags.
  vector <bool> combinedflags;
  vector <unsigned int> combinedseconds;
  for (unsigned int i = 0; i < masterseconds.size(); i++) {
    combinedflags.push_back (true);
    combinedseconds.push_back (masterseconds[i]);    
  }
  for (unsigned int i = 0; i < editedseconds.size(); i++) {
    combinedflags.push_back (false);
    combinedseconds.push_back (editedseconds[i]);    
  }
  // Sort the combined set on the time, most recent ones first.
  quick_sort (combinedseconds, combinedflags, 0, combinedseconds.size());
  {
    vector <bool> flags = combinedflags;
    vector <unsigned int> seconds = combinedseconds;
    combinedflags.clear();
    combinedseconds.clear();
    for (int i = flags.size() - 1; i >= 0; i--) {
      combinedflags.push_back (flags[i]);
      combinedseconds.push_back (seconds[i]);
    }
  }

  // Go through the history of both projects, extract the state in history,
  // and compare them in order to find the common ancestor.
  vector <ustring> mastertexts;
  vector <ustring> editedtexts;
  ustring common_ancestor;
  for (unsigned int i = 0; i < combinedseconds.size(); i++) {
    unsigned int second = combinedseconds[i];
    bool master = combinedflags[i];
    if (master) {
      mastertexts.push_back(snapshots_get_chapter(current_master_project, book, chapter, second));
    } else {
      editedtexts.push_back(snapshots_get_chapter(current_edited_project, book, chapter, second));
    }
    for (unsigned int m = 0; m < mastertexts.size(); m++) {
      for (unsigned int e = 0; e < editedtexts.size(); e++) {
        if (common_ancestor.empty()) {
          if (mastertexts[m] == editedtexts[e]) {
            common_ancestor = mastertexts[m];
          }
        }
      }
    }
    if (!common_ancestor.empty()) {
      break;
    }
  }  

  // If no common ancestor was found, give message and bail out.
  if (common_ancestor.empty()) {
    gtkw_dialog_error(NULL, _("Can't merge because a common ancestor was not found"));
    return;
  }
  // Do the merge in a temporal directory.
  workingdirectory = gw_build_filename(Directories->get_temp(), "merge");
  unix_rmdir(workingdirectory);
  gw_mkdir_with_parents(workingdirectory);

  /*
     Merge works with file1, file2 and file3.

     merge [ options ] file1 file2 file3

     merge incorporates all changes that lead from file2 to file3 into file1.
     The result ordinarily goes into file1.
     merge is useful for combining separate changes to an original. 
     Suppose file2 is the original, and both file1 and file3 are modifications of file2. 
     Then merge combines both changes.
   */
  ustring file1 = gw_build_filename(workingdirectory, "file1");
  ustring file2 = gw_build_filename(workingdirectory, "file2");
  ustring file3 = gw_build_filename(workingdirectory, "file3");

  /*
     merge has problems when two consecutive lines are changed, 
     one line in one file and the other line in the other file. 
     Therefore data is going to be cut on the spaces, 
     so that there is one word per line. 
     Each new line is indicated too so as to facilitate joining the loose bits again.
     Another advantage of this is that the merge operation becomes finer grained.
   */

  // Write the data for the common ancestor.
  g_file_set_contents(file2.c_str(), merge_split_data(common_ancestor).c_str(), -1, NULL);

  // Write the data for the main project.
  g_file_set_contents(file1.c_str(), merge_split_data(main_project_data).c_str(), -1, NULL);

  // Write the data for the edited project.
  g_file_set_contents(file3.c_str(), merge_split_data(edited_project_data).c_str(), -1, NULL);

  // Do the three-way merge.
  {
    GwSpawn spawn("merge");
    spawn.workingdirectory(workingdirectory);
    spawn.arg(file1);
    spawn.arg(file2);
    spawn.arg(file3);
    spawn.run();
  }

  // Read the result of the merge.
  ustring merge_result;
  {
    gchar *contents;
    g_file_get_contents(file1.c_str(), &contents, NULL, NULL);
    if (contents) {
      merge_result = contents;
      g_free(contents);
    }
  }

  // Make conflicts human readable.
  merge_result = merge_conflicts_2_human_readable_text(merge_result);

  // Join the bits again.
  merge_result = merge_join_data(merge_result);

  // If there are conflicts, resolve them.
  if (merge_result.find(merge_conflict_markup(1)) != string::npos) {
    MergeDialog dialog(merge_result);
    if (dialog.run() == GTK_RESPONSE_OK) {
      merge_result = dialog.reconciled_text;
    }
  }
  // If there are still conflicts, give a message and bail out.
  if (merge_result.find(merge_conflict_markup(1)) != string::npos) {
    gtkw_dialog_error(NULL, _("The chapters were not merged"));
    return;
  }

  if (approve) {

    // Setup the approval system.
    approval_setup(main_project_data, merge_result);

  } else {

    // Store the merge result in both chapters.  
    ParseLine parseline(merge_result);
    CategorizeChapterVerse ccv(parseline.lines);
    project_store_chapter(current_master_project, book, ccv);
    project_store_chapter(current_edited_project, book, ccv);
    // A normal snapshot may be removed over time, so we need a persistent one to enable future merges.
    snapshots_shoot_chapter (current_master_project, book, chapter, 0, true);
    snapshots_shoot_chapter (current_edited_project, book, chapter, 0, true);

    // Message ok.
    gtkw_dialog_info(NULL, _("The chapters were successfully merged"));

  }
}


void WindowMerge::copy_master_to_edited_chapter(unsigned int bk, unsigned int ch, bool gui)
{
  // Only copy if the master and edited version differ. This saves a lot of git operations.
  vector <ustring> master_lines = project_retrieve_chapter(current_master_project, bk, ch);
  vector <ustring> edited_lines = project_retrieve_chapter(current_edited_project, bk, ch);
  bool master_is_edited = false;
  if (master_lines.size() == edited_lines.size()) {
    master_is_edited = true;
    for (unsigned int i = 0; i < master_lines.size(); i++) {
      if (master_lines[i] != edited_lines[i]) {
        master_is_edited = false;
      }
    }
  }
  CategorizeChapterVerse ccv(master_lines);
  if (master_is_edited) {
    if (gui) 
      gtkw_dialog_info(NULL, _("Both chapters are already the same"));
  } else {
    project_store_chapter(current_edited_project, bk, ccv);
    // A normal snapshot may be removed over time, so we need a persistent one to enable future merges.
    snapshots_shoot_chapter (current_master_project, bk, ch, 0, true);
    snapshots_shoot_chapter (current_edited_project, bk, ch, 0, true);
    if (gui) {
      ustring message = books_id_to_english(bk) + " " + convert_to_string(ch) + _(" was copied from project ") + current_master_project + _(" to project ") + current_edited_project;
      gtkw_dialog_info(NULL, message.c_str());
    }
  }
}


void WindowMerge::copy_master_to_edited_all()
{
  {
    vector <unsigned int> books = project_get_books(current_master_project);
    ProgressWindow progresswindow(_("Copying..."), false);
    progresswindow.set_iterate(0, 1, books.size());
    for (unsigned int bk = 0; bk < books.size(); bk++) {
      progresswindow.iterate();
      vector < unsigned int >chapters = project_get_chapters(current_master_project, books[bk]);
      for (unsigned int ch = 0; ch < chapters.size(); ch++) {
        copy_master_to_edited_chapter(books[bk], chapters[ch], false);
      }
    }
  }
  ustring message = _("All chapters of project ") + current_master_project + _(" were copied to project ") + current_edited_project;
  gtkw_dialog_info(NULL, message.c_str());
}


ustring WindowMerge::merge_conflicts_2_human_readable_text(const ustring & data)
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
  ParseLine parseline(data);

  // Process all the lines.
  for (unsigned int i = 0; i < parseline.lines.size(); i++) {
    if (parseline.lines[i].find("<<<<<<<") == 0) {
      parseline.lines[i] = merge_conflict_markup(1);
    } else if (parseline.lines[i] == "=======") {
      parseline.lines[i] = merge_conflict_markup(2);
    } else if (parseline.lines[i].find(">>>>>>>") == 0) {
      parseline.lines[i] = merge_conflict_markup(3);
    }
    // Add the line.
    text.append(parseline.lines[i]);
    text.append("\n");
  }

  // Give result.
  return text;
}


void WindowMerge::show_comparison()
// Shows the comparison.
{
  ParseLine parseline_main(main_project_data);
  ParseLine parseline_edited(edited_project_data);
  vector < ustring > comparison;
  compare_usfm_text(parseline_main.lines, parseline_edited.lines, comparison, true);
  display_changes_gui->display (comparison);
}


void WindowMerge::approval_setup(const ustring & maindata, const ustring & mergedata)
{
  // Initialize the approval system's variables and gui.
  approve_master_project = current_master_project;
  approve_edited_project = current_edited_project;
  approve_book = book;
  approve_chapter = chapter;
  ustring label = _("Changes approval, ") + books_id_to_english(approve_book) + " " + convert_to_string(approve_chapter);
  gtk_label_set_text(GTK_LABEL(label_approve), label.c_str());
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1), 1);
  approve_master_file = gw_build_filename(workingdirectory, "master");
  approve_merge_file = gw_build_filename(workingdirectory, "merged");

  // Save both sets of data to file.
  g_file_set_contents(approve_master_file.c_str(), merge_split_data(trim(maindata)).c_str(), -1, NULL);
  g_file_set_contents(approve_merge_file.c_str(), merge_split_data(trim(mergedata)).c_str(), -1, NULL);

  // Show differences in the GUI.
  approval_show_diff();

  // Info for user.
  gtkw_dialog_info(NULL, _("The chapters are ready for approving the individual changes"));
}


void WindowMerge::approval_show_diff()
// Looks for the differences and shows them in the GUI.
{
  // Create a patch file by running a diff.
  approve_patch_file = gw_build_filename(workingdirectory, "patch");
  ustring command = shell_quote_space(Directories->get_diff()) + shell_quote_space(approve_master_file) + shell_quote_space(approve_merge_file) + ">" + shell_quote_space(approve_patch_file);
  if (system(command.c_str())) ; // This one does not work with GwSpawn because of the pipes used.

  // Clear items.
  gtk_text_buffer_set_text(approve_buffer, "", 0);
  approve_buttons.clear();

  // Read the patch.
  vector < Patch > patches = merge_read_patch(approve_patch_file);

  // Show the master file in the textview, with buttons for approval of patches.
  ReadText rt(approve_master_file, true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {

    // Text iterator.
    GtkTextIter iter;

    // Handle new line.
    string s = merge_new_line_indicator();
    if (rt.lines[i].find(trim(s)) != string::npos) {
      gtk_text_buffer_get_end_iter(approve_buffer, &iter);
      gtk_text_buffer_insert(approve_buffer, &iter, "\n", -1);
      continue;
    }
    // Skip verse indicators.
    if (rt.lines[i].find(merge_verse_indicator()) != string::npos) {
      continue;
    }
    // Insert normal text.
    gtk_text_buffer_get_end_iter(approve_buffer, &iter);
    gtk_text_buffer_insert(approve_buffer, &iter, rt.lines[i].c_str(), -1);

    // If there's a patch here, show it.
    for (unsigned int i2 = 0; i2 < patches.size(); i2++) {
      if (i + 1 == patches[i2].linenumber) {    // (diff starts at line 1, but we start at line 0).

        // Add a space before the button.
        gtk_text_buffer_get_end_iter(approve_buffer, &iter);
        gtk_text_buffer_insert(approve_buffer, &iter, " ", 1);

        // Insert a button with the patch.
        gtk_text_buffer_get_end_iter(approve_buffer, &iter);
        GtkTextChildAnchor *childanchor = gtk_text_buffer_create_child_anchor(approve_buffer, &iter);
        GtkWidget *button = gtk_button_new();

        GtkWidget *alignment;
        alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
        gtk_widget_show(alignment);
        gtk_container_add(GTK_CONTAINER(button), alignment);

        GtkWidget *hbox;
        hbox = gtk_hbox_new(FALSE, 2);
        gtk_widget_show(hbox);
        gtk_container_add(GTK_CONTAINER(alignment), hbox);

        GtkWidget *image;
        if (patches[i2].addition)
          image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
        else
          image = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_BUTTON);
        gtk_widget_show(image);
        gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

        GtkWidget *label;
        label = gtk_label_new_with_mnemonic(patches[i2].change.c_str());
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

        gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(textview_approval), button, childanchor);
        gtk_widget_show_all(button);
        g_signal_connect((gpointer) button, "clicked", G_CALLBACK(on_button_approve_clicked), gpointer(this));

        // Store button and patch.
        ApproveButton approvebutton(0);
        approvebutton.button = GTK_BUTTON(button);
        approvebutton.patch = patches[i2];
        approve_buttons.push_back(approvebutton);

      }
    }

    // Add space after the text.
    gtk_text_buffer_get_end_iter(approve_buffer, &iter);
    gtk_text_buffer_insert(approve_buffer, &iter, " ", 1);

  }
}


void WindowMerge::on_button_approve_clicked(GtkButton * button, gpointer user_data)
// Called when the user clicks one of the approval button.
{
  ((WindowMerge *) user_data)->approval_approve(button);
}


void WindowMerge::approval_approve(GtkButton * button)
// Handles the user's approval of a change.
{
  // Go through the approval buttons to find the one clicked.
  for (unsigned int i = 0; i < approve_buttons.size(); i++) {
    if (button == approve_buttons[i].button) {

      // Write a new patch file to disk.
      vector < ustring > lines;
      ustring s;
      s = convert_to_string(approve_buttons[i].patch.linenumber);
      if (approve_buttons[i].patch.addition)
        s.append("a");
      else
        s.append("d");
      s.append("0");
      lines.push_back(s);
      if (approve_buttons[i].patch.addition)
        s = ">";
      else
        s = "<";
      lines.push_back(s + " " + approve_buttons[i].patch.change);
      write_lines(approve_patch_file, lines);

      // Apply the patch to master.
      ustring command = "patch" + shell_quote_space(approve_master_file) + shell_quote_space(approve_patch_file);
      if (system(command.c_str())) ; // This one could use GwSpawn, but it was not tested.

      // Show the new differences after the change has been accepted.
      approval_show_diff();

    }
  }
}


void WindowMerge::on_button_ready_clicked(GtkButton * button, gpointer user_data)
// Called when ready approving.
{
  ((WindowMerge *) user_data)->button_ready_clicked();
}


void WindowMerge::button_ready_clicked()
// Called when ready approving.
{
  // Store the merge result in main project's chapter.
  gchar *contents;
  g_file_get_contents(approve_master_file.c_str(), &contents, NULL, NULL);
  ustring text(contents);
  g_free(contents);
  text = merge_join_data(text);
  ParseLine parseline(text);
  CategorizeChapterVerse ccv(parseline.lines);
  project_store_chapter(approve_master_project, approve_book, ccv);

  // Store persistent snapshots to enable future merges.
  snapshots_shoot_chapter (approve_master_project, approve_book, approve_chapter, 0, true);
  snapshots_shoot_chapter (approve_edited_project, approve_book, approve_chapter, 0, true);

  // GUI.
  gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1), 0);

  // Reload the editors.
  gtk_button_clicked(GTK_BUTTON(reload_editors_button));
}


