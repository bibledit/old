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


#include "opendocument.h"
#include "usfm.h"
#include "bible.h"
#include "usfmtools.h"
#include "gwrappers.h"
#include "shell.h"
#include "utilities.h"
#include "directories.h"
#include "date_time_utils.h"
#include "projectutils.h"
#include "books.h"
#include "xmlutils.h"
#include "usfmtools.h"
#include "unixwrappers.h"
#include "resource_utils.h"
#include "color.h"
#include "textreplacement.h"
#include "tiny_utilities.h"
#include "stylesheetutils.h"
#include <glib/gi18n.h>

/*
 Some properties of the stylesheet are not implemented for certain reasons.

 The "spancolumns" property is not implemented, because it is not so easy in
 OpenOffice Writer to copy text divided into various sections. Switching from
 one to two columns would divide the text into these sections.

 The chapter number is not put at verse one, but just where it occurs. This is
 because frames would be needed, and this makes copying text more difficult.
 */


OpenDocument::OpenDocument(const ustring & project, const ustring & filename, set < unsigned int >*selection)
// Export a project to OpenDocument format. Allows books selection.
{
  projectconfig = new ProjectConfiguration(project, false);
  usfm = new Usfm(stylesheet_get_actual ());

  // Check on basic markers.
  if (!usfm_basic_markers_present(*usfm, true))
    return;

  // Progress information.  
  progresswindow = new ProgressWindow(_("Exporting to OpenDocument"), true);

  // Prepare for inline text markers, notes and xrefs.
  usfm_inline_markers = new UsfmInlineMarkers(*usfm);
  odtfootnote = new OdtFootnote(*usfm);
  odtendnote = new OdtEndnote(*usfm);
  odtxref = new OdtXref(*usfm);

  // Retrieve and process template: page layout, font.
  workingdirectory = gw_build_filename(Directories->get_temp(), "opendocument");
  unpack_template();
  odt_set_font(workingdirectory, fontname);

  // Get the available books. If no selection given, take all books available.
  if (selection) {
    vector < unsigned int >bks = project_get_books(project);
    for (unsigned int i = 0; i < bks.size(); i++) {
      if (selection->find(bks[i]) != selection->end())
        books.push_back(bks[i]);
    }
  } else {
    books = project_get_books(project);
  }

  // Progress
  if (progresswindow)
    progresswindow->set_iterate(0, 1, books.size());

  // Generate the cover.
  cover();

  // Prepare for placing anchors for the Bibledit Resource Viewer.
  anchor_book = 0;
  anchor_chapter = 0;

  // Go through all the books.
  for (unsigned int i = 0; i < books.size(); i++) {

    // Progress.
    progresswindow->iterate();
    if (progresswindow->cancel) {      
      return;
    }

    // Skip "Other Material".
    if (books_id_to_type(books[i]) == btOtherMaterial)
      continue;

    // Signal new book to footnotes object.
    odtfootnote->new_book();

    // Set book for Resource Viewer.
    anchor_book = books[i];

    // Open the book. Do any replacements.
    vector <ustring> book_lines;
    book_lines = project_retrieve_book(project, books[i]);
    text_replacement(book_lines);

    // Format the lines of the book.
    format_general(book_lines);
  }

  // Store content.xml.
  odt_insert_content(workingdirectory, odtlines);

  // Produce the styles.xml
  generate_styles_xml(projectconfig->right_to_left_get());

  // Automatic .odt extension if needed.
  ustring filename_odt(filename);
  if (!g_str_has_suffix(filename.c_str(), ".odt"))
    filename_odt.append(".odt");

  // Compress the file.
  zip(filename_odt);
}


OpenDocument::~OpenDocument()
{
  delete progresswindow;
  delete projectconfig;
  delete usfm;
  delete usfm_inline_markers;
  delete odtfootnote;
  delete odtendnote;
  delete odtxref;
}


void OpenDocument::unpack_template()
{
  // Clear working directory.
  unix_rmdir(workingdirectory);
  gw_mkdir_with_parents(workingdirectory);
  // Copy template there.
  // Note: To create the template use zip -r template.odt *
#ifdef WIN32
  ustring command = "unzip -o ";
  command.append(gw_build_filename(Directories->get_package_data(), "template.odt"));
  command.append(" -d ");
  command.append(shell_quote_space(workingdirectory));
#else
  ustring command = "cd";
  command.append(shell_quote_space(workingdirectory));
  command.append("; cp ");
  command.append(gw_build_filename(Directories->get_package_data(), "template.odt"));
  command.append(" .; unzip *; rm *.odt");
#endif
  if (system(command.c_str())) ; // This one does not work with GwSpawn because of the wildcards used.
}


void OpenDocument::cover()
// Generate cover page.
{
  // Store the lines containing the cover.  
  vector < ustring > lines;

  // Collect the lines: Look for the \cov section in the Other Material book. 
  for (unsigned int i = 0; i < books.size(); i++) {
    if (books_id_to_type(books[i]) == btOtherMaterial)
      continue;
    vector < ustring > rawlines = project_retrieve_chapter(myproject, books[i], 0);
    bool within_cover_section = false;
    for (unsigned int i2 = 0; i2 < rawlines.size(); i2++) {
      ustring line = rawlines[i2];
      ustring marker = usfm_extract_marker(line);
      if (usfm->is_peripheral(marker))
        if (usfm->peripheral_get_subtype(marker) != ptCover)
          within_cover_section = false;
      if (within_cover_section) {
        lines.push_back(rawlines[i2]);
      }
      if (usfm->is_peripheral(marker)) {
        if (usfm->peripheral_get_subtype(marker) == ptCover)
          within_cover_section = true;
      }
    }
  }

  // Format cover.
  format_general(lines);
}


void OpenDocument::format_general(vector <ustring>& lines)
// General formatter for USFM lines given.
{
  // Go through all the lines.
  odttextparagraph = NULL;
  for (unsigned int ln = 0; ln < lines.size(); ln++) {
    ustring line = lines[ln];
    // Take any elastics out, put the \b marker instead.
    replace_text(line, ELASTIC_MARKER, "b");
    // Change certain characters to xml entities.
    xml_handle_entities(line, NULL);
    // Deal with footnotes.
    odtfootnote->transform(line);
    // Deal with endnotes.
    odtendnote->transform(line);
    // Deal with crossreferences.
    odtxref->transform(line);
    // Deal with inline text.
    usfm_handle_inline_text(line, usfm_inline_markers, NULL, imOpenDocument, NULL);
    // Signal new line.
    if (odttextparagraph)
      odttextparagraph->newline();
    // Get the style belonging to the marker.
    ustring marker = usfm_extract_marker(line);
    if (usfm->is_identifier(marker)) {
      // Handle some identifiers.
      IdentifierType identifiertype = usfm->identifier_get_subtype(marker);
      // New book.
      if (identifiertype == itBook) {
        if (odttextparagraph)
          delete odttextparagraph;
        odttextparagraph = new OdtTextParagraph(&odtlines, marker);
      }
    } else if (usfm->is_verse_number(marker)) {
      // Because of dealing with portions to include/exclude, handle verse first.
      // Get verse number. Handle combined verses too, e.g. 10-12b, etc.
      size_t position = line.find(" ");
      position = CLAMP(position, 0, line.length());
      ustring versenumber = line.substr(0, position);
      position++;
      line.erase(0, position);
      // There was a bug that the chapter number was missing from a book,
      // and that results in text being inserted without the opening xml code.
      // Solution: If no paragraph has been opened, open a default one.
      if (odttextparagraph == NULL)
        odttextparagraph = new OdtTextParagraph(&odtlines, "");
      // Insert a bookmark at the verse.
      // This will become an anchor for the Bibledit Resource Viewer,
      // once OpenOffice saves the document to a HTML Document.
      // Example bookmark: <text:bookmark text:name="Malachi_1_1"/>
      ustring bookmark = resource_viewer_produce_anchor(anchor_book, anchor_chapter, convert_to_int(number_in_string(versenumber)));
      bookmark.insert(0, "<text:bookmark text:name=\"");
      bookmark.append("\"/>");
      odttextparagraph->plaintext(bookmark);
      // When the usfm is a verse number, then the number is put in the 
      // format specified by the stylesheet, but the remaining part of the
      // line inherits the formatting from the paragraph it is in.
      odttextparagraph->spannedtext(versenumber, marker);
      odttextparagraph->plaintext(line);
    } else if (usfm->is_starting_paragraph(marker)) {
      if (odttextparagraph)
        delete odttextparagraph;
      odttextparagraph = new OdtTextParagraph(&odtlines, marker);
      if (!line.empty())
        odttextparagraph->plaintext(line);
    } else if (usfm->is_inline_text(marker)) {
      // Inline text, has been dealt with before (therefore should never occur here).
      if (odttextparagraph)
        odttextparagraph->spannedtext(line, marker);
    } else if (usfm->is_chapter_number(marker)) {
      // Close possible open paragraph.
      if (odttextparagraph)
        delete odttextparagraph;
      // Signal new chapter to footnotes object.
      odtfootnote->new_chapter();
      // Store chapter for the anchors.
      anchor_chapter = convert_to_int(number_in_string(line));
      // Insert or prepare chapter text.
      odttextparagraph = new OdtTextParagraph(&odtlines, marker);
      odttextparagraph->plaintext(line);
    } else if (usfm->is_peripheral(marker)) {
    } else if (usfm->is_picture(marker)) {
    } else if (usfm->is_pagebreak(marker)) {
      if (odttextparagraph)
        delete odttextparagraph;
      odttextparagraph = new OdtTextParagraph(&odtlines, marker);
    } else {
      // Fallback for unknown marker or no marker.
      if (!odttextparagraph)
        odttextparagraph = new OdtTextParagraph(&odtlines, "");
      odttextparagraph->plaintext(line);
      // Make a note of this marker, that it was not formatted.
      unformatted_markers.insert (marker);
    }
  }
  // Close possible last paragraph.
  if (odttextparagraph)
    delete odttextparagraph;
}


void OpenDocument::generate_styles(xmlTextWriterPtr writer)
{
  // Go through all the styles and generate the appropriate code.
  for (unsigned int i = 0; i < usfm->styles.size(); i++) {
    ustring marker = usfm->styles[i].marker;
    if (usfm->is_identifier(marker)) {
      switch (usfm->identifier_get_subtype(marker)) {
      case itBook:
        paragraph_style(writer, marker, usfm->name(), fontname, 12, 100, OFF, OFF, OFF, OFF, "", 0, 0, 0, 0, 0, false, usfm->userbool1());
        break;
      case itEncoding:
        break;
      case itComment:
        break;
      case itRunningHeader:
        break;
      case itLongTOC:
        break;
      case itShortTOC:
        break;
      case itBookAbbrev:
        break;
      case itChapterLabel:
        break;
      case itPublishedChapterMarker:
        break;
      case itCommentWithEndmarker:
        break;
      }
    }
    if (usfm->is_starting_paragraph(marker)) {
      paragraph_style(writer, marker, usfm->name(), fontname, usfm->fontsize(), 100, usfm->italic(), usfm->bold(), usfm->underline(), usfm->smallcaps(), usfm->justification(), usfm->spacebefore(), usfm->spaceafter(), usfm->leftmargin(), usfm->rightmargin(), usfm->firstlineindent(), false, false);
    }
    if (usfm->is_inline_text(marker)) {
      span_style(writer, marker, usfm->name(), fontname, 100, usfm->italic(), usfm->bold(), usfm->underline(), usfm->smallcaps(), usfm->superscript(), usfm->color());
    }
    if (usfm->is_chapter_number(marker)) {
      paragraph_style(writer, marker, usfm->name(), fontname, usfm->fontsize(), 100, usfm->italic(), usfm->bold(), usfm->underline(), usfm->smallcaps(), usfm->justification(), usfm->spacebefore(), usfm->spaceafter(), usfm->leftmargin(), usfm->rightmargin(), usfm->firstlineindent(), false, false);
    }
    if (usfm->is_verse_number(marker)) {
      span_style(writer, marker, usfm->name(), fontname, 100, usfm->italic(), usfm->bold(), usfm->underline(), usfm->smallcaps(), usfm->superscript(), usfm->color());
    }
    if (usfm->is_foot_endnote(marker)) {
      switch (usfm->foot_endnote_get_subtype(marker)) {
      case fentFootnote:
        break;
      case fentEndnote:
        break;
      case fentStandardContent:
      case fentContent:
      case fentContentWithEndmarker:
        span_style(writer, marker, usfm->name(), fontname, 100, usfm->italic(), usfm->bold(), usfm->underline(), OFF, usfm->superscript(), usfm->color());
        break;
      case fentParagraph:
        break;
      }
    }
    if (usfm->is_xref(marker)) {
      switch (usfm->xref_get_subtype(marker)) {
      case ctCrossreference:
        break;
      case ctStandardContent:
      case ctContent:
      case ctContentWithEndmarker:
        span_style(writer, marker, usfm->name(), fontname, 100, usfm->italic(), usfm->bold(), usfm->underline(), OFF, usfm->superscript(), usfm->color());
        break;
      }
    }
    if (usfm->is_peripheral(marker)) {
      switch (usfm->peripheral_get_subtype(marker)) {
      case ptPublication:
        break;
      case ptTableOfContents:
        break;
      case ptPreface:
        break;
      case ptIntroduction:
        break;
      case ptGlossary:
        break;
      case ptConcordance:
        break;
      case ptIndex:
        break;
      case ptMapIndex:
        break;
      case ptCover:
        break;
      case ptSpine:
        break;
      }
    }
    if (usfm->is_picture(marker)) {
    }
    if (usfm->is_pagebreak(marker)) {
      paragraph_style(writer, marker, usfm->name(), fontname, 12, 100, OFF, OFF, OFF, OFF, "", 0, 0, 0, 0, 0, false, true);
    }
  }
}


void OpenDocument::zip(const ustring filename)
{
  ustring command = "cd ";
  command.append(shell_quote_space(workingdirectory));
#ifdef WIN32
  command.append(" && ");
  gchar *path;
  path = g_find_program_in_path("zip.exe");
  if (path) {
    command.append(path);
    g_free(path);
    command.append(" -r ");
  }
#else
  command.append("; zip -r");
#endif
  command.append(shell_quote_space(filename));
  command.append(" *");
  if (system(command.c_str())) ; // This one does not work with GwSpawn because of the wildcards used.
}


void OpenDocument::generate_styles_xml(bool right_to_left)
// This generates the file "styles.xml" in the OpenDocument.
{
  // Start the new xml document.
  xmlBufferPtr buffer = xmlBufferCreate();
  xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent(writer, 1);
  xmlTextWriterStartElement(writer, BAD_CAST "office:document-styles");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:office", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:style", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:text", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:table", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:draw", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:fo", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xlink", BAD_CAST "http://www.w3.org/1999/xlink");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:dc", BAD_CAST "http://purl.org/dc/elements/1.1/");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:meta", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:number", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:svg", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:chart", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:dr3d", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:math", BAD_CAST "http://www.w3.org/1998/Math/MathML");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:form", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:script", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:script:1.0");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:ooo", BAD_CAST "http://openoffice.org/2004/office");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:ooow", BAD_CAST "http://openoffice.org/2004/writer");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:oooc", BAD_CAST "http://openoffice.org/2004/calc");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:dom", BAD_CAST "http://www.w3.org/2001/xml-events");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "office:version", BAD_CAST "1.0");

  // Font face declarations.
  xmlTextWriterStartElement(writer, BAD_CAST "office:font-face-decls");
  xmlTextWriterEndElement(writer);

  // Styles.
  xmlTextWriterStartElement(writer, BAD_CAST "office:styles");
  generate_styles(writer);
  xmlTextWriterEndElement(writer);

  // Automatic styles.
  xmlTextWriterStartElement(writer, BAD_CAST "office:automatic-styles");
  xmlTextWriterStartElement(writer, BAD_CAST "style:page-layout");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "style:name", BAD_CAST "pm1");
  xmlTextWriterStartElement(writer, BAD_CAST "style:page-layout-properties");
  extern Settings *settings;
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:page-width", "%.2fcm", settings->genconfig.paper_width_get());
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:page-height", "%.2fcm", settings->genconfig.paper_height_get());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "style:print-orientation", BAD_CAST "portrait");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "style:num-format", BAD_CAST "1");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-top", "%.2fcm", settings->genconfig.paper_top_margin_get());
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-bottom", "%.2fcm", settings->genconfig.paper_bottom_margin_get());
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-left", "%.2fcm", settings->genconfig.paper_inside_margin_get());
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-right", "%.2fcm", settings->genconfig.paper_outside_margin_get());
  if (right_to_left) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:writing-mode", BAD_CAST "rl-tb");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "writing-mode", BAD_CAST "rl-tb");
  }
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // Master styles.
  xmlTextWriterStartElement(writer, BAD_CAST "office:master-styles");
  xmlTextWriterStartElement(writer, BAD_CAST "style:master-page");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "style:name", BAD_CAST "Standard");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "style:page-layout-name", BAD_CAST "pm1");
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // Close document.
  xmlTextWriterEndElement(writer);

  // Close document and write it to disk.
  xmlTextWriterEndDocument(writer);
  xmlTextWriterFlush(writer);
  ustring filename = gw_build_filename(workingdirectory, "styles.xml");
  g_file_set_contents(filename.c_str(), (const gchar *)buffer->content, -1, NULL);

  // Free memory.
  if (writer)
    xmlFreeTextWriter(writer);
  if (buffer)
    xmlBufferFree(buffer);
}


void OpenDocument::paragraph_style(xmlTextWriterPtr writer, const ustring & marker, const ustring & name, const ustring & fontname, double fontsize, int lineheight, const ustring & italic, const ustring & bold, const ustring & underline, const ustring & smallcaps, ustring justification, double spacebefore, double spaceafter, double leftmargin, double rightmargin, double firstlineindent, bool spancolumns, bool startpage)
{
  // Style properties.
  xmlTextWriterStartElement(writer, BAD_CAST "style:style");
  {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:name", BAD_CAST marker.c_str());
    ustring marker_name = marker + " " + name;
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:display-name", BAD_CAST marker_name.c_str());
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:parent-style-name", BAD_CAST "Standard");

    // Paragraph properties.  
    xmlTextWriterStartElement(writer, BAD_CAST "style:paragraph-properties");
    {
      if (lineheight != 100) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:line-height", "%d%%", lineheight);
      }
      if (!justification.empty()) {
        if (justification == LEFT) {
          justification = "start";
        } else if (justification == RIGHT) {
          justification = "end";
        }
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:text-align", BAD_CAST justification.c_str());
        xmlTextWriterWriteAttribute(writer, BAD_CAST "style:justify-single-word", BAD_CAST "false");
      }
      if (spacebefore != 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-top", "%.1fmm", spacebefore);
      }
      if (spaceafter != 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-bottom", "%.1fmm", spaceafter);
      }
      if (leftmargin != 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-left", "%.1fmm", leftmargin);
      }
      if (rightmargin != 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:margin-right", "%.1fmm", rightmargin);
      }
      if (firstlineindent != 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:text-indent", "%.1fmm", firstlineindent);
      }
      // Starting odd pages is not supported in OpenDocument 1.0.
      if (startpage) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:break-before", BAD_CAST "page");
      }
      // Spanning of columns is not implemented because that interferes with how
      // easily the text can be edited.
    }
    xmlTextWriterEndElement(writer);

    // Text properties.
    xmlTextWriterStartElement(writer, BAD_CAST "style:text-properties");
    {
      xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:font-size", "%.1fpt", fontsize);
      xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:font-size-asian", "%.1fpt", fontsize);
      xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:font-size-complex", "%.1fpt", fontsize);
      if (!fontname.empty()) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "style:font-name", "%s", fontname.c_str());
      }
      if (italic == ON) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-style", BAD_CAST "italic");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-style-asian", BAD_CAST "italic");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-style-complex", BAD_CAST "italic");
      }
      if (bold == ON) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-weight", BAD_CAST "bold");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-weight-asian", BAD_CAST "bold");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-weight-complex", BAD_CAST "bold");
      }
      if (underline == ON) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "style:text-underline-style", BAD_CAST "solid");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "style:text-underline-width", BAD_CAST "auto");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "style:text-underline-color", BAD_CAST "font-color");
      }
      if (smallcaps == ON) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-variant", BAD_CAST "small-caps");
      }
    }
    xmlTextWriterEndElement(writer);
  }

  // Close style  
  xmlTextWriterEndElement(writer);
}


void OpenDocument::span_style(xmlTextWriterPtr writer, const ustring & marker, const ustring & name, const ustring & fontname, double fontpercentage, ustring italic, ustring bold, ustring underline, ustring smallcaps, bool superscript, unsigned int color)
{
  // Open the style.
  xmlTextWriterStartElement(writer, BAD_CAST "style:style");
  {

    // Style properties.
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:name", BAD_CAST marker.c_str());
    ustring marker_name = marker + " " + name;
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:display-name", BAD_CAST marker_name.c_str());
    xmlTextWriterWriteAttribute(writer, BAD_CAST "style:family", BAD_CAST "text");

    // Text properties.
    xmlTextWriterStartElement(writer, BAD_CAST "style:text-properties");
    {
      if (!fontname.empty()) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "style:font-name", "%s", fontname.c_str());
      }
      if (fontpercentage != 100) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:font-size", "%.1f%%", fontpercentage);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:font-size-asian", "%.1f%%", fontpercentage);
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:font-size-complex", "%.1f%%", fontpercentage);
      }
      if ((italic == ON) || (italic == TOGGLE))
        italic = "italic";
      else
        italic = "normal";
      // Note that ON and TOGGlE both switch italic on 
      // and OFF and INHERIT switch italic off. 
      // Improvements could be made here.
      // Bold, underline, etc., work the same.
      xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-style", BAD_CAST italic.c_str());
      xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-style-asian", BAD_CAST italic.c_str());
      xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-style-complex", BAD_CAST italic.c_str());
      if ((bold == ON) || (bold == TOGGLE))
        bold = "bold";
      else
        bold = "normal";
      xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-weight", BAD_CAST bold.c_str());
      xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-weight-asian", BAD_CAST bold.c_str());
      xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-weight-complex", BAD_CAST bold.c_str());
      if ((underline == ON) || (underline == TOGGLE))
        underline = "solid";
      else
        underline = "none";
      xmlTextWriterWriteAttribute(writer, BAD_CAST "style:text-underline-style", BAD_CAST underline.c_str());
      xmlTextWriterWriteAttribute(writer, BAD_CAST "style:text-underline-width", BAD_CAST "auto");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "style:text-underline-color", BAD_CAST "font-color");
      if ((smallcaps == ON) || (smallcaps == TOGGLE))
        smallcaps = "small-caps";
      else
        smallcaps = "normal";
      xmlTextWriterWriteAttribute(writer, BAD_CAST "fo:font-variant", BAD_CAST smallcaps.c_str());
      if (superscript) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "style:text-position", BAD_CAST "super 58%");
      }
      if (color != 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "fo:color", "#%s", color_decimal_to_hex(color).c_str());
      }
    }
    xmlTextWriterEndElement(writer);
  }

  // Close style.
  xmlTextWriterEndElement(writer);
}


void OpenDocument::note_unformatted_markers (vector <ustring>& markers)
{
  // Add the markers from the object to the container.
  vector <ustring> temporal_markers (unformatted_markers.begin(), unformatted_markers.end());
  for (unsigned int i = 0; i < temporal_markers.size(); i++) {
    markers.push_back (temporal_markers[i]);
  }
  // Remove double ones.
  set <ustring> temporal_set (markers.begin(), markers.end());
  markers.clear();
  markers.assign (temporal_set.begin(), temporal_set.end());
}

