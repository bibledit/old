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
#include "resourceviewergui.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "moziller.h"
#include "resource_utils.h"
#include "dialogradiobutton.h"


ResourcesGUI::ResourcesGUI (GtkWidget * notebook_vbox)
{
  // Save and initialize varables.
  my_notebook_vbox = notebook_vbox;

  // Build gui.
  toolbar = gtk_toolbar_new ();
  gtk_widget_show (toolbar);
  gtk_box_pack_start (GTK_BOX (notebook_vbox), toolbar, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH);
  gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar));

  toolitem4 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem4);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem4);

  button_back = gtk_button_new ();
  gtk_widget_show (button_back);
  gtk_container_add (GTK_CONTAINER (toolitem4), button_back);
  gtk_button_set_focus_on_click (GTK_BUTTON (button_back), FALSE);

  image4 = gtk_image_new_from_stock ("gtk-go-back", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image4);
  gtk_container_add (GTK_CONTAINER (button_back), image4);

  toolitem5 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem5);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem5);

  button_forward = gtk_button_new ();
  gtk_widget_show (button_forward);
  gtk_container_add (GTK_CONTAINER (toolitem5), button_forward);
  gtk_button_set_focus_on_click (GTK_BUTTON (button_forward), FALSE);

  image5 = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_container_add (GTK_CONTAINER (button_forward), image5);

  toolitem1 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem1);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem1);

  button_refresh = gtk_button_new ();
  gtk_widget_show (button_refresh);
  gtk_container_add (GTK_CONTAINER (toolitem1), button_refresh);
  gtk_button_set_focus_on_click (GTK_BUTTON (button_refresh), FALSE);

  image1 = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_container_add (GTK_CONTAINER (button_refresh), image1);

  toolitem2 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem2);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem2);

  button_stop = gtk_button_new ();
  gtk_widget_show (button_stop);
  gtk_container_add (GTK_CONTAINER (toolitem2), button_stop);
  gtk_button_set_focus_on_click (GTK_BUTTON (button_stop), FALSE);

  gtk_widget_set_sensitive (button_stop, false);
  
  image2 = gtk_image_new_from_stock ("gtk-stop", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_container_add (GTK_CONTAINER (button_stop), image2);

  toolitem3 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem3);
  gtk_container_add (GTK_CONTAINER (toolbar), toolitem3);

  button_home = gtk_button_new ();
  gtk_widget_show (button_home);
  gtk_container_add (GTK_CONTAINER (toolitem3), button_home);
  gtk_button_set_focus_on_click (GTK_BUTTON (button_home), FALSE);

  image3 = gtk_image_new_from_stock ("gtk-home", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_container_add (GTK_CONTAINER (button_home), image3);

  notebook = gtk_notebook_new ();
  gtk_widget_show (notebook);
  gtk_box_pack_start (GTK_BOX (notebook_vbox), notebook, TRUE, TRUE, 0);
  
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), true);

  g_signal_connect ((gpointer) button_back, "clicked", G_CALLBACK (on_button_back_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_forward, "clicked", G_CALLBACK (on_button_forward_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_refresh, "clicked", G_CALLBACK (on_button_refresh_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_stop, "clicked", G_CALLBACK (on_button_stop_clicked), gpointer(this));
  g_signal_connect ((gpointer) button_home, "clicked", G_CALLBACK (on_button_home_clicked), gpointer(this));
  g_signal_connect ((gpointer) notebook, "switch_page", G_CALLBACK (on_notebook_switch_page), gpointer(this));
  
  gtk_widget_grab_focus (notebook);

  ustring profile = gw_build_filename (directories_get_temp (), "mozilla");
  gtk_moz_embed_set_profile_path (profile.c_str(), "bibledit");
  gtk_moz_embed_push_startup ();
  
  // Timers.
  g_timeout_add (100, GSourceFunc (on_update_gui_timeout), gpointer (this));
  
  // Load desktop.
  desktop_load ();
}


ResourcesGUI::~ResourcesGUI ()
{
  // Save desktop.
  desktop_save ();
  
  // Destroy all resources.
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    delete resource_objects[i];
  }
  
  // Embedded browser shutdown.
  gtk_moz_embed_pop_startup ();
}


void ResourcesGUI::open (const ustring& filename, int method)
/*
Open a new resource.
filename: The template filename to use.
method  : -1 : Ask.
           0 : In current window.
           1 : In a new tab.
*/
{
  // Ask where to open a resource if the method is negative 
  // and if there are no resources yet.
  if (resource_objects.empty ()) {
    method = 1;
  } else {
    if (method < 0) {
      method = 0;
      vector <ustring> labels;
      labels.push_back ("In the current window");
      labels.push_back ("In a new tab");
      extern Settings * settings;
      RadiobuttonDialog dialog ("Open resource", "Where would you like this new resource to be displayed?", labels, settings->genconfig.split_view_open_method_get ());
      if (dialog.run () == GTK_RESPONSE_OK) {
        method = dialog.selection;
        settings->genconfig.split_view_open_method_set (method);
      }
    }
  }
  
  // The vertical box for the split view, and the label.
  GtkWidget *vbox_split;
  GtkWidget *label;
  
  // Handle display method, whether a new tab or to add it to the current tab.
  if (method == 1) {
    
    // Display in a new tab.

    // Create the vertical box.
    vbox_split = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_split);

    // The label for the notebook tab.
    label = gtk_label_new ("");
    gtk_widget_show (label);

    // Append a page to the notebook with the paneds and the label.
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_split, label);
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), -1);
    
  } else {
    
    // Display in existing tab.
    gint pagenumber = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));
    vbox_split = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pagenumber);
    label = gtk_notebook_get_tab_label (GTK_NOTEBOOK (notebook), vbox_split);
    
  }

  // Add a new vertical box for holding the progressbar and the browser.
  GtkWidget *vbox;
  vbox = gtk_vbox_new (false, 0);
  gtk_widget_show (vbox);
  gtk_box_pack_start (GTK_BOX (vbox_split), vbox, TRUE, TRUE, 0);
  
  // Create the new resource.
  Resource * resource = new Resource (vbox, gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), -1), label);
  resource_objects.push_back (resource);
  resource->open (filename);
  
  // Focus handlers.
  g_signal_connect ((gpointer) resource->focus_signal, "clicked", G_CALLBACK (on_focus_signal_clicked), gpointer(this));
  resource->focus ();

  // If there is more than one tab, show them.
  gint page_count = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook));
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), page_count > 1);
}


void ResourcesGUI::close ()
{
  // Get the focused resource.
  Resource * resource = focused_resource ();
  if (resource == NULL) return;

  // Erase the resource's pointer from the container.  
  vector<Resource *>::iterator iter;
  iter = resource_objects.begin();
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    if (resource == resource_objects[i]) {
      resource_objects.erase (iter);
      break;
    }
    iter++;
  }

  // Get the page of this resource.
  GtkWidget * page = resource->parent_notebook_page ();
  
  // Delete the resource.
  delete resource;

  // Count the number of remaining resources that use this notebook page.
  // At the same time focus the first one using it.
  unsigned int resources_using_this_page = 0;
  bool resource_focused = false;
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    if (page == resource_objects[i]->parent_notebook_page ()) {
      resources_using_this_page++;
      if (!resource_focused) {
        resource_objects[i]->focus ();
        resource_focused = true;
      }
    }
  }
  
  // Delete the page from the notebook if no resource is using it anymore.
  if (resources_using_this_page == 0) {
    for (int i = 0; i < gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)); i++) {
      if (page == gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i)) {
        gtk_notebook_remove_page (GTK_NOTEBOOK (notebook), i);
        break;
      }
    }
  }
}


Resource * ResourcesGUI::focused_resource ()
{
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    if (resource_objects[i]->focused ()) {
      return resource_objects[i];
    }
  }
  return NULL;
}


void ResourcesGUI::go_to (const Reference& reference)
{
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    resource_objects[i]->go_to (reference);
  }
}


void ResourcesGUI::on_button_back_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourcesGUI *) user_data)->on_button_back ();
}


void ResourcesGUI::on_button_back ()
{
  Resource * resource = focused_resource ();
  if (resource) {
    resource->go_back ();
  }
}


void ResourcesGUI::on_button_forward_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourcesGUI *) user_data)->on_button_forward ();
}


void ResourcesGUI::on_button_forward ()
{
  Resource * resource = focused_resource ();
  if (resource) {
    resource->go_forward ();
  }
}


void ResourcesGUI::on_button_refresh_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourcesGUI *) user_data)->on_button_refresh ();
}


void ResourcesGUI::on_button_refresh ()
{
  Resource * resource = focused_resource ();
  if (resource) {
    resource->reload ();
  }
}


void ResourcesGUI::on_button_stop_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourcesGUI *) user_data)->on_button_stop ();
}


void ResourcesGUI::on_button_stop ()
{
  Resource * resource = focused_resource ();
  if (resource) {
    resource->stop ();
  }
}


void ResourcesGUI::on_button_home_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourcesGUI *) user_data)->on_button_home ();
}


void ResourcesGUI::on_button_home ()
{
  Resource * resource = focused_resource ();
  if (resource) {
    resource->home ();
  }
}


bool ResourcesGUI::on_update_gui_timeout (gpointer user_data)
{
  return ((ResourcesGUI *) user_data)->on_update_gui ();
}


bool ResourcesGUI::on_update_gui ()
{
  Resource * resource = focused_resource ();
  gtk_widget_set_sensitive (button_back, resource && resource->can_go_back ());
  gtk_widget_set_sensitive (button_forward, resource && resource->can_go_forward ());
  gtk_widget_set_sensitive (button_stop, resource && resource->loading);
  gtk_widget_set_sensitive (button_refresh, resource != NULL);
  gtk_widget_set_sensitive (button_home, resource != NULL);
  return true;
}


void ResourcesGUI::reload (const ustring& oldfilename, const ustring& newfilename)
{
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    if (oldfilename == resource_objects[i]->template_get ()) {
      resource_objects[i]->open (newfilename);
    }
  }
}


void ResourcesGUI::on_focus_signal_clicked (GtkButton *button, gpointer user_data)
{
  ((ResourcesGUI *) user_data)->on_focus_signal (button);
}


void ResourcesGUI::on_focus_signal (GtkButton *button)
{
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    if (GTK_WIDGET (button) != resource_objects[i]->focus_signal) {
      resource_objects[i]->defocus ();
    }
  }
}


void ResourcesGUI::on_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
  ((ResourcesGUI *) user_data)->notebook_switch_page (page_num);
}


void ResourcesGUI::notebook_switch_page (guint page_num)
{
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    resource_objects[i]->parent_notebook_switches_to_page (gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), page_num));
  }
}


void ResourcesGUI::desktop_save ()
// Save the resources desktop.
{
  // Get the filenames of the templates, sorted on page number.
  vector <unsigned int> pagenumbers;
  vector <ustring> filenames;
  for (unsigned int i = 0; i < resource_objects.size (); i++) {
    ustring filename = resource_objects[i]->template_get ();
    unsigned int pagenumber = 0;
    GtkWidget * notebook_page = resource_objects[i]->parent_notebook_page ();
    for (int i2 = 0; i2 < gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)); i2++) {
      if (notebook_page == gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i2)) {
        pagenumber = i2;
      }
    }
    pagenumbers.push_back (pagenumber);
    filenames.push_back (filename);
  }
  quick_sort (pagenumbers, filenames, 0, filenames.size());
  
  // Save them, including their page numbers.
  extern Settings * settings;
  settings->genconfig.resources_set (filenames);
  vector <int> pages;
  for (unsigned int i = 0; i < pagenumbers.size (); i++) {
    pages.push_back (pagenumbers[i]);
  }
  settings->genconfig.resource_pages_set (pages);
}


void ResourcesGUI::desktop_load ()
// Load the saved resources desktop.
{
  // Get the templates of the resources, and the page numbers they were loaded on on last shutdown.
  extern Settings * settings;
  vector <ustring> filenames = settings->genconfig.resources_get ();
  vector <int> pagenumbers = settings->genconfig.resource_pages_get ();
  if (filenames.size () != pagenumbers.size ()) {
    pagenumbers.clear ();
    for (unsigned int i = 0; i < filenames.size (); i++) {
      pagenumbers.push_back (i);
    }
  }
  // Open the resources in the right notebook pages.
  int previouspage = -1;
  for (unsigned int i = 0; i < filenames.size (); i++) {
    open (filenames[i], pagenumbers[i] - previouspage);
    previouspage = pagenumbers[i];
  }
}
