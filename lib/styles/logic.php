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


define('StyleTypeIdentifier',           0);
define('StyleTypeNotUsedComment',       1);
define('StyleTypeNotUsedRunningHeader', 2);
define('StyleTypeStartsParagraph',      3);
define('StyleTypeInlineText',           4);
define('StyleTypeChapterNumber',        5);
define('StyleTypeVerseNumber',          6);
define('StyleTypeFootEndNote',          7);
define('StyleTypeCrossreference',       8);
define('StyleTypePeripheral',           9);
define('StyleTypePicture',              10);
define('StyleTypePageBreak',            11);
define('StyleTypeTableElement',         12);
define('StyleTypeWordlistElement',      13);


define('IdentifierSubtypeBook',                    0);
define('IdentifierSubtypeEncoding',                1);
define('IdentifierSubtypeComment',                 2);
define('IdentifierSubtypeRunningHeader',           3);
define('IdentifierSubtypeLongTOC',                 4);
define('IdentifierSubtypeShortTOC',                5);
define('IdentifierSubtypeBookAbbrev',              6);
define('IdentifierSubtypeChapterLabel',            7);
define('IdentifierSubtypePublishedChapterMarker',  8);
define('IdentifierSubtypeCommentWithEndmarker',    9);


define('FootEndNoteSubtypeFootnote',                0);
define('FootEndNoteSubtypeEndnote',                 1);
define('FootEndNoteSubtypeStandardContent',         2);
define('FootEndNoteSubtypeContent',                 3);
define('FootEndNoteSubtypeContentWithEndmarker',    4);
define('FootEndNoteSubtypeParagraph',               5);


define('CrossreferenceSubtypeCrossreference',       0);
define('CrossreferenceSubtypeStandardContent',      1);
define('CrossreferenceSubtypeContent',              2);
define('CrossreferenceSubtypeContentWithEndmarker', 3);


define('ParagraphSubtypeMainTitle',                 0);
define('ParagraphSubtypeSubTitle',                  1);
define('ParagraphSubtypeSectionHeading',            2);
define('ParagraphSubtypeNormalParagraph',           3);


define('PeripheralSubtypePublication',      0);
define('PeripheralSubtypeTableOfContents',  1);
define('PeripheralSubtypePreface',          2);
define('PeripheralSubtypeIntroduction',     3);
define('PeripheralSubtypeGlossary',         4);
define('PeripheralSubtypeConcordance',      5);
define('PeripheralSubtypeIndex',            6);
define('PeripheralSubtypeMapIndex',         7);
define('PeripheralSubtypeCover',            8);
define('PeripheralSubtypeSpine',            9);


define('TableElementSubtypeRow',            0);
define('TableElementSubtypeHeading',        1);
define('TableElementSubtypeCell',           2);


define('WorListElementSubtypeWordlistGlossaryDictionary', 0);
define('WorListElementSubtypeHebrewWordlistEntry',        1);
define('WorListElementSubtypeGreekWordlistEntry',         2);
define('WorListElementSubtypeSubjectIndexEntry',          3);


define('ooitOff',     0);
define('ooitOn',      1);
define('ooitInherit', 2);
define('ooitToggle',  3);


define('AlignmentLeft',     0);
define('AlignmentCenter',   1);
define('AlignmentRight',    2);
define('AlignmentJustify',  3);


define('UserBool1None',                     0);
define('UserBool1PrintChapterAtFirstVerse', 1);
define('UserBool1IdStartsNewPage',          2);
define('UserBool1NoteAppliesToApocrypha',   3);
define('UserBool1VerseRestartsParagraph',   4);


define('UserBool2None',                       0);
define('UserBool2IdStartsOddPage',            1); // \id: whether to start an odd page number. Not implemented due to limitations in OpenDocument.
define('UserBool2ChapterInLeftRunningHeader', 2); // Chapter number (\c): Whether to include it in the left running header.
define('UserBool2RunningHeaderLeft',          3); // Running header (\h(#): Whether to include it in the left running header.


define('UserBool3None',                         0);
define('UserBool3ChapterInRightRunningHeader',  1); // Chapter number (\c): Whether to include it in the right running header.
define('UserBool3RunningHeaderRight',           2); // Running header (\h(#): Whether to include it in the right running header.


define('UserInt1None',              0);
define('UserInt1NoteNumbering',     1); // Applies to footnote, endnote, crossreference.
define('UserInt1TableColumnNumber', 2);


define('UserInt2None',                  0);
define('UserInt2NoteNumberingRestart',  1); // Applies to footnote, crossreference.
define('UserInt2EndnotePosition',       2);


define('UserInt3None',                  0);


define('UserString1None',                   0);
define('UserString1NoteNumberingSequence',  1);
define('UserString1WordListEntryAddition',  2); // Word list entries: text to add after entry.


define('UserString2None',             0);
define('UserString2DumpEndnotesHere', 1); // Endnotes: dump notes upon encountering this marker.


define('UserString3None', 0);


define('NoteNumbering123',  0);
define('NoteNumberingAbc',  1);
define('NoteNumberingUser', 2);


define('NoteRestartNumberingNever',        0);
define('NoteRestartNumberingEveryBook',    1);
define('NoteRestartNumberingEveryChapter', 2);


define('EndNotePositionAfterBook', 0);
define('EndNotePositionVeryEnd',   1);
define('EndNotePositionAtMarker',  2);


/**
* This contains the styles logic.
*/
class Styles_Logic
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Styles_Logic();
    }
    return self::$instance;
  }


  /**
  * categoryText - Returns the $category as human readable text.
  */
  public function categoryText ($category)
  {
    $texts = array (
      "id"  => Locale_Translate::_("Identification information"),
      "ith" => Locale_Translate::_("Introduction titles and headings"),
      "ipp" => Locale_Translate::_("Introduction paragraphs and poetry"),
      "ioe" => Locale_Translate::_("Introduction other elements"),
      "t"   => Locale_Translate::_("Titles"),
      "h"   => Locale_Translate::_("Headings"),
      "cv"  => Locale_Translate::_("Chapters and verses"),
      "p"   => Locale_Translate::_("Paragraphs"),
      "l"   => Locale_Translate::_("Lists"),
      "pe"  => Locale_Translate::_("Poetry elements"),
      "te"  => Locale_Translate::_("Table elements"),
      "f"   => Locale_Translate::_("Footnotes"),
      "x"   => Locale_Translate::_("Crossreferences"),
      "xsn" => Locale_Translate::_("Extended study notes"),
      "st"  => Locale_Translate::_("Special text"),
      "cs"  => Locale_Translate::_("Character styles"),
      "sb"  => Locale_Translate::_("Spacings and breaks"),
      "sf"  => Locale_Translate::_("Special features"),
      "pm"  => Locale_Translate::_("Peripheral materials")
    );
    @$text = $texts[$category];
    if ($text == "") {
      $text = Locale_Translate::_("Extra styles");
    }
    return $text;
  }


  /**
  * typeText - Returns the $type as human readable text.
  */
  public function typeText ($type)
  {
    $texts = array (
      StyleTypeIdentifier       => Locale_Translate::_("is an identifier"),
      StyleTypeStartsParagraph  => Locale_Translate::_("starts a new paragraph"),
      StyleTypeInlineText       => Locale_Translate::_("is inline text with endmarker"),
      StyleTypeChapterNumber    => Locale_Translate::_("is a chapter number"),
      StyleTypeVerseNumber      => Locale_Translate::_("is a verse number"),
      StyleTypeFootEndNote      => Locale_Translate::_("is a footnote or endnote"),
      StyleTypeCrossreference   => Locale_Translate::_("is a crossreference"),
      StyleTypePeripheral       => Locale_Translate::_("is a peripheral element"),
      StyleTypePicture          => Locale_Translate::_("is a picture"),
      StyleTypePageBreak        => Locale_Translate::_("starts a new page"),
      StyleTypeTableElement     => Locale_Translate::_("is a table element"),
      StyleTypeWordlistElement  => Locale_Translate::_("is a word list element")
    );
    @$text = $texts[$type];
    if ($text == "") {
      $text = "--";
    }
    return $text;
  }


  /**
  * subtypeText - Returns the $subtype as human readable text.
  */
  public function subtypeText ($type, $subtype)
  {
    $texts = array (
      StyleTypeIdentifier => array (
        IdentifierSubtypeBook                   => Locale_Translate::_("identifies the book"),
        IdentifierSubtypeEncoding               => Locale_Translate::_("identifies the encoding"),
        IdentifierSubtypeComment                => Locale_Translate::_("is a comment"),
        IdentifierSubtypeRunningHeader          => Locale_Translate::_("is a running header"),
        IdentifierSubtypeLongTOC                => Locale_Translate::_("is long table of contents text"),
        IdentifierSubtypeShortTOC               => Locale_Translate::_("is short table of contents text"),
        IdentifierSubtypeBookAbbrev             => Locale_Translate::_("is the book abbreviation"),
        IdentifierSubtypeChapterLabel           => Locale_Translate::_("is the chapter label"),
        IdentifierSubtypePublishedChapterMarker => Locale_Translate::_("is the published chapter marker"),
        IdentifierSubtypeCommentWithEndmarker   => Locale_Translate::_("is a comment with an endmarker")
      ),
      StyleTypeStartsParagraph => array (
        0 => Locale_Translate::_("is a main title"),       // MainTitle
        1 => Locale_Translate::_("is a subtitle"),         // SubTitle
        2 => Locale_Translate::_("is a section heading"),  // SectionHeading
        3 => Locale_Translate::_("is a normal paragraph")  // NormalParagraph
      ),
      StyleTypeInlineText => array (
      ),
      StyleTypeChapterNumber => array (
      ),
      StyleTypeVerseNumber => array (
      ),
      StyleTypeFootEndNote => array (
        FootEndNoteSubtypeFootnote              => Locale_Translate::_("starts a footnote"),
        FootEndNoteSubtypeEndnote               => Locale_Translate::_("starts an endnote"),
        FootEndNoteSubtypeStandardContent       => Locale_Translate::_("is standard content"),
        FootEndNoteSubtypeContent               => Locale_Translate::_("is content"),
        FootEndNoteSubtypeContentWithEndmarker  => Locale_Translate::_("is content with endmarker"),
        FootEndNoteSubtypeParagraph             => Locale_Translate::_("starts another paragraph")
      ),
      StyleTypeCrossreference => array (
        ParagraphSubtypeMainTitle       => Locale_Translate::_("starts a crossreference"),
        ParagraphSubtypeSubTitle        => Locale_Translate::_("is standard content"),
        ParagraphSubtypeSectionHeading  => Locale_Translate::_("is content"),
        ParagraphSubtypeNormalParagraph => Locale_Translate::_("is content with endmarker")
      ),
      StyleTypePeripheral => array (
        PeripheralSubtypePublication      => Locale_Translate::_("starts publication data"),
        PeripheralSubtypeTableOfContents  => Locale_Translate::_("starts table of contents"),
        PeripheralSubtypePreface          => Locale_Translate::_("starts preface"),
        PeripheralSubtypeIntroduction     => Locale_Translate::_("starts introduction"),
        PeripheralSubtypeGlossary         => Locale_Translate::_("starts concordance"),
        PeripheralSubtypeConcordance      => Locale_Translate::_("starts glossary"),
        PeripheralSubtypeIndex            => Locale_Translate::_("starts index"),
        PeripheralSubtypeMapIndex         => Locale_Translate::_("starts map index"),
        PeripheralSubtypeCover            => Locale_Translate::_("starts cover"),
        PeripheralSubtypeSpine            => Locale_Translate::_("starts spine")
      ),
      StyleTypePicture => array (
      ),
      StyleTypePageBreak => array (
      ),
      StyleTypeTableElement => array (
        TableElementSubtypeRow     => Locale_Translate::_("starts a new row"),
        TableElementSubtypeHeading => Locale_Translate::_("is a column heading"),
        TableElementSubtypeCell    => Locale_Translate::_("is cell data")
      ),
      StyleTypeWordlistElement => array (
        WorListElementSubtypeWordlistGlossaryDictionary => Locale_Translate::_("is a wordlist / glossary / dictionary entry"),
        WorListElementSubtypeHebrewWordlistEntry        => Locale_Translate::_("is a Hebrew wordlist entry"),
        WorListElementSubtypeGreekWordlistEntry         => Locale_Translate::_("is a Greek wordlist entry"),
        WorListElementSubtypeSubjectIndexEntry          => Locale_Translate::_("is a subject index entry")
      )
    );
    @$text = $texts[$type][$subtype];
    if ($text == "") $text = "--";
    return $text;
  }


  /**
  * fontsizeIsRelevant - Returns true if the fontsize is relevant for $type and $subtype
  */
  public function fontsizeIsRelevant ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeStartsParagraph : return true;
      case StyleTypeChapterNumber : return true;
      case StyleTypeFootEndNote :
      {
        switch ($subtype) {
          case FootEndNoteSubtypeStandardContent : return true;
          case FootEndNoteSubtypeParagraph       : return true;
        }
        break;
      }
      case StyleTypeCrossreference :
      {
        switch ($subtype) {
          case CrossreferenceSubtypeStandardContent : return true;
        }
        break;
      }
      case StyleTypeTableElement :
      {
        switch ($subtype) {
          case TableElementSubtypeHeading : return true;
          case TableElementSubtypeCell    : return true;
        }
        break;
      }
    }
    return false;
  }


  /**
  * italicBoldUnderLineSmallcapsAreRelevant - Returns true if the italic, bold, etc. settings
  * are relevant for $type and $subtype
  */
  public function italicBoldUnderLineSmallcapsAreRelevant ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeStartsParagraph : return true;
      case StyleTypeInlineText      : return true;
      case StyleTypeChapterNumber   : return true;
      case StyleTypeVerseNumber     : return true;
      case StyleTypeFootEndNote     : return true;
      case StyleTypeCrossreference  : return true;
      case StyleTypeTableElement :
      {
        switch ($subtype) {
          case TableElementSubtypeHeading : return true;
          case TableElementSubtypeCell    : return true;
        }
        break;
      }
    }
    return false;
  }


  /**
  * italicBoldUnderLineSmallcapsAreFull - Returns true if the italic, bold, etc. settings
  * are fully applicable for $type and $subtype. Full means it also has inherit and toggle values.
  */
  public function italicBoldUnderLineSmallcapsAreFull ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeInlineText      : return true;
      case StyleTypeVerseNumber     : return true;
      case StyleTypeFootEndNote :
      {
        switch ($subtype) {
          case FootEndNoteSubtypeFootnote             : return true;
          case FootEndNoteSubtypeEndnote              : return true;
          case FootEndNoteSubtypeContent              : return true;
          case FootEndNoteSubtypeContentWithEndmarker : return true;
        }
        break;
      }
      case StyleTypeCrossreference :
      {
        switch ($subtype) {
          case CrossreferenceSubtypeCrossreference        : return true;
          case CrossreferenceSubtypeContent               : return true;
          case CrossreferenceSubtypeContentWithEndmarker  : return true;
        }
        break;
      }
    }
    return false;
  }


  /**
  * OffOnInheritToggleText - Returns the $value as human readable text.
  */
  public function OffOnInheritToggleText ($value)
  {
    $texts = array (
      ooitOff     => Locale_Translate::_("Off"),
      ooitOn      => Locale_Translate::_("On"),
      ooitInherit => Locale_Translate::_("Inherit"),
      ooitToggle  => Locale_Translate::_("Toggle")
    );
    @$text = $texts[$value];
    if ($text == "") {
      $text = $value;
    }
    if ($text == "") $text = "--";
    return $text;
  }


  /**
  * superscriptIsRelevant - Returns true if the superscript setting is relevant for $type and $subtype
  */
  public function superscriptIsRelevant ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeInlineText      : return true;
      case StyleTypeVerseNumber     : return true;
      case StyleTypeFootEndNote :
      {
        switch ($subtype) {
          case FootEndNoteSubtypeFootnote             : return true;
          case FootEndNoteSubtypeEndnote              : return true;
          case FootEndNoteSubtypeStandardContent      : return true;
          case FootEndNoteSubtypeParagraph            : return true;
        }
        break;
      }
      case StyleTypeCrossreference :
      {
        switch ($subtype) {
          case CrossreferenceSubtypeCrossreference        : return true;
          case CrossreferenceSubtypeStandardContent       : return true;
        }
        break;
      }
    }
    return false;
  }


  /**
  * paragraphTreatsAreRelevant - Returns true if the paragraph treats are relevant for $type and $subtype
  */
  public function paragraphTreatsAreRelevant ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeStartsParagraph : return true;
      case StyleTypeChapterNumber   : return true;
      case StyleTypeFootEndNote :
      {
        switch ($subtype) {
          case FootEndNoteSubtypeStandardContent : return true;
          case FootEndNoteSubtypeParagraph       : return true;
        }
        break;
      }
      case StyleTypeCrossreference :
      {
        switch ($subtype) {
          case CrossreferenceSubtypeStandardContent : return true;
        }
        break;
      }
      case StyleTypeTableElement :
      {
        switch ($subtype) {
          case TableElementSubtypeHeading : return true;
          case TableElementSubtypeCell    : return true;
        }
        break;
      }
    }
    return false;
  }


  /**
  * alignmentText - Returns the $value as human readable text.
  */
  public function alignmentText ($value)
  {
    $texts = array (
      AlignmentLeft     => Locale_Translate::_("Start (Left in left-to-right script)"),
      AlignmentCenter   => Locale_Translate::_("Center"),
      AlignmentRight    => Locale_Translate::_("End (Right in left-to-right script)"),
      AlignmentJustify  => Locale_Translate::_("Justify")
    );
    @$text = $texts[$value];
    if ($text == "") {
      $text = $value;
    }
    if ($text == "") $text = "--";
    return $text;
  }


  /**
  * columnsAreRelevant - Returns true if the columns are relevant for $type and $subtype
  */
  public function columnsAreRelevant ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeStartsParagraph : return true;
      case StyleTypeChapterNumber   : return true;
    }
    return false;
  }


  /**
  * colorIsRelevant - Returns true if the color is relevant for $type and $subtype
  */
  public function colorIsRelevant ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeInlineText  : return true;
      case StyleTypeVerseNumber : return true;
      case StyleTypeFootEndNote :
      {
        switch ($subtype) {
          case FootEndNoteSubtypeContent              : return true;
          case FootEndNoteSubtypeContentWithEndmarker : return true;
        }
        break;
      }
      case StyleTypeCrossreference :
      {
        switch ($subtype) {
          case CrossreferenceSubtypeContent              : return true;
          case CrossreferenceSubtypeContentWithEndmarker : return true;
        }
        break;
      }
    }
    return false;
  }


  /**
  * printIsRelevant - Returns true if the print setting is relevant for $type and $subtype
  */
  public function printIsRelevant ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeVerseNumber : return true;
      case StyleTypeFootEndNote :
      {
        switch ($subtype) {
          case FootEndNoteSubtypeFootnote : return true;
          case FootEndNoteSubtypeEndnote  : return true;
        }
        break;
      }
      case StyleTypeCrossreference :
      {
        switch ($subtype) {
          case CrossreferenceSubtypeCrossreference  : return true;
        }
        break;
      }
    }
    return false;
  }


  /**
  * getUserBool1Function - Returns the function of userbool1 for $type and $subtype
  */
  public function getUserBool1Function ($type, $subtype)
  {
    if ($type == StyleTypeChapterNumber) {
      return UserBool1PrintChapterAtFirstVerse;
    }

    if ($type == StyleTypeIdentifier) {
      if ($subtype == IdentifierSubtypeBook)
        return UserBool1IdStartsNewPage;
    }

    if ($type == StyleTypeFootEndNote) {
      if (($subtype != FootEndNoteSubtypeFootnote) && ($subtype == FootEndNoteSubtypeEndnote))
        return UserBool1NoteAppliesToApocrypha;
    }

    if ($type == StyleTypeCrossreference) {
      if ($subtype != CrossreferenceSubtypeCrossreference)
        return UserBool1NoteAppliesToApocrypha;
    }

    if ($type == StyleTypeVerseNumber) {
      return UserBool1VerseRestartsParagraph;
    }

    return UserBool1None;
  }


  /**
  * getUserBool1Text - Returns the text of userbool1's function.
  */
  public function getUserBool1Text ($function)
  {
    switch ($function) {
      case UserBool1PrintChapterAtFirstVerse : return Locale_Translate::_("Print chapter number at first verse");
      case UserBool1IdStartsNewPage          : return Locale_Translate::_("Start on a new page");
      case UserBool1NoteAppliesToApocrypha   : return Locale_Translate::_("Refers to the Apocrypha");
      case UserBool1VerseRestartsParagraph   : return Locale_Translate::_("Restart paragraph");
    }
    return "--";
  }


  /**
  * getUserBool2Function - Returns the function of userbool2 for $type and $subtype
  */
  public function getUserBool2Function ($type, $subtype)
  {
    if ($type == StyleTypeIdentifier) {
      /*
      if ($subtype == IdentifierSubtypeBook)
        return UserBool2IdStartsOddPage;
      */
      if ($subtype == IdentifierSubtypeRunningHeader)
        return UserBool2RunningHeaderLeft;
    }

    if ($type == StyleTypeChapterNumber) {
      return UserBool2ChapterInLeftRunningHeader;
    }

    return UserBool2None;
  }


  /**
  * getUserBool2Text - Returns the text of userbool2's function.
  */
  public function getUserBool2Text ($function)
  {
    switch ($function) {
      case UserBool2IdStartsOddPage            : return Locale_Translate::_("New page starts with an odd number (not implemented due to limitations in OpenDocument)");
      case UserBool2ChapterInLeftRunningHeader : return Locale_Translate::_("Print chapter number in the running header of the left page");
      case UserBool2RunningHeaderLeft          : return Locale_Translate::_("Print this in the running header of the left page");
    }
    return "";
  }


  /**
  * getUserBool3Function - Returns the function of userbool3 for $type and $subtype
  */
  public function getUserBool3Function ($type, $subtype)
  {
    if ($type == StyleTypeIdentifier) {
      if ($subtype == IdentifierSubtypeRunningHeader)
        return UserBool3RunningHeaderRight;
    }

    if ($type == StyleTypeChapterNumber) {
      return UserBool3ChapterInRightRunningHeader;
    }

    return UserBool3None;
  }


  /**
  * getUserBool3Text - Returns the text of userbool3's function.
  */
  public function getUserBool3Text ($function)
  {
    switch ($function) {
      case UserBool3ChapterInRightRunningHeader : return Locale_Translate::_("Print chapter number in the running header of the right page");
      case UserBool3RunningHeaderRight          : return Locale_Translate::_("Print this in the running header of the right page");
    }
    return "";
  }


  /**
  * getUserInt1Function - Returns the function of userint1 for $type and $subtype
  */
  public function getUserInt1Function ($type, $subtype)
  {
    if ($type == StyleTypeFootEndNote) {
      if ($subtype == FootEndNoteSubtypeFootnote)
        return UserInt1NoteNumbering;
      if ($subtype == FootEndNoteSubtypeEndnote)
        return UserInt1NoteNumbering;
    }

    if ($type == StyleTypeCrossreference) {
      if ($subtype == CrossreferenceSubtypeCrossreference)
        return UserInt1NoteNumbering;
    }

    if ($type == StyleTypeTableElement) {
      if ($subtype == TableElementSubtypeHeading)
        return UserInt1TableColumnNumber;
      if ($subtype == TableElementSubtypeCell)
        return UserInt1TableColumnNumber;
    }

    return UserInt1None;
  }


  /**
  * noteNumberingText - Returns the $value as human readable text for note numbering.
  */
  public function noteNumberingText ($value)
  {
    $texts = array (
      NoteNumbering123   => "1, 2, 3 ...",
      NoteNumberingAbc   => "a, b, c ...",
      NoteNumberingUser  => Locale_Translate::_("User defined sequence"),
    );
    $text = $texts[$value];
    if ($text == "") {
      $text = $value;
    }
    return $text;
  }


  /**
  * getUserInt2Function - Returns the function of userint2 for $type and $subtype
  */
  public function getUserInt2Function ($type, $subtype)
  {
    if ($type == StyleTypeFootEndNote) {
      if ($subtype == FootEndNoteSubtypeFootnote)
        return UserInt2NoteNumberingRestart;
      if ($subtype == FootEndNoteSubtypeEndnote)
        return UserInt2EndnotePosition;
    }

    if ($type == StyleTypeCrossreference) {
      if ($subtype == CrossreferenceSubtypeCrossreference)
        return UserInt2NoteNumberingRestart;
    }

    return UserInt2None;
  }


  /**
  * noteRestartNumberingText - Returns the $value as human readable text for when to restart the note numbering.
  */
  public function noteRestartNumberingText ($value)
  {
    $texts = array (
      NoteRestartNumberingNever         => Locale_Translate::_("Never"),
      NoteRestartNumberingEveryBook     => Locale_Translate::_("Every book"),
      NoteRestartNumberingEveryChapter  => Locale_Translate::_("Every chapter"),
    );
    $text = $texts[$value];
    if ($text == "") {
      $text = $value;
    }
    return $text;
  }


  /**
  * endNotePositionText - Returns the $value as human readable text for the position of the endnotes.
  */
  public function endNotePositionText ($value)
  {
    $texts = array (
      EndNotePositionAfterBook  => Locale_Translate::_("After each book"),
      EndNotePositionVeryEnd    => Locale_Translate::_("After everything else"),
      EndNotePositionAtMarker   => Locale_Translate::_("Upon encountering a certain marker"),
    );
    $text = $texts[$value];
    if ($text == "") {
      $text = $value;
    }
    return $text;
  }


  /**
  * getUserInt3Function - Returns the function of userint3 for $type and $subtype
  */
  public function getUserInt3Function ($type, $subtype)
  {
    return UserInt3None;
  }


  /**
  * getUserString1Function - Returns the function of userstring1 for $type and $subtype
  */
  public function getUserString1Function ($type, $subtype)
  {
    if ($type == StyleTypeFootEndNote) {
      if ($subtype == FootEndNoteSubtypeFootnote)
        return UserString1NoteNumberingSequence;
      if ($subtype == FootEndNoteSubtypeEndnote)
        return UserString1NoteNumberingSequence;
    }

    if ($type == StyleTypeCrossreference) {
      if ($subtype == CrossreferenceSubtypeCrossreference)
        return UserString1NoteNumberingSequence;
    }

    if ($type == StyleTypeWordlistElement) {
      return UserString1WordListEntryAddition;
    }

    return UserString1None;
  }


  /**
  * getUserString2Function - Returns the function of userstring2 for $type and $subtype
  */
  public function getUserString2Function ($type, $subtype)
  {
    if ($type == StyleTypeFootEndNote) {
      if ($subtype == FootEndNoteSubtypeEndnote)
        return UserString2DumpEndnotesHere;
    }
    return UserString2None;
  }


  /**
  * getUserString3Function - Returns the function of userstring3 for $type and $subtype
  */
  public function getUserString3Function ($type, $subtype)
  {
    return UserString3None;
  }


  /**
  * It returns true if the combination of $type and $subtype start a new line in well-formed USFM.
  * Otherwise it returns false
  */
  public function startsNewLineInUsfm ($type, $subtype)
  {
    switch ($type) {
      case StyleTypeIdentifier :
      {
        return true;
        break;
      }
      case StyleTypeNotUsedComment :
      {
        return true;
        break;
      }
      case StyleTypeNotUsedRunningHeader :
      {
        return true;
        break;
      }
      case StyleTypeStartsParagraph :
      {
        return true;
        break;
      }
      case StyleTypeInlineText :
      {
        return false;
        break;
      }
      case StyleTypeChapterNumber :
      {
        return true;
        break;
      }
      case StyleTypeVerseNumber :
      {
        return true;
        break;
      }
      case StyleTypeFootEndNote :
      {
        return false;
        break;
      }
      case StyleTypeCrossreference :
      {
        return false;
        break;
      }
      case StyleTypePeripheral :
      {
        return true;
        break;
      }
      case StyleTypePicture :
      {
        return true;
        break;
      }
      case StyleTypePageBreak :
      {
        return true;
        break;
      }
      case StyleTypeTableElement :
      {
        if ($subtype == TableElementSubtypeRow) return true;
        return false;
        break;
      }
      case StyleTypeWordlistElement :
      {
        return false;
        break;
      }
    }
    return true;
  }



}


?>
