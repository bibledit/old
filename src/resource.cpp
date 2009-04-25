/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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


#include "utilities.h"
#include <glib.h>
#include "resource.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "resource_utils.h"
extern "C" {
  #include <gtkhtml/gtkhtml.h>
}


Resource::Resource(GtkWidget * window)
{
  // Save and initialize varables.
  resource_type = rtEnd;
  browser2 = NULL;

  // Build GUI.
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  GtkWidget *hbox;
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  label = gtk_label_new("");
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(hbox), label, true, true, 0);
  gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_MIDDLE);

  homebutton = gtk_button_new();
  gtk_widget_show(homebutton);
  gtk_box_pack_start(GTK_BOX(hbox), homebutton, false, false, 0);
  gtk_button_set_focus_on_click(GTK_BUTTON(homebutton), FALSE);

  GtkWidget *image;
  image = gtk_image_new_from_stock("gtk-home", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(image);
  gtk_container_add(GTK_CONTAINER(homebutton), image);

  browser = new WebkitBrowser(vbox);

  g_signal_connect((gpointer) homebutton, "clicked", G_CALLBACK(on_homebutton_clicked), gpointer(this));
}


Resource::~Resource()
{
  delete browser;
  if (browser2)
    delete browser2;
  gtk_widget_destroy(vbox);
}


void Resource::focus()
{
  browser->focus();
}


bool Resource::focused()
{
  bool focus = false;
  if (browser->focused())
    focus = true;
  if (browser2)
    if (browser2->focused())
      focus = true;
  return focus;
}


void Resource::copy()
{
  browser->copy();
  if (browser2)
    browser2->copy();
}


void Resource::go_to(const Reference & reference)
{
  ustring url;
  switch (resource_type) {
  case rtForeignDataURLForEachVerse:
  case rtURLForEachVerse:
    {
      url = resource_construct_url(url_structure, book_renderer, reference);
      break;
    }
  case rtURLForEachVerseAboveURLFilterBelowWithDifferentAnchors:
    {
      url = resource_construct_url(url_structure, book_renderer, anchor_renderer, reference);
      break;
    }
  case rtEnd:
    {
      break;
    }
  }
  if (!url.empty()) {
    browser->go_to(url);
  }
}


void Resource::open(const ustring & filename)
{
  mytemplatefile = filename;
  resource_type = resource_get_type(filename);
  url_structure = resource_url_modifier(resource_get_url_constructor(filename), resource_type, filename);
  index_file_structure = resource_get_index_file_constructor(filename);
  book_renderer = resource_get_books(filename);
  anchor_renderer = resource_get_anchors(filename);
  gtk_label_set_text(GTK_LABEL(label), resource_get_title(filename).c_str());
  homepage = resource_url_modifier(resource_get_home_page(filename), resource_type, filename);
  homepage2 = resource_url_modifier(resource_get_lower_home_page(filename), resource_type, filename);
  url_filter = resource_get_lower_url_filter(filename);
  browser->set_home_page (homepage);
  browser->go_to(homepage);
  focus();
}


ustring Resource::template_get()
{
  return mytemplatefile;
}


void Resource::on_homebutton_clicked(GtkButton * button, gpointer user_data)
{
  ((Resource *) user_data)->homebutton_clicked();
}


void Resource::homebutton_clicked()
{
  browser->go_to(homepage);
  if (browser2)
    browser2->go_to(homepage2);
}


time_t Resource::last_focused_time()
{
  return browser->last_focused_time;
}


/*

Todo Please make two sample resources with verse anchors

Need to move to libwebkit for better display. 

Try all current resources, whether they work well.

libcurl may be dropped, together with the pages cache.

Need to remove the double header display in a Resource.

The NextBible site, so please create a similar sample resource for
http://net.bible.org/bible.php?book=Pro&chapter=1

Related to this, BE will need to be able to log in to GoogleDocs if I am going to be able to access pages we have created like the Sabda pages. 
I tried basing a new resource on the Sabda.org resource, using the GoogleDoc address. GoogleDocs that are "published" do show up, 
but private pages are password protected.

Imaging editing the GoogleDoc from Bibledit's browser!

We may need an easier Resource Editor, because the current one seems to be too difficult.
Probably something with tabs, and the editor at the left, and direct testing of the effects right in the editor.

We may need an option to download all verses and store these locally.
So Bibledit goes to a website, downloads all verses, stores them under the URL's, so these can be kept for later use offline.

rtURLForEachVerseAboveURLFilterBelowWithDifferentAnchors This option no longer valid, because there's no second browser.
 
*/

/*

Todo switching to libwebkit.

Bibledit may use httrack to download sites to local files for offline viewing. But httrack may produce huge files, so a warning may have to be given
to the user. Else Bibledit can manually download the page for each verse, using htmltrack one level deep and store that info, then 
retrieve it later for each verse.

To remove the home button from the Resource, and transfer it to the Browser, so the Browser needs to be informed what the home page is.

We may have to use the WebKitFrame (or so) to control the various frames in the NET Bible.

The users has to manually edit a text file, and there are controls that automatically test the result of any change.
Whether to test the home page, various verses, and so on.
Or an automated test of all possible chapters. Or verses per chapter, or indeed, everything.
Or we have a dialog that does the testing, then applies this test to the currently open Resource,
so the user can press the hOme button, can go to certain places in the text, or indeed let it automatically run all things.

Entry of the home url or verse url goes so that it is general, and the user can for local things, can enter "<local>".

*/
