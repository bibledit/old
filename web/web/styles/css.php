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


class Styles_Css
{


  private $stylesheet;
  private $css = array ();
  private $editor = false; // Whether to generate CSS for the Bible text editor.
  private $exports = false; // Whether to generate CSS for exported Bibles.


  public function __construct ($stylesheet)
  {
    $this->stylesheet = $stylesheet;
  }


  // Call this method to add styles suitable for the Bible editor.
  public function editor ()
  {
    $this->editor = true;
  }


  // Call this method to add styles suitable for exported Bible.
  public function exports ()
  {
    $this->exports = true;
  }


  // Generates the CSS.
  public function generate ()
  {
    $this->css [] = '.superscript { font-size: x-small; vertical-align: super; }';
    if ($this->exports) {
      $this->add_exports_styles ();
    }
    if ($this->editor) {
      $this->add_editor_styles ();
    }
    $database_styles = Database_Styles::getInstance ();
    $markers = $database_styles->getMarkers ($this->stylesheet);
    foreach ($markers as $marker) {
      $style = $database_styles->getMarkerData ($this->stylesheet, $marker);
      $this->evaluate ($style);
    }
  }


  // Evaluates the style so as to decide how it should look.
  private function evaluate ($style)
  {
    // Get the relevant styles information included.
    $styles_logic = Styles_Logic::getInstance ();

    switch ($style['type'])
    {
      case StyleTypeIdentifier: break;
      case StyleTypeNotUsedComment: break;
      case StyleTypeNotUsedRunningHeader: break;
      case StyleTypeStartsParagraph:
      {
        switch ($style['subtype'])
        {
          case ParagraphSubtypeMainTitle:
          case ParagraphSubtypeSubTitle:
          case ParagraphSubtypeSectionHeading:
          {
            $this->add ($style, true, true);
            break;
          }
          case ParagraphSubtypeNormalParagraph:
          {
            $this->add ($style, true, false);
            break;
          }
        }
        break;
      }
      case StyleTypeInlineText:
      {
        $this->add ($style, false, false);
        break;
      }
      case StyleTypeChapterNumber:
      {
        $this->add ($style, true, false);
        break;
      }
      case StyleTypeVerseNumber:
      {
        $this->add ($style, false, false);
        break;
      }
      case StyleTypeFootEndNote:
      {
        switch ($style['subtype'])
        {
          case FootEndNoteSubtypeFootnote:
          case FootEndNoteSubtypeEndnote:
          {
            $this->add ($style, true, false);
            break;
          }
          case FootEndNoteSubtypeStandardContent:
          case FootEndNoteSubtypeContent:
          case FootEndNoteSubtypeContentWithEndmarker:
          case FootEndNoteSubtypeParagraph:
          {
            $this->add ($style, false, false);
            break;
          }
        }
        break;
      }
      case StyleTypeCrossreference:
      {
        switch ($style['subtype'])
        {
          case CrossreferenceSubtypeCrossreference:
          {
            $this->add ($style, true, false);
            break;
          }
          case CrossreferenceSubtypeStandardContent:
          case CrossreferenceSubtypeContent:
          case CrossreferenceSubtypeContentWithEndmarker:
          {
            $this->add ($style, false, false);
            break;
          }
        }
        break;
      }
    }
  }


  // This function adds a style to the internal CSS.
  // $style: Array with the Bibledit-Web style information.
  // $paragraph: True: Is paragraph. False: Is inline text.
  // $keepwithnext: Keep text in this style together with the next paragraph.
  private function add ($style, $paragraph, $keepwithnext)
  {
    // Start with the class. Notice the dot.
    $class = $style ['marker'];
    $this->css [] = ".$class {";

    // Font size.
    if ($paragraph) {
      $fontsize = $style["fontsize"];
      $fontsize .= "pt";
      $this->css [] = "font-size: $fontsize;";
    }

    // Italics, bold, underline, small caps.
    $italic = $style["italic"];
    $bold = $style["bold"];
    $underline = $style["underline"];
    $smallcaps = $style["smallcaps"];

    // Italics, bold, underline, small caps can be either ooitOff or ooitOn for a paragraph.
    if ($paragraph) {
      if ($italic != ooitOff) {
        $this->css [] = "font-style: italic;";
      }
      if ($bold != ooitOff) {
        $this->css [] = "font-weight: bold;";
      }
      if ($underline != ooitOff) {
        $this->css [] = "text-decoration: underline;";
      }
      if ($smallcaps != ooitOff) {
        $this->css [] = "font-variant: small-caps;";
      }
    }

    // For inline text.
    // Italics, bold, underline, small caps can be ooitOff or ooitOn or ooitInherit or ooitToggle.
    // Not all features have been implemented.
    if (!$paragraph) {
      if (($italic == ooitOn) || ($italic == ooitToggle)) {
        $this->css [] = "font-style: italic;";
      }
      if (($bold == ooitOn) || ($bold == ooitToggle)) {
        $this->css [] = "font-weight: bold;";
      }
      if (($underline == ooitOn) || ($underline == ooitToggle)) {
        $this->css [] = "text-decoration: underline;";
      }
      if (($smallcaps == ooitOn) || ($smallcaps == ooitToggle)) {
        $this->css [] = "font-variant: small-caps;";
      }
    }

    // Paragraph layout properties
    if ($paragraph) {
      $alignment = $style["justification"];
      $spacebefore = $style["spacebefore"];
      $spaceafter = $style["spaceafter"];
      $leftmargin = $style["leftmargin"];
      $rightmargin = $style["rightmargin"];
      $firstlineindent = $style["firstlineindent"];

      // Text alignment options.
      switch ($alignment) {
        case AlignmentLeft:    $alignment = "left"; break;
        case AlignmentCenter:  $alignment = "center"; break;
        case AlignmentRight:   $alignment = "right"; break;
        case AlignmentJustify: $alignment = "justify"; break;
      }
      $this->css [] = "text-align: $alignment;";

      // Paragraph measurements; given in mm.
      $spacebefore .= "mm";
      $this->css [] = "margin-top: $spacebefore;";
      $spaceafter .= "mm";
      $this->css [] = "margin-bottom: $spaceafter;";
      $leftmargin .= "mm";
      $this->css [] = "margin-left: $leftmargin;";
      $rightmargin .= "mm";
      $this->css [] = "margin-right: $rightmargin;";
      $firstlineindent .= "mm";
      $this->css [] = "text-indent: $firstlineindent;";

      // Columns have not yet been implemented.
      $spancolumns = $style["spancolumns"];

      // Drop caps have not yet been implemented.
      $dropcaps = false;

      // Keeping text with the next paragraph.
      if ($keepwithnext) {
        $this->css [] = "page-break-inside: avoid;";
      }

    }

    // Superscript and color for inline text.
    if (!$paragraph) {

      @$superscript = $style["superscript"];
      if ($superscript) {
        $this->css [] = "font-size: x-small;";
        $this->css [] = "vertical-align: super;";
      }

      @$color = $style["color"];
      if ($color != "000000") {
        $this->css [] = "color: #$color;";
      }

    }

    // Close style.
    $this->css [] = "}";
  }


  // Deliver the created CSS.
  // $path: If given, it saves the CSS to $path.
  // The function returns the CSS as a string.
  public function css ($path = "")
  {
    $css = implode ("\n", $this->css);
    if ($path != "") {
      file_put_contents ($path, $css);
    }
    return $css;
  }


  // This adds the styles for the exports.
  private function add_exports_styles ()
  {
    $this->css [] = 'body { }';
    $this->css [] = 'p { margin-top: 0; margin-bottom: 0; }';
    $this->css [] = 'p.page { page-break-after: always; }';
    $this->css [] = 'span.dropcaps { float: left; font-size: 300%; line-height: 0.85em; margin-right: 0.03em; margin-bottom:-0.25em; }';
    $this->css [] = 'a { text-decoration: none; background: none; }';
    $this->css [] = 'a:visited { color: #5a3696; }';
    $this->css [] = 'a:active { color: #faa700; }';
    $this->css [] = 'a:hover { text-decoration: underline; }';
    $this->css [] = '.breadcrumbs { font-size: normal; }';
    $this->css [] = '.navigationbar { font-size: normal; }';
  }


  // This adds the styles for the Bible text editor.
  private function add_editor_styles ()
  {
  }


  public function customize ($bible)
  {
    $database_config_bible = Database_Config_Bible::getInstance ();
    $class = Filter_CustomCSS::getClass ($bible);
    $font = $database_config_bible->getTextFont ($bible);
    $uploaded_font = Fonts_Logic::fontExists ($font);
    $font = Fonts_Logic::getFontPath ($font);
    $direction = $database_config_bible->getTextDirection ($bible);
    $css = Filter_CustomCSS::getCss ($class, $font, $direction);
    if ($uploaded_font) $css = str_replace ("../fonts/", "", $css);
    $this->css [] = $css;
  }
  

}


?>
