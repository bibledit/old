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


/**
* Class for creating OpenDocument text documents.
* At first the ODF Toolkit was used. But the Java code to generate this became too big for the compiler.
* The other thing is that Java is slow as compared to this method employed here.
*/
class Odf_Text
{

  private $bible;
  
  private $unpackedOdtFolder;

  private $contentDom; // The content.xml DOMDocument.
  private $officeTextDomNode; // The office:text DOMNode.

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


  public function __construct ($bible)
  {
    $this->bible = $bible;
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
    $this->currentTextStyle = array ();
    $this->frameCount = 0;
    $this->noteCount = 0;
    $this->currentNoteTextStyle = array ();

    $database_config_general = Database_Config_General::getInstance ();
    $database_config_bible = Database_Config_Bible::getInstance ();

    $template = dirname (__FILE__) . "/template.odt";
    $this->unpackedOdtFolder = Filter_Archive::unzip ($template, false);
    Filter_Rmdir::rmdir ($this->unpackedOdtFolder . "/Configurations2");

    $this->contentDom = new DOMDocument;
    $this->contentDom->load($this->unpackedOdtFolder . "/content.xml");

    $contentXpath = new DOMXpath ($this->contentDom);

    $this->officeTextDomNode = $contentXpath->query ("office:body/office:text")->item (0);

    // Remove the default content from the template. This is a text:p element.
    $node = $contentXpath->query ("text:p", $this->officeTextDomNode)->item (0);
    $this->officeTextDomNode->removeChild ($node);

    $this->createdStyles = array ();

    $this->stylesDom = new DOMDocument;
    $this->stylesDom->load($this->unpackedOdtFolder . "/styles.xml");

    $stylesXpath = new DOMXpath ($this->stylesDom);

    $this->officeStylesDomNode = $stylesXpath->query ("office:styles")->item (0);

    // Set the page size and margins.
    $pageLayoutProperties = $stylesXpath->query ("descendant::style:page-layout-properties")->item (0);
    $pageLayoutProperties->setAttribute ("fo:page-width", $database_config_bible->getPageWidth ($this->bible) . "mm");
    $pageLayoutProperties->setAttribute ("fo:page-height", $database_config_bible->getPageHeight ($this->bible) . "mm");
    $pageLayoutProperties->setAttribute ("fo:margin-left", $database_config_bible->getInnerMargin ($this->bible) . "mm");
    $pageLayoutProperties->setAttribute ("fo:margin-right", $database_config_bible->getOuterMargin ($this->bible) . "mm");
    $pageLayoutProperties->setAttribute ("fo:margin-top", $database_config_bible->getTopMargin ($this->bible) . "mm");
    $pageLayoutProperties->setAttribute ("fo:margin-bottom", $database_config_bible->getBottomMargin ($this->bible) . "mm");

    // Update the tab-stops in the relevant header styles. The tab stops depend on page and margin dimensions.
    $nodeList = $stylesXpath->query ("descendant::style:style[contains(attribute::style:parent-style-name,'Header')]//descendant::style:tab-stop");
    $centerPosition = $database_config_bible->getPageWidth ($this->bible) - $database_config_bible->getInnerMargin ($this->bible) - $database_config_bible->getOuterMargin ($this->bible);
    $centerPosition /= 2;
    $counter = 0;
    foreach ($nodeList as $node) {
      $modulus = $counter % 2;
      $node->setAttribute ("style:position", ($centerPosition * ($modulus + 1)) . "mm");
      $counter++;
    }

    // Remove the date style for the running headers, so that it takes the default style.
    $numberDateStyleNode = $stylesXpath->query ("descendant::number:date-style")->item (0);
    $numberDateStyleNode->parentNode->removeChild ($numberDateStyleNode);

    // Whether and how to put the date in the running headers.
    $nodeList = $stylesXpath->query ("descendant::text:date");
    foreach ($nodeList as $node) {
      if ($database_config_bible->getDateInHeader ($this->bible)) {
        $node->removeAttribute ("text:date-value");
        $node->nodeValue = "";
      } else {
        $node->parentNode->removeChild ($node);
      }
    }

  }


  public function __destruct ()
  {
    Filter_Rmdir::rmdir ($this->unpackedOdtFolder);
  }


  public function newParagraph ($style = "Standard")
  {
    $this->currentTextPDomElement = $this->contentDom->createElement ("text:p");
    $this->currentTextPDomElementNameNode = $this->currentTextPDomElement->setAttribute ("text:style-name", $style);
    $this->officeTextDomNode->appendChild ($this->currentTextPDomElement);
    $this->currentParagraphStyle = $style;
    $this->currentParagraphContent = "";
  }


  /**
  * This function adds text to the current paragraph.
  * $text: The text to add.
  */
  public function addText ($text)
  {
    // Bail out if there's no text.
    if ($text == "") return;

    // Ensure a paragraph has started.
    if (!isset ($this->currentTextPDomElement)) {
      $this->newParagraph ();
    }
    
    // Temporal styles array should have at least one style for the code below to work.
    $styles = $this->currentTextStyle;
    if (empty ($styles)) $styles [] = "";
    
    // Write a text span element, nesting the second and later ones.
    $domElement = $this->currentTextPDomElement;
    foreach ($styles as $style) {
      $textSpanDomElement = $this->contentDom->createElement ("text:span");
      $domElement->appendChild ($textSpanDomElement);
      if ($style) {
        $textSpanDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
      }
      $domElement = $textSpanDomElement;
    }
    $domElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
    
    // Update public paragraph text.
    $this->currentParagraphContent .= $text;
  }


  /**
  * This creates a heading with contents styled "Heading 1".
  * $text: Contents.
  */
  public function newHeading1($text, $hide = false)
  {
    $this->newNamedHeading ("Heading 1", $text, $hide);
  }


  /**
  * This creates the page break style.
  */
  public function createPageBreakStyle ()
  {
    // This is how the style looks in styles.xml:
    // <style:style style:display-name="Page Break" style:family="paragraph" style:name="Page_20_Break">
    // <style:paragraph-properties fo:break-after="page" fo:line-height="0.05cm" fo:margin-bottom="0cm" fo:margin-top="0cm"/>
    // <style:text-properties fo:font-size="2pt" style:font-size-asian="2pt" style:font-size-complex="2pt"/>
    // </style:style>
    $styleDomElement = $this->stylesDom->createElement ("style:style");
    $styleDomElement->setAttribute ("style:name", "Page_20_Break");
    $styleDomElement->setAttribute ("style:display-name", "Page Break");
    $styleDomElement->setAttribute ("style:family", "paragraph");
    $this->officeStylesDomNode->appendChild ($styleDomElement);

    $styleParagraphPropertiesDomElement = $this->stylesDom->createElement ("style:paragraph-properties");
    $styleParagraphPropertiesDomElement->setAttribute ("fo:break-after", "page");
    $styleParagraphPropertiesDomElement->setAttribute ("fo:line-height", "0.05cm");
    $styleParagraphPropertiesDomElement->setAttribute ("fo:margin-bottom", "0cm");
    $styleParagraphPropertiesDomElement->setAttribute ("fo:margin-top", "0cm");
    $styleDomElement->appendChild ($styleParagraphPropertiesDomElement);

    $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size", "2pt");
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "2pt");
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "2pt");
    $styleDomElement->appendChild ($styleTextPropertiesDomElement);
  }


  /**
  * This applies a page break.
  */
  public function newPageBreak ()
  {
    $this->newParagraph ("Page_20_Break");
    // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
    // since it would be nearly invisible, and thus text would mysteriously get lost.
    unset ($this->currentTextPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }


  /**
  * This creates a paragraph style.
  * $name: the name of the style, e.g. 'p'.
  * $dropcaps: If 0, there are no drop caps.
  *            If greater than 0, it the number of characters in drop caps style.
  */
  public function createParagraphStyle ($name, $fontsize, $italic, $bold, $underline, $smallcaps, $alignment, $spacebefore, $spaceafter, $leftmargin, $rightmargin, $firstlineindent, $keepWithNext, $dropcaps)
  {
    // It looks like this in styles.xml:
    // <style:style style:display-name="p_c1" style:family="paragraph" style:name="p_c1">
    //   <style:paragraph-properties fo:margin-bottom="0mm" fo:margin-left="0mm" fo:margin-right="0mm" fo:margin-top="0mm" fo:text-align="justify" fo:text-indent="0mm"/>
    //     <style:drop-cap style:distance="0.15cm" style:length="1" style:lines="2"/>
    //   <style:paragraph-properties>
    //   <style:text-properties fo:font-size="12pt" style:font-size-asian="12pt" style:font-size-complex="12pt"/>
    // </style:style>
    $styleDomElement = $this->stylesDom->createElement ("style:style");
    $styleDomElement->setAttribute ("style:name", $this->convertStyleName ($name));
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
  public function updateCurrentParagraphStyle ($name)
  {
    if (!isset ($this->currentTextPDomElement)) {
      $this->newParagraph ();
    }
    $this->currentTextPDomElement->removeAttributeNode ($this->currentTextPDomElementNameNode);
    $this->currentTextPDomElementNameNode = $this->currentTextPDomElement->setAttribute ("text:style-name", $this->convertStyleName ($name));
  }


  /**
  * This opens a text style.
  * $style: the array containing the style variables.
  * $note: Whether this refers to notes.
  * $embed: boolean: Whether nest $style in an existing character style.
  */
  public function openTextStyle ($style, $note, $embed)
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
      $styleDomElement->setAttribute ("style:name", $this->convertStyleName ($marker));
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

    if ($note) {
      if (!$embed) $this->currentNoteTextStyle = array ();
      $this->currentNoteTextStyle [] = $marker;
    } else {
      if (!$embed) $this->currentTextStyle = array ();
      $this->currentTextStyle [] = $marker;
    }
  }



  /**
  * This closes any open text style.
  * $note: Whether this refers to notes.
  * $embed: boolean: Whether to close embedded style.
  */
  public function closeTextStyle ($note, $embed)
  {
    if ($note) {
      if (!$embed) $this->currentNoteTextStyle = array ();
      if (count ($this->currentNoteTextStyle)) array_pop ($this->currentNoteTextStyle);
    } else {
      if (!$embed) $this->currentTextStyle = array ();
      if (count ($this->currentTextStyle)) array_pop ($this->currentTextStyle);
    }
  }



  /**
  * This places text in a frame in OpenDocument.
  * It does all the housekeeping to get it display properly.
  * $text - the text to place in the frame.
  * $style - the name of the style of the $text.
  * $fontsize - given in points.
  * $italic, $bold - boolean values.
  */
  public function placeTextInFrame ($text, $style, $fontsize, $italic, $bold)
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
    $drawFrameDomElement = $this->contentDom->createElement ("draw:frame");
    $this->currentTextPDomElement->appendChild ($drawFrameDomElement);
    $drawFrameDomElement->setAttribute ("draw:style-name", "chapterframe");
    $this->frameCount++;
    $drawFrameDomElement->setAttribute ("draw:name", "frame" . $this->frameCount);
    $drawFrameDomElement->setAttribute ("text:anchor-type", "paragraph");
    $drawFrameDomElement->setAttribute ("svg:y", "0cm");
    $drawFrameDomElement->setAttribute ("fo:min-width", "0.34cm");
    $drawFrameDomElement->setAttribute ("draw:z-index", "0");

    $drawTextBoxDomElement = $this->contentDom->createElement ("draw:text-box");
    $drawFrameDomElement->appendChild ($drawTextBoxDomElement);
    $drawTextBoxDomElement->setAttribute ("fo:min-height", "0.34cm");

    $textPDomElement = $this->contentDom->createElement ("text:p");
    $drawTextBoxDomElement->appendChild ($textPDomElement);
    $textPDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
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
      $styleDomElement->setAttribute ("style:name", $this->convertStyleName ($style));
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
  public function createSuperscriptStyle ()
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
  public function addNote ($caller, $style, $endnote = false)
  {
    // Ensure that a paragraph is open, so that the note can be added to it.
    if (!isset ($this->currentTextPDomElement)) {
      $this->newParagraph ();
    }

    $textNoteDomElement = $this->contentDom->createElement ("text:note");
    $this->currentTextPDomElement->appendChild ($textNoteDomElement);
    $textNoteDomElement->setAttribute ("text:id", "ftn" . $this->noteCount);
    $this->noteCount++;
    if ($endnote) $noteclass = "endnote";
    else $noteclass = "footnote";
    $textNoteDomElement->setAttribute ("text:note-class", $noteclass);

    // The note citation, the 'caller' is normally in superscript in the OpenDocument.
    // The default values of the application are used. The Bibledit-Web stylesheet is not consulted.
    $textNoteCitationDomElement = $this->contentDom->createElement ("text:note-citation");
    $textNoteDomElement->appendChild ($textNoteCitationDomElement);
    $textNoteCitationDomElement->setAttribute ("text:label", htmlspecialchars ($caller, ENT_QUOTES, "UTF-8"));
    $textNoteCitationDomElement->nodeValue = htmlspecialchars ($caller, ENT_QUOTES, "UTF-8");

    $textNoteBodyDomElement = $this->contentDom->createElement ("text:note-body");
    $textNoteDomElement->appendChild ($textNoteBodyDomElement);

    $this->noteTextPDomElement = $this->contentDom->createElement ("text:p");
    $textNoteBodyDomElement->appendChild ($this->noteTextPDomElement);
    $this->noteTextPDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));

    $this->closeTextStyle (true, false);
  }


  /**
  * This function adds text to the current footnote.
  * $text: The text to add.
  */
  public function addNoteText ($text)
  {
    // Bail out if there's no text.
    if ($text == "") return;

    // Ensure a note has started.
    if (!isset ($this->noteTextPDomElement)) {
      $this->addNote ("?", "");
    }

    // Temporal styles array should have at least one style for the code below to work.
    $styles = $this->currentNoteTextStyle;
    if (empty ($styles)) $styles [] = "";

    // Write a text span element, nesting the second and later ones.
    $domElement = $this->noteTextPDomElement;
    foreach ($styles as $style) {
      $textSpanDomElement = $this->contentDom->createElement ("text:span");
      $domElement->appendChild ($textSpanDomElement);
      if ($style) {
        $textSpanDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
      }
      $domElement = $textSpanDomElement;
    }
    $domElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
  }


  /**
  * This function closes the current footnote.
  */
  public function closeCurrentNote ()
  {
    $this->closeTextStyle (true, false);
    $this->noteTextPDomElement = NULL;
  }


  /**
  * This saves the OpenDocument to file
  * $name: the name of the file to save to.
  */
  public function save ($name)
  {
    // Create the content.xml file.
    // No formatting because some white space is processed. $this->contentDom->formatOutput = true;
    $string = $this->contentDom->save ($this->unpackedOdtFolder . "/content.xml");

    // Create the styles.xml file.
    // No formatting because some white space is processed. $this->stylesDom->formatOutput = true;
    $string = $this->stylesDom->save ($this->unpackedOdtFolder . "/styles.xml");

    // Save the OpenDocument file.
    $zippedfile = Filter_Archive::zip ($this->unpackedOdtFolder);
    file_put_contents ($name, file_get_contents ($zippedfile));
    unlink ($zippedfile);
  }


  /**
  * This creates a heading with styled content.
  * $style: A style name.
  * $text: Content.
  */
  private function newNamedHeading ($style, $text, $hide = false)
  {
    // Heading looks like this in content.xml:
    // <text:h text:style-name="Heading_20_1" text:outline-level="1">Text</text:h>
    $textHDomElement = $this->contentDom->createElement ("text:h");
    $textHDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
    $textHDomElement->setAttribute ("text:outline-level", 1);
    $this->officeTextDomNode->appendChild ($textHDomElement);
    $textHDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");

    // Heading style looks like this in styles.xml:
    // <style:style style:name="Heading_20_1" style:display-name="Heading 1" style:family="paragraph" style:parent-style-name="Heading" style:next-style-name="Text_20_body" style:default-outline-level="1" style:class="text">
    // <style:text-properties fo:font-size="115%" fo:font-weight="bold" style:font-size-asian="115%" style:font-weight-asian="bold" style:font-size-complex="115%" style:font-weight-complex="bold"/>
    // </style:style>
    // Create the style if it does not yet exist.
    if (!in_array ($style, $this->createdStyles)) {
      $styleDomElement = $this->stylesDom->createElement ("style:style");
      $styleDomElement->setAttribute ("style:name", $this->convertStyleName ($style));
      $styleDomElement->setAttribute ("style:display-name", $style);
      $styleDomElement->setAttribute ("style:family", "paragraph");
      $styleDomElement->setAttribute ("style:parent-style-name", "Heading");
      $styleDomElement->setAttribute ("style:next-style-name", "Text_20_body");
      $styleDomElement->setAttribute ("style:default-outline-level", 1);
      $styleDomElement->setAttribute ("style:class", "text");
      $this->officeStylesDomNode->appendChild ($styleDomElement);
      {
        $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-size", "115%");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight", "bold");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "115%");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-asian", "bold");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "115%");
        $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-complex", "bold");
        if ($hide) {
          $styleTextPropertiesDomElement->setAttribute ("text:display", "none");
        }
        $styleDomElement->appendChild ($styleTextPropertiesDomElement);
      }
      $this->createdStyles [] = $style;
    }

    // Make paragraph null, so that adding subsequent text creates a new paragraph.
    unset ($this->currentTextPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }


  /**
  * This converts the name of a style so that it is fit for use in OpenDocument files.
  * E.g. 'Heading 1' becomes 'Heading_20_1'
  * $style: Input
  * It returns the converted style name.
  */
  private function convertStyleName ($style)
  {
    $style = str_replace (" ", "_20_", $style);
    return $style;
  }


}


?>
