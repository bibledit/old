<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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

  public $passages;
  public $translations;


  private $bible;
  private $stylesheet;
  private $siteUrl;
  private $response;
  

  public function response ()
  {
    $database_config_general = Database_Config_General::getInstance ();
    $database_config_user = Database_Config_User::getInstance ();
    $this->bible = $database_config_user->getBible ();
    $this->siteUrl = $database_config_general->getSiteURL ();
    $this->stylesheet = $database_config_general->getExportStylesheet ();
    $this->response = array ();
    $passages = explode ("\n", $this->passages);
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
          $text = Filter_Books::passageDisplay ($book, $chapter, $verse);
          $line = "<a href=\"" . $this->siteUrl . "/editusfm/index.php?switchbook=$book&switchchapter=$chapter&switchverse=$verse\" target=\"_blank\">$text</a>";
          $line .= " ";
          $line .= $this->verseText ($book, $chapter, $verse);
          $this->response [] = $line;
          $previousPassage = $passage;
        } else {
          $this->response [] = '<span class="error">' . gettext ("Unknown passage") . " " . $line . '</span>';
        }
      }
    }
    $response = "";
    foreach ($this->response as $line) {
      $response .= "<div>$line</div>\n";
    }
    return $response;
  }


  private function verseText ($book, $chapter, $verse)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $usfm = $database_bibles->getChapter ($this->bible, $book, $chapter);
    $usfm = Filter_Usfm::getVerseText ($usfm, $verse);
    $filter_text = new Filter_Text ("");
    //$filter_text->html_text_standard = new Html_Text (gettext ("Bible"));
    $filter_text->text_text = new Text_Text ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ($this->stylesheet);
    //$html = $filter_text->html_text_standard->getHtml ();
    $text = $filter_text->text_text->get ();
    return $text;
  }
  
  
}


?>
