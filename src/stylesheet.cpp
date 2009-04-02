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
  myname = name_in;

  // If the name is empty, read from the template.
  ustring filename = stylesheet_xml_filename(myname);
  if (myname.empty())
    filename = stylesheet_xml_template_filename();
  cout << "creating and reading stylesheet " << filename << endl; // Todo

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
              cout << "style marker is " << style->marker << endl; // Todo
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
              loaded.push_back (style);
              styles[style->marker] = style;
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
  cout << "destroying stylesheet " << myname << endl; // Todo
  for (unsigned int i = 0; i < loaded.size(); i++) {
    delete loaded[i];
  }
}


StyleV2 * Stylesheet::style (const ustring& marker) // Todo
// This returns the style for "marker", or NULL.
{
  return NULL;
}


// Todo to implement this one.

// Todo we need to have a <style marker="x"> style of doing things, so as to speed it up a lot.
// Todo In the mean time we leave the <marker>x</marker> style in till we've moved completely.
