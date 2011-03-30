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
  
  public function __construct () 
  {
    $this->javaCode = array ();

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


  public static void main(String[] args) {

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

    try
    {
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

EOD;
    $this->javaCode = array_merge ($this->javaCode, explode ("\n", $code));
  }
  
  public function newParagraph ($style = "Standard")
  {
    $this->javaCode[] = "";
    $this->javaCode[] = "paragraph = new OdfTextParagraph(contentDom, \"$style\");";
    $this->javaCode[] = "officeText.appendChild(paragraph);";
  }
  
  public function addText ($text)
  {
    $this->javaCode[] = "if (paragraph == null)";
    $this->javaCode[] = "{";
    $this->javaCode[] = "  paragraph = new OdfTextParagraph(contentDom, \"Standard\");";
    $this->javaCode[] = "  officeText.appendChild(paragraph);";
    $this->javaCode[] = "}";
    $text = addslashes ($text);
    $this->javaCode[] = "paragraph.addContent(\"$text\");";
  }

  /**
  * This creates a heading with named contents.
  * $style: An existing style name.
  * $text: Contents.
  */
  private function newNamedHeading ($style, $text)
  {
    $style = $this->convertStyleName ($style);
    $this->javaCode[] = "";
    // Make paragraph null, so any next subsequent text would create a new paragraph.
    $this->javaCode[] = "paragraph = null;";
    $this->javaCode[] = "{";
    $this->javaCode[] = "  OdfTextHeading heading;";
    $this->javaCode[] = "  heading = new OdfTextHeading(contentDom);";
    $this->javaCode[] = "  heading.addStyledContent(\"$style\", \"$text\");";
    $this->javaCode[] = "  officeText.appendChild(heading);";
    $this->javaCode[] = "}";
    $this->javaCode[] = "";
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
    $this->javaCode[] = "      document.save(\"$filename\");";
    $this->javaCode[] = "    }";
    $this->javaCode[] = "    catch (Exception e)";
    $this->javaCode[] = "    {";
    $this->javaCode[] = "      System.err.println(e.getMessage());";
    $this->javaCode[] = "    }";
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
    // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
    // since it would be nearly invisible, and thus text would mysteriously get lost.
    $this->javaCode[] = "paragraph = null;";
    $this->javaCode[] = "";
  }

}


?>
