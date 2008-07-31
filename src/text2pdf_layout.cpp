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

#include "text2pdf_layout.h"
#include "text2pdf_area.h"
#include "text2pdf_utils.h"
#include "text2pdf_ref_area.h"
#include "text2pdf_utils.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "text2pdf_block.h"
#include "color.h"

T2PLayoutContainer::T2PLayoutContainer(PangoRectangle rectangle_in, T2PArea * parent_in, cairo_t *cairo) :
  T2PArea(rectangle_in)
// This is a container for one PangoLayout.
{
  parent = parent_in;
  layout = pango_cairo_create_layout(cairo);
  pango_layout_set_width(layout, rectangle.width);
  first_pass_layout = pango_cairo_create_layout(cairo);
  pango_layout_set_width(first_pass_layout, rectangle.width);
}

T2PLayoutContainer::~T2PLayoutContainer()
// Destructor.
{
  g_object_unref(layout);
  g_object_unref(first_pass_layout);
}

void T2PLayoutContainer::layout_text(T2PInputParagraph * paragraph, unsigned int line_number, ustring& text)
// This lays text out in the object, one line.
// The text to be laid out comes in "text".
// The part of the text that didn't fit is returned through "text". 
{
  // Text direction.
  pango_layout_set_auto_dir(layout, false);
  PangoContext * context = pango_layout_get_context(layout);
  if (paragraph) {
    if (paragraph->right_to_left) {
      pango_context_set_base_dir(context, PANGO_DIRECTION_RTL);
    } else {
      pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);
    }
  }

  // Attributes.
  PangoAttrList *attrs = pango_attr_list_new();

  if (paragraph) {

    // Font.
    set_font(paragraph, attrs);

    indentation_width_margins_alignment(paragraph, line_number == 0);
    set_italic(paragraph, attrs);
    set_bold(paragraph, attrs);
    set_underline(paragraph, attrs);
    set_small_caps(paragraph, attrs);

    set_superscript_font_scale_weight(paragraph, attrs);

    set_colour(paragraph, attrs);

    set_strike_through(paragraph, attrs);
  }

  pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
  pango_layout_set_text(layout, text.c_str(), -1);
  pango_layout_set_attributes(layout, attrs);

  // Get whether this is the last line.
  bool last_line_of_paragraph_loaded = pango_layout_get_line_count(layout) == 1;

  // Get the original length of the text.
  size_t original_length_of_text = text.length();

  // Get the first line in the layout, and ensure that there's only one in the layout.
  // Store any remnant of the input text.
  // Very often the last character of the first line in the layout is a space because of the wrapping.
  // This space is removed to give a better visual justification.
  PangoLayoutLine * layoutline = pango_layout_get_line(layout, 0);
  ustring line;
  if (layoutline) {
    line = text.substr(layoutline->start_index, layoutline->length);
    text.erase(0, line.length());
    line = trim(line);
    if (pango_layout_get_line_count(layout) != 1) {
      pango_layout_set_text(layout, line.c_str(), -1);
    }
  } else {
    text.clear();
  }

  // Justification.
  if (paragraph && paragraph->alignment == t2patJustified && !paragraph->no_justification) {
    justify(paragraph, line, last_line_of_paragraph_loaded, attrs);
  }

  // Store the height of the layout before superscript is applied.
  pango_layout_get_size(layout, NULL, &(rectangle.height));

  /*
   Set superscript.
   If superscript is there, then the height of the PangoLayout increases.
   This increased height gives the visual effect of a bigger line distance.
   However, this is undesireable.
   Therefore there is a mechanism that corrects for the increased height.
   The height of the PangoLayout is measured without superscript applied.
   Then the superscript, if any, is applied.
   The new height is measured, which will be bigger if there was any superscript.
   To correct for that, an offset for the base line is set.
   At printing time this will move up the base line of the text.
   */
  if (paragraph && set_superscript_ascent(paragraph, attrs, original_length_of_text)) {
    pango_layout_set_attributes(layout, attrs);
    int height_with_superscript;
    pango_layout_get_size(layout, NULL, &height_with_superscript);
    if (height_with_superscript != rectangle.height) {
      baseline_offset_pango_units = rectangle.height - height_with_superscript;
    }
  }

  // Free attributes.
  pango_attr_list_unref(attrs);

  // Line spacing.
  if (paragraph && (paragraph->line_spacing != 100)) {
    rectangle.height *= int ((double)(paragraph->line_spacing) / 100);
  }

  // Have the parent fit the container in.
  if (parent)
    ((T2PBlock *)parent)->refit_layout_container(this);

  // Have the parent store properties about the paragraph.
  if (parent)
    ((T2PBlock *)parent)->set_widow_orphan_data(line_number, last_line_of_paragraph_loaded);

  // Store any notes that belong to the bit of text that was laid out.
  if (paragraph)
    note_paragraphs = paragraph->get_notes(original_length_of_text, line.length());

  // Set the width of the container.
  pango_layout_get_size(layout, &rectangle.width, NULL);
}

void T2PLayoutContainer::print(cairo_t *cairo)
// Print the content.
{
  // Write text in black.
  cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
  // Move into position.
  cairo_move_to(cairo, pango_units_to_points(rectangle.x), pango_units_to_points(rectangle.y + baseline_offset_pango_units));
  // Show text.
  if (layout) {
    pango_cairo_show_layout(cairo, layout);
  }
}

void T2PLayoutContainer::index_white_space(const ustring& text, vector<guint>& offsets)
// Gets the offsets of all white space in the text.
{
  offsets.clear();
  for (guint i = 0; i < text.length(); i++) {
    gunichar character = g_utf8_get_char(text.substr(i, 1).c_str());
    if (g_unichar_isspace(character)) {
      offsets.push_back(i);
    }
  }
}

void T2PLayoutContainer::indentation_width_margins_alignment(T2PInputParagraph * paragraph, bool first_line)
/*
 Handles the indentation, the width, the margins, and the alignment of the paragraph.
 paragraph: Contains information about indentation, margins and alignment.
 first_line: whether the layout contains the first line of the paragraph.
 
 Things to test are positive and negative indentation, with any combination of the alignment.
 Also left and right margins.
 */
{
  // First line indentation is only applied to the first line,
  // and if there's any indentation at all. This of course is obvious...
  if (first_line && paragraph->first_line_indent_mm != 0) {
    int first_line_indent_pango_units = millimeters_to_pango_units(paragraph->first_line_indent_mm);
    rectangle.width -= first_line_indent_pango_units;
    rectangle.x += first_line_indent_pango_units;
  }

  // Left margin.
  if (paragraph->left_margin_mm != 0) {
    int left_margin_pango_units = millimeters_to_pango_units(paragraph->left_margin_mm);
    rectangle.width -= left_margin_pango_units;
    rectangle.x += left_margin_pango_units;
  }

  // Right margin.
  if (paragraph->right_margin_mm != 0) {
    int right_margin_pango_units = millimeters_to_pango_units(paragraph->right_margin_mm);
    rectangle.width -= right_margin_pango_units;
  }

  // Set the width in the Pango layout.
  pango_layout_set_width(layout, rectangle.width);

  // Paragraph alignment.
  switch (paragraph->alignment)
  {
    case t2patLeft:
      pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
      break;
    case t2patCenter:
      pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
      break;
    case t2patRight:
      pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
      break;
    case t2patJustified:
      pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
      // This should be "right" when the text flows from right to left.
      break;
  }
}

void T2PLayoutContainer::justify(T2PInputParagraph * paragraph, const ustring& line, bool last_line, PangoAttrList *attrs)
/* 
 Justifies the text in the layout.
 paragraph: paragraph properties.
 line: the line of text loaded.
 last_line: whether this is the last line of the paragraph.
 attrs: Pango attribute list.
 
 Points to test:
 - positive indents and negative indents when justified.
 - that the justfied edge is not rough but smooth.
 - if there are no spaces in the line, that then the letters are spaced instead of the spaces.
 */
{
  // If the last line of the paragraph is loaded, no justification should be done.
  if (last_line)
    return;

  // If there's no text, there's nothing to be justified.
  if (line.empty())
    return;

  // Justification is done by adjusting the letter spacing of all white space 
  // so that the total text becomes as wide as is desired to give the effect of justification.
  int desired_width = rectangle.width;
  int actual_width;
  pango_layout_get_size(layout, &actual_width, NULL);
  vector <guint> start_indexes;
  index_white_space(line, start_indexes);
  if (start_indexes.empty()) {
    // No white space found: Set the letter spacing of everything.
    int letter_spacing = (desired_width - actual_width) / line.length();
    PangoAttribute *attr;
    attr = pango_attr_letter_spacing_new(letter_spacing);
    attr->start_index = 0;
    attr->end_index = -1;
    pango_attr_list_insert(attrs, attr);
  } else {
    // White space found: Set the letter spacing of all white space.
    int white_space_spacing = (desired_width - actual_width) / start_indexes.size();
    for (unsigned idx = 0; idx < start_indexes.size(); idx++) {
      PangoAttribute *attr;
      attr = pango_attr_letter_spacing_new(white_space_spacing);
      attr->start_index = start_indexes[idx];
      attr->end_index = start_indexes[idx] + 1;
      pango_attr_list_insert(attrs, attr);
    }
  }
  pango_layout_set_attributes(layout, attrs);
}

ustring T2PLayoutContainer::text()
// Shows the text.
{
  ustring s;
  if (layout) {
    s = pango_layout_get_text(layout);
  }
  return s;
}

void T2PLayoutContainer::set_font(T2PInputParagraph * paragraph, PangoAttrList *attrs)
// Sets the font name and size.
{
  // Set the name and the size of the font for the paragraph.
  PangoFontDescription *desc;
  desc = pango_font_description_from_string(paragraph->font_name.c_str());
  pango_font_description_set_size(desc, paragraph->font_size_points * PANGO_SCALE);
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  // Set attributes for the inline font size.
  bool in_range;
  int index = 0;
  int start_index, end_index;
  int font_size_pango_units;
  do {
    if (paragraph->inline_get_font_size(index, in_range, font_size_pango_units, start_index, end_index)) {
      PangoAttribute *attr;
      attr = pango_attr_size_new(font_size_pango_units);
      attr->start_index = start_index;
      attr->end_index = end_index;
      pango_attr_list_insert(attrs, attr);
    }
    index++;
  } while (in_range);
}

void T2PLayoutContainer::set_italic(T2PInputParagraph * paragraph, PangoAttrList *attrs)
// Sets the italics, if any.
{
  bool in_range;
  int index = 0;
  int start_index, end_index;
  bool italic;
  do {
    if (paragraph->inline_get_italic(index, in_range, italic, start_index, end_index)) {
      if (italic) {
        PangoAttribute *attr;
        attr = pango_attr_style_new(PANGO_STYLE_ITALIC);
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
    }
    index++;
  } while (in_range);
}

void T2PLayoutContainer::set_bold(T2PInputParagraph * paragraph, PangoAttrList *attrs)
// Sets bold text.
{
  bool in_range;
  int index = 0;
  int start_index, end_index;
  bool bold;
  do {
    if (paragraph->inline_get_bold(index, in_range, bold, start_index, end_index)) {
      if (bold) {
        PangoAttribute *attr;
        attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
    }
    index++;
  } while (in_range);
}

void T2PLayoutContainer::set_underline(T2PInputParagraph * paragraph, PangoAttrList *attrs)
// Sets the underline attribute.
{
  bool in_range;
  int index = 0;
  int start_index, end_index;
  bool underline;
  do {
    if (paragraph->inline_get_underline(index, in_range, underline, start_index, end_index)) {
      if (underline) {
        PangoAttribute *attr;
        attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
    }
    index++;
  } while (in_range);
}

void T2PLayoutContainer::set_small_caps(T2PInputParagraph * paragraph, PangoAttrList *attrs)
/*
 Sets the small caps. But this is not yet implemented in Pango.

 Small capitals (usually abbreviated small caps) are uppercase (capital) characters set at the same height as surrounding 
 lowercase (small) letters or text figures. 

 Typically, the height of a small capital will be one ex, the same height as most lowercase characters in the font; 
 classically, small caps were very slightly taller than x-height. Well-designed small capitals are not simply scaled-down 
 versions of normal capitals; they normally retain the same stroke weight as other letters, and a wider aspect ratio 
 to facilitate readability.

 Many word processors and text formatting systems include an option to format text in caps and small caps; 
 this leaves uppercase letters as they are but converts lowercase letters to small caps. 
 How this is implemented depends on the typesetting system; some can use true small caps associated with a font, 
 making text look proportional, but most modern digital fonts do not have a small-caps case, so the typesetting 
 system simply reduces the uppercase letters by a fraction, making them look out of proportion. 
 (Often, in text, the next bolder version of the small caps generated by such systems will match well with the normal 
 weights of capitals and lower case, especially when such small caps are extended about 5% or letterspaced a half point 
 or a point.)

 In typography, the x-height or corpus size refers to the distance between the baseline and the mean line in a typeface. 
 Typically, this is the height of the letter x in the font (which is where the terminology came from), 
 as well as the u, v, w, and z. (The a, c, e, m, n, o, r and s all tend to exceed the x-height slightly, 
 due to overshoot.) However, in modern typography, the x-height is simply a design characteristic of the font, 
 and while an x is usually exactly one x-height in height, in some more decorative or script designs, 
 this may not always be the case.
 */
{
  bool in_range;
  int index = 0;
  int start_index, end_index;
  bool small_caps;
  do {
    if (paragraph->inline_get_small_caps(index, in_range, small_caps, start_index, end_index)) {
      if (small_caps) {
        PangoAttribute *attr;
        attr = pango_attr_variant_new(PANGO_VARIANT_SMALL_CAPS);
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
    }
    index++;
  } while (in_range);
}

void T2PLayoutContainer::set_superscript_font_scale_weight(T2PInputParagraph * paragraph, PangoAttrList *attrs)
// Sets the superscript font scale and weight.
{
  bool in_range;
  int index = 0;
  int start_index, end_index;
  bool superscript;
  do {
    if (paragraph->inline_get_superscript(index, in_range, superscript, start_index, end_index, paragraph->text.length())) {
      {
        PangoAttribute *attr;
        // In professional typesetting software, the size of the superscript character is 58%
        attr = pango_attr_scale_new(0.58);
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
      {
        // To keep the superscript character visually similar to the rest of the font, 
        // the weight should be slightly heavier than the reduced-size character would be.
        PangoAttribute *attr;
        attr = pango_attr_weight_new((PangoWeight)((PANGO_WEIGHT_NORMAL + PANGO_WEIGHT_SEMIBOLD)/2));
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
    }
    index++;
  } while (in_range);

}

bool T2PLayoutContainer::set_superscript_ascent(T2PInputParagraph * paragraph, PangoAttrList *attrs, size_t text_length)
// Sets the superscript markup.
// Returns whether any superscript is present in the paragraph.
{
  bool superscript_present = false;
  bool in_range;
  int index = 0;
  int start_index, end_index;
  bool superscript;
  do {
    if (paragraph->inline_get_superscript(index, in_range, superscript, start_index, end_index, text_length)) {
      {
        // In professional typesetting software, the superscript character is raised by 33%.
        PangoAttribute *attr;
        attr = pango_attr_rise_new(int(paragraph->font_size_points * PANGO_SCALE * 0.33));
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
      superscript_present = true;
    }
    index++;
  } while (in_range);
  return superscript_present;
}

void T2PLayoutContainer::set_colour(T2PInputParagraph * paragraph, PangoAttrList *attrs)
// Sets the colour.
{
  // Set attributes for the inline font size.
  bool in_range;
  int index = 0;
  int start_index, end_index;
  int colour;
  do {
    if (paragraph->inline_get_colour(index, in_range, colour, start_index, end_index)) {
      ustring colour_string = "#" + color_decimal_to_hex(colour);
      PangoColor pangocolor;
      pango_color_parse(&pangocolor, colour_string.c_str());
      PangoAttribute *attr;
      attr = pango_attr_foreground_new(pangocolor.red, pangocolor.green, pangocolor.blue);
      attr->start_index = start_index;
      attr->end_index = end_index;
      pango_attr_list_insert(attrs, attr);
    }
    index++;
  } while (in_range);
}

void T2PLayoutContainer::set_strike_through(T2PInputParagraph * paragraph, PangoAttrList *attrs)
// Sets the strike-through markup, if any.
{
  bool in_range;
  int index = 0;
  int start_index, end_index;
  bool strike_through;
  do {
    if (paragraph->inline_get_strike_through(index, in_range, strike_through, start_index, end_index)) {
      if (strike_through) {
        PangoAttribute *attr;
        attr = pango_attr_strikethrough_new(true);
        attr->start_index = start_index;
        attr->end_index = end_index;
        pango_attr_list_insert(attrs, attr);
      }
    }
    index++;
  } while (in_range);
}

