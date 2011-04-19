<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
* Class for creating Html text documents.
*/
class Html_Text
{
  private $htmlDom;
  private $headDomNode;
  private $bodyDomNode;
  private $styleDomNode;
  private $notesDivDomNode;

  private $createdStyles; // An array with styles already created in the $stylesDom.

  private $currentPDomElement; // The current text:p DOMElement.
  private $currentPDomElementNameNode; // The DOMAttr of the name of the style of the current p element.
  public $currentParagraphStyle;
  public $currentParagraphContent;
  public $currentTextStyle;
  
  private $noteCount;
  private $notePDomElement; // The text:p DOMElement of the current footnote, if any.
  private $currentNoteTextStyle;
  
  
  public function __construct ($title)
  {
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
    $this->currentTextStyle = "";
    $this->frameCount = 0;
    $this->noteCount = 0;
    $this->currentNoteTextStyle = "";
    
    $database_config_general = Database_Config_General::getInstance ();

    $this->htmlDom = new DOMDocument;
    $this->htmlDom->load(dirname (__FILE__) . "/template.html");
    
    $htmlXpath = new DOMXpath ($this->htmlDom);

    $nodeList = $htmlXpath->query ("*");
    $this->headDomNode = $nodeList->item (0);
    $element = $this->htmlDom->createElement ("title");
    $this->headDomNode->appendChild ($element);
    $element->nodeValue = htmlspecialchars ($title, ENT_QUOTES, "UTF-8");
    
    $this->bodyDomNode = $nodeList->item (1);

    $nodeList = $htmlXpath->query ("*", $this->headDomNode);
    foreach ($nodeList as $node) {
      if ($node->nodeName == "style") {
        $this->styleDomNode = $node;
      }
    }
    
    $this->notesDivDomNode = $this->htmlDom->createElement ("div");

    $this->createdStyles = array ();
  }

  
  public function newParagraph ($style = "")
  {
    $this->currentPDomElement = $this->htmlDom->createElement ("p");
    if ($style != "") {
      $this->currentPDomElementNameNode = $this->currentPDomElement->setAttribute ("class", $style);
    }
    $this->bodyDomNode->appendChild ($this->currentPDomElement);
    $this->currentParagraphStyle = $style;
    $this->currentParagraphContent = "";
  }

  
  /**
  * This function adds text to the current paragraph.
  * $text: The text to add.
  */
  public function addText ($text)
  {
    if ($text != "") {
      if (!isset ($this->currentPDomElement)) {
        $this->newParagraph ();
      }
      $spanDomElement = $this->htmlDom->createElement ("span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->currentPDomElement->appendChild ($spanDomElement);
      if ($this->currentTextStyle != "") {
        // Take character style as specified in this object.
        $spanDomElement->setAttribute ("class", $this->currentTextStyle);
      }
      $this->currentParagraphContent .= $text;
    }
  }


  /**
  * This creates a h1 heading with contents.
  * $text: Contents.
  */
  public function newHeading1($text, $hide = false)
  {
    $this->newNamedHeading ("h1", $text, $hide);
  }
  

  /**
  * This applies a page break.
  * (The style is already in the css).
  */
  public function newPageBreak ()
  {
    $this->newParagraph ("break");
    // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
    // since it would be nearly invisible, and thus text would mysteriously get lost.
    unset ($this->currentPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }


  /**
  * This creates a paragraph style in the html file's internal css.
  * $name: the name of the style, e.g. 'p'.
  * $dropcaps: Not used. The variable is kept here so as to make it consistant with the OpenDocument routines.
  */
  public function createParagraphStyle ($name, $fontsize, $italic, $bold, $underline, $smallcaps, $alignment, $spacebefore, $spaceafter, $leftmargin, $rightmargin, $firstlineindent, $keepWithNext, $dropcaps)
  {
    // Start with the class. Notice the dot.
    $css = ".$name { ";
 
    // Font size given in points.
    $fontsize .= "pt";
    $css .= "font-size: $fontsize; ";

    // Italics, bold, underline, small caps can be either ooitOff or ooitOn for a paragraph.
    if ($italic != ooitOff) {
      $css .= "font-style: italic; ";
    }
    if ($bold != ooitOff) {
      $css .= "font-weight: bold; ";
    }
    if ($underline != ooitOff) {
      $css .= "text-decoration: underline; ";
    }
    if ($smallcaps != ooitOff) { 
      $css .= "font-variant: small-caps; ";
    }

    // Text alignment can be: AlignmentLeft, AlignmentCenter, AlignmentRight, AlignmentJustify.
    switch ($alignment) {
      case AlignmentLeft:    $alignment = "left"; break;
      case AlignmentCenter:  $alignment = "center"; break;
      case AlignmentRight:   $alignment = "right"; break;
      case AlignmentJustify: $alignment = "justify"; break;
    }
    $css .= "text-align: $alignment; ";

    // Paragraph measurements; given in mm.
    $spacebefore .= "mm";
    $css .= "margin-top: $spacebefore; ";
    $spaceafter .= "mm";
    $css .= "margin-bottom: $spaceafter; ";
    $leftmargin .= "mm";
    $css .= "margin-left: $leftmargin; ";
    $rightmargin .= "mm";
    $css .= "margin-right: $rightmargin; ";
    $firstlineindent .= "mm";
    $css .= "text-indent: $firstlineindent; ";

    if ($keepWithNext) {
      $css .= "page-break-inside: avoid; ";
      // Not yet implemented: "keep-with-next", "always";
    }

    $css .= "}\n";
    $this->styleDomNode->nodeValue .= $css;
  }


  /**
  * This opens a text style.
  * $style: the array containing the style variables.
  * $note: Whether this refers to notes.
  */
  public function openTextStyle ($style, $note = false)
  {
    $marker = $style["marker"];
    if (!in_array ($marker, $this->createdStyles)) {
      @$italic = $style["italic"];
      @$bold = $style["bold"];
      @$underline = $style["underline"];
      @$smallcaps = $style["smallcaps"];
      @$superscript = $style["superscript"];
      @$color = $style["color"];
      $this->createdStyles [] = $marker;

      // Start with the class. Notice the dot.
      $css = ".$marker { ";
 
      // Italics, bold, underline, small caps can be ooitOff or ooitOn or ooitInherit or ooitToggle.
      // Not all features are implemented.
      if (($italic == ooitOn) || ($italic == ooitToggle)) {
        $css .= "font-style: italic; ";
      }
      if (($bold == ooitOn) || ($bold == ooitToggle)) {
        $css .= "font-weight: bold; ";
      }
      if (($underline == ooitOn) || ($underline == ooitToggle)) {
        $css .= "text-decoration: underline; ";
      }
      if (($smallcaps == ooitOn) || ($smallcaps == ooitToggle)) { 
        $css .= "font-variant: small-caps; ";
      }

      if ($superscript) {
        $css .= "font-size: x-small; ";
        $css .= "vertical-align: super; ";
      }

      if ($color != "000000") {
        $css .= "color: #$color; ";
      }

      $css .= "}\n";
      $this->styleDomNode->nodeValue .= $css;

    }

    if ($note) $this->currentNoteTextStyle = $marker;
    else $this->currentTextStyle = $marker;
  }



  /**
  * This closes any open text style.
  * $note: Whether this refers to notes.
  */
  public function closeTextStyle ($note = false)
  {
    if ($note) $this->currentNoteTextStyle = "";
    else $this->currentTextStyle = "";
  }
  
  
  
  /**
  * This function adds a note to the current paragraph.
  * $citation: The text of the note citation.
  * $style: Style name for the paragraph in the note body.
  * $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
  */
  public function addNote ($citation, $style, $endnote = false)
  {
    // Ensure that a paragraph is open, so that the note can be added to it.
    if (!isset ($this->currentPDomElement)) {
      $this->newParagraph ();
    }
    
    $this->noteCount++;

    // Add the link with all relevant data for the note citation.
    $reference = "#note" . $this->noteCount;
    $identifier = "citation" . $this->noteCount;
    $title = gettext ("See footnote");
    $this->addLink ($this->currentPDomElement, $reference, $identifier, $title, "superscript", $citation);

    // Open a paragraph element for the note body.
    $this->notePDomElement = $this->htmlDom->createElement ("p");
    $this->notesDivDomNode->appendChild ($this->notePDomElement);
    $this->notePDomElement->setAttribute ("class", $style);

    $this->closeTextStyle (true);

    // Add the link with all relevant data for the note body.
    $reference = "#citation" . $this->noteCount;
    $identifier = "note" . $this->noteCount;
    $title = gettext ("Return to text");
    $this->addLink ($this->notePDomElement, $reference, $identifier, $title, "", $citation);

    // Add a space.
    $this->addNoteText (" ");
  }



  /**
  * This function adds text to the current footnote.
  * $text: The text to add.
  */
  public function addNoteText ($text)
  {
    if ($text != "") {
      if (!isset ($this->notePDomElement)) {
        $this->addNote ("?", "");
      }
      $spanDomElement = $this->htmlDom->createElement ("span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->notePDomElement->appendChild ($spanDomElement);
      if ($this->currentNoteTextStyle != "") {
        // Take character style as specified in this object.
        $spanDomElement->setAttribute ("class", $this->currentNoteTextStyle);
      }
    }
  }



  /**
  * This function closes the current footnote.
  */
  public function closeCurrentNote ()
  {
    $this->closeTextStyle (true);
    $this->notePDomElement = NULL;
  }



 /**
  * This adds a link.
  * $domNode: The DOM node where to add the link to.
  * $reference: The link's href, e.g. where it links to.
  * $identifier: The link's identifier. Others can link to it.
  * $title: The link's title, to make it accessible, e.g. for screenreaders.
  * $style: The link text's style.
  * $text: The link's text.
  */
  public function addLink ($domNode, $reference, $identifier, $title, $style, $text)
  {
    $aDomElement = $this->htmlDom->createElement ("a");
    $domNode->appendChild ($aDomElement);
    $aDomElement->setAttribute ("href", $reference);
    $aDomElement->setAttribute ("id", $identifier);
    $aDomElement->setAttribute ("title", $title);
    if ($style != "") $aDomElement->setAttribute ("class", $style);
    $aDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
  }



  /**
  * This saves the web page to file
  * $name: the name of the file to save to.
  */
  public function save ($name)
  {
    // Add possible notes.
    if (isset ($this->notesDivDomNode)) {
      $this->bodyDomNode->appendChild ($this->notesDivDomNode);
      unset ($this->notesDivDomNode);
    }
    // Save.
    $this->htmlDom->formatOutput = true;
    $string = $this->htmlDom->saveHTMLFile ($name);
  }


  /**
  * This creates a heading with styled content.
  * $style: A style name.
  * $text: Content.
  */
  private function newNamedHeading ($style, $text, $hide = false)
  {
    $textHDomElement = $this->htmlDom->createElement ($style);
    $this->bodyDomNode->appendChild ($textHDomElement);
    $textHDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
    // Make paragraph null, so that adding subsequent text creates a new paragraph.
    unset ($this->currentPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }

  
}


?>
