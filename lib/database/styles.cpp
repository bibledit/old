/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <database/styles.h>
#include <database/sqlite.h>
#include <filter/url.h>
#include <filter/string.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>


using namespace std;


// This is the database for the styles.
// Resilience: It is hardly written to. 
// Chances of corruption are low.


Database_Styles::Database_Styles ()
{
}


Database_Styles::~Database_Styles ()
{
}


sqlite3 * Database_Styles::connect ()
{
  return database_sqlite_connect ("styles");
}


void Database_Styles::create ()
{
  sqlite3 * db = connect ();
  string sql;
  sql = "CREATE TABLE IF NOT EXISTS styles ("
        "sheet text,"
        "marker text,"
        "name text,"
        "info text,"
        "category text,"
        "type integer,"
        "subtype integer,"
        "fontsize real,"
        "italic integer,"
        "bold integer,"
        "underline integer,"
        "smallcaps integer,"
        "superscript integer,"
        "justification integer,"
        "spacebefore real,"
        "spaceafter real,"
        "leftmargin real,"
        "rightmargin real,"
        "firstlineindent real,"
        "spancolumns integer,"
        "color text,"
        "print integer,"
        "userbool1 integer,"
        "userbool2 integer,"
        "userbool3 integer,"
        "userint1 integer,"
        "userint2 integer,"
        "userint3 integer,"
        "userstring1 text,"
        "userstring2 text,"
        "userstring3 text"
        ");";
  database_sqlite_exec (db, sql);
  sql = "CREATE INDEX IF NOT EXISTS speedup ON styles (sheet, marker)";
  database_sqlite_exec (db, sql);
  sql = "CREATE TABLE IF NOT EXISTS users ("
        "user text,"
        "sheet text"
        ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::optimize ()
{
  sqlite3 * db = connect ();
  string sql;
  sql = "REINDEX styles;";
  database_sqlite_exec (db, sql);
  sql = "VACUUM styles;";
  database_sqlite_exec (db, sql);
  sql = "VACUUM users;";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Creates the "Standard" stylesheet, if no name is given.
// Else creates the stylesheet given in variable "name".
// It returns the name of the stylesheet it has created.
string Database_Styles::createStandardSheet (string name)
{
  if (name == "") name = "Standard";
  createSheet (name);
  return name;
}


// Creates a stylesheet.
void Database_Styles::createSheet (string sheet)
{
  deleteSheet (sheet);
  string file = filter_url_create_root_path ("styles", "sheet.xml");
  string contents = filter_url_file_get_contents (file);
  importXml (sheet, contents);
}


// Returns an array with the available stylesheets.
vector <string> Database_Styles::getSheets ()
{
  string sql = "SELECT DISTINCT sheet FROM styles ORDER BY sheet ASC;";
  sqlite3 * db = connect ();
  vector <string> sheets = database_sqlite_query (db, sql) ["sheet"];
  database_sqlite_disconnect (db);
  if (sheets.empty ()) {
    string sheet = createStandardSheet ();
    sheets.push_back (sheet);
  }
  return sheets;
}


void Database_Styles::importXml (string sheet, string xml) 
{
  sqlite3 * db = connect ();
  sheet = database_sqlite_no_sql_injection (sheet);
  database_sqlite_exec (db, "PRAGMA synchronous = OFF;");
  database_sqlite_exec (db, "BEGIN;");
  xmlParserInputBufferPtr inputbuffer = xmlParserInputBufferCreateMem (xml.c_str(), strlen (xml.c_str()), XML_CHAR_ENCODING_NONE);
  xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);
  if (reader) {
    Database_Styles_Item database_styles_item;
    string elementname;
    while ((xmlTextReaderRead (reader) == 1)) {
      switch (xmlTextReaderNodeType (reader)) {
        case XML_READER_TYPE_ELEMENT:
        {
          char *opening_element = (char *) xmlTextReaderName (reader);
          elementname.assign (opening_element);
          if (!strcmp (opening_element, "style")) {
            // Reset the styles.
            database_styles_item = Database_Styles_Item ();
          }
          break;
        }
        case XML_READER_TYPE_TEXT:
        {
          char *text = (char *) xmlTextReaderValue (reader);
          if (text) {
            if (elementname == "marker")          database_styles_item.marker =          text;
            if (elementname == "name")            database_styles_item.name =            text;
            if (elementname == "info")            database_styles_item.info =            text;
            if (elementname == "category")        database_styles_item.category =        text;
            if (elementname == "type")            database_styles_item.type =            convert_to_int (text);
            if (elementname == "subtype")         database_styles_item.subtype =         convert_to_int (text);
            if (elementname == "fontsize")        database_styles_item.fontsize =        text;
            if (elementname == "italic")          database_styles_item.italic =          convert_to_int (text);
            if (elementname == "bold")            database_styles_item.bold =            convert_to_int (text);
            if (elementname == "underline")       database_styles_item.underline =       convert_to_int (text);
            if (elementname == "smallcaps")       database_styles_item.smallcaps =       convert_to_int (text);
            if (elementname == "superscript")     database_styles_item.superscript =     convert_to_int (text);
            if (elementname == "justification")   database_styles_item.justification =   convert_to_int (text);
            if (elementname == "spacebefore")     database_styles_item.spacebefore =     text;
            if (elementname == "spaceafter")      database_styles_item.spaceafter =      text;
            if (elementname == "leftmargin")      database_styles_item.leftmargin =      text;
            if (elementname == "rightmargin")     database_styles_item.rightmargin =     text;
            if (elementname == "firstlineindent") database_styles_item.firstlineindent = text;
            if (elementname == "spancolumns")     database_styles_item.spancolumns =     convert_to_int (text);
            if (elementname == "color")           database_styles_item.color =           text;
            if (elementname == "print")           database_styles_item.print =           convert_to_int (text);
            if (elementname == "userbool1")       database_styles_item.userbool1 =       convert_to_int (text);
            if (elementname == "userbool2")       database_styles_item.userbool2 =       convert_to_int (text);
            if (elementname == "userbool3")       database_styles_item.userbool3 =       convert_to_int (text);
            if (elementname == "userint1")        database_styles_item.userint1 =        convert_to_int (text);
            if (elementname == "userint2")        database_styles_item.userint2 =        convert_to_int (text);
            if (elementname == "userint3")        database_styles_item.userint3 =        convert_to_int (text);
            if (elementname == "userstring1")     database_styles_item.userstring1 =     text;
            if (elementname == "userstring2")     database_styles_item.userstring2 =     text;
            if (elementname == "userstring3")     database_styles_item.userstring3 =     text;
            free (text);
          }
          break;
        }
        case XML_READER_TYPE_END_ELEMENT:
        {
          char *closing_element = (char *)xmlTextReaderName (reader);
          if (!strcmp (closing_element, "style")) {
            // Write to database.
            string sql = "INSERT INTO styles (sheet, marker, name, info, category, type, subtype, fontsize, italic, bold, underline, smallcaps, superscript, justification, spacebefore, spaceafter, leftmargin, rightmargin, firstlineindent, spancolumns, color, print, userbool1, userbool2, userbool3, userint1, userint2, userint3, userstring1, userstring2, userstring3) VALUES (";
            sql.append ("'" + database_sqlite_no_sql_injection (sheet) + "'");
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.marker) + "'");
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.name) + "'");
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.info) + "'");
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.category) + "'");
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.type));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.subtype));
            sql.append (", ");
            sql.append (database_sqlite_no_sql_injection (database_styles_item.fontsize));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.italic));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.bold));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.underline));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.smallcaps));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.superscript));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.justification));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.spacebefore));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.spaceafter));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.leftmargin));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.rightmargin));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.firstlineindent));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.spancolumns));
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.color) + "'");
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.print));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.userbool1));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.userbool2));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.userbool3));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.userint1));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.userint2));
            sql.append (", ");
            sql.append (convert_to_string (database_styles_item.userint3));
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.userstring1) + "'");
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.userstring2) + "'");
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.userstring3) + "'");
            sql.append (");");
            database_sqlite_exec (db, sql);
          }
          break;
        }
      }
    }
  }
  database_sqlite_exec (db, "COMMIT;");
  database_sqlite_disconnect (db);
  if (reader) xmlFreeTextReader(reader);
  if (inputbuffer) xmlFreeParserInputBuffer(inputbuffer);
}


string Database_Styles::exportXml (string sheet)
{
  // XML writer.
  xmlBufferPtr xmlbuffer = xmlBufferCreate ();
  xmlTextWriterPtr xmlwriter = xmlNewTextWriterMemory (xmlbuffer, 0);

  // Setup and start document.
  xmlTextWriterStartDocument (xmlwriter, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent (xmlwriter, 1);
  xmlTextWriterStartElement (xmlwriter, BAD_CAST "stylesheet");

  vector <string> markers = getMarkers (sheet);
  for (string marker : markers) {
    xmlTextWriterStartElement (xmlwriter, BAD_CAST "style");
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "marker");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", marker.c_str());
      xmlTextWriterEndElement (xmlwriter);
      Database_Styles_Item styles_item = getMarkerData (sheet, marker);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "name");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.name.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "info");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.info.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "category");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.category.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "type");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.type);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "subtype");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.subtype);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "fontsize");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.fontsize.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "italic");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.italic);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "bold");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.bold);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "underline");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.underline);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "smallcaps");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.smallcaps);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "superscript");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.superscript);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "justification");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.justification);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "spacebefore");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.spacebefore.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "spaceafter");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.spaceafter.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "leftmargin");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.leftmargin.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "rightmargin");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.rightmargin.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "firstlineindent");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.firstlineindent.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "spancolumns");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.spancolumns);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "color");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.color.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "print");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.print);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userbool1");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.userbool1);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userbool2");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.userbool2);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userbool3");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.userbool3);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userint1");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.userint1);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userint2");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.userint2);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userint3");
      xmlTextWriterWriteFormatString (xmlwriter, "%d", styles_item.userint3);
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userstring1");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.userstring1.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userstring2");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.userstring2.c_str());
      xmlTextWriterEndElement (xmlwriter);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "userstring3");
      xmlTextWriterWriteFormatString (xmlwriter, "%s", styles_item.userstring3.c_str());
      xmlTextWriterEndElement (xmlwriter);
    xmlTextWriterEndElement (xmlwriter); // style
  }
  
  // Close document and get its contents.
  xmlTextWriterEndDocument (xmlwriter);
  xmlTextWriterFlush (xmlwriter);
  string xml = (char *) xmlbuffer->content;
  
  // Free memory.
  if (xmlwriter) xmlFreeTextWriter(xmlwriter);
  if (xmlbuffer) xmlBufferFree(xmlbuffer);

  return xml;
}


// Deletes a stylesheet.
void Database_Styles::deleteSheet (string name)
{
  SqliteSQL sql;
  sql.add ("DELETE FROM styles WHERE sheet =");
  sql.add (name);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Adds a marker to the stylesheet.
void Database_Styles::addMarker (string sheet, string marker)
{
  SqliteSQL sql;
  sql.add ("INSERT INTO styles (sheet, marker, name, info) VALUES (");
  sql.add (sheet);
  sql.add (",");
  sql.add (marker);
  sql.add (", 'marker name', 'marker information');");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Deletes a marker from a stylesheet.
void Database_Styles::deleteMarker (string sheet, string marker)
{
  SqliteSQL sql;
  sql.add ("DELETE FROM styles WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Returns a map with all the markers and the names of the styles in the stylesheet.
map <string, vector <string> > Database_Styles::getMarkersAndNames (string sheet)
{
  sqlite3 * db = connect ();
  SqliteSQL sql;
  sql.add ("SELECT marker, name FROM styles WHERE sheet =");
  sql.add (sheet);
  sql.add ("ORDER BY marker ASC;");
  map <string, vector <string> > markers_names = database_sqlite_query (db, sql.sql);
  database_sqlite_disconnect (db);
  return markers_names;
}


// Returns an array with all the markers of the styles in the stylesheet.
vector <string> Database_Styles::getMarkers (string sheet)
{
  sqlite3 * db = connect ();
  SqliteSQL sql;
  sql.add ("SELECT marker FROM styles WHERE sheet =");
  sql.add (sheet);
  sql.add ("ORDER BY marker ASC;");
  vector <string> markers = database_sqlite_query (db, sql.sql) ["marker"];
  database_sqlite_disconnect (db);
  if (markers.empty ()) {
    createSheet (sheet);
    return getMarkers (sheet);
  }
  return markers;
}


// Returns a map with all data belonging to a marker.
// If none, it returns an empty map.
Database_Styles_Item Database_Styles::getMarkerData (string sheet, string marker)
{
  sqlite3 * db = connect ();
  SqliteSQL sql;
  sql.add ("SELECT * FROM styles WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  Database_Styles_Item marker_data = Database_Styles_Item ();
  if (!result ["marker"].empty ()) {
    marker_data.marker = result ["marker"] [0];
    marker_data.name = result ["name"] [0];
    marker_data.info = result ["info"] [0];
    marker_data.category = result ["category"] [0];
    marker_data.type = convert_to_int (result ["type"] [0]);
    marker_data.subtype = convert_to_int (result ["subtype"] [0]);
    marker_data.fontsize = result ["fontsize"] [0];
    marker_data.italic = convert_to_int (result ["italic"] [0]);
    marker_data.bold = convert_to_int (result ["bold"] [0]);
    marker_data.underline = convert_to_int (result ["underline"] [0]);
    marker_data.smallcaps = convert_to_int (result ["smallcaps"] [0]);
    marker_data.superscript = convert_to_int (result ["superscript"] [0]);
    marker_data.justification = convert_to_int (result ["justification"] [0]);
    marker_data.spacebefore = result ["spacebefore"] [0];
    marker_data.spaceafter = result ["spaceafter"] [0];
    marker_data.leftmargin = result ["leftmargin"] [0];
    marker_data.rightmargin = result ["rightmargin"] [0];
    marker_data.firstlineindent = result ["firstlineindent"] [0];
    marker_data.spancolumns = convert_to_int (result ["spancolumns"] [0]);
    marker_data.color = result ["color"] [0];
    // Pad the color with preceding zeroes so the color shows properly in the html page.
    marker_data.color = filter_string_fill (marker_data.color, 6, '0');
    marker_data.print = convert_to_int (result ["print"] [0]);
    marker_data.userbool1 = convert_to_int (result ["userbool1"] [0]);
    marker_data.userbool2 = convert_to_int (result ["userbool2"] [0]);
    marker_data.userbool3 = convert_to_int (result ["userbool3"] [0]);
    marker_data.userint1 = convert_to_int (result ["userint1"] [0]);
    marker_data.userint2 = convert_to_int (result ["userint2"] [0]);
    marker_data.userint3 = convert_to_int (result ["userint3"] [0]);
    marker_data.userstring1 = result ["userstring1"] [0];
    marker_data.userstring2 = result ["userstring2"] [0];
    marker_data.userstring3 = result ["userstring3"] [0];
  }
  return marker_data;
}


// Updates a style's name.
void Database_Styles::updateName (string sheet, string marker, string name)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET name =");
  sql.add (name);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's info.
void Database_Styles::updateInfo (string sheet, string marker, string info)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET info =");
  sql.add (info);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's category.
void Database_Styles::updateCategory (string sheet, string marker, string category)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET category =");
  sql.add (category);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's type.
void Database_Styles::updateType (string sheet, string marker, string type)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET type =");
  sql.add (type);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's subtype.
void Database_Styles::updateSubType (string sheet, string marker, string subtype)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET subtype =");
  sql.add (subtype);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's font size.
void Database_Styles::updateFontsize (string sheet, string marker, string fontsize)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET fontsize =");
  sql.add (fontsize);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker = ");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's italic setting.
void Database_Styles::updateItalic (string sheet, string marker, int italic)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET italic =");
  sql.add (italic);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's bold setting.
void Database_Styles::updateBold (string sheet, string marker, int bold)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET bold =");
  sql.add (bold);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker = ");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's underline setting.
void Database_Styles::updateUnderline (string sheet, string marker, int underline)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET underline =");
  sql.add (underline);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Updates a style's small caps setting.
void Database_Styles::updateSmallcaps (string sheet, string marker, int smallcaps)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET smallcaps =");
  sql.add (smallcaps);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateSuperscript (string sheet, string marker, int superscript)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET superscript =");
  sql.add (superscript);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateJustification (string sheet, string marker, int justification)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET justification =");
  sql.add (justification);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateSpaceBefore (string sheet, string marker, string spacebefore)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET spacebefore =");
  sql.add (spacebefore);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateSpaceAfter (string sheet, string marker, string spaceafter)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET spaceafter =");
  sql.add (spaceafter);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateLeftMargin (string sheet, string marker, string leftmargin)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET leftmargin =");
  sql.add (leftmargin);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateRightMargin (string sheet, string marker, string rightmargin)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET rightmargin =");
  sql.add (rightmargin);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateFirstLineIndent (string sheet, string marker, string firstlineindent)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET firstlineindent =");
  sql.add (firstlineindent);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateSpanColumns (string sheet, string marker, int spancolumns)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET spancolumns =");
  sql.add (spancolumns);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateColor (string sheet, string marker, string color)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET color =");
  sql.add (color);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updatePrint (string sheet, string marker, int print)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET print =");
  sql.add (print);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserbool1 (string sheet, string marker, int userbool1)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userbool1 =");
  sql.add (userbool1);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserbool2 (string sheet, string marker, int userbool2)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userbool2 =");
  sql.add (userbool2);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserbool3 (string sheet, string marker, int userbool3)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userbool3 =");
  sql.add (userbool3);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserint1 (string sheet, string marker, int userint1)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userint1 =");
  sql.add (userint1);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserint2 (string sheet, string marker, int userint2)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userint2 =");
  sql.add (userint2);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserstring1 (string sheet, string marker, string userstring1)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userstring1 =");
  sql.add (userstring1);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserstring2 (string sheet, string marker, string userstring2)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userstring2 =");
  sql.add (userstring2);
  sql.add ("WHERE sheet =");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Styles::updateUserstring3 (string sheet, string marker, string userstring3)
{
  SqliteSQL sql;
  sql.add ("UPDATE styles SET userstring3 =");
  sql.add (userstring3);
  sql.add ("WHERE sheet");
  sql.add (sheet);
  sql.add ("AND marker =");
  sql.add (marker);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Grant $user write access to stylesheet $sheet.
void Database_Styles::grantWriteAccess (string user, string sheet)
{
  SqliteSQL sql;
  sql.add ("INSERT INTO users VALUES (");
  sql.add (user);
  sql.add (",");
  sql.add (sheet);
  sql.add (");");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Revoke a $user's write access to stylesheet $sheet.
// If the $user is empty, then revoke write access of anybody to that $sheet.
void Database_Styles::revokeWriteAccess (string user, string sheet)
{
  SqliteSQL sql;
  sql.add ("DELETE FROM users WHERE");
  if (!user.empty ()) {
    sql.add ("user =");
    sql.add (user);
    sql.add ("AND");
  }
  sql.add ("sheet =");
  sql.add (sheet);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Returns true or false depending on whether $user has write access to $sheet.
bool Database_Styles::hasWriteAccess (string user, string sheet)
{
  SqliteSQL sql;
  sql.add ("SELECT rowid FROM users WHERE user =");
  sql.add (user);
  sql.add ("AND sheet =");
  sql.add (sheet);
  sql.add (";");
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  return !result["rowid"].empty ();
}


Database_Styles_Item::Database_Styles_Item ()
{
  // Defaults.
  type = 0;
  subtype = 0;
  fontsize = "12";
  italic = 0;
  bold = 0;
  underline = 0;
  smallcaps = 0;
  superscript = 0;
  justification = 0;
  spacebefore = "0";
  spaceafter = "0";
  leftmargin = "0";
  rightmargin = "0";
  firstlineindent = "0";
  spancolumns = 0;
  string color = "0";
  print = 0;
  userbool1 = 0;
  userbool2 = 0;
  userbool3 = 0;
  userint1 = 0;
  userint2 = 0;
  userint3 = 0;
  userstring1 = "";
  userstring2 = "";
  userstring3 = "";
}


Database_Styles_Item::~Database_Styles_Item ()
{
}


