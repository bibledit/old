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


require_once ("bootstrap/bootstrap.php");


class Editor_Styles
{


  public static function getRecentlyUsed ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_styles = Database_Styles::getInstance ();

    $stylesheet = $database_config_user->getStylesheet ();

    // The recent styles.
    $styles = $database_config_user->getRecentlyAppliedStyles ();
    $styles = explode (" ", $styles);
    $fragment = Locale_Translate::_("Select style") . ": ";
    foreach ($styles as $offset => $marker) {
      if ($offset) $fragment .= " | ";
      $data = $database_styles->getMarkerData ($stylesheet, $marker);
      if (!$data) continue;
      $name = $data ['name'] . " ($marker)";
      $info = $data ['info'];
      $fragment .= "<a href=\"$marker\" title=\"$info\">$name</a>";
    }

    // Links for cancelling and for all styles.
    $fragment .= " ";
    $fragment .= '<a href="cancel">[' . Locale_Translate::_("cancel") . ']</a>';
    $fragment .= " ";
    $fragment .= '<a href="all" >[' . Locale_Translate::_("all") . ']</a>';

    return $fragment;
  }


  public static function getAll ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $styles_logic = Styles_Logic::getInstance ();

    $stylesheet = $database_config_user->getStylesheet ();

    // The styles.
    $data = $database_styles->getMarkersAndNames ($stylesheet);

    $lines = array ();

    $lines [] = '<select id="styleslist">';

    $line = Locale_Translate::_("Select style");
    $lines [] = "<option>$line</option>";

    foreach ($data as $item) {
      $marker = $item ['marker'];
      $data = $database_styles->getMarkerData ($stylesheet, $marker);
      $category = $data ['category'];
      $category = $styles_logic->categoryText ($category);
      $line = $marker . " " . $item ['name'] . " (" . $category . ")";
      $lines [] = "<option>$line</option>";
    }

    $lines [] = '</select>';

    // Link for cancelling.
    $lines [] = " ";
    $lines [] = '<a href="cancel">[' . Locale_Translate::_("cancel") . ']</a>';

    $html = implode ("\n", $lines);

    return $html;
  }


  public static function recordUsage ($style)
  {
    if ($style == "") return;
    $database_config_user = Database_Config_User::getInstance ();
    $styles = $database_config_user->getRecentlyAppliedStyles ();
    $styles = explode (" ", $styles);
    $key = array_search ($style, $styles);
    if ($key !== false) {
      unset ($styles [$key]);
    }
    array_unshift ($styles, $style);
    if (count ($styles) > 8) {
      array_pop ($styles);
    }
    $styles = implode (" ", $styles);
    $database_config_user->setRecentlyAppliedStyles ($styles);
  }


  public static function getAction ($style)
  {
    $database_config_user = Database_Config_User::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $styles_logic = Styles_Logic::getInstance ();

    $stylesheet = $database_config_user->getStylesheet ();
    $data = $database_styles->getMarkerData ($stylesheet, $style);
    @$type = $data ["type"];
    @$subtype = $data ["subtype"];

    switch ($type)
    {
      case StyleTypeIdentifier:
      case StyleTypeNotUsedComment:
      case StyleTypeNotUsedRunningHeader:
        return self::mono ();
      case StyleTypeStartsParagraph:
        return self::paragraph ();
      case StyleTypeInlineText:
        return self::character ();
      case StyleTypeChapterNumber:
        return self::paragraph ();
      case StyleTypeVerseNumber:
        return self::character ();
      case StyleTypeFootEndNote:
      {
        switch ($subtype)
        {
          case FootEndNoteSubtypeFootnote:
          case FootEndNoteSubtypeEndnote:
            return self::note ();
          case FootEndNoteSubtypeContent:
          case FootEndNoteSubtypeContentWithEndmarker:
            return self::character ();
          case FootEndNoteSubtypeStandardContent:
          case FootEndNoteSubtypeParagraph:
            return self::paragraph ();
          default:
            return self::unknown ();
        }
        break;
      }
      case StyleTypeCrossreference:
      {
        switch ($subtype)
        {
          case CrossreferenceSubtypeCrossreference:
            return self::note ();
          case CrossreferenceSubtypeContent:
          case CrossreferenceSubtypeContentWithEndmarker:
            return self::character ();
          case CrossreferenceSubtypeStandardContent:
            return self::paragraph ();
          default:
            return self::unknown ();
        }
        break;
      }
      case StyleTypePeripheral:
        return self::mono ();
      case StyleTypePicture:
        return self::mono ();
      case StyleTypePageBreak:
        return self::unknown ();
      case StyleTypeTableElement:
      {
        switch ($subtype)
        {
          case TableElementSubtypeRow:
          case TableElementSubtypeHeading:
          case TableElementSubtypeCell:
          default:
            return self::mono ();
        }
        break;
      }
      case StyleTypeWordlistElement:
      {
        return self::character ();
      }
      default:
        return self::unknown ();
    }
  }


  private static function unknown ()
  {
    return 'u';
  }


  private static function paragraph ()
  {
    return 'p';
  }


  private static function character ()
  {
    return "c";
  }


  private static function mono ()
  {
    return "m";
  }


  private static function note ()
  {
    return "n";
  }


}


?>
