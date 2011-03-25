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
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Odfdom_Text();
    }
    return self::$instance;
  }

  public $javaCode;
  
  public function initialize () 
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
  public static void main(String[] args) {

    // The document to build.
    OdfTextDocument document;

    // the office:text element in the content.xml file
    OfficeTextElement officeText;

    try
    {
      document = OdfTextDocument.newTextDocument();
      officeText = document.getContentRoot();

      // The templates included in the ODFDOM toolkit have content in them; 
      // a newly-created text document has a paragraph that contains no text.
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
    $this->javaCode = array_merge ($this->javaCode, explode ("\n", $code)); // Todo
  }
  
  public function newParagraph ()
  {
    $this->javaCode[] = "document.newParagraph();";
  }
  
  public function addText ($text)
  {
    $text = addslashes ($text);
    $this->javaCode[] = "document.addText(\"$text\");";
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


}


?>
