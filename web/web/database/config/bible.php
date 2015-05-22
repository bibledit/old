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


class Database_Config_Bible
{
  // Singleton object instance logic.
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Config_Bible ();
    }
    return self::$instance;
  }


  private function file ($bible, $key)
  {
    $path = realpath (__DIR__ . "/../../databases/config/bible");
    $file = "$path/$bible/$key";
    return $file;
  }


  public function getValue ($bible, $key, $default)
  {
    $file = $this->file ($bible, $key);
    @$value = file_get_contents ($file);
    if ($value !== false) return $value;
    return $default;
  }


  public function setValue ($bible, $key, $value)
  {
    $file = $this->file ($bible, $key);
    $dir = dirname ($file);
    if (!file_exists ($dir)) mkdir ($dir);
    file_put_contents ($file, $value);
  }


  public function getRemoteRepositoryUrl ($bible)
  {
    return $this->getValue ($bible, "remote-repo-url", "");
  }
  public function setRemoteRepositoryUrl ($bible, $url)
  {
    $this->setValue ($bible, "remote-repo-url", $url);
  }


  public function getViewableByAllUsers ($bible)
  {
    return $this->getValue ($bible, "viewable-by-all-users", 0);
  }
  public function setViewableByAllUsers ($bible, $url)
  {
    $this->setValue ($bible, "viewable-by-all-users", $url);
  }


  public function getCheckDoubleSpacesUsfm ($bible) 
  {
    return $this->getValue ($bible, "double-spaces-usfm", "");
  }
  public function setCheckDoubleSpacesUsfm ($bible, $value) 
  {
    $this->setValue ($bible, "double-spaces-usfm", $value);
  }


  public function getCheckFullStopInHeadings ($bible) 
  {
    return $this->getValue ($bible, "full-stop-headings", "");
  }
  public function setCheckFullStopInHeadings ($bible, $value) 
  {
    $this->setValue ($bible, "full-stop-headings", $value);
  }


  public function getCheckSpaceBeforePunctuation ($bible) 
  {
    return $this->getValue ($bible, "space-before-punctuation", "");
  }
  public function setCheckSpaceBeforePunctuation ($bible, $value) 
  {
    $this->setValue ($bible, "space-before-punctuation", $value);
  }


  public function getCheckSentenceStructure ($bible) 
  {
    return $this->getValue ($bible, "sentence-structure", "");
  }
  public function setCheckSentenceStructure ($bible, $value) 
  {
    $this->setValue ($bible, "sentence-structure", $value);
  }


  public function getCheckParagraphStructure ($bible) 
  {
    return $this->getValue ($bible, "paragraph-structure", "");
  }
  public function setCheckParagraphStructure ($bible, $value)
  {
    $this->setValue ($bible, "paragraph-structure", $value);
  }


  public function getCheckChaptesVersesVersification ($bible) 
  {
    return $this->getValue ($bible, "check-chapters-verses-versification", "");
  }
  public function setCheckChaptesVersesVersification ($bible, $value) 
  {
    $this->setValue ($bible, "check-chapters-verses-versification", $value);
  }


  public function getCheckWellFormedUsfm ($bible)
  {
    return $this->getValue ($bible, "check-well-formed-usfm", "");
  }
  public function setCheckWellFormedUsfm ($bible, $value)
  {
    $this->setValue ($bible, "check-well-formed-usfm", $value);
  }


  public function getCheckMissingPunctuationEndVerse ($bible) 
  {
    return $this->getValue ($bible, "missing-punctuation-end-verse", "");
  }
  public function setCheckMissingPunctuationEndVerse ($bible, $value) 
  {
    $this->setValue ($bible, "missing-punctuation-end-verse", $value);
  }


  public function getCheckPatterns ($bible)
  {
    return $this->getValue ($bible, "check_patterns", "");
  }
  public function setCheckPatterns ($bible, $value) 
  {
    $this->setValue ($bible, "check_patterns", $value);
  }


  public function getCheckingPatterns ($bible) 
  {
    return $this->getValue ($bible, "checking-patterns", "");
  }
  public function setCheckingPatterns ($bible, $value) 
  {
    $this->setValue ($bible, "checking-patterns", $value);
  }


  public function getSentenceStructureCapitals ($bible)
  {
    return $this->getValue ($bible, "sentence-structure-capitals", "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
  }
  public function setSentenceStructureCapitals ($bible, $value) 
  {
    $this->setValue ($bible, "sentence-structure-capitals", $value);
  }


  public function getSentenceStructureSmallLetters ($bible)
  {
    return $this->getValue ($bible, "sentence-structure-small-letters", "a b c d e f g h i j k l m n o p q r s t u v w x y z");
  }
  public function setSentenceStructureSmallLetters ($bible, $value) 
  {
    $this->setValue ($bible, "sentence-structure-small-letters", $value);
  }


  public function getSentenceStructureEndPunctuation ($bible) 
  {
    return $this->getValue ($bible, "sentence-structure-end-punctuation", ". ! ? :");
  }
  public function setSentenceStructureEndPunctuation ($bible, $value) 
  {
    $this->setValue ($bible, "sentence-structure-end-punctuation", $value);
  }


  public function getSentenceStructureMiddlePunctuation ($bible) 
  {
    return $this->getValue ($bible, "sentence-structure-middle-punctuation", ", ;");
  }
  public function setSentenceStructureMiddlePunctuation ($bible, $value) 
  {
    $this->setValue ($bible, "sentence-structure-middle-punctuation", $value);
  }


  public function getSentenceStructureDisregards ($bible) 
  {
    return $this->getValue ($bible, "sentence-structure-disregards", "( ) [ ] { } ' \" * - 0 1 2 3 4 5 6 7 8 9");
  }
  public function setSentenceStructureDisregards ($bible, $value) 
  {
    $this->setValue ($bible, "sentence-structure-disregards", $value);
  }


  public function getSentenceStructureNames ($bible) 
  {
    return $this->getValue ($bible, "sentence-structure-names", "");
  }
  public function setSentenceStructureNames ($bible, $value) 
  {
    $this->setValue ($bible, "sentence-structure-names", $value);
  }


  public function getSprintTaskCompletionCategories ($bible) 
  {
    return $this->getValue ($bible, "sprint-task-completion-categories", "Translate\nCheck\nHebrew/Greek\nDiscussions");
  }
  public function setSprintTaskCompletionCategories ($bible, $value) 
  {
    $this->setValue ($bible, "sprint-task-completion-categories", $value);
  }


  public function getRepeatSendReceive ($bible)
  {
    return $this->getValue ($bible, "repeat-send-receive", false);
  }
  public function setRepeatSendReceive ($bible, $value) 
  {
    $this->setValue ($bible, "repeat-send-receive", $value);
  }


  public function getExportChapterDropCapsFrames ($bible)
  {
    return $this->getValue ($bible, "export-chapter-drop-caps-frames", 0);
  }
  public function setExportChapterDropCapsFrames ($bible, $value) 
  {
    $this->setValue ($bible, "export-chapter-drop-caps-frames", $value);
  }


  public function getPageWidth ($bible) 
  {
    return $this->getValue ($bible, "page-width", 210);
  }
  public function setPageWidth  ($bible, $value) 
  {
    $this->setValue ($bible, "page-width", $value);
  }


  public function getPageHeight ($bible) 
  {
    return $this->getValue ($bible, "page-height", 297);
  }
  public function setPageHeight  ($bible, $value) 
  {
    $this->setValue ($bible, "page-height", $value);
  }


  public function getInnerMargin ($bible) 
  {
    return $this->getValue ($bible, "inner-margin", 20);
  }
  public function setInnerMargin  ($bible, $value) 
  {
    $this->setValue ($bible, "inner-margin", $value);
  }


  public function getOuterMargin ($bible) 
  {
    return $this->getValue ($bible, "outer-margin", 10);
  }
  public function setOuterMargin  ($bible, $value) 
  {
    $this->setValue ($bible, "outer-margin", $value);
  }


  public function getTopMargin ($bible)
  {
    return $this->getValue ($bible, "top-margin", 10);
  }
  public function setTopMargin  ($bible, $value) 
  {
    $this->setValue ($bible, "top-margin", $value);
  }


  public function getBottomMargin ($bible) 
  {
    return $this->getValue ($bible, "bottom-margin", 10);
  }
  public function setBottomMargin  ($bible, $value) 
  {
    $this->setValue ($bible, "bottom-margin", $value);
  }


  public function getDateInHeader ($bible) 
  {
    return $this->getValue ($bible, "date-in-header", false);
  }
  public function setDateInHeader  ($bible, $value) 
  {
    $this->setValue ($bible, "date-in-header", $value);
  }


  public function getHyphenationFirstSet ($bible) 
  {
    return $this->getValue ($bible, "hyphenation-first-set", "");
  }
  public function setHyphenationFirstSet ($bible, $value) 
  {
    $this->setValue ($bible, "hyphenation-first-set", $value);
  }


  public function getHyphenationSecondSet ($bible)
  {
    return $this->getValue ($bible, "hyphenation-second-set", "");
  }
  public function setHyphenationSecondSet ($bible, $value) 
  {
    $this->setValue ($bible, "hyphenation-second-set", $value);
  }


  public function getExportStylesheet ($bible)
  {
    return $this->getValue ($bible, "export-stylesheet", "Standard");
  }
  public function setExportStylesheet ($bible, $value)
  {
    $this->setValue ($bible, "export-stylesheet", $value);
  }


  public function getVersificationSystem ($bible)
  {
    return $this->getValue ($bible, "versification-system", "English");
  }
  public function setVersificationSystem ($bible, $value)
  {
    $this->setValue ($bible, "versification-system", $value);
  }


  public function getVerseMapping ($bible)
  {
    return $this->getValue ($bible, "verse-mapping", "English");
  }
  public function setVerseMapping ($bible, $value)
  {
    $this->setValue ($bible, "verse-mapping", $value);
  }


  public function getBookAbbreviations ($bible)
  {
    return $this->getValue ($bible, "book-abbreviations", "");
  }
  public function setBookAbbreviations ($bible, $value)
  {
    $this->setValue ($bible, "book-abbreviations", $value);
  }


  public function getExportWebDuringNight ($bible)
  {
    return $this->getValue ($bible, "export-web-during-night", false);
  }
  public function setExportWebDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "export-web-during-night", $value);
  }


  public function getExportHtmlDuringNight ($bible)
  {
    return $this->getValue ($bible, "export-html-during-night", false);
  }
  public function setExportHtmlDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "export-html-during-night", $value);
  }


  public function getExportUsfmDuringNight ($bible)
  {
    return $this->getValue ($bible, "export-usfm-during-night", false);
  }
  public function setExportUsfmDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "export-usfm-during-night", $value);
  }


  public function getExportTextDuringNight ($bible)
  {
    return $this->getValue ($bible, "export-text-during-night", false);
  }
  public function setExportTextDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "export-text-during-night", $value);
  }


  public function getExportOdtDuringNight ($bible)
  {
    return $this->getValue ($bible, "export-odt-during-night", false);
  }
  public function setExportOdtDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "export-odt-during-night", $value);
  }


  public function getGenerateInfoDuringNight ($bible)
  {
    return $this->getValue ($bible, "generate-info-during-night", false);
  }
  public function setGenerateInfoDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "generate-info-during-night", $value);
  }


  public function getExportESwordDuringNight ($bible)
  {
    return $this->getValue ($bible, "export-esword-during-night", false);
  }
  public function setExportESwordDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "export-esword-during-night", $value);
  }


  public function getExportOnlineBibleDuringNight ($bible)
  {
    return $this->getValue ($bible, "export-onlinebible-during-night", false);
  }
  public function setExportOnlineBibleDuringNight ($bible, $value) 
  {
    $this->setValue ($bible, "export-onlinebible-during-night", $value);
  }


  public function getExportPassword ($bible)
  {
    return $this->getValue ($bible, "export-password", false);
  }
  public function setExportPassword ($bible, $value) 
  {
    $this->setValue ($bible, "export-password", $value);
  }


  public function getSecureUsfmExport ($bible)
  {
    return $this->getValue ($bible, "secure-usfm-export", false);
  }
  public function setSecureUsfmExport ($bible, $value) 
  {
    $this->setValue ($bible, "secure-usfm-export", $value);
  }


  public function getSecureOdtExport ($bible)
  {
    return $this->getValue ($bible, "secure-odt-export", false);
  }
  public function setSecureOdtExport ($bible, $value) 
  {
    $this->setValue ($bible, "secure-odt-export", $value);
  }


  public function getBookOrder ($bible)
  {
    return $this->getValue ($bible, "book-order", "");
  }
  public function setBookOrder ($bible, $value) 
  {
    $this->setValue ($bible, "book-order", $value);
  }


  public function getTextDirection ($bible)
  {
    return $this->getValue ($bible, "text-direction", 0);
  }
  public function setTextDirection ($bible, $value) 
  {
    $this->setValue ($bible, "text-direction", $value);
  }


  public function getTextFont ($bible)
  {
    return $this->getValue ($bible, "text-font", "");
  }
  public function setTextFont ($bible, $value) 
  {
    $this->setValue ($bible, "text-font", $value);
  }


}


?>
