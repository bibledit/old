<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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
    if ( empty( self::$instance ) ) {
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
      "id"  => gettext ("Identification information"),
      "ith" => gettext ("Introduction titles and headings"),
      "ipp" => gettext ("Introduction paragraphs and poetry"),
      "ioe" => gettext ("Introduction other elements"),
      "t"   => gettext ("Titles"),
      "h"   => gettext ("Headings"),
      "cv"  => gettext ("Chapters and verses"),
      "p"   => gettext ("Paragraphs"),
      "l"   => gettext ("Lists"),
      "pe"  => gettext ("Poetry elements"),
      "te"  => gettext ("Table elements"),
      "f"   => gettext ("Footnotes"),
      "x"   => gettext ("Crossreferences"),
      "xsn" => gettext ("Extended study notes"),
      "st"  => gettext ("Special text"),
      "cs"  => gettext ("Character styles"),
      "sb"  => gettext ("Spacings and breaks"),
      "sf"  => gettext ("Special features"),
      "pm"  => gettext ("Peripheral materials")
    );
    $text = $texts[$category];
    if ($text == "") {
      $text = gettext ("Extra styles");
    }
    return $text;
  }
  

  /**
  * typeText - Returns the $type as human readable text.
  */
  public function typeText ($type)
  {
    $texts = array (
      StyleTypeIdentifier       => gettext ("is an identifier"),
      StyleTypeStartsParagraph  => gettext ("starts a new paragraph"),
      StyleTypeInlineText       => gettext ("is inline text with endmarker"),
      StyleTypeChapterNumber    => gettext ("is a chapter number"),
      StyleTypeVerseNumber      => gettext ("is a verse number"),
      StyleTypeFootEndNote      => gettext ("is a footnote or endnote"),
      StyleTypeCrossreference   => gettext ("is a crossreference"),
      StyleTypePeripheral       => gettext ("is a peripheral element"),
      StyleTypePicture          => gettext ("is a picture"),
      StyleTypePageBreak        => gettext ("starts a new page"),
      StyleTypeTableElement     => gettext ("is a table element"),
      StyleTypeWordlistElement  => gettext ("is a word list element")
    );
    $text = $texts[$type];
    if ($text == "") {
      $text = $type;
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
        IdentifierSubtypeBook                   => gettext ("identifies the book"),
        IdentifierSubtypeEncoding               => gettext ("identifies the encoding"),
        IdentifierSubtypeComment                => gettext ("is a comment"),
        IdentifierSubtypeRunningHeader          => gettext ("is a running header"),
        IdentifierSubtypeLongTOC                => gettext ("is long table of contents text"),
        IdentifierSubtypeShortTOC               => gettext ("is short table of contents text"),
        IdentifierSubtypeBookAbbrev             => gettext ("is the book abbreviation"),
        IdentifierSubtypeChapterLabel           => gettext ("is the chapter label"),
        IdentifierSubtypePublishedChapterMarker => gettext ("is the published chapter marker"),
        IdentifierSubtypeCommentWithEndmarker   => gettext ("is a comment with an endmarker")
      ),
      StyleTypeStartsParagraph => array (
        0 => gettext ("is a main title"),       // MainTitle
        1 => gettext ("is a subtitle"),         // SubTitle
        2 => gettext ("is a section heading"),  // SectionHeading
        3 => gettext ("is a normal paragraph")  // NormalParagraph
      ),
      StyleTypeInlineText => array (
      ),
      StyleTypeChapterNumber => array (
      ),
      StyleTypeVerseNumber => array (
      ),
      StyleTypeFootEndNote => array (
        FootEndNoteSubtypeFootnote              => gettext ("starts a footnote"),
        FootEndNoteSubtypeEndnote               => gettext ("starts an endnote"),
        FootEndNoteSubtypeStandardContent       => gettext ("is standard content"),
        FootEndNoteSubtypeContent               => gettext ("is content"),
        FootEndNoteSubtypeContentWithEndmarker  => gettext ("is content with endmarker"),
        FootEndNoteSubtypeParagraph             => gettext ("starts another paragraph")
      ),
      StyleTypeCrossreference => array (
        ParagraphSubtypeMainTitle       => gettext ("starts a crossreference"),
        ParagraphSubtypeSubTitle        => gettext ("is standard content"),
        ParagraphSubtypeSectionHeading  => gettext ("is content"),
        ParagraphSubtypeNormalParagraph => gettext ("is content with endmarker")
      ),
      StyleTypePeripheral => array (
        PeripheralSubtypePublication      => gettext ("starts publication data"),
        PeripheralSubtypeTableOfContents  => gettext ("starts table of contents"),
        PeripheralSubtypePreface          => gettext ("starts preface"),
        PeripheralSubtypeIntroduction     => gettext ("starts introduction"),
        PeripheralSubtypeGlossary         => gettext ("starts concordance"),
        PeripheralSubtypeConcordance      => gettext ("starts glossary"),
        PeripheralSubtypeIndex            => gettext ("starts index"),
        PeripheralSubtypeMapIndex         => gettext ("starts map index"),
        PeripheralSubtypeCover            => gettext ("starts cover"),
        PeripheralSubtypeSpine            => gettext ("starts spine")
      ),
      StyleTypePicture => array (
      ),
      StyleTypePageBreak => array (
      ),
      StyleTypeTableElement => array (
        TableElementSubtypeRow     => gettext ("starts a new row"),
        TableElementSubtypeHeading => gettext ("is a column heading"),
        TableElementSubtypeCell    => gettext ("is cell data")
      ),
      StyleTypeWordlistElement => array (
        WorListElementSubtypeWordlistGlossaryDictionary => gettext ("is a wordlist / glossary / dictionary entry"),
        WorListElementSubtypeHebrewWordlistEntry        => gettext ("is a Hebrew wordlist entry"),
        WorListElementSubtypeGreekWordlistEntry         => gettext ("is a Greek wordlist entry"),
        WorListElementSubtypeSubjectIndexEntry          => gettext ("is a subject index entry")
      )
    );
    @$text = $texts[$type][$subtype];
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
      ooitOff     => gettext ("Off"),
      ooitOn      => gettext ("On"),
      ooitInherit => gettext ("Inherit"),
      ooitToggle  => gettext ("Toggle")
    );
    $text = $texts[$value];
    if ($text == "") {
      $text = $value;
    }
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
      AlignmentLeft     => gettext ("Start (Left in left-to-right script)"),
      AlignmentCenter   => gettext ("Center"),
      AlignmentRight    => gettext ("End (Right in left-to-right script)"),
      AlignmentJustify  => gettext ("Justify")
    );
    $text = $texts[$value];
    if ($text == "") {
      $text = $value;
    }
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
      case UserBool1PrintChapterAtFirstVerse : return gettext ("Print chapter number at first verse");
      case UserBool1IdStartsNewPage          : return gettext ("Start on a new page");
      case UserBool1NoteAppliesToApocrypha   : return gettext ("Refers to the Apocrypha");
      case UserBool1VerseRestartsParagraph   : return gettext ("Restart paragraph");
    }
    return "";
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
      case UserBool2IdStartsOddPage            : return gettext ("New page starts with an odd number (not implemented due to limitations in OpenDocument)");
      case UserBool2ChapterInLeftRunningHeader : return gettext ("Print chapter number in the running header of the left page");
      case UserBool2RunningHeaderLeft          : return gettext ("Print this in the running header of the left page");
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
      case UserBool3ChapterInRightRunningHeader : return gettext ("Print chapter number in the running header of the right page");
      case UserBool3RunningHeaderRight          : return gettext ("Print this in the running header of the right page");
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
      NoteNumberingUser  => gettext ("User defined sequence"),
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
      NoteRestartNumberingNever         => gettext ("Never"),
      NoteRestartNumberingEveryBook     => gettext ("Every book"),
      NoteRestartNumberingEveryChapter  => gettext ("Every chapter"),
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
      EndNotePositionAfterBook  => gettext ("After each book"),
      EndNotePositionVeryEnd    => gettext ("After everything else"),
      EndNotePositionAtMarker   => gettext ("Upon encountering a certain marker"),
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
