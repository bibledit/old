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


EditStatusDialog::EditStatusDialog (const ustring& project_in, unsigned int book, unsigned int chapter)
{
  // Initialize variables.
  project = project_in;
  currentbook = 0;
  currentchapter = 0;
  settingbooks = false;
  settingchapters = false;
  
  // Settings.
  extern Settings * settings;

  // Load the possible tasks and their parameters.
  alltasks = settings->genconfig.project_tasks_names_get ();
  vector <double> alldurations = settings->genconfig.project_tasks_durations_get ();
  reporting_check_tasks_and_durations (alltasks, &alldurations);
  reporting_durations_to_percentages (alldurations, allpercentages);
  
  // Project status.
  projectstatus = new ProjectStatus (project, &alltasks, true);

  // Build dialog.  
  editstatusdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (editstatusdialog), "Edit Status");
  gtk_window_set_position (GTK_WINDOW (editstatusdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (editstatusdialog), TRUE);

  // Set the size of the dialog big.
  gint height = (gint) (settings->genconfig.screen_height_get () * 0.9);
  gtk_window_set_default_size (GTK_WINDOW (editstatusdialog), -1, height);
  
  dialog_vbox1 = GTK_DIALOG (editstatusdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  viewport1 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), viewport1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (viewport1), hbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new ("Status of project");
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);

  button_project = gtk_button_new ();
  gtk_widget_show (button_project);
  gtk_box_pack_start (GTK_BOX (vbox1), button_project, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_project), alignment3);

  hbox4 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox4);

  progressbar1 = gtk_progress_bar_new ();
  gtk_widget_show (progressbar1);
  gtk_box_pack_start (GTK_BOX (hbox4), progressbar1, FALSE, FALSE, 0);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_show (vseparator1);
  gtk_box_pack_start (GTK_BOX (hbox1), vseparator1, FALSE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox2, TRUE, TRUE, 0);

  label2 = gtk_label_new ("Status of books");
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox2), label2, FALSE, FALSE, 0);

  // Books gui built later on.

  vseparator2 = gtk_vseparator_new ();
  gtk_widget_show (vseparator2);
  gtk_box_pack_start (GTK_BOX (hbox1), vseparator2, FALSE, TRUE, 0);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox3, TRUE, TRUE, 0);

  label3 = gtk_label_new ("Status of chapters");
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (vbox3), label3, FALSE, FALSE, 0);

  // Build gui here, but hidden.
  for (unsigned int i = 0; i <= MAX_CHAPTERS; i++) {

    togglebutton_chapter = gtk_toggle_button_new ();
    gtk_box_pack_start (GTK_BOX (vbox3), togglebutton_chapter, FALSE, FALSE, 0);

    alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (togglebutton_chapter), alignment4);

    hbox5 = gtk_hbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (alignment4), hbox5);

    progressbar_chapter = gtk_progress_bar_new ();
    gtk_box_pack_start (GTK_BOX (hbox5), progressbar_chapter, FALSE, FALSE, 0);

    g_signal_connect ((gpointer) togglebutton_chapter, "toggled",
                      G_CALLBACK (on_togglebutton_chapter_toggled),
                      gpointer (this));
    g_signal_connect ((gpointer) togglebutton_chapter, "clicked",
                      G_CALLBACK (on_togglebutton_chapter_clicked),
                      gpointer (this));
                      
    allchapterbuttons.push_back (togglebutton_chapter);
    allchapterprogressbars.push_back (progressbar_chapter);
  }
  
  vseparator3 = gtk_vseparator_new ();
  gtk_widget_show (vseparator3);
  gtk_box_pack_start (GTK_BOX (hbox1), vseparator3, FALSE, TRUE, 0);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox4, TRUE, TRUE, 0);

  label4 = gtk_label_new ("Status of verses");
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (vbox4), label4, FALSE, FALSE, 0);

  // Build gui here but hidden.
  for (unsigned int i = 0; i <= MAX_VERSES; i++) {
    
    button_verse = gtk_button_new ();
    gtk_box_pack_start (GTK_BOX (vbox4), button_verse, FALSE, FALSE, 0);

    alignment5 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (button_verse), alignment5);

    hbox6 = gtk_hbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (alignment5), hbox6);

    progressbar_verse = gtk_progress_bar_new ();
    gtk_box_pack_start (GTK_BOX (hbox6), progressbar_verse, FALSE, FALSE, 0);

    g_signal_connect ((gpointer) button_verse, "clicked",
                      G_CALLBACK (on_button_verse_clicked),
                      gpointer (this));
                      
    allversebuttons.push_back (button_verse);
    allverseprogressbars.push_back (progressbar_verse);
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar_verse), convert_to_string (i).c_str ());
    
  }

  dialog_action_area1 = GTK_DIALOG (editstatusdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (editstatusdialog, NULL, "edit_status");

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (editstatusdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (editstatusdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);
  
  g_signal_connect ((gpointer) button_project, "clicked",
                    G_CALLBACK (on_button_project_clicked),
                    gpointer (this));
  g_signal_connect ((gpointer) okbutton, "clicked",
                    G_CALLBACK (on_okbutton_clicked),
                    gpointer (this));
  
  gtk_widget_grab_focus (okbutton);
  gtk_widget_grab_default (okbutton);

  // Build the books gui.
  build_books ();
  
  // Press the button for the book that is open in the editor.
  for (unsigned int i = 0; i < bookbuttons.size (); i++) {
    if (book == projectstatus->books[i]->book) {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (bookbuttons[i]), true);
    }
  }
  
  // Press the button for the chapter that is openin the editor.
  for (unsigned int i = 0; i < chapterbuttons.size (); i++) {
    if (chapter == projectstatus->books[currentbook]->chapters[i]->chapter) {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chapterbuttons[i]), true);
    }
  }
}


EditStatusDialog::~EditStatusDialog ()
{
  delete projectstatus;
  gtk_widget_destroy (editstatusdialog);
}


int EditStatusDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (editstatusdialog));
}


void EditStatusDialog::on_button_project_clicked (GtkButton *button, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_button_project ();
}


void EditStatusDialog::on_button_project ()
{
  // Run the dialog to set the status for the whole project.
  ProjectStatusRecord statusrecord = projectstatus->get ();
  SetStatusDialog dialog (&statusrecord);
  if (dialog.run () == GTK_RESPONSE_OK) {
    projectstatus->set (statusrecord);
    setpercentages ();
  }
}


void EditStatusDialog::on_togglebutton_book_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_togglebutton_book_toggle (togglebutton);
}


void EditStatusDialog::on_togglebutton_book_toggle (GtkToggleButton *togglebutton)
{
  // Bail out if we're not interested in the action.
  if (settingbooks) return;
  if (!gtk_toggle_button_get_active (togglebutton)) return;

  // Cast the togglebutton to a widget for later compare.
  GtkWidget * togglewidget = GTK_WIDGET (togglebutton);
  
  // The button was depressed: release any of the other ones.
  for (unsigned int i = 0; i < bookbuttons.size (); i++) {
    GtkToggleButton * bookbutton = GTK_TOGGLE_BUTTON (bookbuttons[i]);
    if (bookbuttons[i] != togglewidget) {
      if (gtk_toggle_button_get_active (bookbutton)) {
        settingbooks = true;
        gtk_toggle_button_set_active (bookbutton, false);
        settingbooks = false;
      }
    }
  }

  // Take appropriate action on whether the button was pressed for the first or second time.
  for (unsigned int i = 0; i < bookbuttons.size (); i++) {
    if (bookbuttons[i] == togglewidget) {
      if (!bookhistories[i]) {
        
        // Button depressed for the first time.

        // Set the number of the current book.
        currentbook = i;

        // Build chapter gui.
        build_chapters ();

        // Activate first chapter.
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (allchapterbuttons[0]), true);
        
        // Set percentages ready.
        setpercentages ();
        
      } else {
        
        // Button depressed for the second time.
        // Run the dialog to set the status for the book.
        ProjectStatusBook * project_status_book = projectstatus->books.at (currentbook);
        ProjectStatusRecord statusrecord = project_status_book->get ();
        SetStatusDialog dialog (&statusrecord);
        if (dialog.run () == GTK_RESPONSE_OK) {
          project_status_book->set (statusrecord);
          setpercentages ();
        }
        
      }
    }
  }

  // Set the button history in accordance with the actual state of the buttons.
  for (unsigned int i = 0; i < bookbuttons.size (); i++) {
    bookhistories[i] = (bookbuttons[i] == togglewidget);
  }  
}


void EditStatusDialog::on_togglebutton_book_clicked (GtkButton *button, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_togglebutton_book_click (button);
}


void EditStatusDialog::on_togglebutton_book_click (GtkButton *button)
{
  // Bail out if we're not interested in the action.
  if (settingbooks) return;
 
  // Keep the button depressed.
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button))) {
    g_timeout_add (5, GSourceFunc (on_depress_button_timeout), gpointer (button));
  }
}


void EditStatusDialog::on_togglebutton_chapter_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_togglebutton_chapter_toggle (togglebutton);
}


void EditStatusDialog::on_togglebutton_chapter_toggle (GtkToggleButton *togglebutton)
{
  // Bail out if we're not interested in the action.
  if (settingchapters) return;
  if (!gtk_toggle_button_get_active (togglebutton)) return;

  // Cast the togglebutton to a widget for later compare.
  GtkWidget * togglewidget = GTK_WIDGET (togglebutton);
  
  // The button was depressed: release any of the other ones.
  for (unsigned int i = 0; i < chapterbuttons.size (); i++) {
    if (chapterbuttons[i] != togglewidget) {
      GtkToggleButton * chapterbutton = GTK_TOGGLE_BUTTON (chapterbuttons[i]);
      if (gtk_toggle_button_get_active (chapterbutton)) {
        settingchapters = true;
        gtk_toggle_button_set_active (chapterbutton, false);
        settingchapters = false;
      }
    }
  }

  // Take appropriate action on whether the button was pressed for the first or second time.
  for (unsigned int i = 0; i < chapterbuttons.size (); i++) {
    if (chapterbuttons[i] == togglewidget) {
      if (!chapterhistories[i]) {
        
        // Button depressed for the first time.

        // Set the number of the current book.
        currentchapter = i;

        // Build chapter gui.
        build_verses ();
        
        // Set percentages ready.
        setpercentages ();
        
      } else {
        
        // Button depressed for the second time.
        // Run the dialog to set the status for the chapter.
        ProjectStatusBook * project_status_book = projectstatus->books.at (currentbook);
        ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at (currentchapter);
        ProjectStatusRecord statusrecord = project_status_chapter->get ();
        SetStatusDialog dialog (&statusrecord);
        if (dialog.run () == GTK_RESPONSE_OK) {
          project_status_chapter->set (statusrecord);
          setpercentages ();
        }
        
      }
    }
  }

  // Set the button history in accordance with the actual state of the buttons.
  for (unsigned int i = 0; i < chapterbuttons.size (); i++) {
    chapterhistories[i] = (chapterbuttons[i] == togglewidget);
  }  
}


void EditStatusDialog::on_togglebutton_chapter_clicked (GtkButton *button, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_togglebutton_chapter_click (button);
}


void EditStatusDialog::on_togglebutton_chapter_click (GtkButton *button)
{
  // Bail out if we're not interested in the action.
  if (settingchapters) return;
 
  // Keep the button depressed.
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button))) {
    g_timeout_add (5, GSourceFunc (on_depress_button_timeout), gpointer (button));
  }
}


void EditStatusDialog::on_button_verse_clicked (GtkButton *button, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_button_verse (button);
}


void EditStatusDialog::on_button_verse (GtkButton *button)
{
  ProjectStatusBook * project_status_book = projectstatus->books.at(currentbook);
  ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at(currentchapter);
  for (unsigned int i = 0; i < versebuttons.size (); i++) {
    if (button == GTK_BUTTON (versebuttons[i])) {
      ProjectStatusRecord * project_status_record = &project_status_chapter->status[i];
      SetStatusDialog dialog (project_status_record);
      if (dialog.run () == GTK_RESPONSE_OK) {
        setpercentages ();
      }
    }
  }
}


void EditStatusDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((EditStatusDialog *) user_data)->on_okbutton (button);
}


void EditStatusDialog::on_okbutton (GtkButton *button)
{
  projectstatus->save ();
}


bool EditStatusDialog::on_depress_button_timeout (gpointer data)
// This function depresses a button. It cannot be done in the callback: that
// would crash GTK.
{
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (data), true);
  return false;
}


void EditStatusDialog::build_books ()
{
  // If there is an old books gui, destroy that first.
  for (unsigned int i = 0; i < bookbuttons.size (); i++) {
    gtk_widget_destroy (bookbuttons[i]);
  }
  bookbuttons.clear ();
  bookhistories.clear ();
  bookprogressbars.clear ();

  // Build gui. Build as many units as there are books in the project.
  for (unsigned int i = 0; i < projectstatus->books.size (); i++) {

    ProjectStatusBook * project_status_book = projectstatus->books.at(i);

    togglebutton_book = gtk_toggle_button_new ();
    gtk_widget_show (togglebutton_book);
    gtk_box_pack_start (GTK_BOX (vbox2), togglebutton_book, FALSE, FALSE, 0);

    alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_widget_show (alignment2);
    gtk_container_add (GTK_CONTAINER (togglebutton_book), alignment2);

    hbox3 = gtk_hbox_new (FALSE, 2);
    gtk_widget_show (hbox3);
    gtk_container_add (GTK_CONTAINER (alignment2), hbox3);

    progressbar_book = gtk_progress_bar_new ();
    gtk_widget_show (progressbar_book);
    gtk_box_pack_start (GTK_BOX (hbox3), progressbar_book, FALSE, FALSE, 0);
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar_book), books_id_to_english (project_status_book->book).c_str());

    g_signal_connect ((gpointer) togglebutton_book, "toggled",
                      G_CALLBACK (on_togglebutton_book_toggled),
                      gpointer (this));
    g_signal_connect ((gpointer) togglebutton_book, "clicked",
                      G_CALLBACK (on_togglebutton_book_clicked),
                      gpointer (this));

    bookbuttons.push_back (togglebutton_book);
    bookhistories.push_back (false);
    bookprogressbars.push_back (progressbar_book);
  }
}


void EditStatusDialog::build_chapters ()
{
  // If there is an old chapters gui, hide that first.
  for (unsigned int i = 0; i < chapterbuttons.size (); i++) {
    gtk_widget_hide_all (chapterbuttons[i]);
  }
  chapterbuttons.clear ();
  chapterhistories.clear ();
  chapterprogressbars.clear ();

  // Get the object for the book we're interested in.  
  ProjectStatusBook * project_status_book = projectstatus->books.at(currentbook);
  
  // Build gui. Show as many units as there are chapters in the book.
  for (unsigned int i = 0; i < project_status_book->chapters.size (); i++) {

    ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at(i);

    gtk_widget_show_all (allchapterbuttons[i]);
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (allchapterprogressbars[i]), convert_to_string (project_status_chapter->chapter).c_str());

    chapterbuttons.push_back (allchapterbuttons[i]);
    chapterhistories.push_back (false);
    chapterprogressbars.push_back (allchapterprogressbars[i]);
  }
}


void EditStatusDialog::build_verses ()
{
  // Hide any old verses gui.
  // We could have destroyed it and then rebuild it, but due to a problem in 
  // GTK the button's no longer allow to be clicked after being rebuilt.
  // Therefore we hide them, and later show the relevant ones again.
  for (unsigned int i = 0; i < versebuttons.size (); i++) {
    gtk_widget_hide_all (versebuttons[i]);
  }
  versebuttons.clear ();
  verseprogressbars.clear ();

  // Get the object for the chapter we're interested in.  
  ProjectStatusChapter * project_status_chapter = projectstatus->books.at(currentbook)->chapters.at(currentchapter);
  
  // Show as many gui elements are there are verses in the chapter.
  for (unsigned int i = 0; i < project_status_chapter->status.size (); i++) {

    gtk_widget_show_all (allversebuttons[i]);

    g_signal_connect ((gpointer) button_verse, "clicked",
                      G_CALLBACK (on_button_verse_clicked),
                      gpointer (this));
    
    versebuttons.push_back (allversebuttons[i]);
    verseprogressbars.push_back (allverseprogressbars[i]);
  }

  // Update percentages.
  setpercentages ();
}


void EditStatusDialog::setpercentages ()
{
  // Percentages containers.
  vector <unsigned int> visible_verse_percentages;
  vector <unsigned int> visible_chapter_percentages;
  vector <unsigned int> book_percentages;
  unsigned int project_percentage = 0;
  // Calculate all percentages ready.
  for (unsigned int i = 0; i < projectstatus->books.size (); i++) {
    ProjectStatusBook * project_status_book = projectstatus->books.at(i);
    unsigned int accumulated_chapter_percentage = 0;
    for (unsigned int i2 = 0; i2 < project_status_book->chapters.size (); i2++) {
      ProjectStatusChapter * project_status_chapter = project_status_book->chapters.at(i2);
      unsigned int accumulated_verse_percentage = 0;
      for (unsigned int i3 = 0; i3 < project_status_chapter->status.size (); i3++) {
        unsigned int percentage = project_status_chapter->status[i3].calculate_percentage (allpercentages);
        if ((i == currentbook) && (i2 == currentchapter)) {
          visible_verse_percentages.push_back (percentage);
        }
        accumulated_verse_percentage += percentage;
      }
      unsigned int chapter_percentage = (unsigned int) (double (accumulated_verse_percentage) / project_status_chapter->status.size ());
      if (i == currentbook) {
        visible_chapter_percentages.push_back (chapter_percentage);
      }
      accumulated_chapter_percentage += chapter_percentage;
    }
    unsigned int book_percentage = (unsigned int) (double (accumulated_chapter_percentage) / project_status_book->chapters.size ());
    book_percentages.push_back (book_percentage);
    project_percentage += book_percentage;
  }
  project_percentage = (unsigned int) (double (project_percentage) / projectstatus->books.size ());
  
  // Set the percentages ready for the visible verses.
  for (unsigned int i = 0; i < visible_verse_percentages.size (); i++) {
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (verseprogressbars[i]), (double) (visible_verse_percentages[i]) / 100);
  }
  // Set percentages ready for the visible chapters.
  for (unsigned int i = 0; i < visible_chapter_percentages.size (); i++) {
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (chapterprogressbars[i]), (double) (visible_chapter_percentages[i]) / 100);
  }
  // Set percentages ready for the visible books.
  for (unsigned int i = 0; i < book_percentages.size (); i++) {
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bookprogressbars[i]), (double) (book_percentages[i]) / 100);
  }
  // Set percentage for project.
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar1), (double) (project_percentage) / 100);
}
