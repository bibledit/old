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


#ifndef INCLUDED_CONFIG_XML_H
#define INCLUDED_CONFIG_XML_H


#include "libraries.h"
#include "constants.h"


bool config_xml_bool_get (const ustring& file, const ustring& key, bool standard);
int config_xml_int_get (const ustring& file, const ustring& key, int standard);
ustring config_xml_string_get (const ustring& file, const ustring& key, const ustring& standard);
double config_xml_double_get (const ustring& file, const ustring& key, double standard);

vector<bool> config_xml_vector_bool_get (const ustring& file, const ustring& key);
vector<ustring> config_xml_vector_string_get (const ustring& file, const ustring& key);
vector<int> config_xml_vector_int_get (const ustring& file, const ustring& key);
vector<double> config_xml_vector_double_get (const ustring& file, const ustring& key);

ustring config_xml_produce_element_name (ustring key);

class ConfigXmlPair
{
public:
  ConfigXmlPair (const ustring& key_in, const vector <ustring>& value_in);
  ustring key;
  vector <ustring> value;
};

void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, bool value);
void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, int value);
void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, const ustring& value);
void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, double value);
void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, const vector <bool>& value);
void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, const vector <int>& value);
void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, const vector <ustring>& value);
void config_xml_values_set_assemble (vector <ConfigXmlPair>& values, const ustring& key, const vector <double>& value);
void config_xml_values_set_execute (const ustring& file, const vector <ConfigXmlPair>& values);


#endif
