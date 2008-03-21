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
#include <glib.h>
#include "dialognewresource.h"
#include "help.h"
#include "dialoglistview.h"
#include "gui.h"
#include "tiny_utilities.h"
#include "dialogresourcebooks.h"
#include "dialogtestresources.h"
#include "shell.h"
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "books.h"
#include "unixwrappers.h"
#include "gtkwrappers.h"
#include "progresswindow.h"
#include "dialogresourceconverter2.h"


NewResourceDialog::NewResourceDialog (const ustring& templatefile)
{
  // Initialize variables and widgets.
  mytemplatefile = templatefile;
  table_attachment_offset = 0;
  resource_type = rtEnd;
  table1 = NULL;
  create_working_directory (templatefile);
  resource_conversion_type = rctEnd;
  chapter_pattern_index = 0;
  verse_pattern_index = 0;
  
  // Shortcuts.
  Shortcuts shortcuts (0);
  
  // Dialog.
  newresourcedialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (newresourcedialog), "Resource Editor");
  gtk_window_set_position (GTK_WINDOW (newresourcedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (newresourcedialog), TRUE);
  
  dialog_vbox1 = GTK_DIALOG (newresourcedialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  label1 = gtk_label_new ("");
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  build_table_and_type (shortcuts);

  // Dialog action.
  dialog_action_area1 = GTK_DIALOG (newresourcedialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (newresourcedialog, &shortcuts, "resource_new");

  helpbutton = gtk_button_new_from_stock ("gtk-help");
  gtk_dialog_add_action_widget (GTK_DIALOG (newresourcedialog), helpbutton, GTK_RESPONSE_NONE);

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (newresourcedialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);
  shortcuts.stockbutton (cancelbutton);
  
  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (newresourcedialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);
  shortcuts.stockbutton (okbutton);
  
  gtk_widget_set_sensitive (okbutton, false);

  g_signal_connect ((gpointer) okbutton, "clicked", G_CALLBACK (on_okbutton_clicked), gpointer (this));

  gtk_widget_grab_focus (okbutton);
  gtk_widget_grab_default (okbutton);

  resource_type = resource_get_type (mytemplatefile);
  build_gui_from_resource_type (shortcuts, mytemplatefile);
  shortcuts.process ();
  if (type_gui ()) {
    gui ();
  }
}


NewResourceDialog::~NewResourceDialog ()
{
  gtk_widget_destroy (newresourcedialog);
}


int NewResourceDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (newresourcedialog));
}


void NewResourceDialog::build_table_and_type (Shortcuts& shortcuts)
{
  // Destroy anything old.
  if (table1) gtk_widget_destroy (table1);
    
  // The table that contains the widgets.
  table1 = gtk_table_new (6, 4, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 3);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 3);
  
  // Type of the resource.
  build_button (image_type_ok, label_type_ok, label_type_short, button_type, "Type", shortcuts, G_CALLBACK (on_type_button_clicked), label_type_long);
}


void NewResourceDialog::build_entry (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label, gchar * label_text, GtkWidget *& entry, const ustring& entry_text, GCallback handler)
{
  GtkWidget *hseparator;
  hseparator = gtk_hseparator_new ();
  gtk_widget_show (hseparator);
  gtk_table_attach (GTK_TABLE (table1), hseparator, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  table_attachment_offset++;

  image_ok = gtk_image_new_from_icon_name ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_ok);
  gtk_table_attach (GTK_TABLE (table1), image_ok, 0, 1, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label_ok = gtk_label_new ("");
  gtk_widget_show (label_ok);
  gtk_table_attach (GTK_TABLE (table1), label_ok, 1, 2, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_ok), 0, 0.5);

  label = gtk_label_new (label_text);
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table1), label, 2, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
  table_attachment_offset++;

  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (table1), entry, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  table_attachment_offset++;
  
  gtk_entry_set_text (GTK_ENTRY (entry), entry_text.c_str());

  g_signal_connect ((gpointer) entry, "changed", handler, gpointer (this));
}


/*
void NewResourceDialog::on_entry_changed (GtkEditable *editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry ();
}


void NewResourceDialog::on_entry ()
{
}
*/


void NewResourceDialog::build_button (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label_short, GtkWidget *& button, gchar * button_text, Shortcuts& shortcuts, GCallback handler, GtkWidget *& label_long)
{
  GtkWidget *hseparator;
  hseparator = gtk_hseparator_new ();
  gtk_widget_show (hseparator);
  gtk_table_attach (GTK_TABLE (table1), hseparator, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  table_attachment_offset++;

  image_ok = gtk_image_new_from_icon_name ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_ok);
  gtk_table_attach (GTK_TABLE (table1), image_ok, 0, 1, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_ok = gtk_label_new ("");
  gtk_widget_show (label_ok);
  gtk_table_attach (GTK_TABLE (table1), label_ok, 1, 2, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_ok), 0, 0.5);

  label_short = gtk_label_new ("");
  gtk_widget_show (label_short);
  gtk_table_attach (GTK_TABLE (table1), label_short, 2, 3, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_short), 0, 0.5);

  button = gtk_button_new ();
  gtk_widget_show (button);
  gtk_table_attach (GTK_TABLE (table1), button, 3, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  table_attachment_offset++;

  GtkWidget *alignment;
  alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment);
  gtk_container_add (GTK_CONTAINER (button), alignment);

  GtkWidget *hbox;
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox);
  gtk_container_add (GTK_CONTAINER (alignment), hbox);

  GtkWidget *image_button_surface;
  image_button_surface = gtk_image_new_from_stock ("gtk-index", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_button_surface);
  gtk_box_pack_start (GTK_BOX (hbox), image_button_surface, FALSE, FALSE, 0);

  GtkWidget *label_button_surface;
  label_button_surface = gtk_label_new_with_mnemonic (button_text);
  gtk_widget_show (label_button_surface);
  gtk_box_pack_start (GTK_BOX (hbox), label_button_surface, FALSE, FALSE, 0);

  shortcuts.label (label_button_surface);
  
  label_long = gtk_label_new ("");
  gtk_widget_show (label_long);
  gtk_table_attach (GTK_TABLE (table1), label_long, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_long), 0, 0.5);
  table_attachment_offset++;

  g_signal_connect ((gpointer) button, "clicked", handler, gpointer (this));
}


/*
void NewResourceDialog::on_button_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_button ();
}


void NewResourceDialog::on_button ()
{
}
*/


void NewResourceDialog::build_checkbutton_button (GtkWidget *& image_ok, GtkWidget *& label_ok, 
                                                  GtkWidget *& checkbutton, gchar * checkbutton_text, GCallback checkbutton_handler, 
                                                  GtkWidget *& button, gchar * button_text, GCallback button_handler, 
                                                  Shortcuts& shortcuts, 
                                                  GtkWidget *& label)
{
  GtkWidget * hseparator = gtk_hseparator_new ();
  gtk_widget_show (hseparator);
  gtk_table_attach (GTK_TABLE (table1), hseparator, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  table_attachment_offset++;

  image_ok = gtk_image_new_from_icon_name ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_ok);
  gtk_table_attach (GTK_TABLE (table1), image_ok, 0, 1, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  label_ok = gtk_label_new ("Done");
  gtk_widget_show (label_ok);
  gtk_table_attach (GTK_TABLE (table1), label_ok, 1, 2, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_ok), 0, 0.5);

  checkbutton = gtk_check_button_new_with_mnemonic (checkbutton_text);
  gtk_widget_show (checkbutton);
  gtk_table_attach (GTK_TABLE (table1), checkbutton, 2, 3, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  shortcuts.button (checkbutton);

  button = gtk_button_new ();
  gtk_widget_show (button);
  gtk_table_attach (GTK_TABLE (table1), button, 3, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  table_attachment_offset++;

  GtkWidget * alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button), alignment2);

  GtkWidget * hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  GtkWidget * image2 = gtk_image_new_from_stock ("gtk-index", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  GtkWidget * label2 = gtk_label_new_with_mnemonic (button_text);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
  
  shortcuts.label (label2);

  label = gtk_label_new ("");
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table1), label, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
  table_attachment_offset++;

  g_signal_connect ((gpointer) checkbutton, "toggled", checkbutton_handler, gpointer (this));
  g_signal_connect ((gpointer) button, "clicked", button_handler, gpointer (this));
}


/*
static void NewResourceDialog::on_checkbutton_checkbutton_button_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_checkbutton ();
}


void NewResourceDialog::on_checkbutton ()
{
}


static void NewResourceDialog::on_button_checkbutton_button_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_button ();
}


void NewResourceDialog::on_button ()
{
}
*/


void NewResourceDialog::build_textview (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label,
                                        GtkWidget *& textview, gchar * text, GCallback handler)
{
  GtkWidget *hseparator;
  hseparator = gtk_hseparator_new ();
  gtk_widget_show (hseparator);
  gtk_table_attach (GTK_TABLE (table1), hseparator, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  table_attachment_offset++;

  image_ok = gtk_image_new_from_icon_name ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_ok);
  gtk_table_attach (GTK_TABLE (table1), image_ok, 0, 1, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  label_ok = gtk_label_new ("Done");
  gtk_widget_show (label_ok);
  gtk_table_attach (GTK_TABLE (table1), label_ok, 1, 2, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_ok), 0, 0.5);

  label = gtk_label_new ("Title");
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table1), label, 2, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
  table_attachment_offset++;
  
  GtkWidget *scrolledwindow;
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_table_attach (GTK_TABLE (table1), scrolledwindow, 0, 4, table_attachment_offset, table_attachment_offset + 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  textview = gtk_text_view_new ();
  gtk_widget_show (textview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), textview);
  table_attachment_offset++;

  if (text) {
    GtkTextBuffer * buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_set_text (buffer, text, -1);
  }
  
  g_signal_connect ((gpointer) textview, "key_press_event", handler, gpointer (this));
}


/*
gboolean NewResourceDialog::on_textview_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_textview ();
  return false;
}


void NewResourceDialog::on_textview ()
{
}
*/


// Resource type.


void NewResourceDialog::on_type_button_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_type_button ();
}


void NewResourceDialog::on_type_button ()
{
  vector <ustring> types;
  for (unsigned int i = 0; i < rtEnd; i++) {
    types.push_back (resource_type_to_text ((ResourceType)i));
  }
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources (filenames, true);
  for (unsigned int i = 0; i < resources.size (); i++) {
    types.push_back (based_upon () + resources[i]);
  }
  ListviewDialog dialog ("Select resource type", types, resource_type_to_text (resource_type), false, NULL);
  if (dialog.run () == GTK_RESPONSE_OK) {
    ustring filename_based_upon;
    resource_type = resource_text_to_type (dialog.focus);
    if (resource_type == rtEnd) {
      dialog.focus.erase (0, based_upon ().length ());
      for (unsigned int i = 0; i < resources.size (); i++) {
        if (dialog.focus == resources[i]) {
          filename_based_upon = filenames[i];
          resource_type = resource_get_type (filename_based_upon);
        }
      }
    }
    Shortcuts shortcuts (0);
    shortcuts.stockbutton (okbutton);
    shortcuts.stockbutton (cancelbutton);
    shortcuts.stockbutton (helpbutton);
    build_table_and_type (shortcuts);
    build_gui_from_resource_type (shortcuts, filename_based_upon);
    shortcuts.process ();
  }
  if (type_gui ()) {
    gui ();
  }
}


bool NewResourceDialog::type_gui ()
{
  ustring type = resource_type_to_text (resource_type);
  bool ok = !type.empty ();
  gtk_label_set_text (GTK_LABEL (label_type_short), "Select the resource type");
  gtk_label_set_text (GTK_LABEL (label_type_long), type.c_str());
  gui_okay (image_type_ok, label_type_ok, ok);
  if (!mytemplatefile.empty ()) {
    gtk_widget_set_sensitive (image_type_ok, false);
    gtk_widget_set_sensitive (label_type_ok, false);
    gtk_widget_set_sensitive (label_type_short, false);
    gtk_widget_set_sensitive (button_type, false);
  }
  return ok;
}


ustring NewResourceDialog::based_upon ()
{
  return "Based upon ";
}


// Title.


void NewResourceDialog::on_entry_title_changed (GtkEditable *editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_title ();
}


void NewResourceDialog::on_entry_title ()
{
  title = gtk_entry_get_text (GTK_ENTRY (entry_title));
  title = trim (title);
  gui ();
}


bool NewResourceDialog::title_gui ()
{
  bool ok = true;
  ustring text = "Enter the title";
  if (title.empty ()) ok = false;
  if (mytemplatefile.empty ()) {
    vector <ustring> resources;
    vector <ustring> filenames;
    resources = resource_get_resources (filenames, false);
    set <ustring> resource_set (resources.begin (), resources.end ());
    if (resource_set.find (title) != resource_set.end ()) {
      ok = false;
      text = "A resource with this name already exists";
    }
  } else {
    gtk_widget_set_sensitive (image_title_ok, false);
    gtk_widget_set_sensitive (label_title_ok, false);
    gtk_widget_set_sensitive (label_title, false);
    gtk_widget_set_sensitive (entry_title, false);
  }
  gui_okay (image_title_ok, label_title_ok, ok);
  gtk_label_set_text (GTK_LABEL (label_title), text.c_str());
  return ok;
}


// Home page.


void NewResourceDialog::on_entry_home_page_changed (GtkEditable *editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_home_page ();
}


void NewResourceDialog::on_entry_home_page ()
{
  home_page = gtk_entry_get_text (GTK_ENTRY (entry_home_page));
  home_page = trim (home_page);
  gui ();
}


bool NewResourceDialog::home_page_gui ()
{
  bool ok = true;
  ustring text = "Enter the home page";
  if (home_page.empty ()) ok = false;
  gui_okay (image_home_page_ok, label_home_page_ok, ok);
  gtk_label_set_text (GTK_LABEL (label_home_page), text.c_str());
  return ok;
}


// URL constructor.


void NewResourceDialog::on_entry_url_constructor_changed (GtkEditable *editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_url_constructor ();
}


void NewResourceDialog::on_entry_url_constructor ()
{
  url_constructor = gtk_entry_get_text (GTK_ENTRY (entry_url_constructor));
  url_constructor = trim (url_constructor);
  gui ();
}


bool NewResourceDialog::url_constructor_gui ()
{
  bool ok = true;
  ustring text = "Enter the url constructor";
  if (url_constructor.empty ()) ok = false;
  if (ok && (url_constructor.find (resource_url_constructor_book ()) == string::npos)) {
    ok = false;
    text.append (" - no \"");
    text.append (resource_url_constructor_book ());
    text.append ("\" found");
  }
  if (ok && (url_constructor.find (resource_url_constructor_chapter ()) == string::npos)) {
    ok = false;
    text.append (" - no \"");
    text.append (resource_url_constructor_chapter ());
    text.append ("\" found");
  }
  if (ok && (url_constructor.find (resource_url_constructor_verse ()) == string::npos)) {
    ok = false;
    text.append (" - no \"");
    text.append (resource_url_constructor_verse ());
    text.append ("\" found");
  }
  gui_okay (image_url_constructor_ok, label_url_constructor_ok, ok);
  gtk_label_set_text (GTK_LABEL (label_url_constructor), text.c_str());
  return ok;
}


// Index file constructor.


void NewResourceDialog::on_entry_index_constructor_changed (GtkEditable *editable, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_entry_index_constructor ();
}


void NewResourceDialog::on_entry_index_constructor ()
{
  index_constructor = gtk_entry_get_text (GTK_ENTRY (entry_index_constructor));
  index_constructor = trim (index_constructor);
  gui ();
}


bool NewResourceDialog::index_constructor_gui ()
{
  bool ok = true;
  ustring text = "Enter the index file constructor";
  if (index_constructor.empty ()) ok = false;
  if (ok && (index_constructor.find (resource_url_constructor_book ()) == string::npos)) {
    ok = false;
    text.append (" - no \"");
    text.append (resource_url_constructor_book ());
    text.append ("\" found");
  }
  if (ok && (index_constructor.find (resource_url_constructor_chapter ()) == string::npos)) {
    ok = false;
    text.append (" - no \"");
    text.append (resource_url_constructor_chapter ());
    text.append ("\" found");
  }
  if (ok && (index_constructor.find (resource_url_constructor_verse ()) == string::npos)) {
    ok = false;
    text.append (" - no \"");
    text.append (resource_url_constructor_verse ());
    text.append ("\" found");
  }
  gui_okay (image_index_constructor_ok, label_index_constructor_ok, ok);
  gtk_label_set_text (GTK_LABEL (label_index_constructor), text.c_str());
  return ok;
}


// Book names.


void NewResourceDialog::on_books_button_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_books_button ();
}


void NewResourceDialog::on_books_button ()
{
  ResourceBooksDialog dialog (books);
  if (dialog.run () == GTK_RESPONSE_OK) {
    books = dialog.newbooks;
    if (books_gui ()) {
      gui ();
    }
  }
}


bool NewResourceDialog::books_gui ()
{
  bool ok = true;
  gtk_label_set_text (GTK_LABEL (label_books_short), "Enter the book names for in the constructor");
  if (books.empty ()) ok = false;
  ustring text;
  text.append ("Books entered: ");
  text.append (convert_to_string (books.size ()));
  gtk_label_set_text (GTK_LABEL (label_books_long), text.c_str());
  gui_okay (image_books_ok, label_books_ok, ok);
  return ok;
}


// Book test.


void NewResourceDialog::on_checkbutton_book_test_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_checkbutton_book_test ();
}


void NewResourceDialog::on_checkbutton_book_test ()
{
  if (book_test_gui ())
    gui ();
}


void NewResourceDialog::on_button_book_test_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_button_book_test ();
}


void NewResourceDialog::on_button_book_test ()
{
  TestResourcesDialog dialog (0);
  switch (resource_type) {
    case rtForeignDataURLForEachVerse:
    {
      dialog.tests (resource_type, url_constructor, books, "");
      break;
    }
    case rtIndexFileOnFlyForEachVerseWithDifferentAnchors:
    {
      dialog.tests (resource_type, index_constructor, books, workingdirectory);
      break;
    }
    case rtURLForEachVerse:
    {
      dialog.tests (resource_type, url_constructor, books, workingdirectory);
      break;
    }
    case rtEnd:
    {
      break;
    }
  }
  if (dialog.run () == GTK_RESPONSE_OK) {
    book_test_gui ();
  }
}


bool NewResourceDialog::book_test_gui ()
{
  bool ok = true;
  ok = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_book_test));
  gui_okay (image_book_test_ok, label_book_test_ok, ok);
  ustring text;
  if (ok) text = "All books are considered correct";
  else text = "Click button to test all books";
  gtk_label_set_text (GTK_LABEL (label_book_test), text.c_str ());
  return ok;
}


// Add files.


void NewResourceDialog::on_add_files_button_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_add_files_button ();
}


void NewResourceDialog::on_add_files_button ()
{
  vector <ustring> files;
  files = gtkw_file_chooser_open_multiple (NULL, "", "");
  if (files.empty ()) return;
  ProgressWindow progresswindow ("Adding files", false);
  progresswindow.set_iterate (0, 1, files.size ());
  for (unsigned int i = 0; i < files.size (); i++) {
    progresswindow.iterate ();
    unix_cp (files[i], workingdirectory);
  }
  if (add_files_gui ()) {
    gui ();
  }
}


bool NewResourceDialog::add_files_gui ()
{
  bool ok = true;
  gtk_label_set_text (GTK_LABEL (label_add_files_short), "Add files to the resource");
  ReadFiles rf (workingdirectory, "", "");
  if (rf.files.empty ()) ok = false;
  ustring text;
  text.append ("Files in resource: ");
  text.append (convert_to_string (rf.files.size ()));
  gtk_label_set_text (GTK_LABEL (label_add_files_long), text.c_str());
  gui_okay (image_add_files_ok, label_add_files_ok, ok);
  return ok;
}


// Add directories.


void NewResourceDialog::on_add_directories_button_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_add_directories_button ();
}


void NewResourceDialog::on_add_directories_button ()
{
  ustring directory = gtkw_file_chooser_select_folder (NULL, "", "");
  if (directory.empty ()) return;
  ProgressWindow progresswindow ("Adding directory", false);
  unix_cp_r (directory, workingdirectory);
  if (add_directories_gui ()) {
    gui ();
  }
}


bool NewResourceDialog::add_directories_gui ()
{
  bool ok = true;
  gtk_label_set_text (GTK_LABEL (label_add_directories_short), "Add directories to the resource");
  ReadDirectories rd (workingdirectory, "", "");
  ustring text;
  text.append ("Directories in resource: ");
  text.append (convert_to_string (rd.directories.size ()));
  gtk_label_set_text (GTK_LABEL (label_add_directories_long), text.c_str());
  gui_okay (image_add_directories_ok, label_add_directories_ok, ok);
  return ok;
}


// Anchor names.


void NewResourceDialog::on_anchors_button_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_anchors_button ();
}


void NewResourceDialog::on_anchors_button ()
{
  ResourceBooksDialog dialog (anchors);
  if (dialog.run () == GTK_RESPONSE_OK) {
    anchors = dialog.newbooks;
    if (anchors_gui ()) {
      gui ();
    }
  }
}


bool NewResourceDialog::anchors_gui ()
{
  bool ok = true;
  gtk_label_set_text (GTK_LABEL (label_anchors_short), "Enter the book names for in the anchors");
  if (anchors.empty ()) ok = false;
  ustring text;
  text.append ("Books entered: ");
  text.append (convert_to_string (anchors.size ()));
  gtk_label_set_text (GTK_LABEL (label_anchors_long), text.c_str());
  gui_okay (image_anchors_ok, label_anchors_ok, ok);
  return ok;
}


// Anchor test.


void NewResourceDialog::on_checkbutton_anchor_test_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_checkbutton_anchor_test ();
}


void NewResourceDialog::on_checkbutton_anchor_test ()
{
  if (anchor_test_gui ())
    gui ();
}


void NewResourceDialog::on_button_anchor_test_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_button_anchor_test ();
}


void NewResourceDialog::on_button_anchor_test ()
{
  TestResourcesDialog dialog (0);
  switch (resource_type) {
    case rtIndexFileOnFlyForEachVerseWithDifferentAnchors:
    {
      dialog.tests (resource_type, index_constructor, books, anchors, workingdirectory);
      break;
    }
    case rtForeignDataURLForEachVerse:
    case rtURLForEachVerse:
    case rtEnd:
    {
      break;
    }
  }
  if (dialog.run () == GTK_RESPONSE_OK) {
    anchor_test_gui ();
  }
}


bool NewResourceDialog::anchor_test_gui ()
{
  bool ok = true;
  ok = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_anchor_test));
  gui_okay (image_anchor_test_ok, label_anchor_test_ok, ok);
  ustring text;
  if (ok) text = "All anchors are considered correct";
  else text = "Click button to test all anchors";
  gtk_label_set_text (GTK_LABEL (label_anchor_test), text.c_str ());
  return ok;
}


// Write anchors.


void NewResourceDialog::on_checkbutton_write_anchors_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_checkbutton_write_anchors ();
}


void NewResourceDialog::on_checkbutton_write_anchors ()
{
  if (write_anchors_gui ())
    gui ();
}


void NewResourceDialog::on_button_write_anchors_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_button_write_anchors ();
}


void NewResourceDialog::on_button_write_anchors ()
{
  ResourceConverter2Dialog dialog (workingdirectory, resource_conversion_type, 
                                   chapter_pattern_prefix, chapter_pattern_index, chapter_pattern_suffix,
                                   verse_pattern_prefix, verse_pattern_index, verse_pattern_suffix);
  if (dialog.run () == GTK_RESPONSE_OK) {
    resource_conversion_type = dialog.resource_conversion_type;
    chapter_pattern_prefix = dialog.chapter_pattern_prefix;
    chapter_pattern_index = dialog.chapter_pattern_index;
    chapter_pattern_suffix = dialog.chapter_pattern_suffix;
    verse_pattern_prefix = dialog.verse_pattern_prefix;
    verse_pattern_index = dialog.verse_pattern_index;
    verse_pattern_suffix = dialog.verse_pattern_suffix;
    write_anchors_gui ();
  }
}


bool NewResourceDialog::write_anchors_gui ()
{
  bool ok = true;
  ok = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_write_anchors));
  gui_okay (image_write_anchors_ok, label_write_anchors_ok, ok);
  ustring text;
  if (ok) text = "All anchors are considered to be there";
  else text = "Click button to write anchors";
  gtk_label_set_text (GTK_LABEL (label_write_anchors), text.c_str ());
  return ok;
}


// General gui.

  
void NewResourceDialog::gui ()
{
  bool ok = true;
  if (!type_gui ()) ok = false;
  switch (resource_type) {
    case rtForeignDataURLForEachVerse:
    {
      if (!title_gui ()) ok = false;
      if (!home_page_gui ()) ok = false;
      if (!url_constructor_gui ()) ok = false;
      if (!books_gui ()) ok = false;
      if (!book_test_gui ()) ok = false;
      break;
    }
    case rtIndexFileOnFlyForEachVerseWithDifferentAnchors:
    {
      if (!title_gui ()) ok = false;
      if (!add_files_gui ()) ok = false;
      if (!add_directories_gui ()) ok = false;
      if (!home_page_gui ()) ok = false;
      if (!index_constructor_gui ()) ok = false;
      if (!books_gui ()) ok = false;
      if (!book_test_gui ()) ok = false;
      if (!anchors_gui ()) ok = false;
      if (!anchor_test_gui ()) ok = false;
      if (!write_anchors_gui ()) ok = false;
      break;
    }
    case rtURLForEachVerse:
    {
      if (!title_gui ()) ok = false;
      if (!add_files_gui ()) ok = false;
      if (!add_directories_gui ()) ok = false;
      if (!home_page_gui ()) ok = false;
      if (!url_constructor_gui ()) ok = false;
      if (!books_gui ()) ok = false;
      if (!book_test_gui ()) ok = false;
      if (!write_anchors_gui ()) ok = false;
      break;
    }
    case rtEnd:
    {
      break;
    }
  }
  gtk_widget_set_sensitive (okbutton, ok);
}


void NewResourceDialog::build_gui_from_resource_type (Shortcuts& shortcuts, const ustring& filename_based_upon)
{
  switch (resource_type) {
    case rtForeignDataURLForEachVerse:
    {
      build_entry (image_title_ok, label_title_ok, label_title, "", entry_title, resource_get_title (filename_based_upon), G_CALLBACK (on_entry_title_changed));
      build_entry (image_home_page_ok, label_home_page_ok, label_home_page, "", entry_home_page, resource_get_home_page (filename_based_upon), G_CALLBACK (on_entry_home_page_changed));
      build_entry (image_url_constructor_ok, label_url_constructor_ok, label_url_constructor, "", entry_url_constructor, resource_get_url_constructor (filename_based_upon), G_CALLBACK (on_entry_url_constructor_changed));
      build_button (image_books_ok, label_books_ok, label_books_short, button_books, "Books", shortcuts, G_CALLBACK (on_books_button_clicked), label_books_long);
      build_checkbutton_button (image_book_test_ok, label_book_test_ok, 
                                checkbutton_book_test, "Books are okay", G_CALLBACK (on_checkbutton_book_test_toggled),
                                button_book_test, "Test books", G_CALLBACK (on_button_book_test_clicked),
                                shortcuts,
                                label_book_test);
      books = resource_get_books (filename_based_upon);
      on_entry_title ();
      on_entry_home_page ();
      on_entry_url_constructor ();
      break;
    }
    case rtIndexFileOnFlyForEachVerseWithDifferentAnchors:
    {
      build_entry (image_title_ok, label_title_ok, label_title, "", entry_title, resource_get_title (filename_based_upon), G_CALLBACK (on_entry_title_changed));
      build_button (image_add_files_ok, label_add_files_ok, label_add_files_short, button_add_files, "Files", shortcuts, G_CALLBACK (on_add_files_button_clicked), label_add_files_long);
      build_button (image_add_directories_ok, label_add_directories_ok, label_add_directories_short, button_add_directories, "Directories", shortcuts, G_CALLBACK (on_add_directories_button_clicked), label_add_directories_long);
      build_entry (image_home_page_ok, label_home_page_ok, label_home_page, "", entry_home_page, resource_get_home_page (filename_based_upon), G_CALLBACK (on_entry_home_page_changed));
      build_entry (image_index_constructor_ok, label_index_constructor_ok, label_index_constructor, "", entry_index_constructor, resource_get_index_file_constructor (filename_based_upon), G_CALLBACK (on_entry_index_constructor_changed));
      build_button (image_books_ok, label_books_ok, label_books_short, button_books, "Books", shortcuts, G_CALLBACK (on_books_button_clicked), label_books_long);
      build_checkbutton_button (image_book_test_ok, label_book_test_ok, 
                                checkbutton_book_test, "Books are okay", G_CALLBACK (on_checkbutton_book_test_toggled),
                                button_book_test, "Test books", G_CALLBACK (on_button_book_test_clicked),
                                shortcuts,
                                label_book_test);
      build_button (image_anchors_ok, label_anchors_ok, label_anchors_short, button_anchors, "Anchors", shortcuts, G_CALLBACK (on_anchors_button_clicked), label_anchors_long);
      build_checkbutton_button (image_anchor_test_ok, label_anchor_test_ok, 
                                checkbutton_anchor_test, "Anchors are okay", G_CALLBACK (on_checkbutton_anchor_test_toggled),
                                button_anchor_test, "Test anchors", G_CALLBACK (on_button_anchor_test_clicked),
                                shortcuts,
                                label_anchor_test);
      build_checkbutton_button (image_write_anchors_ok, label_write_anchors_ok, 
                                checkbutton_write_anchors, "Anchors are there", G_CALLBACK (on_checkbutton_write_anchors_toggled),
                                button_write_anchors, "Write anchors", G_CALLBACK (on_button_write_anchors_clicked),
                                shortcuts,
                                label_write_anchors);
      books = resource_get_books (filename_based_upon);
      anchors = resource_get_anchors (filename_based_upon);
      on_entry_title ();
      on_entry_home_page ();
      on_entry_index_constructor ();
      break;
    }
    case rtURLForEachVerse:
    {
      build_entry (image_title_ok, label_title_ok, label_title, "", entry_title, resource_get_title (filename_based_upon), G_CALLBACK (on_entry_title_changed));
      build_button (image_add_files_ok, label_add_files_ok, label_add_files_short, button_add_files, "Files", shortcuts, G_CALLBACK (on_add_files_button_clicked), label_add_files_long);
      build_button (image_add_directories_ok, label_add_directories_ok, label_add_directories_short, button_add_directories, "Directories", shortcuts, G_CALLBACK (on_add_directories_button_clicked), label_add_directories_long);
      build_entry (image_home_page_ok, label_home_page_ok, label_home_page, "", entry_home_page, resource_get_home_page (filename_based_upon), G_CALLBACK (on_entry_home_page_changed));
      build_entry (image_url_constructor_ok, label_url_constructor_ok, label_url_constructor, "", entry_url_constructor, resource_get_url_constructor (filename_based_upon), G_CALLBACK (on_entry_url_constructor_changed));
      build_button (image_books_ok, label_books_ok, label_books_short, button_books, "Books", shortcuts, G_CALLBACK (on_books_button_clicked), label_books_long);
      build_checkbutton_button (image_book_test_ok, label_book_test_ok, 
                                checkbutton_book_test, "Books are okay", G_CALLBACK (on_checkbutton_book_test_toggled),
                                button_book_test, "Test books", G_CALLBACK (on_button_book_test_clicked),
                                shortcuts,
                                label_book_test);
      build_checkbutton_button (image_write_anchors_ok, label_write_anchors_ok, 
                                checkbutton_write_anchors, "Anchors are there", G_CALLBACK (on_checkbutton_write_anchors_toggled),
                                button_write_anchors, "Write anchors", G_CALLBACK (on_button_write_anchors_clicked),
                                shortcuts,
                                label_write_anchors);
      books = resource_get_books (filename_based_upon);
      on_entry_title ();
      on_entry_home_page ();
      on_entry_url_constructor ();
      break;
    }
    case rtEnd:
    {
      break;
    }
  }
}


void NewResourceDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((NewResourceDialog *) user_data)->on_okbutton ();
}


void NewResourceDialog::on_okbutton ()
{
  // Get a non-existing directory, or if editing an editable resource, take that directory.
  ustring directory = shell_clean_filename (title);
  directory = gw_build_filename (directories_get_resources (), directory);
  if (mytemplatefile.empty ()) {
    while (g_file_test (directory.c_str(), G_FILE_TEST_EXISTS)) {
      directory.append ("1");
    }
  }
  gw_mkdir_with_parents (directory);
  
  // Copy all files from the temporal directory to the resource-directory.
  if (directory != workingdirectory) {
    ustring command = "cd" + shell_quote_space (workingdirectory);
    command.append ("; mv *" + shell_quote_space (directory));
    system (command.c_str());
  }
  
  // Assemble the template.
  GKeyFile * keyfile = g_key_file_new ();
  vector <ustring> templatedata;
  switch (resource_type) {
    case rtForeignDataURLForEachVerse:
    case rtURLForEachVerse:
    {
      g_key_file_set_integer (keyfile, resource_template_general_group (), resource_template_type_key (), int (resource_type));
      g_key_file_set_string (keyfile, resource_template_general_group (), resource_template_title_key (), title.c_str());
      g_key_file_set_string (keyfile, resource_template_general_group (), resource_template_home_page_key (), home_page.c_str());
      g_key_file_set_string (keyfile, resource_template_general_group (), resource_template_url_constructor_key (), url_constructor.c_str());
      vector <unsigned int> ids = books_type_to_ids (btUnknown);
      for (unsigned int i = 0; i < ids.size (); i++) {
        ustring book = books [ids[i]];
        if (!book.empty ()) {
          g_key_file_set_string (keyfile, resource_template_books_group (), books_id_to_english (ids[i]).c_str(), book.c_str());
        }
      }
      break;
    }
    case rtIndexFileOnFlyForEachVerseWithDifferentAnchors:
    {
      g_key_file_set_integer (keyfile, resource_template_general_group (), resource_template_type_key (), int (resource_type));
      g_key_file_set_string (keyfile, resource_template_general_group (), resource_template_title_key (), title.c_str());
      g_key_file_set_string (keyfile, resource_template_general_group (), resource_template_home_page_key (), home_page.c_str());
      g_key_file_set_string (keyfile, resource_template_general_group (), resource_template_index_file_constructor_key (), index_constructor.c_str());
      vector <unsigned int> ids = books_type_to_ids (btUnknown);
      for (unsigned int i = 0; i < ids.size (); i++) {
        ustring book = books [ids[i]];
        if (!book.empty ()) {
          g_key_file_set_string (keyfile, resource_template_books_group (), books_id_to_english (ids[i]).c_str(), book.c_str());
        }
        ustring anchor = anchors [ids[i]];
        if (!anchor.empty ()) {
          g_key_file_set_string (keyfile, resource_template_anchors_group (), books_id_to_english (ids[i]).c_str(), anchor.c_str());
        }
      }
      break;
    }
    case rtEnd:
    {
      break;
    }
  }
  
  // Write the template.
  edited_template_file = gw_build_filename (directory, "resource-template.ini");
  gchar * data = g_key_file_to_data (keyfile, NULL, NULL);
  if (data) {
    g_file_set_contents (edited_template_file.c_str(), data, -1, NULL);
    g_free (data);
  }
  g_key_file_free (keyfile);
}


void NewResourceDialog::create_working_directory (const ustring& templatefile)
// This creates a working directory where files belonging to the resource reside.
{
  workingdirectory = gw_build_filename (directories_get_temp (), "resource-editor");
  if (!templatefile.empty ()) {
    ustring pathname = gw_path_get_dirname (templatefile);
    if (pathname != directories_get_package_data ()) {
      workingdirectory = pathname;
    }
  }
  if (gw_path_get_dirname (workingdirectory) == directories_get_temp ()) {
    unix_rmdir (workingdirectory);
    gw_mkdir_with_parents (workingdirectory);
  }
}
