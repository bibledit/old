/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#include "dialogmaintenance.h"
#include "help.h"
#include "html.h"
#include "kjv.h"
#include "mechonmamre.h"
#include "htmlbrowser.h"


MaintenanceDialog::MaintenanceDialog(int dummy)
{
  dialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog), "Maintenance");
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (dialog), 800, 600);

  dialog_vbox1 = GTK_DIALOG (dialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  htmlview = gtk_html_new();
  gtk_widget_show(htmlview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), htmlview);
  gtk_html_allow_selection(GTK_HTML(htmlview), true);

  g_signal_connect((gpointer) htmlview, "link-clicked", G_CALLBACK(on_html_link_clicked), gpointer(this));
  g_signal_connect((gpointer) htmlview, "submit", G_CALLBACK(on_html_submit), gpointer(this));

  dialog_action_area1 = GTK_DIALOG (dialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);
 
  new InDialogHelp(dialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  gtk_widget_grab_focus(htmlview);
  gtk_widget_grab_default(okbutton);
  
  // Home page.
  html_link_clicked ("");
}


MaintenanceDialog::~MaintenanceDialog()
{
  gtk_widget_destroy(dialog);
}


int MaintenanceDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


gboolean MaintenanceDialog::on_html_link_clicked(GtkHTML * html, const gchar * url, gpointer user_data)
{
  ((MaintenanceDialog *) user_data)->html_link_clicked(url);
  return true;
}


void MaintenanceDialog::html_link_clicked (const gchar * url)
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

  if (active_url == mechon_mamre_html_entry_url ()) {
    html_add_home (htmlwriter);
    mechon_mamre_detailed_page (htmlwriter);
  }

  else if (active_url == mechon_mamre_download_url ()) {
    display_another_page = false;
    htmlbrowser ("http://www.mechon-mamre.org/dlct.htm", false, true);
  }

  else if (active_url == mechon_mamre_import_url ()) {
    mechon_mamre_action_page (htmlwriter);
  }

  else if (active_url == kjv_html_entry_url ()) {
    html_add_home (htmlwriter);
    kjv_detailed_page (htmlwriter);
  }

  else if (active_url == kjv_create_database_url ()) {
    html_add_home (htmlwriter);
    kjv_action_page (htmlwriter);
  }
  
  else {
    // The home page.
    htmlwriter.heading_open (3);
    htmlwriter.text_add ("Maintenance");
    htmlwriter.heading_close ();
    htmlwriter.paragraph_open();
    htmlwriter.text_add ("Some actions may have great impact on Bibledit-Gtk. Use with care.");
    htmlwriter.paragraph_close();

    // Mechon Mamre Hebrew text link.
    mechon_mamre_home_entry (htmlwriter);

    // Import Sword KJV text link.
    kjv_home_entry (htmlwriter);    
    
    /*    
    htmlwriter.paragraph_open();
    htmlwriter.form_open ("form", "page.html", "get");
    htmlwriter.paragraph_close();
    htmlwriter.paragraph_open();
    htmlwriter.input_open ("entry", "text", 25, "Enter");
    htmlwriter.paragraph_close();
    htmlwriter.paragraph_open();
    htmlwriter.input_open (NULL, "submit", 0, "Send");
    htmlwriter.paragraph_close();
    */
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


void MaintenanceDialog::on_html_submit (GtkHTML *html, const gchar *method, const gchar *url, const gchar *encoding, gpointer user_data)
{
  ((MaintenanceDialog *) user_data)->html_submit(method, url, encoding);
}


void MaintenanceDialog::html_submit (const gchar *method, const gchar *url, const gchar *encoding)
{
  char * encoding2 = strdup(encoding);
  html_url_decode(encoding2);
  free(encoding2);
}


void MaintenanceDialog::html_add_home (HtmlWriter2& htmlwriter)
{
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add ("", "Home");
  htmlwriter.paragraph_close();
}

