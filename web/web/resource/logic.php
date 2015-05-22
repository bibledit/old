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


class Resource_Logic
{


  public static function getNames ()
  {
    $names = array ();

    // Take Bibles the user has read access to.
    $bibles = Access_Bible::bibles ();
    $names = array_merge ($names, $bibles);

    // Take USFM Resources.
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $usfm_resources = $database_usfmresources->getResources ();
    $names = array_merge ($names, $usfm_resources);

    // Take external Resources.
    $database_resources = Database_Resources::getInstance ();
    $resources = $database_resources->getNames ();
    $names = array_merge ($names, $resources);

    $names = array_unique ($names);
    sort ($names);

    return $names;
  }


  public static function getExternal ($name, $book, $chapter, $verse, $apply_mapping)
  {
    if ($apply_mapping) {
      $database_resources = Database_Resources::getInstance ();
      $database_mappings = Database_Mappings::getInstance ();
      $database_config_user = Database_Config_User::getInstance ();
      $database_config_bible = Database_Config_Bible::getInstance ();
      $bible = $database_config_user->getBible ();
      $bible_mapping = $database_config_bible->getVerseMapping ($bible);
      $resource_mapping = $database_resources->getMapping ($name);
      $passages = $database_mappings->translate ($bible_mapping, $resource_mapping, $book, $chapter, $verse);
      $output = "";
      foreach ($passages as $passage) {
        $object = new Resource_External ();
        $html = $object->get ($name, $passage [0], $passage [1], $passage [2]);
        unset ($object);
        $output .= $html;
      }
      return $output;
    } else {
      $object = new Resource_External ();
      $html = $object->get ($name, $book, $chapter, $verse);
      unset ($object);
      return $html;
    }
  }


  public static function getHtml ($resource, $book, $chapter, $verse)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_resources = Database_Resources::getInstance ();
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_offlineresources = Database_OfflineResources::getInstance ();
    $database_mappings = Database_Mappings::getInstance ();
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_bible = Database_Config_Bible::getInstance ();

    $bibles = $database_bibles->getBibles ();
    $usfms = $database_usfmresources->getResources ();
    $externals = $database_resources->getNames ();

    $isBible = in_array ($resource, $bibles);
    $isUsfm = in_array ($resource, $usfms);
    if ($isBible || $isUsfm) {
      if ($isBible) $chapter_usfm = $database_bibles->getChapter ($resource, $book, $chapter);
      if ($isUsfm) $chapter_usfm = $database_usfmresources->getUsfm ($resource, $book, $chapter);
      $verse_usfm = Filter_Usfm::getVerseText ($chapter_usfm, $verse);
      $database_config_user = Database_Config_User::getInstance ();
      $stylesheet = $database_config_user->getStylesheet ();
      $filter_text = new Filter_Text ($resource);
      $filter_text->html_text_standard = new Html_Text (Locale_Translate::_("Bible"));
      $filter_text->addUsfmCode ($verse_usfm);
      $filter_text->run ($stylesheet);
      $html = $filter_text->html_text_standard->getInnerHtml ();
    } else if (in_array ($resource, $externals)) {
      // Use offline copy if it exists, else fetch it online.
      if ($database_offlineresources->exists ($resource, $book, $chapter, $verse)) {
        $bible = $database_config_user->getBible ();
        $bible_mapping = $database_config_bible->getVerseMapping ($bible);
        $resource_mapping = $database_resources->getMapping ($resource);
        $passages = $database_mappings->translate ($bible_mapping, $resource_mapping, $book, $chapter, $verse);
        $html = "";
        foreach ($passages as $passage) {
          $html .= $database_offlineresources->get ($resource, $passage [0], $passage [1], $passage [2]);
        }
      } else {
        $html = Resource_Logic::getExternal ($resource, $book, $chapter, $verse, true);
      }
    } else {
      $html = "";
    }

    return $html;
  }


  // Get the resource for the verse in plain text format.
  public static function getText ($resource, $book, $chapter, $verse)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_resources = Database_Resources::getInstance ();
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $database_offlineresources = Database_OfflineResources::getInstance ();

    $bibles = $database_bibles->getBibles ();
    $usfms = $database_usfmresources->getResources ();
    $externals = $database_resources->getNames ();

    $isBible = in_array ($resource, $bibles);
    $isUsfm = in_array ($resource, $usfms);
    if ($isBible || $isUsfm) {
      if ($isBible) $chapter_usfm = $database_bibles->getChapter ($resource, $book, $chapter);
      if ($isUsfm) $chapter_usfm = $database_usfmresources->getUsfm ($resource, $book, $chapter);
      $verse_usfm = Filter_Usfm::getVerseText ($chapter_usfm, $verse);
      $database_config_user = Database_Config_User::getInstance ();
      $stylesheet = $database_config_user->getStylesheet ();
      $filter_text = new Filter_Text ($resource);
      $filter_text->text_text = new Text_Text ();
      $filter_text->addUsfmCode ($verse_usfm);
      $filter_text->run ($stylesheet);
      $text = $filter_text->text_text->get ();
    } else if (in_array ($resource, $externals)) {
      // Use offline copy if it exists, else fetch it online.
      if ($database_offlineresources->exists ($resource, $book, $chapter, $verse)) {
        $text = $database_offlineresources->get ($resource, $book, $chapter, $verse);
      } else {
        $text = Resource_Logic::getExternal ($resource, $book, $chapter, $verse, true);
      }
      $text = Filter_Html::html2text ($text);
    } else {
      $text = "";
    }

    return $text;
  }


}


?>
