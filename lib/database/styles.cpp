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
vector <string> Database_Styles::getSheets () // Todo test it.
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


void Database_Styles::importXml (string sheet, string xml) // Todo
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
            if (elementname == "type")            database_styles_item.type =            filter_string_convert_to_int (text);
            if (elementname == "subtype")         database_styles_item.subtype =         filter_string_convert_to_int (text);
            if (elementname == "fontsize")        database_styles_item.fontsize =        text;
            if (elementname == "italic")          database_styles_item.italic =          filter_string_convert_to_int (text);
            if (elementname == "bold")            database_styles_item.bold =            filter_string_convert_to_int (text);
            if (elementname == "underline")       database_styles_item.underline =       filter_string_convert_to_int (text);
            if (elementname == "smallcaps")       database_styles_item.smallcaps =       filter_string_convert_to_int (text);
            if (elementname == "superscript")     database_styles_item.superscript =     filter_string_convert_to_int (text);
            if (elementname == "justification")   database_styles_item.justification =   filter_string_convert_to_int (text);
            if (elementname == "spacebefore")     database_styles_item.spacebefore =     text;
            if (elementname == "spaceafter")      database_styles_item.spaceafter =      text;
            if (elementname == "leftmargin")      database_styles_item.leftmargin =      text;
            if (elementname == "rightmargin")     database_styles_item.rightmargin =     text;
            if (elementname == "firstlineindent") database_styles_item.firstlineindent = text;
            if (elementname == "spancolumns")     database_styles_item.spancolumns =     filter_string_convert_to_int (text);
            if (elementname == "color")           database_styles_item.color =           text;
            if (elementname == "print")           database_styles_item.print =           filter_string_convert_to_int (text);
            if (elementname == "userbool1")       database_styles_item.userbool1 =       filter_string_convert_to_int (text);
            if (elementname == "userbool2")       database_styles_item.userbool2 =       filter_string_convert_to_int (text);
            if (elementname == "userbool3")       database_styles_item.userbool3 =       filter_string_convert_to_int (text);
            if (elementname == "userint1")        database_styles_item.userint1 =        filter_string_convert_to_int (text);
            if (elementname == "userint2")        database_styles_item.userint2 =        filter_string_convert_to_int (text);
            if (elementname == "userint3")        database_styles_item.userint3 =        filter_string_convert_to_int (text);
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
            // Write to database. Todo
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
            sql.append (filter_string_convert_to_string (database_styles_item.type));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.subtype));
            sql.append (", ");
            sql.append (database_sqlite_no_sql_injection (database_styles_item.fontsize));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.italic));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.bold));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.underline));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.smallcaps));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.superscript));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.justification));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.spacebefore));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.spaceafter));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.leftmargin));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.rightmargin));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.firstlineindent));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.spancolumns));
            sql.append (", ");
            sql.append ("'" + database_sqlite_no_sql_injection (database_styles_item.color) + "'");
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.print));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.userbool1));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.userbool2));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.userbool3));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.userint1));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.userint2));
            sql.append (", ");
            sql.append (filter_string_convert_to_string (database_styles_item.userint3));
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


string Database_Styles::exportXml (string name)
{

  /* PortC++
  $xml = new SimpleXMLElement ("<stylesheet></stylesheet>");
  $markers = $this->getMarkers ($name);
  foreach ($markers as $marker) {
    $style = $xml->addChild ("style");
    $style->addChild ("marker", $marker);
    $marker_data = $this->getMarkerData ($name, $marker);
    $style->addChild ("name",                 $marker_data['name']);
    $style->addChild ("info",                 $marker_data['info']);
    $style->addChild ("category",             $marker_data['category']);
    $style->addChild ("type",                 $marker_data['type']);
    $style->addChild ("subtype",              $marker_data['subtype']);
    $style->addChild ("fontsize",             $marker_data['fontsize']);
    $style->addChild ("italic",               $marker_data['italic']);
    $style->addChild ("bold",                 $marker_data['bold']);
    $style->addChild ("underline",            $marker_data['underline']);
    $style->addChild ("smallcaps",            $marker_data['smallcaps']);
    $style->addChild ("superscript",          $marker_data['superscript']);
    $style->addChild ("justification",        $marker_data['justification']);
    $style->addChild ("spacebefore",          $marker_data['spacebefore']);
    $style->addChild ("spaceafter",           $marker_data['spaceafter']);
    $style->addChild ("leftmargin",           $marker_data['leftmargin']);
    $style->addChild ("rightmargin",          $marker_data['rightmargin']);
    $style->addChild ("firstlineindent",      $marker_data['firstlineindent']);
    $style->addChild ("spancolumns",          $marker_data['spancolumns']);
    $style->addChild ("color",                $marker_data['color']);
    $style->addChild ("print",                $marker_data['print']);
    $style->addChild ("userbool1",            $marker_data['userbool1']);
    $style->addChild ("userbool2",            $marker_data['userbool2']);
    $style->addChild ("userbool3",            $marker_data['userbool3']);
    $style->addChild ("userint1",             $marker_data['userint1']);
    $style->addChild ("userint2",             $marker_data['userint2']);
    $style->addChild ("userint3",             $marker_data['userint3']);
    $style->addChild ("userstring1",          $marker_data['userstring1']);
    $style->addChild ("userstring2",          $marker_data['userstring2']);
    $style->addChild ("userstring3",          $marker_data['userstring3']);
  }
  $domnode = dom_import_simplexml ($xml);
  $dom = new DOMDocument ("1.0", "UTF-8");
  $domnode = $dom->importNode ($domnode, true);
  $dom->appendChild ($domnode);
  $dom->formatOutput = true;
  $string = $dom->saveXML ();
  return $string;
  */
  return name;
}


// Deletes a stylesheet.
void Database_Styles::deleteSheet (string name)
{
  name = database_sqlite_no_sql_injection (name);
  sqlite3 * db = connect ();
  string sql = "DELETE FROM styles WHERE sheet = '" + name + "';";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Adds a marker to the stylesheet.
void Database_Styles::addMarker (string sheet, string marker)
{
  sheet  = database_sqlite_no_sql_injection (sheet);
  marker = database_sqlite_no_sql_injection (marker);
  string sql = "INSERT INTO styles (sheet, marker, name, info) VALUES ('" + sheet + "', '" + marker + "', 'marker name', 'marker information');";
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Deletes a marker from a stylesheet.
void Database_Styles::deleteMarker (string sheet, string marker)
{
  sheet  = database_sqlite_no_sql_injection (sheet);
  marker = database_sqlite_no_sql_injection (marker);
  string sql = "DELETE FROM styles WHERE sheet = '" + sheet + "' AND marker = '" + marker + "';";
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}








/* Todo port.


// Returns an array with all the markers and the names of the styles in the stylesheet.
public function getMarkersAndNames ($sheet)
{
  $markers_names = array ();
  $sheet = Database_SQLiteInjection::no ($sheet);
  $query = "SELECT marker, name FROM styles WHERE sheet = '$sheet' ORDER BY marker ASC;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    unset ($row [0]);
    unset ($row [1]);
    $markers_names [] = $row;
  }
  return $markers_names;
}


// Returns an array with all the markers of the styles in the stylesheet.
public function getMarkers ($sheet)
{
  $sheet = Database_SQLiteInjection::no ($sheet);
  $markers = array ();
  $query = "SELECT marker FROM styles WHERE sheet = '$sheet' ORDER BY marker ASC;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $markers[] = $row[0];
  }
  if (empty ($markers)) {
    $this->createSheet ($sheet);
    return $this->getMarkers ($sheet);
  }
  return $markers;
}


// Returns an array with all data belonging to a marker.
public function getMarkerData ($sheet, $marker)
{
  $sheet = Database_SQLiteInjection::no ($sheet);
  $marker = Database_SQLiteInjection::no ($marker);
  $query = "SELECT * FROM styles WHERE sheet = '$sheet' AND marker = '$marker';";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    // Pad the color with preceding zeroes so the color shows properly in the html page.
    $row ['color'] = str_pad ($row ['color'], 6, "0", STR_PAD_LEFT);
    for ($i = 0; $i < 35; $i++) unset ($row [$i]);
    return $row;
  }
  return NULL;
}


// Updates a style's name.
public function updateName ($sheet, $marker, $name)
{
  $sheet   = Database_SQLiteInjection::no ($sheet);
  $marker  = Database_SQLiteInjection::no ($marker);
  $name    = Database_SQLiteInjection::no ($name);
  $query   = "UPDATE styles SET name = '$name' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's info.
public function updateInfo ($sheet, $marker, $info)
{
  $sheet   = Database_SQLiteInjection::no ($sheet);
  $marker  = Database_SQLiteInjection::no ($marker);
  $info    = Database_SQLiteInjection::no ($info);
  $query   = "UPDATE styles SET info = '$info' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's category.
public function updateCategory ($sheet, $marker, $category)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $category  = Database_SQLiteInjection::no ($category);
  $query     = "UPDATE styles SET category = '$category' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's type.
public function updateType ($sheet, $marker, $type)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $type      = Database_SQLiteInjection::no ($type);
  $query     = "UPDATE styles SET type = '$type' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's subtype.
public function updateSubType ($sheet, $marker, $subtype)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $subtype   = Database_SQLiteInjection::no ($subtype);
  $query     = "UPDATE styles SET subtype = '$subtype' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's font size.
public function updateFontsize ($sheet, $marker, $fontsize)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $fontsize  = Database_SQLiteInjection::no ($fontsize);
  $query     = "UPDATE styles SET fontsize = '$fontsize' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's italic setting.
public function updateItalic ($sheet, $marker, $italic)
{
  $sheet  = Database_SQLiteInjection::no ($sheet);
  $marker = Database_SQLiteInjection::no ($marker);
  $italic = Database_SQLiteInjection::no ($italic);
  $query  = "UPDATE styles SET italic = $italic WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's bold setting.
public function updateBold ($sheet, $marker, $bold)
{
  $sheet  = Database_SQLiteInjection::no ($sheet);
  $marker = Database_SQLiteInjection::no ($marker);
  $bold   = Database_SQLiteInjection::no ($bold);
  $query  = "UPDATE styles SET bold = $bold WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's underline setting.
public function updateUnderline ($sheet, $marker, $underline)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $underline = Database_SQLiteInjection::no ($underline);
  $query  = "UPDATE styles SET underline = $underline WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Updates a style's small caps setting.
public function updateSmallcaps ($sheet, $marker, $smallcaps)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $smallcaps = Database_SQLiteInjection::no ($smallcaps);
  $query  = "UPDATE styles SET smallcaps = $smallcaps WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateSuperscript ($sheet, $marker, $superscript)
{
  $sheet       = Database_SQLiteInjection::no ($sheet);
  $marker      = Database_SQLiteInjection::no ($marker);
  $superscript = Database_SQLiteInjection::no ($superscript);
  $query  = "UPDATE styles SET superscript = $superscript WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateJustification ($sheet, $marker, $justification)
{
  $sheet         = Database_SQLiteInjection::no ($sheet);
  $marker        = Database_SQLiteInjection::no ($marker);
  $justification = Database_SQLiteInjection::no ($justification);
  $query  = "UPDATE styles SET justification = $justification WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateSpaceBefore ($sheet, $marker, $spacebefore)
{
  $sheet       = Database_SQLiteInjection::no ($sheet);
  $marker      = Database_SQLiteInjection::no ($marker);
  $spacebefore = Database_SQLiteInjection::no ($spacebefore);
  $query  = "UPDATE styles SET spacebefore = $spacebefore WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateSpaceAfter ($sheet, $marker, $spaceafter)
{
  $sheet      = Database_SQLiteInjection::no ($sheet);
  $marker     = Database_SQLiteInjection::no ($marker);
  $spaceafter = Database_SQLiteInjection::no ($spaceafter);
  $query  = "UPDATE styles SET spaceafter = $spaceafter WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateLeftMargin ($sheet, $marker, $leftmargin)
{
  $sheet      = Database_SQLiteInjection::no ($sheet);
  $marker     = Database_SQLiteInjection::no ($marker);
  $leftmargin = Database_SQLiteInjection::no ($leftmargin);
  $query  = "UPDATE styles SET leftmargin = $leftmargin WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateRightMargin ($sheet, $marker, $rightmargin)
{
  $sheet       = Database_SQLiteInjection::no ($sheet);
  $marker      = Database_SQLiteInjection::no ($marker);
  $rightmargin = Database_SQLiteInjection::no ($rightmargin);
  $query  = "UPDATE styles SET rightmargin = $rightmargin WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateFirstLineIndent ($sheet, $marker, $firstlineindent)
{
  $sheet           = Database_SQLiteInjection::no ($sheet);
  $marker          = Database_SQLiteInjection::no ($marker);
  $firstlineindent = Database_SQLiteInjection::no ($firstlineindent);
  $query  = "UPDATE styles SET firstlineindent = $firstlineindent WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateSpanColumns ($sheet, $marker, $spancolumns)
{
  $sheet       = Database_SQLiteInjection::no ($sheet);
  $marker      = Database_SQLiteInjection::no ($marker);
  $spancolumns = Database_SQLiteInjection::no ($spancolumns);
  $query  = "UPDATE styles SET spancolumns = $spancolumns WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateColor ($sheet, $marker, $color)
{
  $sheet   = Database_SQLiteInjection::no ($sheet);
  $marker  = Database_SQLiteInjection::no ($marker);
  $color   = Database_SQLiteInjection::no ($color);
  $query  = "UPDATE styles SET color = '$color' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updatePrint ($sheet, $marker, $print)
{
  $sheet  = Database_SQLiteInjection::no ($sheet);
  $marker = Database_SQLiteInjection::no ($marker);
  $print  = Database_SQLiteInjection::no ($print);
  $query  = "UPDATE styles SET print = $print WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserbool1 ($sheet, $marker, $userbool1)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $userbool1 = Database_SQLiteInjection::no ($userbool1);
  $query  = "UPDATE styles SET userbool1 = $userbool1 WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserbool2 ($sheet, $marker, $userbool2)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $userbool2 = Database_SQLiteInjection::no ($userbool2);
  $query  = "UPDATE styles SET userbool2 = $userbool2 WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserbool3 ($sheet, $marker, $userbool3)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $userbool3 = Database_SQLiteInjection::no ($userbool3);
  $query  = "UPDATE styles SET userbool3 = $userbool3 WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserint1 ($sheet, $marker, $userint1)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $userint1  = Database_SQLiteInjection::no ($userint1);
  $query  = "UPDATE styles SET userint1 = $userint1 WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserint2 ($sheet, $marker, $userint2)
{
  $sheet     = Database_SQLiteInjection::no ($sheet);
  $marker    = Database_SQLiteInjection::no ($marker);
  $userint2  = Database_SQLiteInjection::no ($userint2);
  $query  = "UPDATE styles SET userint2 = $userint2 WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserstring1 ($sheet, $marker, $userstring1)
{
  $sheet       = Database_SQLiteInjection::no ($sheet);
  $marker      = Database_SQLiteInjection::no ($marker);
  $userstring1 = Database_SQLiteInjection::no ($userstring1);
  $query  = "UPDATE styles SET userstring1 = '$userstring1' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserstring2 ($sheet, $marker, $userstring2)
{
  $sheet       = Database_SQLiteInjection::no ($sheet);
  $marker      = Database_SQLiteInjection::no ($marker);
  $userstring2 = Database_SQLiteInjection::no ($userstring2);
  $query  = "UPDATE styles SET userstring2 = '$userstring2' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


public function updateUserstring3 ($sheet, $marker, $userstring3)
{
  $sheet       = Database_SQLiteInjection::no ($sheet);
  $marker      = Database_SQLiteInjection::no ($marker);
  $userstring3 = Database_SQLiteInjection::no ($userstring3);
  $query  = "UPDATE styles SET userstring3 = '$userstring3' WHERE sheet = '$sheet' AND marker = '$marker';";
  Database_SQLite::exec ($this->db, $query);
}


// Grant $user write access to stylesheet $sheet.
public function grantWriteAccess ($user, $sheet)
{
  $user = Database_SQLiteInjection::no ($user);
  $sheet = Database_SQLiteInjection::no ($sheet);
  $query = "INSERT INTO users VALUES ('$user', '$sheet');";
  Database_SQLite::exec ($this->db, $query);
}


// Revoke a $user's write access to stylesheet $sheet.
// If the $user is empty, then revoke write access of anybody to that $sheet.
public function revokeWriteAccess ($user, $sheet)
{
  $user = Database_SQLiteInjection::no ($user);
  $sheet = Database_SQLiteInjection::no ($sheet);
  if ($user != "") $query = "DELETE FROM users WHERE user = '$user' AND sheet = '$sheet';";
  else $query = "DELETE FROM users WHERE sheet = '$sheet';";
  Database_SQLite::exec ($this->db, $query);
}


// Returns true or false depending on whether $user has write access to $sheet.
public function hasWriteAccess ($user, $sheet)
{
  $query = "SELECT rowid FROM users WHERE user = '$user' AND sheet = '$sheet';";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    return true;
  }
  return false;
}


*/


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
  userstring1 = "0";
  userstring2 = "0";
  userstring3 = "0";
}


Database_Styles_Item::~Database_Styles_Item ()
{
}


