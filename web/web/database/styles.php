<?php
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


// This is the database for the styles.
// Resilience: It is hardly written to. 
// Chances of corruption are low.


class Database_Styles
{
  private static $instance;
  public $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("styles");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Styles();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS styles (
  sheet text,
  marker text,
  name text,
  info text,
  category text,
  type integer,
  subtype integer,
  fontsize real,
  italic integer,
  bold integer,
  underline integer,
  smallcaps integer,
  superscript integer,
  justification integer,
  spacebefore real,
  spaceafter real,
  leftmargin real,
  rightmargin real,
  firstlineindent real,
  spancolumns integer,
  color text,
  print integer,
  userbool1 integer,
  userbool2 integer,
  userbool3 integer,
  userint1 integer,
  userint2 integer,
  userint3 integer,
  userstring1 text,
  userstring2 text,
  userstring3 text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS speedup ON styles (sheet, marker)";
    Database_SQLite::exec ($this->db, $sql);

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS users (
  user text,
  sheet text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize () {
    Database_SQLite::exec ($this->db, "REINDEX styles;");
    Database_SQLite::exec ($this->db, "VACUUM styles;");
    Database_SQLite::exec ($this->db, "VACUUM users;");
  }


  // Creates the "Standard" stylesheet, if no $name is given.
  // Else creates the stylesheet $name.
  // It returns the name of the stylesheet it has created.
  public function createStandardSheet ($name = "")
  {
    if ($name == "") $name = "Standard";
    $this->createSheet ($name);
    return $name;
  }


  // Returns an array with the available stylesheets.
  public function getSheets ()
  {
    $sheets = array ();
    $query = "SELECT DISTINCT sheet FROM styles ORDER BY sheet ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $sheets[] = $row[0];
    }
    if (empty ($sheets)) {
      $sheets [] = $this->createStandardSheet ();
    }
    return $sheets;
  }


  // Creates a stylesheet.
  public function createSheet ($sheet)
  {
    $this->deleteSheet ($sheet);
    $file = dirname (dirname(__FILE__)) . '/styles/sheet.xml';
    $contents = file_get_contents ($file);
    $this->importXml ($sheet, $contents);
  }


  public function importXml ($sheet, $xml)
  {
    $sheet = Database_SQLiteInjection::no ($sheet);
    Database_SQLite::exec ($this->db, "BEGIN;");
    $stylesheet = simplexml_load_string ($xml);
    foreach ($stylesheet as $style) {
      $marker           = Database_SQLiteInjection::no ($style->marker);
      $name             = Database_SQLiteInjection::no ($style->name);
      $info             = Database_SQLiteInjection::no ($style->info);
      $category         = Database_SQLiteInjection::no ($style->category);
      $type             = Database_SQLiteInjection::no ($style->type);
      $subtype          = Database_SQLiteInjection::no ($style->subtype);
      $fontsize         = Database_SQLiteInjection::no ($style->fontsize);
      $italic           = Database_SQLiteInjection::no ($style->italic);
      $bold             = Database_SQLiteInjection::no ($style->bold);
      $underline        = Database_SQLiteInjection::no ($style->underline);
      $smallcaps        = Database_SQLiteInjection::no ($style->smallcaps);
      $superscript      = Database_SQLiteInjection::no ($style->superscript);
      $justification    = Database_SQLiteInjection::no ($style->justification);
      $spacebefore      = Database_SQLiteInjection::no ($style->spacebefore);
      $spaceafter       = Database_SQLiteInjection::no ($style->spaceafter);
      $leftmargin       = Database_SQLiteInjection::no ($style->leftmargin);
      $rightmargin      = Database_SQLiteInjection::no ($style->rightmargin);
      $firstlineindent  = Database_SQLiteInjection::no ($style->firstlineindent);
      $spancolumns      = Database_SQLiteInjection::no ($style->spancolumns);
      $color            = Database_SQLiteInjection::no ($style->color);
      $print            = Database_SQLiteInjection::no ($style->print);
      $userbool1        = Database_SQLiteInjection::no ($style->userbool1);
      $userbool2        = Database_SQLiteInjection::no ($style->userbool2);
      $userbool3        = Database_SQLiteInjection::no ($style->userbool3);
      $userint1         = Database_SQLiteInjection::no ($style->userint1);
      $userint2         = Database_SQLiteInjection::no ($style->userint2);
      $userint3         = Database_SQLiteInjection::no ($style->userint3);
      $userstring1      = Database_SQLiteInjection::no ($style->userstring1);
      $userstring2      = Database_SQLiteInjection::no ($style->userstring2);
      $userstring3      = Database_SQLiteInjection::no ($style->userstring3);
      $query = "INSERT INTO styles VALUES (
        '$sheet',
        '$marker',
        '$name',
        '$info',
        '$category',
         $type,
         $subtype,
        '$fontsize',
         $italic,
         $bold,
         $underline,
         $smallcaps,
         $superscript,
         $justification,
        '$spacebefore',
        '$spaceafter',
        '$leftmargin',
        '$rightmargin',
        '$firstlineindent',
         $spancolumns,
        '$color',
         $print,
         $userbool1,
         $userbool2,
         $userbool3,
         $userint1,
         $userint2,
         $userint3,
        '$userstring1',
        '$userstring2',
        '$userstring3'
      );";
      Database_SQLite::exec ($this->db, $query);
    }
    Database_SQLite::exec ($this->db, "COMMIT;");
  }


  public function exportXml ($name)
  {
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
  }


  // Deletes a stylesheet.
  public function deleteSheet ($name)
  {
    $name = Database_SQLiteInjection::no ($name);
    $query = "DELETE FROM styles WHERE sheet = '$name';";
    Database_SQLite::exec ($this->db, $query);
  }


  // Adds a marker to the stylesheet.
  public function addMarker ($sheet, $marker)
  {
    $sheet  = Database_SQLiteInjection::no ($sheet);
    $marker = Database_SQLiteInjection::no ($marker);
    $query = "INSERT INTO styles (sheet, marker, name, info) VALUES ('$sheet', '$marker', '$marker name', '$marker information');";
    Database_SQLite::exec ($this->db, $query);
  }
  

  // Deletes a marker from a stylesheet.
  public function deleteMarker ($sheet, $marker)
  {
    $sheet  = Database_SQLiteInjection::no ($sheet);
    $marker = Database_SQLiteInjection::no ($marker);
    $query = "DELETE FROM styles WHERE sheet = '$sheet' and marker = '$marker';";
    Database_SQLite::exec ($this->db, $query);
  }


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


}


?>
