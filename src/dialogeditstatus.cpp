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

#include "dialogeditstatus.h"
#include "help.h"
#include "settings.h"
#include "books.h"
#include "tiny_utilities.h"
#include "progresswindow.h"
#include "dialogsetstatus.h"

#define MAX_CHAPTERS 150
#define MAX_VERSES 200

EditStatusDialog::EditStatusDialog(const ustring& project_in, unsigned int book, unsigned int chapter) {
  // Initialize variables.
  project = project_in;
  currentbook = 0;
  currentchapter = 0;
  settingbooks = false;
  settingchapters = false;

  // Settings.
  extern Settings * settings;

  // Load the possible tasks and their parameters.
  alltasks = settings->genconfig.project_tasks_names_get();
  vector <double> alldurations = settings->genconfig.project_tasks_durations_get();
  reporting_check_tasks_and_durations(alltasks, &alldurations);
  reporting_durations_to_percentages(alldurations, allpercentages);

  // Project status.
  projectstatus = new ProjectStatus (project, &alltasks, true);

  // Build dialog.  
  editstatusdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW (editstatusdialog), "Edit Status");
  gtk_window_set_position(GTK_WINDOW (editstatusdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW (editstatusdialog), TRUE);

  // Set the size of the dialog big.
  gint height = (gint) (settings->genconfig.screen_height_get() * 0.9);
  gtk_window_set_default_size(GTK_WINDOW (editstatusdialog), -1, height);

  dialog_vbox1 = GTK_DIALOG (editstatusdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  viewport1 = gtk_viewport_new(NULL, NULL);
  gtk_widget_show(viewport1);
  gtk_container_add(GTK_CONTAINER (scrolledwindow1), viewport1);

  hbox = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox);
  gtk_container_add(GTK_CONTAINER (viewport1), hbox);

  treeview_books = gtk_tree_view_new();
  gtk_widget_show(treeview_books);
  gtk_box_pack_start(GTK_BOX (hbox), treeview_books, TRUE, TRUE, 0);

  liststore_books = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview_books), GTK_TREE_MODEL (liststore_books));
  g_object_unref (liststore_books);
  GtkCellRenderer * renderer_books = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview_books), -1, "Books", renderer_books, "text", 0, NULL);
  gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_books)), GTK_SELECTION_MULTIPLE);
  
  treeview_chapters = gtk_tree_view_new();
  gtk_widget_show(treeview_chapters);
  gtk_box_pack_start(GTK_BOX (hbox), treeview_chapters, TRUE, TRUE, 0);

  liststore_chapters = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview_chapters), GTK_TREE_MODEL (liststore_chapters));
  g_object_unref (liststore_chapters);
  GtkCellRenderer * renderer_chapters = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview_chapters), -1, "Chapters", renderer_chapters, "text", 0, NULL);
  gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_chapters)), GTK_SELECTION_MULTIPLE);
  
  treeview_verses = gtk_tree_view_new();
  gtk_widget_show(treeview_verses);
  gtk_box_pack_start(GTK_BOX (hbox), treeview_verses, TRUE, TRUE, 0);

  liststore_verses = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview_verses), GTK_TREE_MODEL (liststore_verses));
  g_object_unref (liststore_verses);
  GtkCellRenderer * renderer_verses = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview_verses), -1, "Verses", renderer_verses, "text", 0, NULL);
  gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_verses)), GTK_SELECTION_MULTIPLE);
  
  vbox_status = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox_status);
  gtk_box_pack_start(GTK_BOX (hbox), vbox_status, TRUE, TRUE, 0);

  checkbutton_status = gtk_check_button_new_with_mnemonic("Status");
  gtk_widget_show(checkbutton_status);
  gtk_box_pack_start(GTK_BOX (vbox_status), checkbutton_status, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (editstatusdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (editstatusdialog, NULL, "edit_status");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG (editstatusdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG (editstatusdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) treeview_books, "cursor_changed",
      G_CALLBACK (on_treeview_books_cursor_changed),
      gpointer (this));
  g_signal_connect ((gpointer) treeview_chapters, "cursor_changed",
      G_CALLBACK (on_treeview_chapters_cursor_changed),
      gpointer (this));
  g_signal_connect ((gpointer) treeview_verses, "cursor_changed",
      G_CALLBACK (on_treeview_verses_cursor_changed),
      gpointer (this));
  g_signal_connect ((gpointer) checkbutton_status, "toggled",
      G_CALLBACK (on_checkbutton_status_toggled),
      gpointer (this));
  g_signal_connect ((gpointer) okbutton, "clicked",
      G_CALLBACK (on_okbutton_clicked),
      gpointer (this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);

  /*
  // Build the books gui.
  build_books();

  // Press the button for the book that is open in the editor.
  for (unsigned int i = 0; i < bookbuttons.size(); i++) {
    if (book == projectstatus->books[i]->book) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (bookbuttons[i]), true);
    }
  }

  // Press the button for the chapter that is openin the editor.
  for (unsigned int i = 0; i < chapterbuttons.size(); i++) {
    if (chapter == projectstatus->books[currentbook]->chapters[i]->chapter) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (chapterbuttons[i]), true);
    }
  }
  */
}

EditStatusDialog::~EditStatusDialog() {
  delete projectstatus;
  gtk_widget_destroy(editstatusdialog);
}

int EditStatusDialog::run() {
  return gtk_dialog_run(GTK_DIALOG (editstatusdialog));
}

void EditStatusDialog::on_button_project_clicked(GtkButton *button, gpointer user_data) {
  ((EditStatusDialog *) user_data)->on_button_project();
}

void EditStatusDialog::on_button_project() {
  // Run the dialog to set the status for the whole project.
  ProjectStatusRecord statusrecord = projectstatus->get();
  SetStatusDialog dialog(&statusrecord);
  if (dialog.run() == GTK_RESPONSE_OK) {
    projectstatus->set(statusrecord);
    setpercentages();
  }
}

void EditStatusDialog::on_togglebutton_book_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((EditStatusDialog *) user_data)->on_togglebutton_book_toggle(togglebutton);
}

void EditStatusDialog::on_togglebutton_book_toggle(GtkToggleButton *togglebutton) {
  // Bail out if we're not interested in the action.
  if (settingbooks)
    return;
  if (!gtk_toggle_button_get_active(togglebutton))
    return;

  // Cast the togglebutton to a widget for later compare.
  GtkWidget * togglewidget= GTK_WIDGET (togglebutton);

  // The button was depressed: release any of the other ones.
  for (unsigned int i = 0; i < bookbuttons.size(); i++) {
    GtkToggleButton * bookbutton= GTK_TOGGLE_BUTTON (bookbuttons[i]);
    if (bookbuttons[i] != togglewidget) {
      if (gtk_toggle_button_get_active(bookbutton)) {
        settingbooks = true;
        gtk_toggle_button_set_active(bookbutton, false);
        settingbooks = false;
      }
    }
  }

  // Take appropriate action on whether the button was pressed for the first or second time.
  for (unsigned int i = 0; i < bookbuttons.size(); i++) {
    if (bookbuttons[i] == togglewidget) {
      if (!bookhistories[i]) {

        // Button depressed for the first time.

        // Set the number of the current book.
        currentbook = i;

        // Build chapter gui.
        build_chapters();

        // Activate first chapter.
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (allchapterbuttons[0]), true);

        // Set percentages ready.
        setpercentages();

      } else {

        // Button depressed for the second time.
        // Run the dialog to set the status for the book.
        ProjectStatusBook * project_status_book = projectstatus->books.at(currentbook);
        ProjectStatusRecord statusrecord = project_status_book->get();
        SetStatusDialog dialog(&statusrecord);
        if (dialog.run() == GTK_RESPONSE_OK) {
          project_status_book->set(statusrecord);
          setpercentages();
        }

      }
    }
  }

  // Set the button history in accordance with the actual state of the buttons.
  for (unsigned int i = 0; i < bookbuttons.size(); i++) {
    bookhistories[i] = (bookbuttons[i] == togglewidget);
  }
}

void EditStatusDialog::on_togglebutton_book_clicked(GtkButton *button, gpointer user_data) {
  ((EditStatusDialog *) user_data)->on_togglebutton_book_click(button);
}

void EditStatusDialog::on_togglebutton_book_click(GtkButton *button) {
  // Bail out if we're not interested in the action.
  if (settingbooks)
    return;

  // Keep the button depressed.
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (button))) {
    g_timeout_add(5, GSourceFunc(on_depress_button_timeout), gpointer(button));
  }
}

void EditStatusDialog::on_togglebutton_chapter_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((EditStatusDialog *) user_data)->on_togglebutton_chapter_toggle(togglebutton);
}

void EditStatusDialog::on_togglebutton_chapter_toggle(GtkToggleButton *togglebutton) {
  // Bail out if we're not interested in the action.
  if (settingchapters)
    return;
  if (!gtk_toggle_button_get_active(togglebutton))
    return;

  // Cast the togglebutton to a widget for later compare.
  GtkWidget * togglewidget= GTK_WIDGET (togglebutton);

  // The button was depressed: release any of the other ones.
  for (unsigned int i = 0; i < chapterbuttons.size(); i++) {
    if (chapterbuttons[i] != togglewidget) {
      GtkToggleButton * chapterbutton= GTK_TOGGLE_BUTTON (chapterbuttons[i]);
      if (gtk_toggle_button_get_active(chapterbutton)) {
        settingchapters = true;
        gtk_toggle_button_set_active(chapterbutton, false);
        settingchapters = false;
      }
    }
  }

  // Take appropriate action on whether the button was pressed for the first or second time.
  for (unsigned int i = 0; i < chapterbuttons.size(); i++) {
    if (chapterbuttons[i] == togglewidget) {
      if (!chapterhistories[i]) {

        // Button depressed for the first time.

        // Set the number of the current book.
        currentchapter = i;

        // Build chapter gui.
        build_verses();

        // Set percentages ready.
        setpercentages();

      } else {

        // Button depressed for the second time.
        // Run the dialog to set the status for the chapter.
        ProjectStatusBook * project_status_book = projectstatus->books.at(currentbook);
        ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at(currentchapter);
        ProjectStatusRecord statusrecord = project_status_chapter->get();
        SetStatusDialog dialog(&statusrecord);
        if (dialog.run() == GTK_RESPONSE_OK) {
          project_status_chapter->set(statusrecord);
          setpercentages();
        }

      }
    }
  }

  // Set the button history in accordance with the actual state of the buttons.
  for (unsigned int i = 0; i < chapterbuttons.size(); i++) {
    chapterhistories[i] = (chapterbuttons[i] == togglewidget);
  }
}

void EditStatusDialog::on_togglebutton_chapter_clicked(GtkButton *button, gpointer user_data) {
  ((EditStatusDialog *) user_data)->on_togglebutton_chapter_click(button);
}

void EditStatusDialog::on_togglebutton_chapter_click(GtkButton *button) {
  // Bail out if we're not interested in the action.
  if (settingchapters)
    return;

  // Keep the button depressed.
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (button))) {
    g_timeout_add(5, GSourceFunc(on_depress_button_timeout), gpointer(button));
  }
}

void EditStatusDialog::on_button_verse_clicked(GtkButton *button, gpointer user_data) {
  ((EditStatusDialog *) user_data)->on_button_verse(button);
}

void EditStatusDialog::on_button_verse(GtkButton *button) {
  ProjectStatusBook * project_status_book = projectstatus->books.at(currentbook);
  ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at(currentchapter);
  for (unsigned int i = 0; i < versebuttons.size(); i++) {
    if (button == GTK_BUTTON (versebuttons[i])) {
      ProjectStatusRecord * project_status_record = &project_status_chapter->status[i];
      SetStatusDialog dialog(project_status_record);
      if (dialog.run() == GTK_RESPONSE_OK) {
        setpercentages();
      }
    }
  }
}

void EditStatusDialog::on_okbutton_clicked(GtkButton *button, gpointer user_data) {
  ((EditStatusDialog *) user_data)->on_okbutton(button);
}

void EditStatusDialog::on_okbutton(GtkButton *button) {
  projectstatus->save();
}

bool EditStatusDialog::on_depress_button_timeout(gpointer data)
// This function depresses a button. It cannot be done in the callback: that
// would crash GTK.
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (data), true);
  return false;
}


void EditStatusDialog::build_books() {
  /*
  // If there is an old books gui, destroy that first.
  for (unsigned int i = 0; i < bookbuttons.size(); i++) {
    gtk_widget_destroy(bookbuttons[i]);
  }
  bookbuttons.clear();
  bookhistories.clear();
  bookprogressbars.clear();

  // Build gui. Build as many units as there are books in the project.
  for (unsigned int i = 0; i < projectstatus->books.size(); i++) {

    ProjectStatusBook * project_status_book = projectstatus->books.at(i);

    togglebutton_book = gtk_toggle_button_new();
    gtk_widget_show(togglebutton_book);
    gtk_box_pack_start(GTK_BOX (vbox2), togglebutton_book, FALSE, FALSE, 0);

    alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment2);
    gtk_container_add(GTK_CONTAINER (togglebutton_book), alignment2);

    hbox3 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox3);
    gtk_container_add(GTK_CONTAINER (alignment2), hbox3);

    progressbar_book = gtk_progress_bar_new();
    gtk_widget_show(progressbar_book);
    gtk_box_pack_start(GTK_BOX (hbox3), progressbar_book, FALSE, FALSE, 0);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR (progressbar_book), books_id_to_english (project_status_book->book).c_str());

    g_signal_connect ((gpointer) togglebutton_book, "toggled",
        G_CALLBACK (on_togglebutton_book_toggled),
        gpointer (this));
    g_signal_connect ((gpointer) togglebutton_book, "clicked",
        G_CALLBACK (on_togglebutton_book_clicked),
        gpointer (this));

    bookbuttons.push_back(togglebutton_book);
    bookhistories.push_back(false);
    bookprogressbars.push_back(progressbar_book);
  }
  */
}

void EditStatusDialog::build_chapters() {
  /*
  // If there is an old chapters gui, hide that first.
  for (unsigned int i = 0; i < chapterbuttons.size(); i++) {
    gtk_widget_hide_all(chapterbuttons[i]);
  }
  chapterbuttons.clear();
  chapterhistories.clear();
  chapterprogressbars.clear();

  // Get the object for the book we're interested in.  
  ProjectStatusBook * project_status_book = projectstatus->books.at(currentbook);

  // Build gui. Show as many units as there are chapters in the book.
  for (unsigned int i = 0; i < project_status_book->chapters.size(); i++) {

    ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at(i);

    gtk_widget_show_all(allchapterbuttons[i]);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR (allchapterprogressbars[i]), convert_to_string (project_status_chapter->chapter).c_str());

    chapterbuttons.push_back(allchapterbuttons[i]);
    chapterhistories.push_back(false);
    chapterprogressbars.push_back(allchapterprogressbars[i]);
  }
  */
}

void EditStatusDialog::build_verses() {
  /*
  // Hide any old verses gui.
  // We could have destroyed it and then rebuild it, but due to a problem in 
  // GTK the button's no longer allow to be clicked after being rebuilt.
  // Therefore we hide them, and later show the relevant ones again.
  for (unsigned int i = 0; i < versebuttons.size(); i++) {
    gtk_widget_hide_all(versebuttons[i]);
  }
  versebuttons.clear();
  verseprogressbars.clear();

  // Get the object for the chapter we're interested in.  
  ProjectStatusChapter * project_status_chapter = projectstatus->books.at(currentbook)->chapters.at(currentchapter);

  // Show as many gui elements are there are verses in the chapter.
  for (unsigned int i = 0; i < project_status_chapter->status.size(); i++) {

    gtk_widget_show_all(allversebuttons[i]);

    g_signal_connect ((gpointer) button_verse, "clicked",
        G_CALLBACK (on_button_verse_clicked),
        gpointer (this));

    versebuttons.push_back(allversebuttons[i]);
    verseprogressbars.push_back(allverseprogressbars[i]);
  }

  // Update percentages.
  setpercentages();
  */
}

void EditStatusDialog::setpercentages() {
  /*
  // Percentages containers.
  vector <unsigned int> visible_verse_percentages;
  vector <unsigned int> visible_chapter_percentages;
  vector <unsigned int> book_percentages;
  unsigned int project_percentage = 0;
  // Calculate all percentages ready.
  for (unsigned int i = 0; i < projectstatus->books.size(); i++) {
    ProjectStatusBook * project_status_book = projectstatus->books.at(i);
    unsigned int accumulated_chapter_percentage = 0;
    for (unsigned int i2 = 0; i2 < project_status_book->chapters.size(); i2++) {
      ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at(i2);
      unsigned int accumulated_verse_percentage = 0;
      for (unsigned int i3 = 0; i3 < project_status_chapter->status.size(); i3++) {
        unsigned int percentage = project_status_chapter->status[i3].calculate_percentage(allpercentages);
        if ((i == currentbook) && (i2 == currentchapter)) {
          visible_verse_percentages.push_back(percentage);
        }
        accumulated_verse_percentage += percentage;
      }
      unsigned int chapter_percentage = (unsigned int) (double (accumulated_verse_percentage) / project_status_chapter->status.size());
      if (i == currentbook) {
        visible_chapter_percentages.push_back(chapter_percentage);
      }
      accumulated_chapter_percentage += chapter_percentage;
    }
    unsigned int book_percentage = (unsigned int) (double (accumulated_chapter_percentage) / project_status_book->chapters.size());
    book_percentages.push_back(book_percentage);
    project_percentage += book_percentage;
  }
  project_percentage = (unsigned int) (double (project_percentage) / projectstatus->books.size());

  // Set the percentages ready for the visible verses.
  for (unsigned int i = 0; i < visible_verse_percentages.size(); i++) {
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (verseprogressbars[i]), (double) (visible_verse_percentages[i]) / 100);
  }
  // Set percentages ready for the visible chapters.
  for (unsigned int i = 0; i < visible_chapter_percentages.size(); i++) {
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (chapterprogressbars[i]), (double) (visible_chapter_percentages[i]) / 100);
  }
  // Set percentages ready for the visible books.
  for (unsigned int i = 0; i < book_percentages.size(); i++) {
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (bookprogressbars[i]), (double) (book_percentages[i]) / 100);
  }
  // Set percentage for project.
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (progressbar1), (double) (project_percentage) / 100);
  */
}

void EditStatusDialog::on_treeview_books_cursor_changed(GtkTreeView *treeview, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_books_cursor();
}

void EditStatusDialog::on_treeview_books_cursor()
{
  
}

void EditStatusDialog::on_treeview_chapters_cursor_changed(GtkTreeView *treeview, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_chapters_cursor();
}

void EditStatusDialog::on_treeview_chapters_cursor()
{
  
}

void EditStatusDialog::on_treeview_verses_cursor_changed(GtkTreeView *treeview, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_treeview_verses_cursor();
}

void EditStatusDialog::on_treeview_verses_cursor()
{
  
}

void EditStatusDialog::on_checkbutton_status_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_checkbutton_status(togglebutton);
}

void EditStatusDialog::on_checkbutton_status(GtkToggleButton *togglebutton)
{
  
}


/*

 Todo edit status speedup.

 To speed up entry of status information, we should change the interface so that it uses a listview, 
 where e.g. one book or a range of books can be selected upon which the editing function is going to apply.
 This allows to set a task for a whole range of books / chapters / verses.

 */
