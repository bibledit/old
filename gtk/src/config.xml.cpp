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

#include "config.xml.h"
#include "gwrappers.h"
#include "utilities.h"
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include "tiny_utilities.h"

bool config_xml_bool_get(const ustring & file, const ustring & key, bool standard)
{
  return convert_to_bool(config_xml_string_get(file, key, convert_to_string(standard)));
}

int config_xml_int_get(const ustring & file, const ustring & key, int standard)
{
  return convert_to_int(config_xml_string_get(file, key, convert_to_string(standard)));
}

ustring config_xml_string_get(const ustring & file, const ustring & key, const ustring & standard)
{
  ustring value = standard;
  vector < ustring > values = config_xml_vector_string_get(file, key);
  if (!values.empty())
    value = values[0];
  return value;
}

double config_xml_double_get(const ustring & file, const ustring & key, double standard)
{
  return convert_to_double(config_xml_string_get(file, key, convert_to_string(standard)));
}

vector < bool > config_xml_vector_bool_get(const ustring & file, const ustring & key)
{
  vector < bool > value;
  vector < ustring > valuelist = config_xml_vector_string_get(file, key);
  for (unsigned int i = 0; i < valuelist.size(); i++)
    value.push_back(convert_to_bool(valuelist[i]));
  return value;
}

vector < int >config_xml_vector_int_get(const ustring & file, const ustring & key)
{
  vector < int >value;
  vector < ustring > valuelist = config_xml_vector_string_get(file, key);
  for (unsigned int i = 0; i < valuelist.size(); i++)
    value.push_back(convert_to_int(valuelist[i]));
  return value;
}

vector < double >config_xml_vector_double_get(const ustring & file, const ustring & key)
{
  vector < double >value;
  vector < ustring > valuelist = config_xml_vector_string_get(file, key);
  for (unsigned int i = 0; i < valuelist.size(); i++)
    value.push_back(convert_to_double(valuelist[i]));
  return value;
}

vector < ustring > config_xml_vector_string_get(const ustring & file, const ustring & key)
{
  // A static mutex was introduced to try to solve the problem of rogue
  // resource creation, but it appeared that it didn't solve the problem.
  // Yet the mutex was left, because the configurations are accessed in 
  // a thread, so it is always good to have a mutex in place.
  static GStaticMutex config_xml_vector_string_get_mutex = G_STATIC_MUTEX_INIT;
  g_static_mutex_lock(&config_xml_vector_string_get_mutex);

  vector < ustring > value;
  if (g_file_test(file.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gchar *contents;
    g_file_get_contents(file.c_str(), &contents, NULL, NULL);
    xmlParserInputBufferPtr inputbuffer;
    inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
    xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
    if (reader) {
      gchar *myelement = g_strdup(config_xml_produce_element_name(key).c_str());
      bool keyfound = false;
      while ((xmlTextReaderRead(reader) == 1)) {
        switch (xmlTextReaderNodeType(reader)) {
        case XML_READER_TYPE_ELEMENT:
          {
            xmlChar *element_name = xmlTextReaderName(reader);
            if (!xmlStrcmp(element_name, BAD_CAST myelement))
              keyfound = true;
            if (element_name)
              xmlFree(element_name);
            break;
          }
        case XML_READER_TYPE_TEXT:
          {
            if (keyfound) {
              xmlChar *text = xmlTextReaderValue(reader);
              if (text) {
                value.push_back((const char *)text);
                xmlFree(text);
              }
            }
            break;
          }
        case XML_READER_TYPE_END_ELEMENT:
          {
            xmlChar *element_name = xmlTextReaderName(reader);
            if (!xmlStrcmp(element_name, BAD_CAST myelement))
              keyfound = false;
            if (element_name)
              xmlFree(element_name);
            break;
          }
        }
      }
      if (myelement)
        g_free(myelement);
    }
    if (reader)
      xmlFreeTextReader(reader);
    if (inputbuffer)
      xmlFreeParserInputBuffer(inputbuffer);
    if (contents)
      g_free(contents);
  }

  g_static_mutex_unlock(&config_xml_vector_string_get_mutex);

  return value;
}

ustring config_xml_produce_element_name(ustring key)
{
  replace_text(key, "_", "-");
  return key;
}

ConfigXmlPair::ConfigXmlPair(const ustring & key_in, const vector < ustring > &value_in)
{
  key = key_in;
  value = value_in;
};

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, bool value)
{
  vector < ustring > strings;
  strings.push_back(convert_to_string(value));
  config_xml_values_set_assemble(values, key, strings);
}

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, int value)
{
  vector < ustring > strings;
  strings.push_back(convert_to_string(value));
  config_xml_values_set_assemble(values, key, strings);
}

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, const ustring & value)
{
  vector < ustring > strings;
  strings.push_back(value);
  config_xml_values_set_assemble(values, key, strings);
}

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, double value)
{
  vector < ustring > strings;
  strings.push_back(convert_to_string(value));
  config_xml_values_set_assemble(values, key, strings);
}

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, const vector < bool > &value)
{
  vector < ustring > strings;
  for (unsigned int i = 0; i < value.size(); i++) {
    strings.push_back(convert_to_string(value[i]));
  }
  config_xml_values_set_assemble(values, key, strings);
}

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, const vector < int >&value)
{
  vector < ustring > strings;
  for (unsigned int i = 0; i < value.size(); i++) {
    strings.push_back(convert_to_string(value[i]));
  }
  config_xml_values_set_assemble(values, key, strings);
}

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, const vector < ustring > &value)
{
  ConfigXmlPair pair(key, value);
  pair.key = config_xml_produce_element_name(key);
  values.push_back(pair);
}

void config_xml_values_set_assemble(vector < ConfigXmlPair > &values, const ustring & key, const vector < double >&value)
{
  vector < ustring > strings;
  for (unsigned int i = 0; i < value.size(); i++) {
    strings.push_back(convert_to_string(value[i]));
  }
  config_xml_values_set_assemble(values, key, strings);
}

void config_xml_values_set_execute(const ustring & file, const vector < ConfigXmlPair > &values)
{
  // Bail out if there is nothing to write.
  if (values.empty())
    return;

  // Read the information that is already in the configuration file.
  vector < ConfigXmlPair > existingpairs;
  if (g_file_test(file.c_str(), G_FILE_TEST_IS_REGULAR)) {
    bool firstelementfound = false;
    gchar *contents;
    g_file_get_contents(file.c_str(), &contents, NULL, NULL);
    xmlParserInputBufferPtr inputbuffer;
    inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);
    xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
    if (reader) {
      vector < ustring > dummy;
      ConfigXmlPair pair("", dummy);
      while ((xmlTextReaderRead(reader) == 1)) {
        switch (xmlTextReaderNodeType(reader)) {
        case XML_READER_TYPE_ELEMENT:
          {
            if (firstelementfound) {
              char *element_name = (char *)xmlTextReaderName(reader);
              if (strcmp(element_name, "value")) {
                pair.key = element_name;
                pair.value.clear();
              }
              if (element_name)
                xmlFree(element_name);
            }
            firstelementfound = true;
            break;
          }
        case XML_READER_TYPE_TEXT:
          {
            char *text = (char *)xmlTextReaderValue(reader);
            if (text) {
              pair.value.push_back(text);
              xmlFree(text);
            }
            break;
          }
        case XML_READER_TYPE_END_ELEMENT:
          {
            char *element_name = (char *)xmlTextReaderName(reader);
            ustring element(element_name);
            if (element == pair.key) {
              existingpairs.push_back(pair);
              pair.key.clear();
              pair.value.clear();
            }
            if (element_name)
              xmlFree(element_name);
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
  }
  // The information to be saved, insert it in the already existing information,
  // replacing the values it had before.
  // Or if that information isn't there yet, append it.
  for (unsigned int nw = 0; nw < values.size(); nw++) {
    bool inserted = false;
    for (unsigned int old = 0; old < existingpairs.size(); old++) {
      if (values[nw].key == existingpairs[old].key) {
        existingpairs[old].value = values[nw].value;
        inserted = true;
        break;
      }
    }
    if (!inserted) {
      existingpairs.push_back(values[nw]);
    }
  }

  // Start the new xml document.
  xmlBufferPtr buffer = xmlBufferCreate();
  xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent(writer, 1);
  xmlTextWriterStartElement(writer, BAD_CAST "bibledit-configuration");

  // Get the combined information, and write it to the document.
  for (unsigned int i = 0; i < existingpairs.size(); i++) {
    // Do not write empty elements. This resolves a bug that occurred 
    // if elements like "<resources-viewer-urls/>" were written.
    // Next time the file was read, it took contextual information.
    // In that case rogue resources were created that didn't exist.
    if (existingpairs[i].value.empty())
      continue;
    xmlTextWriterStartElement(writer, BAD_CAST existingpairs[i].key.c_str());
    for (unsigned int i2 = 0; i2 < existingpairs[i].value.size(); i2++) {
      xmlTextWriterStartElement(writer, BAD_CAST "value");
      xmlTextWriterWriteFormatString(writer, "%s", existingpairs[i].value[i2].c_str());
      xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
  }

  // Close document and write it to disk.
  xmlTextWriterEndDocument(writer);
  xmlTextWriterFlush(writer);
  g_file_set_contents(file.c_str(), (const gchar *)buffer->content, -1, NULL);

  // Free memory.
  if (writer)
    xmlFreeTextWriter(writer);
  if (buffer)
    xmlBufferFree(buffer);
}
