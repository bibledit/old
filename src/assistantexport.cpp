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


#include "libraries.h"
#include <glib.h>
#include "assistantexport.h"
#include "help.h"
#include "settings.h"
#include "utilities.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "unixwrappers.h"
#include "git.h"
#include "projectutils.h"
#include "snapshots.h"
#include "gtkwrappers.h"
#include "compress.h"


ExportAssistant::ExportAssistant(int dummy) :
AssistantBase("Export", "")
// Export assistant.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  introduction ("This helps you exporting your data");

  // Configuration and initialization.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();

  // Build the GUI for the task selector.
  vbox_select_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_select_type);
  page_number_select_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_select_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_select_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_select_type, "What would you like to export?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_select_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_select_type, true);

  GSList *radiobutton_select_type_group = NULL;

  radiobutton_select_type_bible = gtk_radio_button_new_with_mnemonic (NULL, "Bible");
  gtk_widget_show (radiobutton_select_type_bible);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_bible, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_bible), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_bible));

  radiobutton_select_type_references = gtk_radio_button_new_with_mnemonic (NULL, "References");
  gtk_widget_show (radiobutton_select_type_references);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_references, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_references), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_references));

  radiobutton_select_type_stylesheet = gtk_radio_button_new_with_mnemonic (NULL, "Stylesheet");
  gtk_widget_show (radiobutton_select_type_stylesheet);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_stylesheet, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_stylesheet), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_stylesheet));

  radiobutton_select_type_notes = gtk_radio_button_new_with_mnemonic (NULL, "Notes");
  gtk_widget_show (radiobutton_select_type_notes);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_notes, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_notes), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_notes));

  Shortcuts shortcuts_select_type (0);
  shortcuts_select_type.button (radiobutton_select_type_bible);
  shortcuts_select_type.button (radiobutton_select_type_references);
  shortcuts_select_type.button (radiobutton_select_type_stylesheet);
  shortcuts_select_type.button (radiobutton_select_type_notes);
  shortcuts_select_type.consider_assistant();
  shortcuts_select_type.process();

  // Confirm or change Bible.
  vbox_bible_name = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox_bible_name);
  page_number_bible_name = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_name);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_name), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_name, "Is this the right Bible?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_name, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, true);

  label_project_name = gtk_label_new ("Bible name");
  gtk_widget_show (label_project_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), label_project_name, FALSE, FALSE, 0);

  button_bible_name = gtk_button_new ();
  gtk_widget_show (button_bible_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), button_bible_name, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_bible_name, "clicked", G_CALLBACK (on_button_bible_name_clicked), gpointer (this));

  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label12;

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_bible_name), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic ("Choose another one");
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox1), label12, FALSE, FALSE, 0);

  Shortcuts shortcuts_bible_name (0);
  shortcuts_bible_name.label (label12);
  shortcuts_bible_name.consider_assistant();
  shortcuts_bible_name.process();

  // Select what type to export a Bible to.
  vbox_bible_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_bible_type);
  page_number_bible_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_type, "What would you like to export it to?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_type, true);

  GSList *radiobutton_bible_type_group = NULL;

  radiobutton_bible_usfm = gtk_radio_button_new_with_mnemonic (NULL, "Unified Standard Format Marker (USFM)");
  gtk_widget_show (radiobutton_bible_usfm);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_usfm, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_usfm), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_usfm));

  radiobutton_bible_bibleworks = gtk_radio_button_new_with_mnemonic (NULL, "BibleWorks Version Database Compiler");
  gtk_widget_show (radiobutton_bible_bibleworks);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_bibleworks, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_bibleworks), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_bibleworks));

  radiobutton_bible_osis = gtk_radio_button_new_with_mnemonic (NULL, "Open Scripture Information Standard (OSIS)");
  gtk_widget_show (radiobutton_bible_osis);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_osis, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_osis), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_osis));

  radiobutton_bible_sword = gtk_radio_button_new_with_mnemonic (NULL, "CrossWire SWORD");
  gtk_widget_show (radiobutton_bible_sword);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_sword, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_sword), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_sword));

  radiobutton_bible_opendocument = gtk_radio_button_new_with_mnemonic (NULL, "OpenDocument");
  gtk_widget_show (radiobutton_bible_opendocument);
  gtk_box_pack_start (GTK_BOX (vbox_bible_type), radiobutton_bible_opendocument, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_opendocument), radiobutton_bible_type_group);
  radiobutton_bible_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_opendocument));

  Shortcuts shortcuts_select_bible_type (0);
  shortcuts_select_bible_type.button (radiobutton_bible_usfm);
  shortcuts_select_bible_type.button (radiobutton_bible_bibleworks);
  shortcuts_select_bible_type.button (radiobutton_bible_osis);
  shortcuts_select_bible_type.button (radiobutton_bible_sword);
  shortcuts_select_bible_type.button (radiobutton_bible_opendocument);
  shortcuts_select_bible_type.consider_assistant();
  shortcuts_select_bible_type.process();

  // Select file where to save to.
  vbox_file = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_file);
  page_number_file = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_file);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_file), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_file, "Where would you like to save it?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_file, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_file, false);

  button_file = gtk_button_new ();
  gtk_widget_show (button_file);
  gtk_box_pack_start (GTK_BOX (vbox_file), button_file, FALSE, FALSE, 0);

  GtkWidget *alignment2;
  GtkWidget *hbox2;
  GtkWidget *image2;

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_file), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  label_file = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label_file);
  gtk_box_pack_start (GTK_BOX (hbox2), label_file, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_file, "clicked", G_CALLBACK (on_button_file_clicked), gpointer(this));

  // Build the confirmation stuff.
  label_confirm = gtk_label_new ("Backup is about to be made");
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Backup is about to be made");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Backup was made");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish building assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

ExportAssistant::~ExportAssistant()
{
}

void ExportAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_assistant_prepare(page);
}


void ExportAssistant::on_assistant_prepare (GtkWidget *page)
{
  extern Settings *settings;

  if (page == vbox_bible_name) {
    // Prepare for the page to confirm or change the Bible.
    if (bible_name.empty()) {
      bible_name = settings->genconfig.project_get();
    }
    gtk_label_set_text (GTK_LABEL (label_project_name), bible_name.c_str());
    if (bible_name.empty()) {
      gtk_label_set_text (GTK_LABEL (label_project_name), "No Bible selected");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, !bible_name.empty());
  }

  if (page == vbox_file) {
    gtk_label_set_text (GTK_LABEL (label_file), filename.c_str());
    if (filename.empty()) {
      gtk_label_set_text (GTK_LABEL (label_file), "(None)");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_file, !filename.empty());
  }
}


void ExportAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_assistant_apply();
}


void ExportAssistant::on_assistant_apply ()
{
  /*
  // Save all configurations.
  extern Settings *settings;
  settings->save();

  // Make the backup.
  switch (get_type()) {
    case btBible:
    {
      backup_bible (bible_name, filename);
      break;
    }
    case btNotes:
    {
      backup_notes (filename);
      break;
    }
    case btAll:
    {
      backup_all (filename);
      break;
    }
  }
  */
  
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint ExportAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ExportAssistant *) user_data)->assistant_forward (current_page);
}

gint ExportAssistant::assistant_forward (gint current_page)
{
  // Default behaviour is to go to the next page.
  gint new_page_number = current_page + 1;

  if (current_page == page_number_select_type) {
    if (get_type () != etBible) {
      new_page_number = page_number_file;
    }
  }

  // Return the new page.
  return new_page_number;
}


void ExportAssistant::on_button_bible_name_clicked (GtkButton *button, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_button_bible_name ();
}


void ExportAssistant::on_button_bible_name ()
{
  project_select(bible_name);
  on_assistant_prepare (vbox_bible_name);
}


void ExportAssistant::on_button_file_clicked (GtkButton *button, gpointer user_data)
{
  ((ExportAssistant *) user_data)->on_button_file ();
}


void ExportAssistant::on_button_file ()
{
  ustring file = gtkw_file_chooser_save (assistant, "", filename);
  if (!file.empty()) {
    filename = file;
    compress_ensure_tar_gz_suffix (filename);
    on_assistant_prepare (vbox_file);
  }
}


ExportType ExportAssistant::get_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_bible))) {
    return etBible;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_references))) {
    return etReferences;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_stylesheet))) {
    return etStylesheet;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_notes))) {
    return etNotes;
  }
  return etBible;
}


/*

Todo version number in the man pages not updated

(2) The man pages all still say they are version 3.6. This is not a big
deal at all, and I left them alone, but long term it might be good to
automatically generate them at build time with a current version number
and build date in the top line of each (the lines that start with .TH). 





Todo OSIS file troubles


To create an Export Assistant, and move all export functions into that one.

Bible
  Unified Standard Format Marker (USFM) files
  * zipped?
  BibleWorks Version Database Compiler
  OSIS file
  * Recommended USFM to OSIS transformation
  * Stripped down for the Go Bible
  SWORD module
  OpenDocument
References
Stylesheet
Project notes







I'm a newcomer to all this, but I took a quick look at the Shona one. I
used bibledit 3.7 (and SWORD 1.6.0RC3) to export it as a "SWORD module
and OSIS file". Using the "old method" mostly worked, although based on
some of the output from osis2mod, I suspect I am (or bibledit is) using
an incorrect versification... what versification system do these Shona
and Ndebele bibles use? If that info is encoded in the *.usfm files
somehow, forgive me, but I didn't see it when I looked at them.

Once I found the XML file (see below), I discovered that the OSIS XML
file does not validate, according to the command:

xmllint --noout --schema
http://www.bibletechnologies.net/osisCore.2.1.1.xsd ~/osis-from-usfm.xml

It generates over 1800 lines of error messages. I think that Bibledit
should be careful to generate 100% valid OSIS XML. In fact, perhaps if
xmllint is available at run time, bibledit could use it to validate the
OSIS export file, before running it through osis2mod? Maybe this use of
xmllint can be a checkbox option in the export dialog, or something like
that?

I don't know exactly what you sent to modules@crosswire.org, but ideally
you would provide an OSIS file which (a) is valid OSIS XML and (b)
osis2mod can use without generating much (or even any!) warning or
informational text. If you also provide a workable .conf file for the
module with appropriate translator and copyright info etc. in it, I
think that is all that is needed :)

Incidentally, thinking ahead a little, now that osis2mod has a -v for
versification switch you may want to add the ability for bibledit to use
that switch to select the appropriate versification for the project
being exported. The current code in bibledit (in src/export_utils.cpp )
does not seem to do this (probably because the -v switch is very new!).

Lastly, before I forget: the way the OSIS XML file ends up at a fixed
(but undocumented?) filename in the user's home directory feels a bit
unhelpful. I ended up searching for all XML files on my machine that
were less than a day old, in order to discover it :)

Maybe the OSIS XML file name (and path) could be a field that is given
defaults during the export dialog, but which the user can change if
desired, so they can choose (and will know!) where they put the file?
Failing that, or in addition to that, perhaps you could consider
including the full osis2mod command line in the system log, so that
looking in there will help novice users (like me!) find the XML file
more easily.

Jonathan 




*/
