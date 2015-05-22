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


// Class for creating Html text documents.
class Html_Text
{
  private $htmlDom;
  private $headDomNode;
  private $bodyDomNode;
  private $notesDivDomNode;

  public $customClass = ""; // This class to be added to each paragraph. The class to be defined in the stylesheet.css.
  
  public $currentPDomElement; // The current text:p DOMElement.
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
    $this->currentTextStyle = array ();
    $this->frameCount = 0;
    $this->noteCount = 0;
    $this->currentNoteTextStyle = array ();

    $database_config_general = Database_Config_General::getInstance ();

    $this->htmlDom = new DOMDocument;
    $this->htmlDom->load(dirname (__FILE__) . "/template.html");

    $htmlXpath = new DOMXpath ($this->htmlDom);

    $nodeList = $htmlXpath->query ("*");
    $this->headDomNode = $nodeList->item (0);
    $element = $this->newElement ("title");
    $this->headDomNode->appendChild ($element);
    $element->nodeValue = htmlspecialchars ($title, ENT_QUOTES, "UTF-8");

    $this->bodyDomNode = $nodeList->item (1);

    $this->notesDivDomNode = $this->newElement ("div");
  }


  public function newParagraph ($style = "")
  {
    $this->currentPDomElement = $this->newElement ("p");
    if ($style != "") {
      $class = $style;
      if ($this->customClass) {
        $class .= " " . $this->customClass;
      }
      $this->currentPDomElementNameNode = $this->currentPDomElement->setAttribute ("class", $class);
    }
    $this->bodyDomNode->appendChild ($this->currentPDomElement);
    $this->currentParagraphStyle = $style;
    $this->currentParagraphContent = "";
  }


  // This function adds text to the current paragraph.
  // $text: The text to add.
  public function addText ($text, $domNode = NULL)
  {
    if ($text != "") {
      if (!isset ($this->currentPDomElement)) {
        $this->newParagraph ();
      }
      $spanDomElement = $this->newElement ("span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->currentPDomElement->appendChild ($spanDomElement);
      if (count ($this->currentTextStyle)) {
        // Take character style(s) as specified in the object.
        $spanDomElement->setAttribute ("class", implode (" ", $this->currentTextStyle));
      }
      $this->currentParagraphContent .= $text;
    }
  }


  // This creates a h1 heading with contents.
  // $text: Contents.
  public function newHeading1($text, $hide = false)
  {
    $this->newNamedHeading ("h1", $text, $hide);
  }


  // This applies a page break.
  // (The style is already in the css).
  public function newPageBreak ()
  {
    $this->newParagraph ("break");
    // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
    // since it would be nearly invisible, and thus text would mysteriously get lost.
    unset ($this->currentPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }


  // This opens a text style.
  // $style: the array containing the style variables.
  // $note: boolean: Whether this refers to notes.
  // $embed: boolean: Whether to embed the new text style in an existing text style.
  //                  true: add the new style to the existing style.
  //                  false: close any existing text style, and open the new style.
  public function openTextStyle ($style, $note, $embed)
  {
    $marker = $style["marker"];
    if ($note) {
      if (!$embed) $this->currentNoteTextStyle = array ();
      $this->currentNoteTextStyle [] = $marker;
    } else {
      if (!$embed) $this->currentTextStyle = array ();
      $this->currentTextStyle [] = $marker;
    }
  }


  // This closes any open text style.
  // $note: boolean: Whether this refers to notes.
  // $embed: boolean: Whether to embed the new text style in an existing text style.
  //                  true: add the new style to the existing style.
  //                  false: close any existing text style, and open the new style.
  public function closeTextStyle ($note, $embed)
  {
    if ($note) {
      if (count ($this->currentNoteTextStyle)) array_pop ($this->currentNoteTextStyle);
      if (!$embed) $this->currentNoteTextStyle = array ();
    } else {
      if (count ($this->currentTextStyle)) array_pop ($this->currentTextStyle);
      if (!$embed) $this->currentTextStyle = array ();
    }
  }


  // This function adds a note to the current paragraph.
  // $citation: The text of the note citation.
  // $style: Style name for the paragraph in the note body.
  // $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
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
    $this->addLink ($this->currentPDomElement, $reference, $identifier, "", "superscript", $citation);

    // Open a paragraph element for the note body.
    $this->notePDomElement = $this->newElement ("p");
    $this->notesDivDomNode->appendChild ($this->notePDomElement);
    $this->notePDomElement->setAttribute ("class", $style);

    $this->closeTextStyle (true, false);

    // Add the link with all relevant data for the note body.
    $reference = "#citation" . $this->noteCount;
    $identifier = "note" . $this->noteCount;
    $this->addLink ($this->notePDomElement, $reference, $identifier, "", "", $citation);

    // Add a space.
    $this->addNoteText (" ");
  }


  // This function adds text to the current footnote.
  // $text: The text to add.
  public function addNoteText ($text)
  {
    if ($text != "") {
      if (!isset ($this->notePDomElement)) {
        $this->addNote ("?", "");
      }
      $spanDomElement = $this->newElement ("span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->notePDomElement->appendChild ($spanDomElement);
      if (count ($this->currentNoteTextStyle)) {
        // Take character style as specified in this object.
        $spanDomElement->setAttribute ("class", implode (" ", $this->currentNoteTextStyle));
      }
    }
  }


  // This function closes the current footnote.
  public function closeCurrentNote ()
  {
    $this->closeTextStyle (true, false);
    $this->notePDomElement = NULL;
  }


  // This adds a link.
  // $domNode: The DOM node where to add the link to.
  // $reference: The link's href, e.g. where it links to.
  // $identifier: The link's identifier. Others can link to it.
  // $title: The link's title, to make it accessible, e.g. for screenreaders.
  // $style: The link text's style.
  // $text: The link's text.
  public function addLink ($domNode, $reference, $identifier, $title, $style, $text)
  {
    $aDomElement = $this->newElement ("a");
    $domNode->appendChild ($aDomElement);
    $aDomElement->setAttribute ("href", $reference);
    $aDomElement->setAttribute ("id", $identifier);
    if ($title) $aDomElement->setAttribute ("title", $title);
    if ($style != "") $aDomElement->setAttribute ("class", $style);
    $aDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
  }


  // This gets and then returns the html text
  public function getHtml ()
  {
    // Add possible notes.
    if (isset ($this->notesDivDomNode)) {
      $this->bodyDomNode->appendChild ($this->notesDivDomNode);
      unset ($this->notesDivDomNode);
    }
    // Get the html.
    $this->htmlDom->formatOutput = true;
    $string = $this->htmlDom->saveHTML ();
    return $string;
  }


  // Returns the html text within the <body> tags, that is, without the <head> stuff.
  public function getInnerHtml ()
  {
    $page = $this->getHtml ();
    $pos = strpos ($page, '<body>');
    if ($pos !== false) {
      $page = substr ($page, $pos + 7);
      $pos = strpos ($page, '</body>');
      if ($pos !== false) {
        $page = substr ($page, 0, $pos - 1);
      }
    }
    // Remove the empty <div> element at the end.
    $pos = strpos ($page, '<div></div>');
    if ($pos !== false) {
      $page = substr ($page, 0, $pos);
    }
    return $page;
  }


  // This saves the web page to file
  // $name: the name of the file to save to.
  public function save ($name)
  {
    $this->getHtml ();
    $this->htmlDom->saveHTMLFile ($name);
  }


  // This creates a heading with styled content.
  // $style: A style name.
  // $text: Content.
  private function newNamedHeading ($style, $text, $hide = false)
  {
    $textHDomElement = $this->newElement ($style);
    $this->bodyDomNode->appendChild ($textHDomElement);
    $textHDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
    // Make paragraph null, so that adding subsequent text creates a new paragraph.
    unset ($this->currentPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
  }


  // This adds a new table to the html DOM.
  // Returns: The new $tableElement
  public function newTable ()
  {
    // Adding subsequent text should create a new paragraph.
    unset ($this->currentPDomElement);
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
    // Append the table.
    $tableElement = $this->newElement ('table');
    $this->bodyDomNode->appendChild ($tableElement);
    $tableElement->setAttribute ("width", "100%");
    return $tableElement;
  }


  // This adds a new row to an existing $tableElement.
  // Returns: The new $tableRowElement.
  public function newTableRow (&$tableElement)
  {
    $tableRowElement = $this->newElement ('tr');
    $tableElement->appendChild ($tableRowElement);
    return $tableRowElement;
  }


  // This adds a new data cell to an existing $tableRowElement.
  // Returns: The new $tableDataElement.
  public function newTableData (&$tableRowElement, $alignRight = false)
  {
    $tableDataElement = $this->newElement ('td');
    if ($alignRight) $tableDataElement->setAttribute ("align", "right");
    $tableRowElement->appendChild ($tableDataElement);
    return $tableDataElement;
  }


  // This adds a new $element to the htmlDom.
  // Returns: The new element.
  public function newElement ($element)
  {
    $element = $this->htmlDom->createElement ($element);
    return $element;
  }


}


?>
