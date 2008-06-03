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
#include "resource_utils.h"
#include "dialogradiobutton.h"

ResourcesGUI::ResourcesGUI(GtkWidget * notebook_vbox) {
  // Save and initialize varables.
  my_notebook_vbox = notebook_vbox;

  // Build gui.
  notebook = gtk_notebook_new();
  gtk_widget_show(notebook);
  gtk_box_pack_start(GTK_BOX (notebook_vbox), notebook, TRUE, TRUE, 0);

  gtk_notebook_set_scrollable(GTK_NOTEBOOK (notebook), true);

  gtk_widget_grab_focus(notebook);

  // Load desktop.
  desktop_load();
}

ResourcesGUI::~ResourcesGUI() {
  // Save desktop.
  desktop_save();

  // Destroy all resources.
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    delete resource_objects[i];
  }
}

void ResourcesGUI::open(const ustring& filename, int method)
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
  if (resource_objects.empty()) {
    method = 1;
  } else {
    if (method < 0) {
      method = 0;
      vector <ustring> labels;
      labels.push_back("In the current window");
      labels.push_back("In a new tab");
      extern Settings * settings;
      RadiobuttonDialog dialog("Open resource", "Where would you like this new resource to be displayed?", labels, settings->genconfig.split_view_open_method_get());
      if (dialog.run() == GTK_RESPONSE_OK) {
        method = dialog.selection;
        settings->genconfig.split_view_open_method_set(method);
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
    vbox_split = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox_split);

    // The label for the notebook tab.
    label = gtk_label_new("");
    gtk_widget_show(label);

    // Append a page to the notebook with the paneds and the label.
    gtk_notebook_append_page(GTK_NOTEBOOK (notebook), vbox_split, label);
    gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook), -1);

  } else {

    // Display in existing tab.
    gint pagenumber = gtk_notebook_get_current_page(GTK_NOTEBOOK (notebook));
    vbox_split = gtk_notebook_get_nth_page(GTK_NOTEBOOK (notebook), pagenumber);
    label = gtk_notebook_get_tab_label(GTK_NOTEBOOK (notebook), vbox_split);

  }

  // Add a new vertical box for holding the progressbar and the browser.
  GtkWidget *vbox;
  vbox = gtk_vbox_new(false, 0);
  gtk_widget_show(vbox);
  gtk_box_pack_start(GTK_BOX (vbox_split), vbox, TRUE, TRUE, 0);

  // Create the new resource.
  Resource * resource = new Resource (vbox, gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), -1), label);
  resource_objects.push_back(resource);
  resource->open(filename);

  // Focus handling.
  resource->focus();

  // If there is more than one tab, show them.
  gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK (notebook), page_count> 1);
}

void ResourcesGUI::close() {
  // Get the focused resource.
  Resource * resource = focused_resource();
  if (resource == NULL)
    return;

  // Erase the resource's pointer from the container.  
  vector<Resource *>::iterator iter;
  iter = resource_objects.begin();
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    if (resource == resource_objects[i]) {
      resource_objects.erase(iter);
      break;
    }
    iter++;
  }

  // Get the page of this resource.
  GtkWidget * page = resource->parent_notebook_page();

  // Delete the resource.
  delete resource;

  // Count the number of remaining resources that use this notebook page.
  // At the same time focus the first one using it.
  unsigned int resources_using_this_page = 0;
  bool resource_focused = false;
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    if (page == resource_objects[i]->parent_notebook_page()) {
      resources_using_this_page++;
      if (!resource_focused) {
        resource_objects[i]->focus();
        resource_focused = true;
      }
    }
  }

  // Delete the page from the notebook if no resource is using it anymore.
  if (resources_using_this_page == 0) {
    for (int i = 0; i < gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook)); i++) {
      if (page == gtk_notebook_get_nth_page(GTK_NOTEBOOK (notebook), i)) {
        gtk_notebook_remove_page(GTK_NOTEBOOK (notebook), i);
        break;
      }
    }
  }
}

Resource * ResourcesGUI::focused_resource() {
  // This function returns the resource that received focus most recently.
  // That means that the resources does not necessarily need to be focused now.
  Resource * resource= NULL;
  time_t most_recent_focus = 0;
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    if (resource_objects[i]->last_focused_time() > most_recent_focus) {
      most_recent_focus = resource_objects[i]->last_focused_time();
      resource = resource_objects[i];
    }
  }
  return resource;
}

void ResourcesGUI::go_to(const Reference& reference) {
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    resource_objects[i]->go_to(reference);
  }
}

void ResourcesGUI::reload(const ustring& oldfilename, const ustring& newfilename) {
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    if (oldfilename == resource_objects[i]->template_get()) {
      resource_objects[i]->open(newfilename);
    }
  }
}

void ResourcesGUI::on_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data) {
  ((ResourcesGUI *) user_data)->notebook_switch_page(page_num);
}

void ResourcesGUI::notebook_switch_page(guint page_num) {
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    resource_objects[i]->parent_notebook_switches_to_page(gtk_notebook_get_nth_page(GTK_NOTEBOOK (notebook), page_num));
  }
}

void ResourcesGUI::desktop_save()
// Save the resources desktop.
{
  // Get the filenames of the templates, sorted on page number.
  vector <unsigned int> pagenumbers;
  vector <ustring> filenames;
  for (unsigned int i = 0; i < resource_objects.size(); i++) {
    ustring filename = resource_objects[i]->template_get();
    unsigned int pagenumber = 0;
    GtkWidget * notebook_page = resource_objects[i]->parent_notebook_page();
    for (int i2 = 0; i2 < gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook)); i2++) {
      if (notebook_page == gtk_notebook_get_nth_page(GTK_NOTEBOOK (notebook), i2)) {
        pagenumber = i2;
      }
    }
    pagenumbers.push_back(pagenumber);
    filenames.push_back(filename);
  }
  quick_sort(pagenumbers, filenames, 0, filenames.size());

  // Save them, including their page numbers.
  extern Settings * settings;
  settings->genconfig.resources_set(filenames);
  vector <int> pages;
  for (unsigned int i = 0; i < pagenumbers.size(); i++) {
    pages.push_back(pagenumbers[i]);
  }
  settings->genconfig.resource_pages_set(pages);
}

void ResourcesGUI::desktop_load()
// Load the saved resources desktop.
{
  // Get the templates of the resources, and the page numbers they were loaded on on last shutdown.
  extern Settings * settings;
  vector <ustring> filenames = settings->genconfig.resources_get();
  vector <int> pagenumbers = settings->genconfig.resource_pages_get();
  if (filenames.size() != pagenumbers.size()) {
    pagenumbers.clear();
    for (unsigned int i = 0; i < filenames.size(); i++) {
      pagenumbers.push_back(i);
    }
  }
  // Open the resources in the right notebook pages.
  int previouspage = -1;
  for (unsigned int i = 0; i < filenames.size(); i++) {
    open(filenames[i], pagenumbers[i] - previouspage);
    previouspage = pagenumbers[i];
  }
}

