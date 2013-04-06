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

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_widget_show (dialog_vbox1);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

  webview = webkit_web_view_new();
  gtk_widget_show (webview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), webview);

  g_signal_connect((gpointer) webview, "navigation-policy-decision-requested", G_CALLBACK(on_navigation_policy_decision_requested), gpointer(this));

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(dialog));
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);
 
  new InDialogHelp(dialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  gtk_widget_grab_focus (webview);
  gtk_widget_grab_default (okbutton);
  
  // Home page.
  load_webview ("");
}


MaintenanceDialog::~MaintenanceDialog()
{
  gtk_widget_destroy(dialog);
}


int MaintenanceDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


gboolean MaintenanceDialog::on_navigation_policy_decision_requested (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, gpointer user_data)
{
  ((MaintenanceDialog *) user_data)->navigation_policy_decision_requested (request, navigation_action, policy_decision);
  return true;
}


void MaintenanceDialog::navigation_policy_decision_requested (WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision)
{
  // Store scrolling position for the now active url.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  scrolling_position[active_url] = gtk_adjustment_get_value (adjustment);

  // Get the reason for this navigation policy request.
  WebKitWebNavigationReason reason = webkit_web_navigation_action_get_reason (navigation_action);
  
  // If a new page if loaded, allow the navigation, and exit.
  if (reason == WEBKIT_WEB_NAVIGATION_REASON_OTHER) {
    webkit_web_policy_decision_use (policy_decision);
    return;
  }

  // Don't follow pseudo-links clicked on this page.
  webkit_web_policy_decision_ignore (policy_decision);
  
  // Load new page depending on the pseudo-link clicked.
  load_webview (webkit_network_request_get_uri (request));
}


void MaintenanceDialog::load_webview (const gchar * url)
{
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
  }
  
  htmlwriter.finish();
  if (display_another_page) {
    // Load the page.
    webkit_web_view_load_string (WEBKIT_WEB_VIEW (webview), htmlwriter.html.c_str(), NULL, NULL, NULL);
    // Scroll to the position that possibly was stored while this url was last active.
    GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
    gtk_adjustment_set_value (adjustment, scrolling_position[active_url]);
  }
}


void MaintenanceDialog::html_add_home (HtmlWriter2& htmlwriter)
{
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add ("home", "Home");
  htmlwriter.paragraph_close();
}

