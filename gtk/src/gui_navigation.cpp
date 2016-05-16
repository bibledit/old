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

 
#include "utilities.h"
#include <glib.h>
#include "gui_navigation.h"
#include "combobox.h"
#include "settings.h"
#include "books.h"
#include "projectutils.h"
#include "gtkwrappers.h"
#include "bible.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "referencememory.h"
#include "dialogradiobutton.h"
#include <glib/gi18n.h>
#include "completion.h"

GuiNavigation::GuiNavigation(int dummy):
reference(0), track(0)
{
  // Initialize variables.
  parentToolbar = NULL;
  settingcombos = false;
  spinbutton_book_previous_value = 0;
  spinbutton_chapter_previous_value = 0;
  spinbutton_verse_previous_value = 0;
  delayer_event_id = 0;
  track_event_id = 0;
}


GuiNavigation::~GuiNavigation()
{
}


void GuiNavigation::build(GtkWidget * toolbar)
{
  parentToolbar = toolbar; // Save this so we have something to attach
			   // children widgets to (like error message
			   // dialogs), as well as the following
			   // contents.
  // Signalling buttons, but not visible.
  GtkToolItem *toolitem_delayed = gtk_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem_delayed), -1);
  new_reference_signal = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(toolitem_delayed), new_reference_signal);

  // Gui proper.
  GtkToolItem *toolitem_list_back = gtk_tool_item_new ();
  gtk_widget_show (GTK_WIDGET(toolitem_list_back));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem_list_back), -1);

  button_list_back = gtk_button_new ();
  gtk_widget_show (button_list_back);
  gtk_container_add (GTK_CONTAINER (toolitem_list_back), button_list_back);
  gtk_widget_set_can_focus (button_list_back, false);

  image_list_back = gtk_image_new_from_stock ("gtk-go-down", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_list_back);
  gtk_container_add (GTK_CONTAINER (button_list_back), image_list_back);

  GtkToolItem *toolitem1 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem1));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem1), -1);

  button_back = gtk_button_new();
  gtk_widget_show(button_back);
  gtk_container_add(GTK_CONTAINER(toolitem1), button_back);
  gtk_widget_set_can_focus (button_back, false);

  image1 = gtk_image_new_from_stock("gtk-go-back", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image1);
  gtk_container_add(GTK_CONTAINER(button_back), image1);

  GtkToolItem *toolitem2 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem2));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem2), -1);

  button_forward = gtk_button_new();
  gtk_widget_show(button_forward);
  gtk_container_add(GTK_CONTAINER(toolitem2), button_forward);
  gtk_widget_set_can_focus (button_forward, false);

  image2 = gtk_image_new_from_stock("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image2);
  gtk_container_add(GTK_CONTAINER(button_forward), image2);

  GtkToolItem *toolitem_list_forward = gtk_tool_item_new ();
  gtk_widget_show (GTK_WIDGET(toolitem_list_forward));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem_list_forward), -1);

  button_list_forward = gtk_button_new ();
  gtk_widget_show (button_list_forward);
  gtk_container_add (GTK_CONTAINER (toolitem_list_forward), button_list_forward);
  gtk_widget_set_can_focus (button_list_forward, false);

  image_list_forward = gtk_image_new_from_stock ("gtk-go-down", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image_list_forward);
  gtk_container_add (GTK_CONTAINER (button_list_forward), image_list_forward);

  GtkToolItem *toolitem3 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem3));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem3), -1);

  combo_book = gtk_combo_box_new_text();
  gtk_widget_show(combo_book);
  gtk_container_add(GTK_CONTAINER(toolitem3), combo_book);
  gtk_widget_set_can_focus (combo_book, false);

  GtkToolItem *toolitem4 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem4));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem4), -1);

  spinbutton_book_adj = gtk_adjustment_new(0, -1e+06, 1e+06, 1, 10, 0);
  spinbutton_book = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_book_adj), 1, 0);
  gtk_widget_show(spinbutton_book);
  gtk_container_add(GTK_CONTAINER(toolitem4), spinbutton_book);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_book), TRUE);
  gtk_editable_set_editable(GTK_EDITABLE(spinbutton_book), false);
  gtk_entry_set_visibility(GTK_ENTRY(spinbutton_book), false);
  gtk_entry_set_invisible_char(GTK_ENTRY(spinbutton_book), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(spinbutton_book), 0);
  gtk_widget_set_can_focus (spinbutton_book, false);

  GtkToolItem *toolitem6 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem6));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem6), -1);

  combo_chapter = gtk_combo_box_new_text();
  gtk_widget_show(combo_chapter);
  gtk_container_add(GTK_CONTAINER(toolitem6), combo_chapter);
  gtk_widget_set_can_focus (combo_chapter, false);

  GtkToolItem *toolitem5 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem5));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem5), -1);

  spinbutton_chapter_adj = gtk_adjustment_new(0, -1e+06, 1e+06, 1, 10, 0);
  spinbutton_chapter = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_chapter_adj), 1, 0);
  gtk_widget_show(spinbutton_chapter);
  gtk_container_add(GTK_CONTAINER(toolitem5), spinbutton_chapter);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_chapter), TRUE);
  gtk_editable_set_editable(GTK_EDITABLE(spinbutton_chapter), false);
  gtk_entry_set_visibility(GTK_ENTRY(spinbutton_chapter), false);
  gtk_entry_set_invisible_char(GTK_ENTRY(spinbutton_chapter), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(spinbutton_chapter), 0);
  gtk_widget_set_can_focus (spinbutton_chapter, false);

  GtkToolItem *toolitem8 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem8));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem8), -1);

  combo_verse = gtk_combo_box_new_text();
  gtk_widget_show(combo_verse);
  gtk_container_add(GTK_CONTAINER(toolitem8), combo_verse);
  gtk_widget_set_can_focus (combo_verse, false);

  GtkToolItem *toolitem7 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem7));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem7), -1);

  spinbutton_verse_adj = gtk_adjustment_new(0, -1e+06, 1e+06, 1, 10, 0);
  spinbutton_verse = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton_verse_adj), 1, 0);
  gtk_widget_show(spinbutton_verse);
  gtk_container_add(GTK_CONTAINER(toolitem7), spinbutton_verse);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinbutton_verse), TRUE);
  gtk_editable_set_editable(GTK_EDITABLE(spinbutton_verse), false);
  gtk_entry_set_visibility(GTK_ENTRY(spinbutton_verse), false);
  gtk_entry_set_invisible_char(GTK_ENTRY(spinbutton_verse), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(spinbutton_verse), 0);
  gtk_widget_set_can_focus (spinbutton_verse, false);

  // Resize the spinbuttons.
  gint defaultheight;
  GtkRequisition sizerequisition;
  gtk_widget_size_request(combo_verse, &sizerequisition);
  defaultheight = (int)(sizerequisition.height * 0.8);
  gtk_widget_set_size_request(spinbutton_book, int (defaultheight * 0.7), -1);
  gtk_widget_set_size_request(spinbutton_chapter, int (defaultheight * 0.7), -1);
  gtk_widget_set_size_request(spinbutton_verse, int (defaultheight * 0.7), -1);

  // Jump to verse (basically same as Go to reference dialog, built right into the main window navigation toolbar
  GtkSeparatorToolItem *separator = (GtkSeparatorToolItem*)gtk_separator_tool_item_new();
  gtk_separator_tool_item_set_draw (separator, true);
  gtk_widget_show(GTK_WIDGET(separator));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), (GtkToolItem*)separator, -1);
  GtkToolItem *toolitem10 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem10));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem10), -1);
  GtkLabel *jump_label = (GtkLabel *)gtk_label_new(_("Jump to: "));
  gtk_widget_show(GTK_WIDGET(jump_label));
  gtk_container_add(GTK_CONTAINER(toolitem10), GTK_WIDGET(jump_label));
  gtk_widget_set_can_focus (GTK_WIDGET(jump_label), false);

  GtkToolItem *toolitem9 = gtk_tool_item_new();
  gtk_widget_show(GTK_WIDGET(toolitem9));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolitem9), -1);

  GtkEntryBuffer *entry_free_buf = gtk_entry_buffer_new (NULL, 0);
  entry_free = gtk_entry_new_with_buffer(entry_free_buf);
  gtk_editable_select_region(GTK_EDITABLE(entry_free), 0, -1);
  gtk_widget_show(GTK_WIDGET(entry_free));
  gtk_container_add(GTK_CONTAINER(toolitem9), entry_free);
  gtk_widget_set_can_focus (entry_free, true);

  g_signal_connect((gpointer) entry_free, "activate", G_CALLBACK(on_entry_free_activate), gpointer(this));

  g_signal_connect((gpointer) button_list_back, "clicked", G_CALLBACK(on_button_list_back_clicked), gpointer(this));
  g_signal_connect((gpointer) button_back, "clicked", G_CALLBACK(on_button_back_clicked), gpointer(this));
  g_signal_connect((gpointer) button_forward, "clicked", G_CALLBACK(on_button_forward_clicked), gpointer(this));
  g_signal_connect((gpointer) button_list_forward, "clicked", G_CALLBACK(on_button_list_forward_clicked), gpointer(this));
  g_signal_connect((gpointer) combo_book, "changed", G_CALLBACK(on_combo_book_changed), gpointer(this));
  g_signal_connect((gpointer) spinbutton_book, "value_changed", G_CALLBACK(on_spinbutton_book_value_changed), gpointer(this));
  g_signal_connect((gpointer) combo_chapter, "changed", G_CALLBACK(on_combo_chapter_changed), gpointer(this));
  g_signal_connect((gpointer) spinbutton_chapter, "value_changed", G_CALLBACK(on_spinbutton_chapter_value_changed), gpointer(this));
  g_signal_connect((gpointer) combo_verse, "changed", G_CALLBACK(on_combo_verse_changed), gpointer(this));
  g_signal_connect((gpointer) spinbutton_verse, "value_changed", G_CALLBACK(on_spinbutton_verse_value_changed), gpointer(this));
}


void GuiNavigation::sensitive(bool sensitive)
{
  // Tracker.
  if (!sensitive)
    track.clear();
  tracker_sensitivity();
  // Set the sensitivity of the reference controls.
  gtk_widget_set_sensitive(combo_book, sensitive);
  gtk_widget_set_sensitive(spinbutton_book, sensitive);
  gtk_widget_set_sensitive(combo_chapter, sensitive);
  gtk_widget_set_sensitive(spinbutton_chapter, sensitive);
  gtk_widget_set_sensitive(combo_verse, sensitive);
  gtk_widget_set_sensitive(spinbutton_verse, sensitive);
  if (!sensitive) {
    // We are programatically going to change the comboboxes, 
    // and therefore don't want a signal during that operation.
    settingcombos = true;
    project.clear();
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_book), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_chapter), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_verse), -1);
    settingcombos = false;
  }
}


void GuiNavigation::set_project(const ustring & value, bool force)
// Sets the project of the object, and loads the books.
{
  // If the project is the same as the one already loaded, bail out.
  // Except when force is used.
  if (!force)
    if (value == project)
      return;

  // Save project, language.
  project = value;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  language = projectconfig->language_get();

  // Load books.
  load_books();
}


void GuiNavigation::clampref(Reference & reference)
// This clamps the reference, that is, it brings it within the limits of 
// the project.
{
  // If the reference exists, fine, bail out.
  if (reference_exists(reference))
    return;

  // Clamp the book.
  if (!project_book_exists(project, reference.book_get())) {
    vector < unsigned int >books = project_get_books(project);
    if (books.empty()) {
      reference.book_set(0);
    } else {
      reference.book_set(clamp(reference.book_get(), books[0], books[books.size() - 1]));
    }
  }
  // Clamp the chapter.
  vector < unsigned int >chapters = project_get_chapters(project, reference.book_get());
  set < unsigned int >chapterset(chapters.begin(), chapters.end());
  if (chapterset.find(reference.chapter_get()) == chapterset.end()) {
    reference.chapter_set(0);
    if (!chapters.empty()) { reference.chapter_set(chapters[0]); }
  }
  // Clamp the verse.
  vector < ustring > verses = project_get_verses(project, reference.book_get(), reference.chapter_get());
  set < ustring > verseset(verses.begin(), verses.end());
  if (verseset.find(reference.verse_get()) == verseset.end()) {
    reference.verse_set("0");
    if (!verses.empty())
      reference.verse_set(verses[0]);
  }
}

Reference GuiNavigation::get_current_ref (void)
{
  unsigned int currentbook = books_name_to_id(language, combobox_get_active_string(combo_book));
  unsigned int currentchapter = convert_to_int(combobox_get_active_string(combo_chapter));
  ustring currentverse = combobox_get_active_string(combo_verse);
  return Reference(currentbook, currentchapter, currentverse);
}

void GuiNavigation::display (const Reference & ref)
// This has the reference displayed.
{
  // Check whether the book is known to Bibledit. If not, bail out.
  {
    ustring bookname = books_id_to_english(ref.book_get());
    if (bookname.empty())
      return;
  }

  // Project configuration.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  language = projectconfig->language_get();

  // Find out if there is a change in book, chapter, verse.
  Reference currRef = get_current_ref();
  bool newbook    = (ref.book_get()    != currRef.book_get());
  bool newchapter = (ref.chapter_get() != currRef.chapter_get());
  bool newverse   = (ref.verse_get()   != currRef.verse_get());

  // If a new book, then there is also a new chapter, and so on.
  if (newbook)    { newchapter = true; }
  if (newchapter) { newverse = true; }

  // If there is no change in the reference, do nothing.
  if (!newverse) { return; }

  // Handle new book.
  if (newbook) {
    set_book(ref.book_get());
    load_chapters(ref.book_get());
  }
  // Handle new chapter.
  if (newchapter) {
    set_chapter(ref.chapter_get());
    load_verses(ref.book_get(), ref.chapter_get());
  }
  // Handle new verse.
  if (newverse) {
    set_verse(ref.verse_get());
    // Give signal.
    reference = ref;
    signal();
  }
}


void GuiNavigation::nextbook()
{
  vector <ustring> strings = combobox_get_strings(combo_book);
  if (strings.size() == 0)
    return;
  ustring ubook = combobox_get_active_string(combo_book);
  unsigned int index = 0;
  for (unsigned int i = 0; i < strings.size(); i++) {
    if (ubook == strings[i])
      index = i;
  }
  if (index == (strings.size() - 1)) {
    return;
  }
  reference.book_set(books_name_to_id(language, strings[++index]));
  // Consult database for most recent verse and chapter.
  if (!references_memory_retrieve (reference, false)) {
    reference.chapter_set(1);
    // Try to get to verse one.
    vector <ustring> verses = project_get_verses(project, reference.book_get(), reference.chapter_get());
    if (verses.size() > 1) {
      if (verses[0] == "0") { reference.verse_set(verses[1]); }
      else                  { reference.verse_set(verses[0]); }
    }
    else { reference.verse_set("0"); }
  }
  clampref(reference);
  set_book(reference.book_get());
  load_chapters(reference.book_get());
  set_chapter(reference.chapter_get());
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  signal();
}


void GuiNavigation::previousbook()
{
  // TO DO: Lot of common code with nextbook; combine for maintenance purposes
  vector < ustring > strings = combobox_get_strings(combo_book);
  if (strings.size() == 0)
    return;
  ustring ubook = combobox_get_active_string(combo_book);
  unsigned int index = 0;
  for (unsigned int i = 0; i < strings.size(); i++) {
    if (ubook == strings[i])
      index = i;
  }
  if (index == 0) {
    return;
  }
  reference.book_set(books_name_to_id(language, strings[--index]));
  if (!references_memory_retrieve (reference, false)) {
    reference.chapter_set(1);
    // Get the first verse of the chapter which is not "0".
    vector <ustring> verses = project_get_verses(project, reference.book_get(), reference.chapter_get());
    if (verses.size() > 1) {
      if (verses[0] == "0") { reference.verse_set(verses[1]); }
      else                  { reference.verse_set(verses[0]); }
    }
    else { reference.verse_set("0"); }
  }
  clampref(reference);
  set_book(reference.book_get());
  load_chapters(reference.book_get());
  set_chapter(reference.chapter_get());
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  signal();
}


void GuiNavigation::nextchapter()
{
  reference.chapter_set(convert_to_int(combobox_get_active_string(combo_chapter)));
  vector <ustring> strings = combobox_get_strings(combo_chapter);
  if (strings.size() == 0)
    return;
  unsigned int index = 0;
  for (unsigned int i = 0; i < strings.size(); i++) {
    if (reference.chapter_get() == convert_to_int(strings[i]))
      index = i;
  }
  if (index == (strings.size() - 1)) {
    crossboundarieschapter(true);
    return;
  }
  reference.chapter_set(convert_to_int(strings[++index]));
  if (!references_memory_retrieve (reference, true)) {
    // Find proper first verse.
    vector <ustring> verses = project_get_verses(project, reference.book_get(), reference.chapter_get());
    if (verses.size() > 1) { reference.verse_set(verses[1]); }
    else                   { reference.verse_set("0"); }
  }
  clampref(reference);
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  set_chapter(reference.chapter_get());
  signal();
}


void GuiNavigation::previouschapter()
{
  reference.chapter_set(convert_to_int(combobox_get_active_string(combo_chapter)));
  vector <ustring> strings = combobox_get_strings(combo_chapter);
  if (strings.size() == 0)
    return;
  unsigned int index = 0;
  for (unsigned int i = 0; i < strings.size(); i++) {
    if (reference.chapter_get() == convert_to_int(strings[i]))
      index = i;
  }
  if (index == 0) {
    crossboundarieschapter(false);
    return;
  }
  reference.chapter_set(convert_to_int(strings[--index]));
  if (!references_memory_retrieve (reference, true)) {
    // Find proper first verse.
    vector <ustring> verses = project_get_verses(project, reference.book_get(), reference.chapter_get());
    if (verses.size() > 1) { reference.verse_set(verses[1]); }
    else                   { reference.verse_set("0"); }
  }
  clampref(reference);
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  set_chapter(reference.chapter_get());
  signal();
}

void GuiNavigation::nextverse()
{
  ustring verse = combobox_get_active_string(combo_verse);
  vector < ustring > strings = combobox_get_strings(combo_verse);
  if (strings.size() == 0)
    return;
  unsigned int index = 0;
  for (unsigned int i = 0; i < strings.size(); i++) {
    if (verse == strings[i])
      index = i;
  }
  if (index == (strings.size() - 1)) {
    crossboundariesverse(true);
    return;
  }
  verse = strings[++index];
  set_verse(verse);
  reference.verse_set(verse);
  signal();
}


void GuiNavigation::previousverse()
{
  ustring verse = combobox_get_active_string(combo_verse);
  vector < ustring > strings = combobox_get_strings(combo_verse);
  if (strings.size() == 0)
    return;
  unsigned int index = 0;
  for (unsigned int i = 0; i < strings.size(); i++) {
    if (verse == strings[i])
      index = i;
  }
  if (index == 0) {
    crossboundariesverse(false);
    return;
  }
  verse = strings[--index];
  set_verse(verse);
  reference.verse_set(verse);
  signal();
}


void GuiNavigation::on_button_list_back_clicked(GtkButton * button, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_list_back();
}


void GuiNavigation::on_button_back_clicked(GtkButton * button, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_back();
}


void GuiNavigation::on_button_forward_clicked(GtkButton * button, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_forward();
}


void GuiNavigation::on_button_list_forward_clicked(GtkButton * button, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_list_forward();
}


void GuiNavigation::on_combo_book_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_combo_book();
}


void GuiNavigation::on_combo_chapter_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_combo_chapter();
}


void GuiNavigation::on_combo_verse_changed(GtkComboBox * combobox, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_combo_verse();
}


void GuiNavigation::on_spinbutton_book_value_changed(GtkSpinButton * spinbutton, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_spinbutton_book();
}


void GuiNavigation::on_spinbutton_chapter_value_changed(GtkSpinButton * spinbutton, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_spinbutton_chapter();
}


void GuiNavigation::on_spinbutton_verse_value_changed(GtkSpinButton * spinbutton, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_spinbutton_verse();
}


void GuiNavigation::on_back()
{
  if (track.previous_reference_available()) {
    track.get_previous_reference(reference);
    display(reference);
    // Don't store this reference in the tracker:
    signal(false);
  }
}


void GuiNavigation::on_forward()
{
  if (track.next_reference_available()) {
    track.get_next_reference(reference);
    display(reference);
    // Don't store this reference in the tracker:
    signal(false);
  }
}


void GuiNavigation::on_combo_book()
{
  if (settingcombos)
    return;
  reference.book_set(books_name_to_id(language, combobox_get_active_string(combo_book)));
  if (!references_memory_retrieve (reference, false)) {
    reference.chapter_set(1);
    reference.verse_set("1");
  }
  clampref(reference);
  load_chapters(reference.book_get());
  set_chapter(reference.chapter_get());
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  signal();
}


void GuiNavigation::on_combo_chapter()
{
  if (settingcombos)
    return;
  reference.chapter_set(convert_to_int(combobox_get_active_string(combo_chapter)));
  if (!references_memory_retrieve (reference, true)) {
    reference.verse_set("1");
  }
  clampref(reference);
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  signal();
}


void GuiNavigation::on_combo_verse()
{
  if (settingcombos) { return; }
  reference.verse_set(combobox_get_active_string(combo_verse));
  signal();
}


void GuiNavigation::on_spinbutton_book()
{
  if (settingcombos)
    return;
  int value = int (gtk_adjustment_get_value(GTK_ADJUSTMENT(spinbutton_book_adj)));
  bool next = (value < spinbutton_book_previous_value);
  unsigned int amount = abs(value - spinbutton_book_previous_value);
  for (unsigned int i = 0; i < amount; i++) {
    if (next)
      nextbook();
    else
      previousbook();
  }
  spinbutton_book_previous_value = value;
}


void GuiNavigation::on_spinbutton_chapter()
{
  if (settingcombos)
    return;
  int value = int (gtk_adjustment_get_value(GTK_ADJUSTMENT(spinbutton_chapter_adj)));
  bool next = (value < spinbutton_chapter_previous_value);
  unsigned int amount = abs(value - spinbutton_chapter_previous_value);
  for (unsigned int i = 0; i < amount; i++) {
    if (next)
      nextchapter();
    else
      previouschapter();
  }
  spinbutton_chapter_previous_value = value;
}


void GuiNavigation::on_spinbutton_verse()
{
  if (settingcombos)
    return;
  int value = int (gtk_adjustment_get_value(GTK_ADJUSTMENT(spinbutton_verse_adj)));
  bool next = (value < spinbutton_verse_previous_value);
  unsigned int amount = abs(value - spinbutton_verse_previous_value);
  for (unsigned int i = 0; i < amount; i++) {
    if (next)
      nextverse();
    else
      previousverse();
  }
  spinbutton_verse_previous_value = value;
}


bool GuiNavigation::reference_exists(Reference & reference)
// Returns true if the reference exists.
{
  bool exists = project_book_exists(project, reference.book_get());
  if (exists) {
    vector < unsigned int >chapters = project_get_chapters(project, reference.book_get());
    set < unsigned int >chapterset(chapters.begin(), chapters.end());
    if (chapterset.find(reference.chapter_get()) == chapterset.end()) {
      exists = false;
    }
  }
  if (exists) {
    vector < ustring > verses = project_get_verses(project, reference.book_get(), reference.chapter_get());
    set < ustring > verseset(verses.begin(), verses.end());
    if (verseset.find(reference.verse_get()) == verseset.end()) {
      exists = false;
    }
  }
  return exists;
}


void GuiNavigation::load_books()
{
  settingcombos = true;
  vector < unsigned int >books = project_get_books(project);
  vector < ustring > localizedbooks;
  for (unsigned int i = 0; i < books.size(); i++) {
    ustring localizedbook = books_id_to_name(language, books[i]);
    localizedbooks.push_back(localizedbook);
  }
  combobox_set_strings(combo_book, localizedbooks);
  settingcombos = false;
}


void GuiNavigation::set_book(unsigned int book)
{
  settingcombos = true;
  ustring localizedbook = books_id_to_name(language, book);
  combobox_set_string(combo_book, localizedbook);
  settingcombos = false;
}


void GuiNavigation::load_chapters(unsigned int book)
{
  settingcombos = true;
  vector < unsigned int >chapters = project_get_chapters(project, book);
  combobox_set_strings(combo_chapter, chapters);
  settingcombos = false;
}


void GuiNavigation::set_chapter(unsigned int chapter)
{
  settingcombos = true;
  combobox_set_string(combo_chapter, chapter);
  settingcombos = false;
}


void GuiNavigation::load_verses(unsigned int book, unsigned int chapter)
{
  settingcombos = true;
  vector < ustring > verses = project_get_verses(project, book, chapter);
  combobox_set_strings(combo_verse, verses);
  settingcombos = false;
}


void GuiNavigation::set_verse(const ustring & verse)
// Sets the requested verse in the combobox. 
// If that verse is not there, it searches for ranges or sequences, to see
// if the verse is part of those.
{
  // We're going to change the combobox programmatically.
  settingcombos = true;
  // Retrieve all verses the combobox has. 
  // If it has our verse, set it, and we're done.
  bool done = false;
  vector < ustring > verses = combobox_get_strings(combo_verse);
  for (unsigned int i = 0; i < verses.size(); i++) {
    if (verse == verses[i]) {
      combobox_set_string(combo_verse, verse);
      done = true;
    }
  }
  // See whether the requested verse is in a range or sequence of verses.
  unsigned int verse_int = convert_to_int(verse);
  for (unsigned int i = 0; i < verses.size(); i++) {
    if (!done) {
      vector < unsigned int >combined_verses = verse_range_sequence(verses[i]);
      for (unsigned int i2 = 0; i2 < combined_verses.size(); i2++) {
        if (verse_int == combined_verses[i2]) {
          combobox_set_string(combo_verse, verses[i]);
          done = true;
        }
      }
    }
  }
  // We're through changing the combobox programmmatically.
  settingcombos = false;
}


void GuiNavigation::signal(bool track)
{
  // Postpone any active delayed signal.
  gw_destroy_source(delayer_event_id);
  // Start the time out for the delayed signal.
  delayer_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(signal_delayer), gpointer(this), NULL);
  // Same thing again for the tracker signal.
  // This signal has a delay of some seconds, so that, 
  // if a reference is displaying for some seconds, it will be tracked. 
  // References that display for a shorter time will not be tracked.
  gw_destroy_source(track_event_id);
  if (track) {
    track_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 2000, GSourceFunc(signal_track), gpointer(this), NULL);
  }
  // Sensitivity of tracker controls.
  tracker_sensitivity();
  // Store the reference in the references memory, if enabled.
  extern Settings * settings;
  if (settings->genconfig.remember_verse_per_chapter_get()) {
    references_memory_store (reference);
  }
}


bool GuiNavigation::signal_delayer(gpointer user_data)
{
  ((GuiNavigation *) user_data)->signal_delayed();
  return false;
}


void GuiNavigation::signal_delayed()
{
  gtk_button_clicked(GTK_BUTTON(new_reference_signal));
}


bool GuiNavigation::signal_track(gpointer user_data)
{
  ((GuiNavigation *) user_data)->signal_tracking();
  return false;
}


void GuiNavigation::signal_tracking()
{
  track.store(reference);
}


void GuiNavigation::crossboundariesverse(bool forward)
// Handles the situation where a requested change in a verse needs to cross
// the boundaries of a book or a chapter.
{
  // Index of the currently opened book.
  int bookindex = -1;
  vector < unsigned int >allbooks = project_get_books(project);
  for (unsigned int i = 0; i < allbooks.size(); i++) {
    if (reference.book_get() == allbooks[i])
      bookindex = i;
  }
  // If the requested book does not exist, bail out.
  if (bookindex < 0) {
    return;
  }
  // Get the previous book, and the next book.
  int previousbookindex = bookindex - 1;
  previousbookindex = clamp(previousbookindex, 0, bookindex);
  unsigned int nextbookindex = bookindex + 1;
  nextbookindex = clamp(nextbookindex, 0, allbooks.size() - 1);
  // Get a list of all references in these on the most three books.
  vector < unsigned int >books;
  vector < unsigned int >chapters;
  vector < ustring > verses;
  for (unsigned int i = previousbookindex; i <= nextbookindex; i++) {
    // Get the book metrics.
    vector < unsigned int >bookchapters = project_get_chapters(project, allbooks[i]);
    for (unsigned int i2 = 0; i2 < bookchapters.size(); i2++) {
      vector < ustring > chapterverses = project_get_verses(project, allbooks[i], bookchapters[i2]);
      for (unsigned int i3 = 0; i3 < chapterverses.size(); i3++) {
        books.push_back(allbooks[i]);
        chapters.push_back(bookchapters[i2]);
        verses.push_back(chapterverses[i3]);
      }
    }
  }
  // Find the current reference in the list.
  ustring verse = reference.verse_get();
  unsigned int referenceindex = 0;
  for (unsigned int i = 0; i < books.size(); i++) {
    if (verse == verses[i]) {
      if (reference.chapter_get() == chapters[i]) {
        if (reference.book_get() == books[i]) {
          referenceindex = i;
          break;
        }
      }
    }
  }
  // Get the next or previous value.
  if (forward) {
    if (referenceindex == (chapters.size() - 1))
      return;
    referenceindex++;
  } else {
    if (referenceindex == 0)
      return;
    referenceindex--;
  }
  // Ok, give us the new values.
  reference.book_set(books[referenceindex]);
  reference.chapter_set(chapters[referenceindex]);
  reference.verse_set(verses[referenceindex]);
  // Set the values in the comboboxes, and signal a change.
  // TO DO: This sequence of code occurs several times; abstract it out
  set_book(reference.book_get());
  load_chapters(reference.book_get());
  set_chapter(reference.chapter_get());
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  signal();
}


void GuiNavigation::crossboundarieschapter(bool forward)
{
  // Index of the currently opened book.
  int bookindex = -1;
  vector < unsigned int >allbooks = project_get_books(project);
  for (unsigned int i = 0; i < allbooks.size(); i++) {
    if (reference.book_get() == allbooks[i])
      bookindex = i;
  }
  // If the requested book does not exist, bail out.
  if (bookindex < 0) {
    return;
  }
  // Get the previous book, and the next book.
  int previousbookindex = bookindex - 1;
  previousbookindex = clamp(previousbookindex, 0, bookindex);
  unsigned int nextbookindex = bookindex + 1;
  nextbookindex = clamp(nextbookindex, 0, allbooks.size() - 1);
  // Get a list of all references in these on the most three books.
  vector < unsigned int >books;
  vector < unsigned int >chapters;
  vector < ustring > first_verses;
  for (unsigned int i = previousbookindex; i <= nextbookindex; i++) {
    // Get the book metrics.
    vector < unsigned int >bookchapters = project_get_chapters(project, allbooks[i]);
    for (unsigned int i2 = 0; i2 < bookchapters.size(); i2++) {
      books.push_back(allbooks[i]);
      chapters.push_back(bookchapters[i2]);
      vector < ustring > chapterverses = project_get_verses(project, allbooks[i], bookchapters[i2]);
      // We take the first verse of each chapter, if available, else we take v 0.
      if (chapterverses.size() > 1)
        first_verses.push_back(chapterverses[1]);
      else
        first_verses.push_back(chapterverses[0]);
    }
  }
  // Find the current reference in the list.
  unsigned int referenceindex = 0;
  for (unsigned int i = 0; i < books.size(); i++) {
    if (reference.chapter_get() == chapters[i]) {
      if (reference.book_get() == books[i]) {
        referenceindex = i;
        break;
      }
    }
  }
  // Get the next or previous value.
  if (forward) {
    if (referenceindex == (chapters.size() - 1))
      return;
    referenceindex++;
  } else {
    if (referenceindex == 0)
      return;
    referenceindex--;
  }
  // Ok, give us the new values.
  reference.book_set(books[referenceindex]);
  reference.chapter_set(chapters[referenceindex]);
  if (forward) {
    reference.verse_set(first_verses[referenceindex]);
  } else {
    reference.verse_set(first_verses[referenceindex]);
  }
  // Set the values in the comboboxes, and signal a change.
  set_book(reference.book_get());
  load_chapters(reference.book_get());
  set_chapter(reference.chapter_get());
  load_verses(reference.book_get(), reference.chapter_get());
  set_verse(reference.verse_get());
  signal();
}


void GuiNavigation::tracker_sensitivity()
{
  // Buttons.
  gtk_widget_set_sensitive(button_list_back, track.previous_reference_available());
  gtk_widget_set_sensitive(button_back, track.previous_reference_available());
  gtk_widget_set_sensitive(button_forward, track.next_reference_available());
  gtk_widget_set_sensitive(button_list_forward, track.next_reference_available());
}


void GuiNavigation::on_list_back ()
{
  vector <Reference> references;
  track.get_previous_references (references);
  vector <ustring> labels;
  for (unsigned int i = 0; i < references.size(); i++) {
    labels.push_back (references[i].human_readable (language));
  }
  RadiobuttonDialog dialog (_("Go back"), _("Where would you like to go back to?"), labels, 0, true);
  if (dialog.run () == GTK_RESPONSE_OK) {
    for (unsigned int i = 0; i <= dialog.selection; i++) {
      on_back();
    }
  }
}


void GuiNavigation::on_list_forward ()
{
  vector <Reference> references;
  track.get_next_references (references);
  vector <ustring> labels;
  for (unsigned int i = 0; i < references.size(); i++) {
    labels.push_back (references[i].human_readable (language));
  }
  RadiobuttonDialog dialog (_("Go forward"), _("Where would you like to go forward to?"), labels, 0, true);
  if (dialog.run () == GTK_RESPONSE_OK) {
    for (unsigned int i = 0; i <= dialog.selection; i++) {
      on_forward();
    }
  }
}

void GuiNavigation::on_entry_free_activate (GtkEntry *entry_box, gpointer user_data)
{
  ((GuiNavigation *) user_data)->on_entry_free ();
}

void GuiNavigation::on_entry_free ()
{
  bool newreference = false;
  Reference reference; // the new, target scripture reference
  // Code copied from GotoReferenceDialog::on_jump()
  Reference oldRef = get_current_ref();
  if (reference_discover(oldRef, gtk_entry_get_text(GTK_ENTRY(entry_free)), reference, true)) {
    completion_finish(entry_free, cpGoto);
    newreference = true;
  } else {
    // Code copied from GotoReferenceDialog::show_bad_reference()
    ustring message = "No such reference: ";
    message.append(gtk_entry_get_text(GTK_ENTRY(entry_free)));
    gtkw_dialog_error(parentToolbar, message);
  }

  extern Settings *settings;

  if (newreference) {
    settings->genconfig.book_set(reference.book_get());
    settings->genconfig.chapter_set(convert_to_string(reference.chapter_get()));
    settings->genconfig.verse_set(reference.verse_get());
    display(reference);
  }
}
