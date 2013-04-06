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

#include "stylesheet.h"
#include "utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "tiny_utilities.h"
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include "stylesheetutils.h"


Stylesheet::Stylesheet(const ustring & name_in)
{
  // Save the sheet's name.
  name = name_in;

  // If the name is empty, read from the template.
  ustring filename = stylesheet_xml_filename(name);
  if (name.empty())
    filename = stylesheet_xml_template_filename();

  // Read the xml data, bail out on failure.
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  if (contents == NULL) {
    gw_critical("Failure reading stylesheet " + filename);
		return;
  }

  // Parse the stylesheet.  
  xmlParserInputBufferPtr inputbuffer;
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
	ustring value;
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
	if (reader) {
    StyleV2 * style = NULL;
		while ((xmlTextReaderRead(reader) == 1)) {
			switch (xmlTextReaderNodeType(reader)) {
			case XML_READER_TYPE_ELEMENT:
				{
					xmlChar *element_name = xmlTextReaderName(reader);
					if (!xmlStrcmp(element_name, BAD_CAST "style")) {
            char *attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "marker");
            if (attribute) {
              style = new StyleV2 (0);
              style->marker = attribute;
              free(attribute);
            }
					}
					break;
				}
			case XML_READER_TYPE_TEXT:
				{
  				xmlChar *text = xmlTextReaderValue(reader);
					if (text) {
					  value = (gchar *) text;
						xmlFree(text);
					}	
					break;
				}
			case XML_READER_TYPE_END_ELEMENT:
				{
 					xmlChar *element_name = xmlTextReaderName(reader);
          if (style) {
  					if (!xmlStrcmp(element_name, BAD_CAST "marker"))
	  					style->marker = value;
		  			if (!xmlStrcmp(element_name, BAD_CAST "name"))
			  			style->name = value;
				  	if (!xmlStrcmp(element_name, BAD_CAST "info"))
					  	style->info = value;
  					if (!xmlStrcmp(element_name, BAD_CAST "type"))
	  					style->type = (StyleType) convert_to_int(value);
		  			if (!xmlStrcmp(element_name, BAD_CAST "subtype"))
			  			style->subtype = convert_to_int(value);
				  	if (!xmlStrcmp(element_name, BAD_CAST "fontsize"))
					  	style->fontsize = convert_to_double(value);
  					if (!xmlStrcmp(element_name, BAD_CAST "italic"))
	  					style->italic = value;
		  			if (!xmlStrcmp(element_name, BAD_CAST "bold"))
			  			style->bold = value;
				  	if (!xmlStrcmp(element_name, BAD_CAST "underline"))
					  	style->underline = value;
  					if (!xmlStrcmp(element_name, BAD_CAST "smallcaps"))
	  					style->smallcaps = value;
		  			if (!xmlStrcmp(element_name, BAD_CAST "superscript"))
			  			style->superscript = convert_to_bool(value);
				  	if (!xmlStrcmp(element_name, BAD_CAST "justification"))
					  	style->justification = value;
  					if (!xmlStrcmp(element_name, BAD_CAST "spacebefore"))
	  					style->spacebefore = convert_to_double(value);
		  			if (!xmlStrcmp(element_name, BAD_CAST "spaceafter"))
			  			style->spaceafter = convert_to_double(value);
				  	if (!xmlStrcmp(element_name, BAD_CAST "leftmargin"))
					  	style->leftmargin = convert_to_double(value);
  					if (!xmlStrcmp(element_name, BAD_CAST "rightmargin"))
	  					style->rightmargin = convert_to_double(value);
		  			if (!xmlStrcmp(element_name, BAD_CAST "firstlineindent"))
			  			style->firstlineindent = convert_to_double(value);
				  	if (!xmlStrcmp(element_name, BAD_CAST "spancolumns"))
					  	style->spancolumns = convert_to_bool(value);
  					if (!xmlStrcmp(element_name, BAD_CAST "color"))
	  					style->color = convert_to_int(value);
		  			if (!xmlStrcmp(element_name, BAD_CAST "print"))
			  			style->print = convert_to_bool(value);
				  	if (!xmlStrcmp(element_name, BAD_CAST "userbool1"))
					  	style->userbool1 = convert_to_bool(value);
  					if (!xmlStrcmp(element_name, BAD_CAST "userbool2"))
	  					style->userbool2 = convert_to_bool(value);
		  			if (!xmlStrcmp(element_name, BAD_CAST "userbool3"))
			  			style->userbool3 = convert_to_bool(value);
				  	if (!xmlStrcmp(element_name, BAD_CAST "userint1"))
					  	style->userint1 = convert_to_int(value);
  					if (!xmlStrcmp(element_name, BAD_CAST "userint2"))
	  					style->userint2 = convert_to_int(value);
		  			if (!xmlStrcmp(element_name, BAD_CAST "userint3"))
			  			style->userint3 = convert_to_int(value);
				  	if (!xmlStrcmp(element_name, BAD_CAST "userstring1"))
					  	style->userstring1 = value;
  					if (!xmlStrcmp(element_name, BAD_CAST "userstring2"))
	  					style->userstring2 = value;
		  			if (!xmlStrcmp(element_name, BAD_CAST "userstring3"))
			  			style->userstring3 = value;
          }
					value.clear();
					if (!xmlStrcmp(element_name, BAD_CAST "style")) {
            if (style) {
              insert (style);
              style = NULL;
            }
					}
					break;
				}
			}
		}
	}

  // Free memory.
	if (reader)
		xmlFreeTextReader(reader);
	if (inputbuffer)
		xmlFreeParserInputBuffer(inputbuffer);
	if (contents)
		g_free(contents);
}


Stylesheet::~Stylesheet()
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    delete styles[i];
  }
}


void Stylesheet::save ()
// Saves the stylesheet to its native xml format.
{
  // If no name is given, we work with the template. It can't be saved.
  if (name.empty())
    return;

  // Start the new xml document.
  xmlBufferPtr buffer = xmlBufferCreate();
  xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent(writer, 1);
  xmlTextWriterStartElement(writer, BAD_CAST "bibledit-configuration");

  // Get the combined information, and write it to the document.
  for (unsigned int i = 0; i < styles.size(); i++) {

    StyleV2 * style = styles[i];

    // Open a style for the marker
    xmlTextWriterStartElement(writer, BAD_CAST "style");
    xmlTextWriterWriteFormatAttribute (writer, BAD_CAST "marker", "%s", style->marker.c_str());

    // Write values.

    if (!style->name.empty()) {
      xmlTextWriterStartElement(writer, BAD_CAST "name");
      xmlTextWriterWriteFormatString(writer, "%s", style->name.c_str());
      xmlTextWriterEndElement(writer);
    }        

    if (!style->info.empty()) {
      xmlTextWriterStartElement(writer, BAD_CAST "info");
      xmlTextWriterWriteFormatString(writer, "%s", style->info.c_str());
      xmlTextWriterEndElement(writer);
    }

    xmlTextWriterStartElement(writer, BAD_CAST "type");
    xmlTextWriterWriteFormatString(writer, "%d", style->type);
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "subtype");
    xmlTextWriterWriteFormatString(writer, "%d", style->subtype);
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "fontsize");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->fontsize).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "italic");
    xmlTextWriterWriteFormatString(writer, "%s", style->italic.c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "bold");
    xmlTextWriterWriteFormatString(writer, "%s", style->bold.c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "underline");
    xmlTextWriterWriteFormatString(writer, "%s", style->underline.c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "smallcaps");
    xmlTextWriterWriteFormatString(writer, "%s", style->smallcaps.c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "superscript");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->superscript).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "justification");
    xmlTextWriterWriteFormatString(writer, "%s", style->justification.c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "spacebefore");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->spacebefore).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "spaceafter");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->spaceafter).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "leftmargin");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->leftmargin).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "rightmargin");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->rightmargin).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "firstlineindent");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->firstlineindent).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "spancolumns");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->spancolumns).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "color");
    xmlTextWriterWriteFormatString(writer, "%d", style->color);
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "print");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->print).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userbool1");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->userbool1).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userbool2");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->userbool2).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userbool3");
    xmlTextWriterWriteFormatString(writer, "%s", convert_to_string (style->userbool3).c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userint1");
    xmlTextWriterWriteFormatString(writer, "%d", style->userint1);
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userint2");
    xmlTextWriterWriteFormatString(writer, "%d", style->userint2);
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userint3");
    xmlTextWriterWriteFormatString(writer, "%d", style->userint3);
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userstring1");
    xmlTextWriterWriteFormatString(writer, "%s", style->userstring1.c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userstring2");
    xmlTextWriterWriteFormatString(writer, "%s", style->userstring2.c_str());
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "userstring3");
    xmlTextWriterWriteFormatString(writer, "%s", style->userstring3.c_str());
    xmlTextWriterEndElement(writer);

    // Close the style.
    xmlTextWriterEndElement(writer);
  }

  // Close document and write it to disk.
  xmlTextWriterEndDocument(writer);
  xmlTextWriterFlush(writer);
  ustring filename = stylesheet_xml_filename (name);
  g_file_set_contents(filename.c_str(), (const gchar *)buffer->content, -1, NULL);

  // Free memory.
  if (writer)
    xmlFreeTextWriter(writer);
  if (buffer)
    xmlBufferFree(buffer);
}


StyleV2 * Stylesheet::style (const ustring& marker)
// This returns the style for "marker" if the marker is in the stylesheet.
// Else it returns NULL..
{
  StyleV2 * style = styles_map[marker];
  return style;
}


void Stylesheet::erase (const ustring& marker)
// Erases the Style for "marker".
{
  StyleV2 * style_to_erase = style (marker);
  if (style_to_erase == NULL)
    return;
  
  vector <StyleV2 *> styles2 = styles;
  styles.clear();
  for (unsigned int i = 0; i < styles2.size(); i++) {
    if (styles2[i] != style_to_erase) {
      styles.push_back (styles2[i]);
    }
  } 
  delete style_to_erase;
  styles_map[marker] = NULL;  
}


void Stylesheet::insert (StyleV2 * style)
// Inserts "style" into the stylesheet.
{
  styles.push_back (style);
  styles_map[style->marker] = style;
}
