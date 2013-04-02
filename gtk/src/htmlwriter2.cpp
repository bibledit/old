/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#include "htmlwriter2.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include "htmlbrowser.h"


HtmlWriter2::HtmlWriter2(const ustring & title)
{
  heading_opened = false;
  paragraph_opened = false;
  bold_level = 0;
  italics_level = 0;
  highlight_level = 0;

  buffer = xmlBufferCreate();
  writer = xmlNewTextWriterMemory(buffer, 0);
  xmlTextWriterSetIndent(writer, 1);
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

  xmlTextWriterStartElement(writer, BAD_CAST "html");

  xmlTextWriterStartElement(writer, BAD_CAST "head");

  xmlTextWriterStartElement(writer, BAD_CAST "meta");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "content", "text/html; charset=UTF-8");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "http-equiv", "content-type");
  xmlTextWriterEndElement(writer);

  xmlTextWriterStartElement(writer, BAD_CAST "title");
  xmlTextWriterWriteFormatString(writer, "%s", title.c_str());
  xmlTextWriterEndElement(writer);
  
  /* The following does not work to remove the underline from the link
  xmlTextWriterStartElement(writer, BAD_CAST "style");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "type", "text/css");
  xmlTextWriterWriteFormatString(writer, "a { text-decoration:none }");
  xmlTextWriterEndElement(writer);
  */

  // End head.
  xmlTextWriterEndElement(writer);

  xmlTextWriterStartElement(writer, BAD_CAST "body");
}


HtmlWriter2::~HtmlWriter2()
{
  finish ();
  if (writer)
    xmlFreeTextWriter(writer);
  if (buffer)
    xmlBufferFree(buffer);
}

void HtmlWriter2::heading_open(unsigned int level)
{
  heading_close ();
  paragraph_close ();
  ustring element("h");
  element.append(convert_to_string(level));
  xmlTextWriterStartElement(writer, BAD_CAST element.c_str());
  heading_opened = true;
}


void HtmlWriter2::heading_close ()
{
  if (heading_opened) {
    xmlTextWriterEndElement(writer);
    heading_opened = false;
  }
}


void HtmlWriter2::paragraph_open()
{
  heading_close ();
  paragraph_close ();
  xmlTextWriterStartElement(writer, BAD_CAST "p");
  paragraph_opened = true;
}


void HtmlWriter2::paragraph_close()
{
  if (paragraph_opened) {
    xmlTextWriterEndElement(writer);
    paragraph_opened = false;
  }
}


void HtmlWriter2::form_open (const gchar * name, const gchar * action, const gchar * method)
{
  xmlTextWriterStartElement(writer, BAD_CAST "form");
  if (name)
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "name", "%s", name);
  if (action)
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "action", "%s", action);
  if (method)
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "method", "%s", method);
}


void HtmlWriter2::form_close ()
{
  xmlTextWriterEndElement(writer);
}


void HtmlWriter2::input_open (const gchar * name, const gchar * type, unsigned int size, const gchar * value)
{
  xmlTextWriterStartElement(writer, BAD_CAST "input");
  if (name)
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "name", "%s", name);
  if (type)
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "type", "%s", type);
  if (size)
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "size", "%d", size);
  if (value)
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "value", "%s", value);
}


void HtmlWriter2::input_close ()
{
  xmlTextWriterEndElement(writer);
}


void HtmlWriter2::text_add(const ustring& text)
{
  if (!(heading_opened || paragraph_opened)) {
    paragraph_open ();
  }
  xmlTextWriterWriteFormatString(writer, "%s", text.c_str());
}


void HtmlWriter2::hyperlink_add (const ustring& url, const ustring& text)
// <a href="url">text</a>
// Or for links without underline: <a href="url" style="text-decoration:none">text</a> 
{
  if (!(heading_opened || paragraph_opened)) {
    paragraph_open ();
  }
  xmlTextWriterStartElement(writer, BAD_CAST "a");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "href", BAD_CAST url.c_str());
  // Does not work to remove the underline from the link xmlTextWriterWriteAttribute(writer, BAD_CAST "style", BAD_CAST "text-decoration:none");
  xmlTextWriterWriteFormatString(writer, "%s", text.c_str());
  xmlTextWriterEndElement(writer);
}


void HtmlWriter2::bold_open()
{
  xmlTextWriterStartElement(writer, BAD_CAST "b");
  bold_level++;
}


void HtmlWriter2::bold_close()
{
  if (bold_level) {
    xmlTextWriterEndElement(writer);
    bold_level--;
  }
}


void HtmlWriter2::italics_open()
{
  xmlTextWriterStartElement(writer, BAD_CAST "i");
  italics_level++;
}


void HtmlWriter2::italics_close()
{
  if (italics_level) {
    xmlTextWriterEndElement(writer);
    italics_level--;
  }
}

void HtmlWriter2::highlight_open()
{
  xmlTextWriterStartElement(writer, BAD_CAST "FONT style=\"BACKGROUND-COLOR: yellow\"");
  highlight_level++;
}


void HtmlWriter2::highlight_close()
{
  if (highlight_level) {
    xmlTextWriterEndElement(writer);
    highlight_level--;
  }
}
void HtmlWriter2::finish ()
// Finish the html document, and make it available.
{
  xmlTextWriterEndDocument(writer);
  xmlTextWriterFlush(writer);
  html = (const gchar *)buffer->content;
}

