<?php
/**
* @package bibledit
*/
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


/**
* This is a database for the styles.
*/
class Database_Styles
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance()
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Styles();
    }
    // Ensure there's at least one stylesheet.
    self::$instance->ensureOneSheet();
    return self::$instance;
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE styles;");
    $database_instance->runQuery ("OPTIMIZE TABLE styles;");
  }


  /**
  * ensureOneSheet - Ensures that there is at least one sheet in the database.
  */
  public function ensureOneSheet ()
  {
    $sheets = $this->getSheets ();
    $sheetcount = count ($sheets);
    if ($sheetcount == 0) {
      $this->createSheet ("Standard");
    }
  }


  /**
  * getSheets - Returns an array with the available stylesheets.
  */
  public function getSheets ()
  {
    $sheets = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT sheet FROM styles ORDER BY sheet ASC;";
    $result = $database_instance->runQuery ($query);
    if ($result !== false) {
      for ($i = 0; $i < $result->num_rows; $i++) {
        $row = $result->fetch_row();
        $sheets[] = $row[0];
      }
    }
    return $sheets;
  }


  /**
  * createSheet - Creates a stylesheet.
  */
  public function createSheet ($sheet)
  {
    $sheet = Database_SQLInjection::no ($sheet);
    $this->deleteSheet ($sheet);
    $file = dirname (dirname(__FILE__)) . '/styles/sheet.xml';
    $contents = file_get_contents ($file);
    $this->importXml ($sheet, $contents);
  }


  public function importXml ($sheet, $xml)
  {
    $database_instance = Database_Instance::getInstance();
    $stylesheet = simplexml_load_string ($xml);
    foreach ($stylesheet as $style) {
      $marker           = $style->marker;
      $name             = $style->name;
      $name             = addslashes ($name);
      $info             = $style->info;
      $info             = addslashes ($info);
      $category         = $style->category;
      $type             = $style->type;
      $subtype          = $style->subtype;
      $fontsize         = $style->fontsize;
      $italic           = $style->italic;
      $bold             = $style->bold;
      $underline        = $style->underline;
      $smallcaps        = $style->smallcaps;
      $superscript      = $style->superscript;
      $justification    = $style->justification;
      $spacebefore      = $style->spacebefore;
      $spaceafter       = $style->spaceafter;
      $leftmargin       = $style->leftmargin;
      $rightmargin      = $style->rightmargin;
      $firstlineindent  = $style->firstlineindent;
      $spancolumns      = $style->spancolumns;
      $color            = $style->color;
      $print            = $style->print;
      $userbool1        = $style->userbool1;
      $userbool2        = $style->userbool2;
      $userbool3        = $style->userbool3;
      $userint1         = $style->userint1;
      $userint2         = $style->userint2;
      $userint3         = $style->userint3;
      $userstring1      = $style->userstring1;
      $userstring2      = $style->userstring2;
      $userstring3      = $style->userstring3;
      $query = "INSERT INTO styles VALUES (
         NULL, 
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
      $database_instance->runQuery ($query);
    }
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
    $string = htmlentities ($string);
    return $string;
  }
  

  /**
    * deleteSheet - Deletes a stylesheet.
    */      
  public function deleteSheet ($name)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $query = "DELETE FROM styles WHERE sheet = '$name';";
    $database_instance->runQuery ($query);
  }


  /**
    * deleteMarker - Deletes a marker from a stylesheet.
    */      
  public function deleteMarker ($sheet, $marker)
  {
    $sheet  = Database_SQLInjection::no ($sheet);
    $marker = Database_SQLInjection::no ($marker);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM styles WHERE sheet = '$sheet' and marker = '$marker';";
    $database_instance->runQuery ($query);
  }


  /**
  * getMarkersAndNames - Returns an array with all the markers and the names of the styles in the stylesheet.
  */
  public function getMarkersAndNames ($sheet)
  {
    $sheet = Database_SQLInjection::no ($sheet);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT marker, name FROM styles WHERE sheet = '$sheet' ORDER BY marker ASC;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_assoc();
      $markers_names[] = $row;
    }
    return $markers_names;
  }


  /**
  * getMarkers - Returns an array with all the markers of the styles in the stylesheet.
  */
  public function getMarkers ($sheet)
  {
    $sheet = Database_SQLInjection::no ($sheet);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT marker FROM styles WHERE sheet = '$sheet' ORDER BY marker ASC;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $markers[] = $row[0];
    }
    return $markers;
  }


  /**
  * getMarkerData - Returns an array with all data belonging to a marker.
  */
  public function getMarkerData ($sheet, $marker)
  {
    $sheet = Database_SQLInjection::no ($sheet);
    $marker = Database_SQLInjection::no ($marker);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT * FROM styles WHERE sheet = '$sheet' AND marker = '$marker';";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $data = $result->fetch_assoc();
    }
    // Pad the color with preceding zeroes so the color shows properly in the html page.
    $data['color'] = str_pad ($data['color'], 6, "0", STR_PAD_LEFT);
    return $data;
  }


  /**
  * ensureMarkerExists - ensures that the marker exists in the database
  */
  public function ensureMarkerExists ($sheet, $marker) 
  {
    $sheet = Database_SQLInjection::no ($sheet);
    $marker = Database_SQLInjection::no ($marker);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT marker FROM styles WHERE sheet = '$sheet' AND marker = '$marker';";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      return;
    }
    // NOTE: NOTE: This is not yet complete, it should create default marker.
  }


  /**
  * updateName - updates a style's name.
  */
  public function updateName ($sheet, $marker, $name) {
    $sheet   = Database_SQLInjection::no ($sheet);
    $marker  = Database_SQLInjection::no ($marker);
    $name    = Database_SQLInjection::no ($name);
    $server  = Database_Instance::getInstance ();
    $query   = "UPDATE styles SET name = '$name' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateInfo - updates a style's info.
  */
  public function updateInfo ($sheet, $marker, $info) {
    $sheet   = Database_SQLInjection::no ($sheet);
    $marker  = Database_SQLInjection::no ($marker);
    $info    = Database_SQLInjection::no ($info);
    $server  = Database_Instance::getInstance ();
    $query   = "UPDATE styles SET info = '$info' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateCategory - updates a style's category.
  */
  public function updateCategory ($sheet, $marker, $category) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $category  = Database_SQLInjection::no ($category);
    $server    = Database_Instance::getInstance ();
    $query     = "UPDATE styles SET category = '$category' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateType - updates a style's type.
  */
  public function updateType ($sheet, $marker, $type) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $type      = Database_SQLInjection::no ($type);
    $server    = Database_Instance::getInstance ();
    $query     = "UPDATE styles SET type = '$type' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateSubType - updates a style's subtype.
  */
  public function updateSubType ($sheet, $marker, $subtype) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $subtype   = Database_SQLInjection::no ($subtype);
    $server    = Database_Instance::getInstance ();
    $query     = "UPDATE styles SET subtype = '$subtype' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateFontsize - updates a style's font size.
  */
  public function updateFontsize ($sheet, $marker, $fontsize) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $fontsize  = Database_SQLInjection::no ($fontsize);
    $server    = Database_Instance::getInstance ();
    $query     = "UPDATE styles SET fontsize = '$fontsize' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateItalic - updates a style's italic setting.
  */
  public function updateItalic ($sheet, $marker, $italic) {
    $sheet  = Database_SQLInjection::no ($sheet);
    $marker = Database_SQLInjection::no ($marker);
    $italic = Database_SQLInjection::no ($italic);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET italic = $italic WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateBold - updates a style's bold setting.
  */
  public function updateBold ($sheet, $marker, $bold) {
    $sheet  = Database_SQLInjection::no ($sheet);
    $marker = Database_SQLInjection::no ($marker);
    $bold   = Database_SQLInjection::no ($bold);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET bold = $bold WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateUnderline - updates a style's underline setting.
  */
  public function updateUnderline ($sheet, $marker, $underline) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $underline = Database_SQLInjection::no ($underline);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET underline = $underline WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  /**
  * updateSmallcaps - updates a style's small caps setting.
  */
  public function updateSmallcaps ($sheet, $marker, $smallcaps) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $smallcaps = Database_SQLInjection::no ($smallcaps);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET smallcaps = $smallcaps WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateSuperscript ($sheet, $marker, $superscript) {
    $sheet       = Database_SQLInjection::no ($sheet);
    $marker      = Database_SQLInjection::no ($marker);
    $superscript = Database_SQLInjection::no ($superscript);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET superscript = $superscript WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateJustification ($sheet, $marker, $justification) {
    $sheet         = Database_SQLInjection::no ($sheet);
    $marker        = Database_SQLInjection::no ($marker);
    $justification = Database_SQLInjection::no ($justification);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET justification = $justification WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateSpaceBefore ($sheet, $marker, $spacebefore) {
    $sheet       = Database_SQLInjection::no ($sheet);
    $marker      = Database_SQLInjection::no ($marker);
    $spacebefore = Database_SQLInjection::no ($spacebefore);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET spacebefore = $spacebefore WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateSpaceAfter ($sheet, $marker, $spaceafter) {
    $sheet      = Database_SQLInjection::no ($sheet);
    $marker     = Database_SQLInjection::no ($marker);
    $spaceafter = Database_SQLInjection::no ($spaceafter);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET spaceafter = $spaceafter WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateLeftMargin ($sheet, $marker, $leftmargin) {
    $sheet      = Database_SQLInjection::no ($sheet);
    $marker     = Database_SQLInjection::no ($marker);
    $leftmargin = Database_SQLInjection::no ($leftmargin);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET leftmargin = $leftmargin WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }

  
  public function updateRightMargin ($sheet, $marker, $rightmargin) {
    $sheet       = Database_SQLInjection::no ($sheet);
    $marker      = Database_SQLInjection::no ($marker);
    $rightmargin = Database_SQLInjection::no ($rightmargin);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET rightmargin = $rightmargin WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateFirstLineIndent ($sheet, $marker, $firstlineindent) {
    $sheet           = Database_SQLInjection::no ($sheet);
    $marker          = Database_SQLInjection::no ($marker);
    $firstlineindent = Database_SQLInjection::no ($firstlineindent);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET firstlineindent = $firstlineindent WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateSpanColumns ($sheet, $marker, $spancolumns) {
    $sheet       = Database_SQLInjection::no ($sheet);
    $marker      = Database_SQLInjection::no ($marker);
    $spancolumns = Database_SQLInjection::no ($spancolumns);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET spancolumns = $spancolumns WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateColor ($sheet, $marker, $color) {
    $sheet   = Database_SQLInjection::no ($sheet);
    $marker  = Database_SQLInjection::no ($marker);
    $color   = Database_SQLInjection::no ($color);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET color = '$color' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updatePrint ($sheet, $marker, $print) {
    $sheet  = Database_SQLInjection::no ($sheet);
    $marker = Database_SQLInjection::no ($marker);
    $print  = Database_SQLInjection::no ($print);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET print = $print WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserbool1 ($sheet, $marker, $userbool1) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $userbool1 = Database_SQLInjection::no ($userbool1);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userbool1 = $userbool1 WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserbool2 ($sheet, $marker, $userbool2) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $userbool2 = Database_SQLInjection::no ($userbool2);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userbool2 = $userbool2 WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserbool3 ($sheet, $marker, $userbool3) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $userbool3 = Database_SQLInjection::no ($userbool3);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userbool3 = $userbool3 WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserint1 ($sheet, $marker, $userint1) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $userint1  = Database_SQLInjection::no ($userint1);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userint1 = $userint1 WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserint2 ($sheet, $marker, $userint2) {
    $sheet     = Database_SQLInjection::no ($sheet);
    $marker    = Database_SQLInjection::no ($marker);
    $userint2  = Database_SQLInjection::no ($userint2);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userint2 = $userint2 WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserstring1 ($sheet, $marker, $userstring1) {
    $sheet       = Database_SQLInjection::no ($sheet);
    $marker      = Database_SQLInjection::no ($marker);
    $userstring1 = Database_SQLInjection::no ($userstring1);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userstring1 = '$userstring1' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserstring2 ($sheet, $marker, $userstring2) {
    $sheet       = Database_SQLInjection::no ($sheet);
    $marker      = Database_SQLInjection::no ($marker);
    $userstring2 = Database_SQLInjection::no ($userstring2);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userstring2 = '$userstring2' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


  public function updateUserstring3 ($sheet, $marker, $userstring3) {
    $sheet       = Database_SQLInjection::no ($sheet);
    $marker      = Database_SQLInjection::no ($marker);
    $userstring3 = Database_SQLInjection::no ($userstring3);
    $server = Database_Instance::getInstance ();
    $query  = "UPDATE styles SET userstring3 = '$userstring3' WHERE sheet = '$sheet' AND marker = '$marker';";
    $server->runQuery ($query);
  }


}


?>
