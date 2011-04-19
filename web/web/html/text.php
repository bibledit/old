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
class Html_Text // Todo some variables are not needed.
{
  private $htmlDom;
  private $headDomNode;
  private $bodyDomNode;

  private $stylesDom; // The styles.xml DOMDocument.
  private $createdStyles; // An array with styles already created in the $stylesDom.
  private $officeStylesDomNode; // The office:styles DOMNode.
  private $officeAutomaticStylesDomNode; // The office:automatic-styles DOMNode. 

  private $currentTextPDomElement; // The current text:p DOMElement.
  private $currentTextPDomElementNameNode; // The DOMAttr of the name of the style of the current text:p element.
  public $currentParagraphStyle;
  public $currentParagraphContent;
  public $currentTextStyle;
  
  private $frameCount;
  
  private $noteCount;
  private $noteTextPDomElement; // The text:p DOMElement of the current footnote, if any.
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

    $this->createdStyles = array ();
  }

  
  public function newParagraph ($style = "")
  {
    $this->currentTextPDomElement = $this->htmlDom->createElement ("p");
    if ($style != "") {
      $this->currentTextPDomElementNameNode = $this->currentTextPDomElement->setAttribute ("class", $style);
    }
    $this->bodyDomNode->appendChild ($this->currentTextPDomElement);
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
      if (!isset ($this->currentTextPDomElement)) {
        $this->newParagraph ();
      }
      $spanDomElement = $this->htmlDom->createElement ("span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->currentTextPDomElement->appendChild ($spanDomElement);
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
  * This creates a paragraph style.
  * $name: the name of the style, e.g. 'p'.
  * $dropcaps: If 0, there are no drop caps. 
  *            If greater than 0, it the number of characters in drop caps style.
  */
  public function createParagraphStyle ($name, $fontsize, $italic, $bold, $underline, $smallcaps, $alignment, $spacebefore, $spaceafter, $leftmargin, $rightmargin, $firstlineindent, $keepWithNext, $dropcaps) // Todo
  {
    // It looks like this in styles.xml:
    // <style:style style:display-name="p_c1" style:family="paragraph" style:name="p_c1">
    //   <style:paragraph-properties fo:margin-bottom="0mm" fo:margin-left="0mm" fo:margin-right="0mm" fo:margin-top="0mm" fo:text-align="justify" fo:text-indent="0mm"/>
    //     <style:drop-cap style:distance="0.15cm" style:length="1" style:lines="2"/>
    //   <style:paragraph-properties>
    //   <style:text-properties fo:font-size="12pt" style:font-size-asian="12pt" style:font-size-complex="12pt"/>
    // </style:style>
    $styleDomElement = $this->stylesDom->createElement ("style:style");
    $styleDomElement->setAttribute ("style:name", $name);
    $styleDomElement->setAttribute ("style:display-name", $name);
    $styleDomElement->setAttribute ("style:family", "paragraph");
    $this->officeStylesDomNode->appendChild ($styleDomElement);

    $styleParagraphPropertiesDomElement = $this->stylesDom->createElement ("style:paragraph-properties");
    $styleDomElement->appendChild ($styleParagraphPropertiesDomElement);

    $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
    $styleDomElement->appendChild ($styleTextPropertiesDomElement);

    $fontsize .= "pt";
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size", $fontsize);
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", $fontsize);
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", $fontsize);

    // Italics, bold, underline, small caps can be either ooitOff or ooitOn for a paragraph.
    if ($italic != ooitOff) {
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style", "italic");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style-asian", "italic");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style-complex", "italic");
    }
    if ($bold != ooitOff) {
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight", "bold");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-asian", "bold");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-complex", "bold");
    }
    if ($underline != ooitOff) {
      $styleTextPropertiesDomElement->setAttribute ("style:text-underline-style", "solid");
      $styleTextPropertiesDomElement->setAttribute ("style:text-underline-width", "auto");
      $styleTextPropertiesDomElement->setAttribute ("style:text-underline-color", "font-color");
    }
    if ($smallcaps != ooitOff) { 
      $styleTextPropertiesDomElement->setAttribute ("fo:font-variant", "small-caps");
    }

    // Text alignment can be: AlignmentLeft, AlignmentCenter, AlignmentRight, AlignmentJustify.
    switch ($alignment) {
      case AlignmentLeft:    $alignment = "start"; break;
      case AlignmentCenter:  $alignment = "center"; break;
      case AlignmentRight:   $alignment = "end"; break;
      case AlignmentJustify: $alignment = "justify"; break;
    }
    $styleParagraphPropertiesDomElement->setAttribute ("fo:text-align", $alignment);
    $styleParagraphPropertiesDomElement->setAttribute ("style:justify-single-word", "false");

    // Paragraph measurements; given in mm.
    $spacebefore .= "mm";
    $styleParagraphPropertiesDomElement->setAttribute ("fo:margin-top", $spacebefore);
    $spaceafter .= "mm";
    $styleParagraphPropertiesDomElement->setAttribute ("fo:margin-bottom", $spaceafter);
    $leftmargin .= "mm";
    $styleParagraphPropertiesDomElement->setAttribute ("fo:margin-left", $leftmargin);
    $rightmargin .= "mm";
    $styleParagraphPropertiesDomElement->setAttribute ("fo:margin-right", $rightmargin);
    $firstlineindent .= "mm";
    $styleParagraphPropertiesDomElement->setAttribute ("fo:text-indent", $firstlineindent);

    if ($keepWithNext) {
      $styleParagraphPropertiesDomElement->setAttribute ("fo:keep-together", "always");
      $styleParagraphPropertiesDomElement->setAttribute ("fo:keep-with-next", "always");
    }
    
    if ($dropcaps > 0) {
      // E.g.: <style:drop-cap style:lines="2" style:length="2" style:distance="0.15cm"/>
      $styleDropCapDomElement = $this->stylesDom->createElement ("style:drop-cap");
      $styleParagraphPropertiesDomElement->appendChild ($styleDropCapDomElement);
      $styleDropCapDomElement->setAttribute ("style:lines", 2);
      $styleDropCapDomElement->setAttribute ("style:length", $dropcaps);
      $styleDropCapDomElement->setAttribute ("style:distance", "0.15cm");
    }

  }


  /**
  * This updates the style name of the current paragraph.
  * $name: the name of the style, e.g. 'p'.
  */
  public function updateCurrentParagraphStyle ($name) // Todo
  {
    if (!isset ($this->currentTextPDomElement)) {
      $this->newParagraph ();
    }
    $this->currentTextPDomElement->removeAttributeNode ($this->currentTextPDomElementNameNode);
    $this->currentTextPDomElementNameNode = $this->currentTextPDomElement->setAttribute ("text:style-name", $name);
  }


  /**
  * This opens a text style.
  * $style: the array containing the style variables.
  * $note: Whether this refers to notes.
  */
  public function openTextStyle ($style, $note = false) // Todo
  {
    $marker = $style["marker"];
    if (!in_array ($marker, $this->createdStyles)) {
      $italic = $style["italic"];
      $bold = $style["bold"];
      $underline = $style["underline"];
      $smallcaps = $style["smallcaps"];
      $superscript = $style["superscript"];
      $color = $style["color"];
      $this->createdStyles [] = $marker;

      // The style entry looks like this in styles.xml, e.g., for italic:
      // <style:style style:name="T1" style:family="text">
      // <style:text-properties fo:font-style="italic" style:font-style-asian="italic" style:font-style-complex="italic"/>
      // </style:style>
      $styleDomElement = $this->stylesDom->createElement ("style:style");
      $styleDomElement->setAttribute ("style:name", $marker);
      $styleDomElement->setAttribute ("style:display-name", $marker);
      $styleDomElement->setAttribute ("style:family", "text");
      $this->officeStylesDomNode->appendChild ($styleDomElement);

      $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
      $styleDomElement->appendChild ($styleTextPropertiesDomElement);

      // Italics, bold, underline, small caps can be ooitOff or ooitOn or ooitInherit or ooitToggle.
      // Not all features are implemented.
      if (($italic == ooitOn) || ($italic == ooitToggle)) {
        $styleTextPropertiesDomElement->setAttribute ("fo:font-style", "italic");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-style-asian", "italic");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-style-complex", "italic");
      }
      if (($bold == ooitOn) || ($bold == ooitToggle)) {
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight", "bold");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-asian", "bold");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-complex", "bold");
      }
      if (($underline == ooitOn) || ($underline == ooitToggle)) {
        $styleTextPropertiesDomElement->setAttribute ("style:text-underline-style", "solid");
        $styleTextPropertiesDomElement->setAttribute ("style:text-underline-width", "auto");
        $styleTextPropertiesDomElement->setAttribute ("style:text-underline-color", "font-color");
      }
      if (($smallcaps == ooitOn) || ($smallcaps == ooitToggle)) { 
        $styleTextPropertiesDomElement->setAttribute ("fo:font-variant", "small-caps");
      }
      
      if ($superscript) {
        //$styleTextPropertiesDomElement->setAttribute ("style:text-position", "super 58%");
        // If the percentage is not specified, an appropriate font height is used.
        $styleTextPropertiesDomElement->setAttribute ("style:text-position", "super");
        // The mere setting of the superscript value makes the font smaller. No need to set it manually.
        //$styleTextPropertiesDomElement->setAttribute ("fo:font-size", "87%");
        //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "87%");
        //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "87%");
      }
      
      if ($color != "000000") {
        $styleTextPropertiesDomElement->setAttribute ("fo:color", "#$color");
      }

    }

    if ($note) $this->currentNoteTextStyle = $marker;
    else $this->currentTextStyle = $marker;
  }



  /**
  * This closes any open text style.
  * $note: Whether this refers to notes.
  */
  public function closeTextStyle ($note = false) // Todo
  {
    if ($note) $this->currentNoteTextStyle = "";
    else $this->currentTextStyle = "";
  }
  
  
  
  /**
  * This places text in a frame in OpenDocument.
  * It does all the housekeeping to get it display properly.
  * $text - the text to place in the frame.
  * $style - the name of the style of the $text.
  * $fontsize - given in points.
  * $italic, $bold - boolean values.
  */
  public function placeTextInFrame ($text, $style, $fontsize, $italic, $bold) // Todo
  {
    // Empty text is discarded.
    if ($text == "") return;

    // The frame goes in an existing paragraph (text:p) element, just like a 'text:span' element.
    // Ensure that a paragraph is open.
    if (!isset ($this->currentTextPDomElement)) {
      $this->newParagraph ();
    }

    // The frame looks like this, in content.xml:
    // <draw:frame draw:style-name="fr1" draw:name="frame1" text:anchor-type="paragraph" svg:y="0cm" fo:min-width="0.34cm" draw:z-index="0">
    //   <draw:text-box fo:min-height="0.34cm">
    //     <text:p text:style-name="c">1</text:p>
    //   </draw:text-box>
    // </draw:frame>
    $drawFrameDomElement = $this->htmlDom->createElement ("draw:frame");
    $this->currentTextPDomElement->appendChild ($drawFrameDomElement);
    $drawFrameDomElement->setAttribute ("draw:style-name", "chapterframe");
    $this->frameCount++;
    $drawFrameDomElement->setAttribute ("draw:name", "frame" . $this->frameCount);
    $drawFrameDomElement->setAttribute ("text:anchor-type", "paragraph");
    $drawFrameDomElement->setAttribute ("svg:y", "0cm");
    $drawFrameDomElement->setAttribute ("fo:min-width", "0.34cm");
    $drawFrameDomElement->setAttribute ("draw:z-index", "0");

    $drawTextBoxDomElement = $this->htmlDom->createElement ("draw:text-box");
    $drawFrameDomElement->appendChild ($drawTextBoxDomElement);
    $drawTextBoxDomElement->setAttribute ("fo:min-height", "0.34cm");

    $textPDomElement = $this->htmlDom->createElement ("text:p");
    $drawTextBoxDomElement->appendChild ($textPDomElement);
    $textPDomElement->setAttribute ("text:style-name", $style);
    $textPDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
    
    // File styles.xml contains the appropriate styles for this frame and text box and paragraph.
    // Create the styles once for the whole document.
    if (!in_array ($style, $this->createdStyles)) {

      // The style for the text:p element looks like this:
      // <style:style style:name="c" style:family="paragraph">
      //   <style:paragraph-properties fo:text-align="justify" style:justify-single-word="false"/>
      //   <style:text-properties fo:font-size="24pt" fo:font-weight="bold" style:font-size-asian="24pt" style:font-weight-asian="bold" style:font-size-complex="24pt" style:font-weight-complex="bold"/>
      // </style:style>
      $styleDomElement = $this->stylesDom->createElement ("style:style");
      $this->officeStylesDomNode->appendChild ($styleDomElement);
      $styleDomElement->setAttribute ("style:name", $style);
      $styleDomElement->setAttribute ("style:family", "paragraph");

      $styleParagraphPropertiesDomElement = $this->stylesDom->createElement ("style:paragraph-properties");
      $styleDomElement->appendChild ($styleParagraphPropertiesDomElement);
      $styleParagraphPropertiesDomElement->setAttribute ("fo:text-align", "justify");
      $styleParagraphPropertiesDomElement->setAttribute ("style:justify-single-word", "false");

      $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
      $styleDomElement->appendChild ($styleTextPropertiesDomElement);
      $fontsize .= "pt";
      $styleTextPropertiesDomElement->setAttribute ("fo:font-size", $fontsize);
      $styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", $fontsize);
      $styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", $fontsize);
      if ($italic != ooitOff) {
        $styleTextPropertiesDomElement->setAttribute ("fo:font-style", "italic");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-style-asian", "italic");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-style-complex", "italic");
      }
      if ($bold != ooitOff) {
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight", "bold");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-asian", "bold");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-complex", "bold");
      }

      // The style for the draw:frame element looks like this:
      // <style:style style:name="chapterframe" style:family="graphic" style:parent-style-name="ChapterFrameParent">
      //   <style:graphic-properties fo:margin-left="0cm" fo:margin-right="0.199cm" fo:margin-top="0cm" fo:margin-bottom="0cm" style:vertical-pos="from-top" style:vertical-rel="paragraph-content" style:horizontal-pos="left" style:horizontal-rel="paragraph" fo:background-color="transparent" style:background-transparency="100%" fo:padding="0cm" fo:border="none" style:shadow="none" style:flow-with-text="true">
      //   <style:background-image/>
      //   </style:graphic-properties>
      // </style:style>
      $styleDomElement = $this->stylesDom->createElement ("style:style");
      $this->officeStylesDomNode->appendChild ($styleDomElement);
      $styleDomElement->setAttribute ("style:name", "chapterframe");
      $styleDomElement->setAttribute ("style:family", "graphic");

      $styleGraphicPropertiesDomElement = $this->stylesDom->createElement ("style:graphic-properties");
      $styleDomElement->appendChild ($styleGraphicPropertiesDomElement);
      $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-left", "0cm");
      $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-right", "0.2cm");
      $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-top", "0cm");
      $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-bottom", "0cm");
      $styleGraphicPropertiesDomElement->setAttribute ("style:vertical-pos", "from-top");
      $styleGraphicPropertiesDomElement->setAttribute ("style:vertical-rel", "paragraph-content");
      $styleGraphicPropertiesDomElement->setAttribute ("style:horizontal-pos", "left");
      $styleGraphicPropertiesDomElement->setAttribute ("style:horizontal-rel", "paragraph");
      $styleGraphicPropertiesDomElement->setAttribute ("fo:background-color", "transparent");
      $styleGraphicPropertiesDomElement->setAttribute ("style:background-transparency", "100%");
      $styleGraphicPropertiesDomElement->setAttribute ("fo:padding", "0cm");
      $styleGraphicPropertiesDomElement->setAttribute ("fo:border", "none");
      $styleGraphicPropertiesDomElement->setAttribute ("style:shadow", "none");
      $styleGraphicPropertiesDomElement->setAttribute ("style:flow-with-text", "true");
    }

  }
  

  /**
  * This creates the superscript style.
  */
  public function createSuperscriptStyle () // Todo
  {
    // The style entry looks like this in styles.xml:
    // <style:style style:name="superscript" style:family="text">
    //   <style:text-properties style:text-position="super 58%"/>
    // </style:style>
    $styleDomElement = $this->stylesDom->createElement ("style:style");
    $styleDomElement->setAttribute ("style:name", "superscript");
    $styleDomElement->setAttribute ("style:family", "text");
    $this->officeStylesDomNode->appendChild ($styleDomElement);

    $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
    $styleDomElement->appendChild ($styleTextPropertiesDomElement);
    //$styleTextPropertiesDomElement->setAttribute ("style:text-position", "super 58%");
    // If the percentage is not specified, an appropriate font height is used.
    $styleTextPropertiesDomElement->setAttribute ("style:text-position", "super");
    // The mere setting of the superscript value makes the font smaller. No need to set it manually.
    //$styleTextPropertiesDomElement->setAttribute ("fo:font-size", "87%");
    //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "87%");
    //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "87%");
  }


  /**
  * This function adds a note to the current paragraph.
  * $caller: The text of the note caller, that is, the note citation.
  * $style: Style name for the paragraph in the footnote body.
  * $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
  */
  public function addNote ($caller, $style, $endnote = false) // Todo
  {
    // Ensure that a paragraph is open, so that the note can be added to it.
    if (!isset ($this->currentTextPDomElement)) {
      $this->newParagraph ();
    }

    $textNoteDomElement = $this->htmlDom->createElement ("text:note");
    $this->currentTextPDomElement->appendChild ($textNoteDomElement);
    $textNoteDomElement->setAttribute ("text:id", "ftn" . $this->noteCount);
    $this->noteCount++;
    if ($endnote) $noteclass = "endnote";
    else $noteclass = "footnote";
    $textNoteDomElement->setAttribute ("text:note-class", $noteclass);
    
    // The note citation, the 'caller' is normally in superscript in the OpenDocument.
    // The default values of the application are used. The Bibledit-Web stylesheet is not consulted.
    $textNoteCitationDomElement = $this->htmlDom->createElement ("text:note-citation");
    $textNoteDomElement->appendChild ($textNoteCitationDomElement);
    $textNoteCitationDomElement->setAttribute ("text:label", htmlspecialchars ($caller, ENT_QUOTES, "UTF-8"));
    $textNoteCitationDomElement->nodeValue = htmlspecialchars ($caller, ENT_QUOTES, "UTF-8");
    
    $textNoteBodyDomElement = $this->htmlDom->createElement ("text:note-body");
    $textNoteDomElement->appendChild ($textNoteBodyDomElement);
    
    $this->noteTextPDomElement = $this->htmlDom->createElement ("text:p");
    $textNoteBodyDomElement->appendChild ($this->noteTextPDomElement);
    $this->noteTextPDomElement->setAttribute ("text:style-name", $style);

    $this->closeTextStyle (true);
  }



  /**
  * This function adds text to the current footnote.
  * $text: The text to add.
  */
  public function addNoteText ($text) // Todo
  {
    if ($text != "") {
      if (!isset ($this->noteTextPDomElement)) {
        $this->addNote ("?", "");
      }
      $spanDomElement = $this->htmlDom->createElement ("text:span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->noteTextPDomElement->appendChild ($spanDomElement);
      if ($this->currentNoteTextStyle != "") {
        // Take character style as specified in this object.
        $spanDomElement->setAttribute ("text:style-name", $this->currentNoteTextStyle);
      }
    }
  }



  /**
  * This function closes the current footnote.
  */
  public function closeCurrentNote () // Todo
  {
    $this->closeTextStyle (true);
    $this->noteTextPDomElement = NULL;
  }



  /**
  * This saves the OpenDocument to file
  * $name: the name of the file to save to.
  */
  public function save ($name)
  {
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
    unset ($this->currentTextPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }

  
}


?>
