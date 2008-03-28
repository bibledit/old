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


#include <glib.h>
#include "dialogfilters.h"
#include "help.h"
#include "shortcuts.h"
#include "sed.h"
#include "combobox.h"
#include "dialogentry.h"
#include "gtkwrappers.h"
#include "gwrappers.h"


FiltersDialog::FiltersDialog (int dummy)
{
  rulesbuffer_changed_event_id = 0;
  
  Shortcuts shortcuts (0);
  
  filterdialog = gtk_dialog_new ();
  gtk_widget_set_size_request (filterdialog, 640, 640);
  gtk_window_set_title (GTK_WINDOW (filterdialog), "Filters");
  gtk_window_set_position (GTK_WINDOW (filterdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (filterdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (filterdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  label_rules = gtk_label_new ("Rules");
  gtk_widget_show (label_rules);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label_rules, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_rules), 0, 0.5);

  shortcuts.label (label_rules);
  
  scrolledwindow_rules = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_rules);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow_rules, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_rules), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_rules), GTK_SHADOW_IN);

  textview_rules = gtk_text_view_new ();
  gtk_widget_show (textview_rules);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_rules), textview_rules);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label_rules), textview_rules);

  rulesbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview_rules));

  label2 = gtk_label_new ("Input <- Try the rules -> Output");
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label2, FALSE, FALSE, 0);

  shortcuts.label (label2);
  
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox2, FALSE, TRUE, 0);

  scrolledwindow_input = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_input);
  gtk_box_pack_start (GTK_BOX (hbox2), scrolledwindow_input, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_input), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_input), GTK_SHADOW_IN);

  textview_input = gtk_text_view_new ();
  gtk_widget_show (textview_input);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_input), textview_input);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview_input), GTK_WRAP_WORD);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label2), textview_input);

  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview_input)), "This is text to try the filter on", -1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (hbox2), vbox1, FALSE, FALSE, 0);

  button_try = gtk_button_new ();
  gtk_widget_show (button_try);
  gtk_box_pack_start (GTK_BOX (vbox1), button_try, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_try), alignment1);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox3);

  image1 = gtk_image_new_from_stock ("gtk-convert", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox3), image1, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic ("Try");
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
 
  shortcuts.label (label3);

  scrolledwindow_output = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_output);
  gtk_box_pack_start (GTK_BOX (hbox2), scrolledwindow_output, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_output), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_output), GTK_SHADOW_IN);

  textview_output = gtk_text_view_new ();
  gtk_widget_show (textview_output);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_output), textview_output);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview_output), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview_output), GTK_WRAP_WORD);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, FALSE, FALSE, 0);

  button_new = gtk_button_new_from_stock ("gtk-new");
  gtk_widget_show (button_new);
  gtk_box_pack_start (GTK_BOX (hbox1), button_new, FALSE, FALSE, 0);

  shortcuts.stockbutton (button_new);
  
  combobox_filters = gtk_combo_box_new_text ();
  gtk_widget_show (combobox_filters);
  gtk_box_pack_start (GTK_BOX (hbox1), combobox_filters, TRUE, TRUE, 0);

  button_delete = gtk_button_new_from_stock ("gtk-delete");
  gtk_widget_show (button_delete);
  gtk_box_pack_start (GTK_BOX (hbox1), button_delete, FALSE, FALSE, 0);

  shortcuts.stockbutton (button_delete);
  
  dialog_action_area1 = GTK_DIALOG (filterdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (filterdialog, &shortcuts, "preferences_filters");

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (filterdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (filterdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton (cancelbutton);
  shortcuts.stockbutton (okbutton);
  shortcuts.process ();
  
  g_signal_connect ((gpointer) button_try, "clicked",
                    G_CALLBACK (on_button_try_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) button_new, "clicked",
                    G_CALLBACK (on_button_new_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) combobox_filters, "changed",
                    G_CALLBACK (on_combobox_filters_changed),
                    gpointer(this));
  g_signal_connect ((gpointer) button_delete, "clicked",
                    G_CALLBACK (on_button_delete_clicked),
                    gpointer(this));
  g_signal_connect ((gpointer) okbutton, "clicked",
                    G_CALLBACK (on_okbutton_clicked),
                    gpointer(this));
  
  g_signal_connect_after ((gpointer) rulesbuffer, "changed", G_CALLBACK (on_rulesbuffer_changed), gpointer (this));

  gtk_widget_grab_focus (textview_rules);
  gtk_widget_grab_default (okbutton);
  
  load_filters ("");
}


FiltersDialog::~FiltersDialog ()
{
  gtk_widget_destroy (filterdialog);
}


int FiltersDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (filterdialog));
}


void FiltersDialog::load_filters (const ustring& selection)
{
  vector <ustring> filters = sed_available_scripts ();
  combobox_set_strings (combobox_filters, filters);
  if (!selection.empty ())
    combobox_set_string (combobox_filters, selection);
  else if (filters.size () > 1)
    combobox_set_index (combobox_filters, 1);
  else
    combobox_set_index (combobox_filters, 0);
  on_combobox_filters ();
}


void FiltersDialog::on_button_try_clicked (GtkButton *button, gpointer user_data)
{
  ((FiltersDialog *) user_data)->on_button_try ();
}


void FiltersDialog::on_button_try ()
{
  // Iterators.
  GtkTextIter startiter, enditer;
  
  // Input text.
  GtkTextBuffer * inputbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview_input));
  gtk_text_buffer_get_start_iter (inputbuffer, &startiter);
  gtk_text_buffer_get_end_iter (inputbuffer, &enditer);
  ustring inputfile = sed_temporal_input_file ();
  g_file_set_contents (inputfile.c_str(), gtk_text_buffer_get_text (inputbuffer, &startiter, &enditer, false), -1, NULL);

  // Filter.
  ustring scriptfile = sed_script_path ("_$bibledit$temporal$script$_");
  GtkTextBuffer * rulesbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview_rules));
  gtk_text_buffer_get_start_iter (rulesbuffer, &startiter);
  gtk_text_buffer_get_end_iter (rulesbuffer, &enditer);
  g_file_set_contents (scriptfile.c_str(), gtk_text_buffer_get_text (rulesbuffer, &startiter, &enditer, false), -1, NULL);

  // Output file.
  ustring outputfile = sed_temporal_output_file ();
  
  // Run sed filter.
  ustring error = sed_filter (scriptfile, combobox_get_active_string (combobox_filters) == sed_straight_through (), inputfile, outputfile);

  // Remove temporal script file.
  unlink (scriptfile.c_str());
  
  // Show output in textview.  
  gchar * outputtext;
  g_file_get_contents (outputfile.c_str(), &outputtext, NULL, NULL);
  GtkTextBuffer * outputbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview_output));
  if (outputtext) {
    gtk_text_buffer_set_text (outputbuffer, outputtext, -1);
    g_free (outputtext);
  } else {
    gtk_text_buffer_set_text (outputbuffer, "", -1);
  }
  
  // If sed fails, give the error for debugging purposes.
  if (!error.empty ()) {
    gtk_text_buffer_set_text (outputbuffer, error.c_str(), -1);
  }
}


void FiltersDialog::on_button_new_clicked (GtkButton *button, gpointer user_data)
{
  ((FiltersDialog *) user_data)->on_button_new ();
}


void FiltersDialog::on_button_new ()
{
  EntryDialog dialog ("New script", "Enter the name of the new script", "");
  if (dialog.run () == GTK_RESPONSE_OK) {
    if (sed_script_available (dialog.entered_value)) {
      gtkw_dialog_error (filterdialog, "This one already exists");
      return;
    }
    g_file_set_contents (sed_script_path (dialog.entered_value).c_str(), "", -1, NULL);
    load_filters (dialog.entered_value);
  }
}


void FiltersDialog::on_combobox_filters_changed (GtkComboBox *combobox, gpointer user_data)
{
  ((FiltersDialog *) user_data)->on_combobox_filters ();
}


void FiltersDialog::on_combobox_filters ()
{
  // Clear rules buffer.
  gtk_text_buffer_set_text (rulesbuffer, "", -1);

  // Set sensitivity of widgets.
  bool editable = true;
  ustring filter = combobox_get_active_string (combobox_filters);
  if (filter == sed_straight_through ())
    editable = false;
  gtk_widget_set_sensitive (textview_rules, editable);
  gtk_widget_set_sensitive (button_delete, editable);

  // If not sensitive, bail out.
  if (!editable) return;
    
  // Load the rules.
  ustring filename = sed_script_path (filter);
  gchar * contents;
  g_file_get_contents (filename.c_str(), &contents, NULL, NULL);
  if (contents) {
    gtk_text_buffer_set_text (rulesbuffer, contents, -1);
    g_free (contents);
  }
  gtk_text_buffer_set_modified (rulesbuffer, false);
}


void FiltersDialog::on_button_delete_clicked (GtkButton *button, gpointer user_data)
{
  ((FiltersDialog *) user_data)->on_button_delete ();
}


void FiltersDialog::on_button_delete ()
{
  ustring filter = combobox_get_active_string (combobox_filters);
  ustring filename = sed_script_path (filter);
  unlink (filename.c_str());
  load_filters ("");
}


void FiltersDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((FiltersDialog *) user_data)->on_okbutton ();
}


void FiltersDialog::on_okbutton ()
{
}


void FiltersDialog::on_rulesbuffer_changed (GtkTextBuffer * textbuffer, gpointer user_data)
{
  ((FiltersDialog *) user_data)->on_rulesbuffer ();
}


void FiltersDialog::on_rulesbuffer () 
{
  gw_destroy_source (rulesbuffer_changed_event_id);
  rulesbuffer_changed_event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 100, GSourceFunc (on_rulesbuffer_changed_timeout), gpointer(this), NULL);
}


bool FiltersDialog::on_rulesbuffer_changed_timeout (gpointer user_data)
{
  ((FiltersDialog *) user_data)->on_rulesbuffer_changed_execute ();
  return false;
}


void FiltersDialog::on_rulesbuffer_changed_execute ()
{
  if (!gtk_text_buffer_get_modified (rulesbuffer)) return;
  ustring script = combobox_get_active_string (combobox_filters);
  if (script == sed_straight_through ()) return;
  ustring filename = sed_script_path (script);
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter (rulesbuffer, &startiter);
  gtk_text_buffer_get_end_iter (rulesbuffer, &enditer);
  g_file_set_contents (filename.c_str(), gtk_text_buffer_get_text (rulesbuffer, &startiter, &enditer, false), -1, NULL);
}
