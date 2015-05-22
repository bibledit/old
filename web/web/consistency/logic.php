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


class Consistency_Logic
{


  public function __construct ($id) 
  {
    $this->id = $id;
  }


  private $id;


  public function response ()
  {
    // The databases to access.
    $database_config_user = Database_Config_User::getInstance ();
    $database_volatile = Database_Volatile::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();

    // The resources to display in the Consistency tool.
    $resources = array ();
    $resources [] = Access_Bible::clamp ($database_config_user->getBible ());
    $resources = array_merge ($resources, $database_config_user->getConsistencyResources ());

    // The passages entered in the Consistency tool.
    $passages = $database_volatile->getValue ($this->id, "passages");
    $passages = trim ($passages);
    $passages = Filter_String::string2array ($passages);
    
    // The translations from the Consistency tool.
    $translations = $database_volatile->getValue ($this->id, "translations");
    $translations = trim ($translations);
    $translations = Filter_String::string2array ($translations);

    // Contains the response to display.
    $response = array ();

    // Go through the passages interpreting them.
    $previousPassage = array (1, 1, 1);
    foreach ($passages as $line) {
      $line = trim ($line);
      if ($line == "") continue;
      $range_sequence = Filter_Books::handleSequencesRanges ($line);
      foreach ($range_sequence as $line) {
        $passage = Filter_Books::interpretPassage ($previousPassage, $line);
        if ($passage[0] != 0) {
          $book = $passage [0];
          $chapter = $passage [1];
          $verse = $passage [2];
          $line = Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);
          $line .= " ";
          
          // Check whether the chapter identifier has changed for this reference.
          // If so, set a flag so the data can be re-assembled for this verse.
          // If there was no change, then the data can be fetched from the volatile database.
          $redoPassage = false;
          $passageKey = "$book.$chapter.$verse";
          $currentChapterId = $database_bibles->getChapterId ($resources [0], $book, $chapter);
          $storedChapterId = $database_volatile->getValue ($this->id, "$passageKey.id");
          if ($currentChapterId != $storedChapterId) {
            $database_volatile->setValue ($this->id, "$passageKey.id", $currentChapterId);
            $redoPassage = true;
          }

          // Go through each resource.
          foreach ($resources as $resource) {
            
            // Produce new verse text if the passage is to be redone, or else fetch the existing text.
            if ($redoPassage) {
              $text = $this->verseText ($resource, $book, $chapter, $verse);
              if ($translations != "") {
                $text = Filter_Markup::words ($translations, $text);
              }
              $database_volatile->setValue ($this->id, "$passageKey.$resource", $text);
            } else {
              $text = $database_volatile->getValue ($this->id, "$passageKey.$resource");
            }
            
            // Formatting.
            if (count ($resources) > 1) {
              $line .= "<br>";
            }
            $line .= $text;
          }
          $response [] = $line;
          $previousPassage = $passage;
        } else {
          $response [] = '<span class="error">' . Locale_Translate::_("Unknown passage") . " " . $line . '</span>';
        }
      }
    }

    $output = "";
    foreach ($response as $line) {
      $output .= "<div>$line</div>\n";
    }
    return $output;
  }


  private function verseText ($resource, $book, $chapter, $verse)
  {
    $text = Resource_Logic::getText ($resource, $book, $chapter, $verse);
    return $text;
  }
 
  
}


?>
