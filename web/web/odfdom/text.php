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


class Odfdom_Text
{
  public $javaCode;
  private $paragraphOpened;
  public $currentParagraphStyle;
  public $currentParagraphContent;
  
  public function __construct () 
  {
    $this->javaCode = array ();
    $this->paragraphOpened = false;
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";

$code = <<<EOD
// Imports.
import java.net.URI;

import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import org.odftoolkit.odfdom.doc.OdfTextDocument;
import org.odftoolkit.odfdom.dom.element.OdfStyleBase;
import org.odftoolkit.odfdom.dom.element.office.OfficeTextElement;
import org.odftoolkit.odfdom.dom.element.style.StyleParagraphPropertiesElement;
import org.odftoolkit.odfdom.dom.element.style.StyleTabStopElement;
import org.odftoolkit.odfdom.dom.element.style.StyleTabStopsElement;
import org.odftoolkit.odfdom.dom.element.style.StyleTextPropertiesElement;
import org.odftoolkit.odfdom.dom.element.style.StyleDropCapElement;
import org.odftoolkit.odfdom.dom.style.OdfStyleFamily;
import org.odftoolkit.odfdom.incubator.doc.office.OdfOfficeAutomaticStyles;
import org.odftoolkit.odfdom.incubator.doc.office.OdfOfficeStyles;
import org.odftoolkit.odfdom.incubator.doc.style.OdfDefaultStyle;
import org.odftoolkit.odfdom.incubator.doc.style.OdfStyle;
import org.odftoolkit.odfdom.incubator.doc.text.OdfTextHeading;
import org.odftoolkit.odfdom.incubator.doc.text.OdfTextParagraph;
import org.odftoolkit.odfdom.incubator.doc.text.OdfTextSpan;
import org.odftoolkit.odfdom.pkg.OdfFileDom;

// The class.
class odt {

  static void setFontWeight(OdfStyleBase style, String value)
  {
    style.setProperty(StyleTextPropertiesElement.FontWeight, value);
    style.setProperty(StyleTextPropertiesElement.FontWeightAsian, value);
    style.setProperty(StyleTextPropertiesElement.FontWeightComplex, value);
  }
  
  static void setFontStyle(OdfStyleBase style, String value)
  {
    style.setProperty(StyleTextPropertiesElement.FontStyle, value);
    style.setProperty(StyleTextPropertiesElement.FontStyleAsian, value);
    style.setProperty(StyleTextPropertiesElement.FontStyleComplex, value);
  }
  
  static void setFontSize(OdfStyleBase style, String value)
  {
    style.setProperty(StyleTextPropertiesElement.FontSize, value);
    style.setProperty(StyleTextPropertiesElement.FontSizeAsian, value);
    style.setProperty(StyleTextPropertiesElement.FontSizeComplex, value);
  }

  static void setTextUnderline(OdfStyleBase style)
  {
    style.setProperty(StyleTextPropertiesElement.TextUnderlineStyle, "solid");
    style.setProperty(StyleTextPropertiesElement.TextUnderlineWidth, "auto");
    style.setProperty(StyleTextPropertiesElement.TextUnderlineColor, "font-color");
  }
  
  public static void main(String[] args) throws Exception
  {

    OdfTextDocument document; // The document to build.
    OdfFileDom contentDom; // The document object model for content.xml
    OdfFileDom stylesDom; // The document object model for styles.xml

    // The office:automatic-styles element in content.xml.
    OdfOfficeAutomaticStyles contentAutoStyles;
    
    // The office:styles element in styles.xml
    OdfOfficeStyles stylesOfficeStyles;

    // The office:text element in the content.xml file
    OfficeTextElement officeText;
    
    // The paragraph being built in the content.xml file.
    OdfTextParagraph paragraph;

    document = OdfTextDocument.newTextDocument();
    contentDom = document.getContentDom();
    stylesDom = document.getStylesDom();
    contentAutoStyles = contentDom.getOrCreateAutomaticStyles();
    stylesOfficeStyles = document.getOrCreateDocumentStyles();
    officeText = document.getContentRoot();
    paragraph = null;

    // The templates included in the ODFDOM toolkit have content in them;
    // a newly-created text document has a paragraph that contains no text:
    // <text:p>.
    // Get rid of this paragraph, by repeatedly removing the first child of the officeText node
    // until there are no more.
    {
      Node childNode;
      childNode = officeText.getFirstChild();
      while (childNode != null)
      {
        officeText.removeChild(childNode);
        childNode = officeText.getFirstChild();
      }
    }
    
    // Default font size is 12 points.
    OdfDefaultStyle defaultStyle;
    defaultStyle = stylesOfficeStyles.getDefaultStyle(OdfStyleFamily.Paragraph);
    setFontSize(defaultStyle, "12pt");

EOD;
    $this->javaCode = array_merge ($this->javaCode, explode ("\n", $code));
  }
  
  public function newParagraph ($style = "Standard")
  {
    $this->javaCode[] = "";
    $this->javaCode[] = "paragraph = new OdfTextParagraph(contentDom, \"$style\");";
    $this->javaCode[] = "officeText.appendChild(paragraph);";
    $this->paragraphOpened = true;
    $this->currentParagraphStyle = $style;
    $this->currentParagraphContent = "";
  }
  
  public function addText ($text) // Todo
  {
    if ($text != "") {
      if (!$this->paragraphOpened) {
        $this->javaCode[] = "  paragraph = new OdfTextParagraph(contentDom, \"Standard\");";
        $this->javaCode[] = "  officeText.appendChild(paragraph);";
        $this->paragraphOpened = true;
      }
      $text = addslashes ($text);
      $this->javaCode[] = "paragraph.addContent(\"$text\");";
      $this->currentParagraphContent .= $text;
    }
  }

  /**
  * This creates a heading with named contents.
  * $style: An existing style name.
  * $text: Contents.
  */
  private function newNamedHeading ($style, $text)
  {
    // Make paragraph null, so any next subsequent text would create a new paragraph.
    $this->paragraphOpened = false;
    $style = $this->convertStyleName ($style);
    $this->javaCode[] = "";
    $this->javaCode[] = "{";
    $this->javaCode[] = "  OdfTextHeading heading;";
    $this->javaCode[] = "  heading = new OdfTextHeading(contentDom);";
    $this->javaCode[] = "  heading.addStyledContent(\"$style\", \"$text\");";
    $this->javaCode[] = "  officeText.appendChild(heading);";
    $this->javaCode[] = "}";
    $this->javaCode[] = "";
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }
  
  /**
  * This creates a heading with contents styled "Heading 1".
  * $text: Contents.
  */
  public function newHeading1($text)
  {
    $this->newNamedHeading ("Heading 1", $text);
  }
  
  public function finalize ($filename)
  {
    $this->javaCode[] = "";
    $this->javaCode[] = "    document.save(\"$filename\");";
    $this->javaCode[] = "  }";
    $this->javaCode[] = "}";
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
  

  /**
  * This creates the page break style in the automatic styles.
  */
  public function createPageBreakStyle ()
  {
    $this->javaCode[] = "{";
    $this->javaCode[] = "  OdfStyle style;";
    $this->javaCode[] = "  style = stylesOfficeStyles.newStyle(\"Page_20_Break\", OdfStyleFamily.Paragraph);";
    $this->javaCode[] = "  style.setStyleDisplayNameAttribute(\"Page Break\");";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.MarginTop, \"0cm\");";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.MarginBottom, \"0cm\");";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.LineHeight, \"0.05cm\");";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.BreakAfter, \"page\");";
    $this->javaCode[] = "  setFontSize (style, \"2pt\");";
    $this->javaCode[] = "}";
  }


  /**
  * This applies a page break.
  */
  public function newPageBreak ()
  {
    $this->javaCode[] = "";
    $this->javaCode[] = "paragraph = new OdfTextParagraph(contentDom, \"Page_20_Break\");";
    $this->javaCode[] = "officeText.appendChild(paragraph);";
    $this->javaCode[] = "";
    // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
    // since it would be nearly invisible, and thus text would mysteriously get lost.
    $this->paragraphOpened = false;
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
    $this->javaCode[] = "{";

    $stylename = $this->convertStyleName ($name);
    $this->javaCode[] = "  OdfStyle style;";
    $this->javaCode[] = "  style = stylesOfficeStyles.newStyle(\"$stylename\", OdfStyleFamily.Paragraph);";
    $this->javaCode[] = "  style.setStyleDisplayNameAttribute(\"$name\");";

    $fontsize .= "pt";
    $this->javaCode[] = "  setFontSize(style, \"$fontsize\");";

    // Italics, bold, underline, small caps can be either ooitOff or ooitOn for a paragraph.
    if ($italic != ooitOff) {
      $this->javaCode[] = "  setFontStyle(style, \"italic\");";
    }
    if ($bold != ooitOff) {
      $this->javaCode[] = "  setFontWeight(style, \"bold\");";
    }
    if ($underline != ooitOff) {
      $this->javaCode[] = "  setTextUnderline(style);";
    }
    if ($smallcaps != ooitOff) { 
      $this->javaCode[] = "  style.setProperty(StyleTextPropertiesElement.FontVariant, \"small-caps\");";
    }

    // Text alignment can be: AlignmentLeft, AlignmentCenter, AlignmentRight, AlignmentJustify.
    switch ($alignment) {
      case AlignmentLeft:    $alignment = "start"; break;
      case AlignmentCenter:  $alignment = "center"; break;
      case AlignmentRight:   $alignment = "end"; break;
      case AlignmentJustify: $alignment = "justify"; break;
    }
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.TextAlign, \"$alignment\");";

    // Paragraph measurements; given in mm.
    $spacebefore .= "mm";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.MarginTop, \"$spacebefore\");";
    $spaceafter .= "mm";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.MarginBottom, \"$spaceafter\");";
    $leftmargin .= "mm";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.MarginLeft, \"$leftmargin\");";
    $rightmargin .= "mm";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.MarginRight, \"$rightmargin\");";
    $firstlineindent .= "mm";
    $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.TextIndent, \"$firstlineindent\");";

    if ($keepWithNext) {
      $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.KeepTogether, \"always\");";
      $this->javaCode[] = "  style.setProperty(StyleParagraphPropertiesElement.KeepWithNext, \"always\");";
    }
    
    if ($dropcaps > 0) {
      $this->javaCode[] = "  StyleDropCapElement dropcapselement;";
      $this->javaCode[] = "  dropcapselement = new StyleDropCapElement (stylesDom);";
      $this->javaCode[] = "  dropcapselement.setStyleLinesAttribute (2);";
      $this->javaCode[] = "  dropcapselement.setStyleLengthAttribute ($dropcaps);";
      $this->javaCode[] = "  dropcapselement.setStyleDistanceAttribute (\"0.15cm\");";
      $this->javaCode[] = "  StyleParagraphPropertiesElement pProperties;";
      $this->javaCode[] = "  pProperties = new StyleParagraphPropertiesElement(stylesDom);";
      $this->javaCode[] = "  pProperties.appendChild(dropcapselement);";
      $this->javaCode[] = "  style.appendChild(pProperties);";
    }

    $this->javaCode[] = "}";
  }


  /**
  * This updates the style name of the current paragraph.
  * $name: the name of the style, e.g. 'p'.
  */
  public function updateCurrentParagraphStyle ($name)
  {
    if (!$this->paragraphOpened) {
      $this->javaCode[] = "  paragraph = new OdfTextParagraph(contentDom, \"Standard\");";
      $this->javaCode[] = "  officeText.appendChild(paragraph);";
      $this->paragraphOpened = true;
    }
    $this->javaCode[] = "paragraph.addStyledContent(\"$name\", \"\");";
  }



}

/*

Example code is kept here to learn from, as far as it has not yet been put into the object and used there.

void addOfficeStyles()
{
  OdfStyleParagraphProperties pProperties;
  OdfStyleTabStops tabStops;
  OdfStyleTabStop tabStop;

  // Paragraph with tab stop at 7.5cm with a
  // leader of "."
  style = stylesOfficeStyles.newStyle("Cast_20_Para", OdfStyleFamily.Paragraph);
  style.setStyleDisplayNameAttribute("Cast Para");
  style.setStyleFamilyAttribute(OdfStyleFamily.Paragraph.toString());

  // build hierarchy from "inside out"
  tabStop = new OdfStyleTabStop(stylesDom);
  tabStop.setStylePositionAttribute("7.5cm");
  tabStop.setStyleLeaderStyleAttribute("dotted");
  tabStop.setStyleLeaderTextAttribute(".");
  tabStop.setStyleTypeAttribute("right");

  tabStops = new OdfStyleTabStops(stylesDom);
  tabStops.appendChild(tabStop);

  pProperties = new OdfStyleParagraphProperties(stylesDom);
  pProperties.appendChild(tabStops);

  style.appendChild(pProperties);
}


void processTitle(Node movieNode) throws XPathExpressionException
{
  String title;
  String rating;
  OdfTextHeading heading;
  OdfTextSpan stars;
  
  heading = new OdfTextHeading(contentDom);
  heading.addStyledContent("Movie_20_Heading", title + " ");
  
  stars = new OdfTextSpan(contentDom);
  heading.addStyledSpan("Star_20_Span", rating);
  
  officeText.appendChild(heading);
}


void processCast(Node movieNode) throws XPathExpressionException
{
  NodeList actors;
  OdfTextParagraph actor;
  String name;
  String role;

  OdfTextHeading heading;

  heading = new OdfTextHeading(contentDom, "Cast_20_Heading");
  heading.appendChild(contentDom.createTextNode("The Cast"));
  officeText.appendChild(heading);
  
    actor = new OdfTextParagraph(contentDom, "Cast_20_Para");
    actor.addContent(name);
    if (role != null && !role.equals(""))
    {
      actor.addContentWhitespace("\t" + role);
    }
    officeText.appendChild(actor);
  }
}


*/


?>
