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

#include <glib.h>
#include "notetemplate.h"
#include "xmlutils.h"
#include "utilities.h"
#include "tiny_utilities.h"

#define NUMBERING "numbering"
#define ANCHOR "anchor"
#define AUTOMATIC_REFERENCE "automatic-reference"
#define CHAPTER "chapter"
#define DIVISION "division"
#define VERSE "verse"
#define SUFFIX "suffix"
#define CONTENT "content"
#define MARKER "marker"
#define TEXT "text"
#define REMEMBER "remember"
#define SIZE "size"
#define TIDY "tidy"

NoteTemplate::NoteTemplate(const ustring & filename, NoteType notetype, bool write)
{
  // Save variables.
  myfilename = filename;
  mywrite = write;
  // Set default values.
  numbering = 0;
  anchor = "*";
  automatic_reference = false;
  reference_chapter = true;
  reference_division = ".";
  reference_verse = true;
  reference_suffix = ": ";
  // Read the style file.
  bool was_error = false;
  gchar *contents;
  gsize length;
  GError *error;
  error = NULL;
  if (!g_file_get_contents(filename.c_str(), &contents, &length, &error)) {
    cerr << error->message << endl;
    g_error_free(error);
    was_error = true;
  }
  // Short files are likely empty.
  if (length < 10) {
    was_error = true;
  }
  // Set up parser.
  GMarkupParseContext *context = NULL;
  GMarkupParser parser = {
    start_element_handler,
    end_element_handler,
    text_handler,
    passthrough_handler,
    error_handler
  };
  // Parse xml file.
  if (!was_error) {
    context = g_markup_parse_context_new(&parser, GMarkupParseFlags(0), gpointer(this), NULL);
    if (!g_markup_parse_context_parse(context, contents, length, NULL)) {
      g_markup_parse_context_free(context);
      was_error = true;
      cerr << error->message << endl;
    }
  }
  if (!was_error) {
    if (!g_markup_parse_context_end_parse(context, NULL)) {
      g_markup_parse_context_free(context);
      was_error = true;
      cerr << error->message << endl;
    }
  }
  if (!was_error)
    g_markup_parse_context_free(context);
  current_element.clear();
  current_value.clear();
}

NoteTemplate::~NoteTemplate()
{
  if (mywrite) {
    // If the object goes out of scope, it saves the settings.
    vector < ustring > output;
    output.push_back(xml_text_embed_in_tags(0, NUMBERING, convert_to_string(numbering)));
    output.push_back(xml_text_embed_in_tags(0, ANCHOR, anchor));
    output.push_back(xml_text_embed_in_tags(0, AUTOMATIC_REFERENCE, convert_to_string(automatic_reference)));
    output.push_back(xml_text_embed_in_tags(0, CHAPTER, convert_to_string(reference_chapter)));
    output.push_back(xml_text_embed_in_tags(0, DIVISION, reference_division));
    output.push_back(xml_text_embed_in_tags(0, VERSE, convert_to_string(reference_verse)));
    output.push_back(xml_text_embed_in_tags(0, SUFFIX, reference_suffix));
    for (unsigned int i = 0; i < content_markers.size(); i++) {
      output.push_back(xml_tag(0, CONTENT, 0));
      output.push_back(xml_text_embed_in_tags(1, MARKER, content_markers[i]));
      xml_handle_entities(content_texts[i], NULL);
      output.push_back(xml_text_embed_in_tags(1, TEXT, content_texts[i]));
      output.push_back(xml_text_embed_in_tags(1, REMEMBER, convert_to_string(content_remembers[i])));
      output.push_back(xml_text_embed_in_tags(1, SIZE, convert_to_string(content_sizes[i])));
      output.push_back(xml_text_embed_in_tags(1, TIDY, convert_to_string(content_tidies[i])));
      output.push_back(xml_tag(0, CONTENT, 1));
    }
    try {
      write_lines(myfilename, output);
    }
    catch(exception & ex) {
      cerr << ex.what() << endl;
    }
  }
}

void NoteTemplate::start_element_handler(GMarkupParseContext * context, const gchar * element_name, const gchar ** attribute_names, const gchar ** attribute_values, gpointer user_data, GError ** error)
{
  ustring element;
  element = element_name;
  ((NoteTemplate *) user_data)->start_element_handler(element);
}

void NoteTemplate::end_element_handler(GMarkupParseContext * context, const gchar * element_name, gpointer user_data, GError ** error)
{
  ustring element;
  element = element_name;
  ((NoteTemplate *) user_data)->end_element_handler(element);
}

void NoteTemplate::text_handler(GMarkupParseContext * context, const gchar * text, gsize text_len, gpointer user_data, GError ** error)
{
  ustring utext;
  utext = text;
  ((NoteTemplate *) user_data)->text_handler(utext);
}

void NoteTemplate::passthrough_handler(GMarkupParseContext * context, const gchar * passthrough_text, gsize text_len, gpointer user_data, GError ** error)
{
}

void NoteTemplate::error_handler(GMarkupParseContext * context, GError * error, gpointer user_data)
{
  cerr << error->message << endl;
}

void NoteTemplate::start_element_handler(const ustring & element_name)
/*
When we encounter a new element that starts data, this handler deals with that.
*/
{
  current_element = element_name;
  current_value.clear();
}

void NoteTemplate::end_element_handler(const ustring & element_name)
/*
When we encounter an element that ends data, this handler deals with that.
*/
{
  if (current_element == NUMBERING)
    numbering = convert_to_int(current_value);
  if (current_element == ANCHOR)
    anchor = current_value;
  if (current_element == AUTOMATIC_REFERENCE)
    automatic_reference = convert_to_bool(current_value);
  if (current_element == CHAPTER)
    reference_chapter = convert_to_bool(current_value);
  if (current_element == DIVISION)
    reference_division = current_value;
  if (current_element == VERSE)
    reference_verse = convert_to_bool(current_value);
  if (current_element == SUFFIX)
    reference_suffix = current_value;
  if (current_element == MARKER)
    content_marker = current_value;
  if (current_element == TEXT)
    content_text = current_value;
  if (current_element == REMEMBER)
    content_remember = current_value;
  if (current_element == TIDY)
    content_tidy = current_value;
  if (current_element == SIZE)
    content_size = current_value;
  if (element_name == CONTENT) {
    // Empty content markers are not loaded, because these cause unpredictable behaviour.
    if (!trim(content_marker).empty()) {
      content_markers.push_back(content_marker);
      content_texts.push_back(content_text);
      content_remembers.push_back(convert_to_bool(content_remember));
      content_tidies.push_back(convert_to_bool(content_tidy));
      content_sizes.push_back(convert_to_int(content_size));
    }
  }
  current_element.clear();
  current_value.clear();
}

void NoteTemplate::text_handler(const ustring & text)
/*
When we encounter the text inside an element, this handler deals with that.
*/
{
  current_value.append(text);
}
