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


#include "swordkjv.h"
#include <gtk/gtk.h>
#include "gwrappers.h"
#include "progresswindow.h"
#include <libxml/xmlreader.h>
#include "reference.h"


ustring sword_kjv_html_entry_url ()
{
  return "sword_kjv_entry";
}


ustring sword_kjv_action_url ()
{
  return "sword_kjv_action";
}


void import_sword_kjv_home_entry (HtmlWriter2& htmlwriter)
{
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add (sword_kjv_html_entry_url (), "Import the KJV Bible from the Sword library");
  htmlwriter.paragraph_close ();
}


void import_sword_kjv_detailed_page (HtmlWriter2& htmlwriter)
{
  htmlwriter.heading_open (3);
  htmlwriter.text_add ("Import KJV Bible from Sword library");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("The King James version from the Sword library has parsings and other information that is used to assist the translator. Bibledit comes with this data already loaded. If a new version is available in the Sword library, it can be imported here and used subsequently.");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add (sword_kjv_action_url (), "Import");
  htmlwriter.paragraph_close();
}


vector <ustring> import_sword_kjv_action () // Todo
{
  vector <ustring> messages;
  
  // Check whether program mod2osis is available.
  if (!gw_find_program_in_path ("mod2osis")) {
    messages.push_back ("Program mod2osis is needed to do the import, but it is not available on this system. It would normally be available in a packaged called libsword-dev or similar.");
    return messages;
  }

  
  
  /*
  // Read the KJV Sword module.
  ProgressWindow progresswindow ("Reading", false);
  progresswindow.set_iterate (0, 1, 65000);
  ustring osis;
  FILE *stream = popen("mod2osis KJV", "r");
  char buf[8192];
  while (fgets(buf, sizeof(buf), stream)) {
    osis.append(buf);
    progresswindow.iterate ();
  }
  int result = pclose(stream);
  if (result != 0) {
    messages.push_back ("Something didn't work out during import. The system log will give more information. Is the KJV module available in the Sword library?");
    return messages;
  }
  */

  gchar *contents;
  g_file_get_contents("/home/teus/kjv.txt", &contents, NULL, NULL);
  if (!contents) {
    messages.push_back ("Testing only");
    return messages;
  }

  // Parse input.
  xmlParserInputBufferPtr inputbuffer;
//  inputbuffer = xmlParserInputBufferCreateMem(osis.c_str(), osis.length(), XML_CHAR_ENCODING_NONE);
  inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    bool reading_relevant_content = false;
    Reference reference (0, 0, "0");
    while ((xmlTextReaderRead(reader) == 1)) {
      switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          if (!xmlStrcmp(element_name, BAD_CAST "verse")) {
            reading_relevant_content = true;
          }
          if (reading_relevant_content) 
            cout << element_name << endl; // Todo
            /*
            char *attribute;
            attribute = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "title");
            if (attribute) {
              parallelsection.title = attribute;
              free(attribute);
            }
             
             */
          break;
        }
      case XML_READER_TYPE_TEXT:
        {
          xmlChar *text = xmlTextReaderValue(reader);
          if (text) {
            if (reading_relevant_content) 
              cout << text << endl; // Todo
            //value.push_back((const char *)text);
            xmlFree(text);
          }
          break;
        }
      case XML_READER_TYPE_END_ELEMENT:
        {
          xmlChar *element_name = xmlTextReaderName(reader);
          if (reading_relevant_content)
            cout << element_name << endl; // Todo
          if (!xmlStrcmp(element_name, BAD_CAST "verse")) {
            reading_relevant_content = false;
          }
          break;
        }
      }
    }
  }
  if (reader)
    xmlFreeTextReader(reader);
  if (inputbuffer)
    xmlFreeParserInputBuffer(inputbuffer);
  if (contents)
    g_free(contents);

  // Give the okay message.  
  messages.push_back ("The KJV Bible was successfully imported. Full changes take effect after reboot.");
  return messages;
}


void import_sword_kjv_result_page (const vector <ustring>& messages, HtmlWriter2& htmlwriter)
{
  for (unsigned int i = 0; i < messages.size(); i++) {
    htmlwriter.paragraph_open();
    htmlwriter.text_add (messages[i]);
    htmlwriter.paragraph_close();
  } 
}


