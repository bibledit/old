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
* Class for creating OpenDocument text documents.
* At first the ODF Toolkit was used. But the Java code to generate this became too big for the compiler.
* The other thing is that Java is slow as compared to this method employed here.
*/
class Odf_Text
{
  private $unpackedOdtFolder;

  private $contentDom; // The content.xml DOMDocument.
  private $officeTextDomNode; // The office:text DOMNode.
  private $currentTextPDomElement; // The current text:p DOMElement.
  private $currentTextPDomElementNameNode; // The DOMAttr of the name of the style of the current text:p element.

  private $stylesDom; // The styles.xml DOMDocument.
  private $createdStyles; // An array with styles already created in the $stylesDom.
  private $officeStylesDomNode; // The office:styles DOMNode.

  private $paragraphOpened;
  public $currentParagraphStyle;
  public $currentParagraphContent;
  
  
  public function __construct () 
  {
    $this->paragraphOpened = false;
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";

    $template = dirname (__FILE__) . "/template.odt";
    $this->unpackedOdtFolder = Filter_Archive::unzip ($template, false);
    Filter_Rmdir::rmdir ($this->unpackedOdtFolder . "/Configurations2");

    $this->contentDom = new DOMDocument;
    $this->contentDom->load($this->unpackedOdtFolder . "/content.xml");

    // The office:document-content DOM.
    $officeDocumentContent = $this->contentDom->childNodes->item (0);

    // The office:body DOM.
    $officeBody = NULL;
    foreach ($officeDocumentContent->childNodes as $node) {
      if ($node->nodeName == "office:body") $officeBody = $node;
    }

    // Get the office:text DOM.
    $this->officeTextDomNode = NULL;
    foreach ($officeBody->childNodes as $node) {
      if ($node->nodeName == "office:text") $this->officeTextDomNode = $node;
    }
    
    // Remove the default content from the template. 
    // This is: <text:p text:style-name="Standard"/>
    foreach ($this->officeTextDomNode->childNodes as $node) {
      if ($node->nodeName == "text:p") {
        $this->officeTextDomNode->removeChild ($node);
        break;
      }
    }
    
    $this->createdStyles = array ();

    $this->stylesDom = new DOMDocument;
    $this->stylesDom->load($this->unpackedOdtFolder . "/styles.xml");
    
    // The office:document-styles DOM.
    $officeDocumentStyles = $this->stylesDom->childNodes->item (0);

    // The office:styles DOMNode.
    foreach ($officeDocumentStyles->childNodes as $node) {
      if ($node->nodeName == "office:styles") $this->officeStylesDomNode = $node;
    }

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
  * $style: The style for the text. 
  * The $style can be omitted in which case no style is applied.
  */
  public function addText ($text, $style = "")
  {
    if ($text != "") {
      if (!isset ($this->currentTextPDomElement)) {
        $this->newParagraph ();
      }
      $textSpanDomElement = $this->contentDom->createElement ("text:span");
      $textSpanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->currentTextPDomElement->appendChild ($textSpanDomElement);
      if ($style != "") {
        $styleDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
      }
      $this->currentParagraphContent .= $text;
    }
  }

  /**
  * This creates a heading with contents styled "Heading 1".
  * $text: Contents.
  */
  public function newHeading1($text)
  {
    $this->newNamedHeading ("Heading 1", $text);
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
    $zippedfile = Filter_Archive::zipFolder ($this->unpackedOdtFolder, false);
    file_put_contents ($name, file_get_contents ($zippedfile));
  }


  /**
  * This creates a heading with styled content.
  * $style: A style name.
  * $text: Content.
  */
  private function newNamedHeading ($style, $text)
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
