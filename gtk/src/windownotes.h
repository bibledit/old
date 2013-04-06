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


#ifndef INCLUDED_WINDOW_NOTES_H
#define INCLUDED_WINDOW_NOTES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "floatingwindow.h"
#include "reference.h"
#include "note_editor.h"
#include "displayprojectnotes.h"
#include <webkit/webkit.h>


class WindowNotes : public FloatingWindow
{
public:
  WindowNotes(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup);
  virtual ~WindowNotes();
  void go_to (Reference& reference);
  void new_note ();
  void redisplay (bool immediately = false);
  void display (vector <unsigned int>& ids);
  void get_references_from_note (vector<Reference>& references, vector<ustring>& messages);
  void cut ();
  void copy ();
  void paste ();
  void undo ();
  void redo ();
  void decrease_indent ();
  void increase_indent ();
  bool note_being_edited ();
  GtkWidget * references_available_signal_button;
  vector<Reference> available_references;
  void set_fonts ();
private:
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow_notes;
  GtkWidget *webview_notes;
  GtkWidget *hbox14;
  GtkWidget *vbox_note_editor;
  GtkWidget *toolbar_note_editor;
  GtkWidget *toolitem_note_edit_bold;
  GtkWidget *togglebutton_note_edit_bold;
  GtkWidget *image29121;
  GtkWidget *toolitem_note_edit_italics;
  GtkWidget *togglebutton_note_edit_italics;
  GtkWidget *image29122;
  GtkWidget *toolitem_note_edit_underline;
  GtkWidget *togglebutton_note_edit_underline;
  GtkWidget *image29114;
  GtkWidget *toolitem_note_edit_strike_through;
  GtkWidget *togglebutton_note_edit_strike_through;
  GtkWidget *image29123;
  GtkWidget *scrolledwindow_note_editor;
  GtkWidget *webview_note_editor;
  GtkWidget *scrolledwindow_controls;
  GtkWidget *vbox_controls;
  GtkWidget *hbox_ok_cancel;
  GtkWidget *button_note_ok;
  GtkWidget *button_note_cancel;
  GtkWidget *button_more;
  GtkWidget *alignment1;
  GtkWidget *hbox15;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *label_note_category;
  GtkWidget *combobox_note_category;
  GtkWidget *label_note_references;
  GtkWidget *scrolledwindow8;
  GtkWidget *textview_note_references;

  bool project_notes_editable;

  NoteEditor * note_editor;
  void notes_fill_edit_screen(int id, bool newnote);

  // Formatting
  static void on_togglebutton_note_edit_bold_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_bold_toggled ();
  static void on_togglebutton_note_edit_italics_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_italics_toggled ();
  static void on_togglebutton_note_edit_underline_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_underline_toggled ();
  static void on_togglebutton_note_edit_strike_through_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void togglebutton_note_edit_strike_through_toggled ();
  static void on_togglebutton_note_edit_left_justify_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static gboolean on_move_cursor (WebKitWebView *web_view, GtkMovementStep step, gint count, gpointer user_data);
  void move_cursor (WebKitWebView *web_view, GtkMovementStep step, gint count);
  
  guint redisplay_source_id;
  static bool on_redisplay_timeout (gpointer data);
  void redisplay_timeout ();
  void stop_displaying_more_notes ();
  DisplayProjectNotes * displayprojectnotes;

  guint gui_source_id;
  static bool on_gui_timeout (gpointer data);
  void on_gui ();

  static void on_button_cancel_clicked (GtkButton *button, gpointer user_data);
  void on_notes_button_cancel ();
  static void on_button_ok_clicked (GtkButton *button, gpointer user_data);
  void on_notes_button_ok ();
  void on_notes_button_ok_cancel ();

  void get_references_from_id (gint id);

  static gboolean on_navigation_policy_decision_requested (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, gpointer user_data);
  void navigation_policy_decision_requested (WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision);
  void html_link_clicked (const gchar * url);
  void delete_ids (const vector<gint>& ids);

  static void on_document_load_finished (WebKitWebView *web_view, WebKitWebFrame *web_frame, gpointer user_data);
  void document_load_finished ();

  static void on_button_more_clicked (GtkButton *button, gpointer user_data);
  void on_button_more ();
  vector <ustring> projects;
  ustring project;
  ustring created_on;
  ustring created_by;
  ustring edited_on;
  ustring logbook;
  
  unsigned int edited_note_id;
};


#endif
