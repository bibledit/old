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


class Editor_Import
{


  // Array of strings alternating between USFM and text.
  private $markersAndText;
  private $markersAndTextPointer;
  // An array holding arrays of style information.
  private $styles;

  // Html DOMNodes.
  private $htmlDom;
  private $bodyDomNode;
  private $notesDomNode;

  // Standard content markers for notes.
  private $standardContentMarkerFootEndNote;
  private $standardContentMarkerCrossReference;

  private $currentPDomElement; // The current p DOMElement.
  private $currentPDomElementNameNode; // The DOMAttr of the name of the style of the current p element.
  private $currentParagraphStyle;
  private $currentParagraphContent;
  private $currentTextStyles;

  private $noteCount;
  private $notePDomElement; // The text:p DOMElement of the current footnote, if any.
  private $currentNoteTextStyles;

  // Whether note is open.
  private $noteOpened = false;

  // Lengths and offsets.
  private $paragraphCount;
  public $textLength;
  public $verseStartOffsets;
  
  // Debug.
  private $debug;


  private static $instance;

  private function __construct() {
  }

  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Editor_Import ();
    }
    return self::$instance;
  }


  public function load ($usfm)
  {
    // Clean up.
    $usfm = trim ($usfm);
    $usfm .= "\n";
    // Separate it into markers and text.
    $markersAndText = Filter_Usfm::getMarkersAndText ($usfm);
    // Load it into the object.
    $this->markersAndText = array ();
    foreach ($markersAndText as $item) {
      $this->markersAndText [] = $item;
    }
    // Debug.
    //$this->debug = strpos ($usfm, "Footnote fk style") !== false;
  }


  public function stylesheet ($stylesheet)
  {
    $this->styles = array ();
    $styles_logic = Styles_Logic::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $markers = $database_styles->getMarkers ($stylesheet);
    // Load the style information into the object.
    foreach ($markers as $marker) {
      $style = $database_styles->getMarkerData ($stylesheet, $marker);
      $this->styles [$marker] = $style;
      if ($style['type'] == StyleTypeFootEndNote) {
        if ($style['subtype'] == FootEndNoteSubtypeStandardContent) {
          $this->standardContentMarkerFootEndNote = $style['marker'];
        }
      }
      if ($style['type'] == StyleTypeCrossreference) {
        if ($style['subtype'] == CrossreferenceSubtypeStandardContent) {
          $this->standardContentMarkerCrossReference = $style['marker'];
        }
      }
    }
  }


  public function run ()
  {
    $this->preprocess ();
    $this->process ();
    $this->postprocess ();
  }


  public function get ()
  {
    // If there are notes, add the notes <div> after everything else.
    // (It has the <hr> as a child).
    if ($this->notesDomNode->childNodes->length > 1) {
      $this->bodyDomNode->appendChild ($this->notesDomNode);
    }

    // Get the entire html code, including head.
    $this->htmlDom->formatOutput = true;
    $html = $this->htmlDom->saveHTML ();

    // Remain with the stuff within the <body> elements.
    $pos = strpos ($html, '<body>');
    if ($pos !== false) {
      $html = substr ($html, $pos + 7);
      $pos = strpos ($html, '</body>');
      if ($pos !== false) {
        $html = substr ($html, 0, $pos - 1);
      }
    }

    return $html;
  }

  private function preprocess ()
  {
    $this->currentParagraphStyle = "";
    $this->currentParagraphContent = "";
    $this->currentTextStyles = array ();
    $this->noteCount = 0;
    $this->currentNoteTextStyles = array ();
    $this->paragraphCount = 0;
    $this->textLength = 0;
    $this->verseStartOffsets = array (0 => 0);

    $this->htmlDom = new DOMDocument;
    $this->htmlDom->encoding = 'UTF-8';

$template = <<<'EOD'
<!doctype html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body>
</body>
</html>
EOD;
    $this->htmlDom->loadHTML ($template);

    $htmlXpath = new DOMXpath ($this->htmlDom);
    $nodeList = $htmlXpath->query ("//body");
    $this->bodyDomNode = $nodeList->item (0);

    $this->notesDomNode = $this->newElement ("div");
    $this->notesDomNode->setAttribute ("id", "notes");
    $node = $this->newElement ("hr");
    $this->notesDomNode->appendChild ($node);
  }


  private function process ()
  {
    $this->markersAndTextPointer = 0;
    $markersAndTextCount = count ($this->markersAndText);
    for ($this->markersAndTextPointer = 0; $this->markersAndTextPointer < $markersAndTextCount; $this->markersAndTextPointer++) {
      $currentItem = $this->markersAndText[$this->markersAndTextPointer];
      if (Filter_Usfm::isUsfmMarker ($currentItem))
      {
        // Store indicator whether the marker is an opening marker.
        $isOpeningMarker = Filter_Usfm::isOpeningMarker ($currentItem);
        $isEmbeddedMarker = Filter_Usfm::isEmbeddedMarker ($currentItem);
        // Clean up the marker, so we remain with the basic version, e.g. 'id'.
        $marker = Filter_Usfm::getMarker ($currentItem);
        if (array_key_exists ($marker, $this->styles))
        {
          $style = $this->styles[$marker];
          switch ($style['type'])
          {
            case StyleTypeIdentifier:
            case StyleTypeNotUsedComment:
            case StyleTypeNotUsedRunningHeader:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypeStartsParagraph:
            {
              $this->closeTextStyle (false, false);
              $this->newParagraph ($marker);
              break;
            }
            case StyleTypeInlineText:
            {
              if ($isOpeningMarker) {
                $this->openTextStyle ($style, false, $isEmbeddedMarker);
              } else {
                $this->closeTextStyle (false, $isEmbeddedMarker);
              }
              break;
            }
            case StyleTypeChapterNumber:
            {
              $this->closeTextStyle (false, false);
              $this->newParagraph ($marker);
              break;
            }
            case StyleTypeVerseNumber:
            {
              // Close any existing text style.
              $this->closeTextStyle (false, false);
              // Output the space before the verse number in case the paragraph already has some text.
              if ($this->currentParagraphContent != "") {
                $this->addText (" ");
              }
              // Open verse style, record verse/length, add verse number, close style again, and add a space.
              $this->openTextStyle ($style, false, false);
              $textFollowingMarker = Filter_Usfm::getTextFollowingMarker ($this->markersAndText, $this->markersAndTextPointer);
              $number = Filter_Usfm::peekVerseNumber ($textFollowingMarker);
              $this->verseStartOffsets [intval ($number)] = $this->textLength;
              $this->addText ($number);
              $this->closeTextStyle (false, false);
              $this->addText (" ");
              // If there was any text following the \v marker, remove the verse number,
              // put the remainder back into the object, and update the pointer.
              if ($textFollowingMarker != "") {
                $pos = strpos ($textFollowingMarker, $number);
                if ($pos !== false) {
                  $textFollowingMarker = substr ($textFollowingMarker, $pos + strlen ($number));
                }
                $textFollowingMarker = ltrim ($textFollowingMarker);
                $this->markersAndText [$this->markersAndTextPointer] = $textFollowingMarker;
                $this->markersAndTextPointer--;
              }
              break;
            }
            case StyleTypeFootEndNote:
            {
              switch ($style['subtype'])
              {
                case FootEndNoteSubtypeFootnote:
                case FootEndNoteSubtypeEndnote:
                {
                  $this->closeTextStyle (false, false);
                  if ($isOpeningMarker) {
                    $this->noteOpened = true;
                    $caller = $this->noteCount % 9 + 1;
                    $this->addNote ($caller, $marker, false);
                  } else {
                    $this->closeCurrentNote ();
                    $this->noteOpened = false;
                  }
                  break;
                }
                case FootEndNoteSubtypeStandardContent:
                case FootEndNoteSubtypeContent:
                case FootEndNoteSubtypeContentWithEndmarker:
                {
                  if ($isOpeningMarker) {
                    $this->openTextStyle ($style, true, $isEmbeddedMarker);
                  } else {
                    $this->closeTextStyle (true, $isEmbeddedMarker);
                  }
                  break;
                }
                case FootEndNoteSubtypeParagraph:
                default:
                {
                  $this->closeTextStyle (false, false);
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
                  $this->closeTextStyle (false, false);
                  if ($isOpeningMarker) {
                    $this->noteOpened = true;
                    $caller = ($this->noteCount) % 9 + 1;
                    $this->addNote ($caller, $marker, false);
                  } else {
                    $this->closeCurrentNote ();
                    $this->noteOpened = false;
                  }
                  break;
                }
                case CrossreferenceSubtypeContent:
                case CrossreferenceSubtypeContentWithEndmarker:
                {
                  if ($isOpeningMarker) {
                    $this->openTextStyle ($style, true, $isEmbeddedMarker);
                  } else {
                    $this->closeTextStyle (true, $isEmbeddedMarker);
                  }
                  break;
                }
                case CrossreferenceSubtypeStandardContent:
                default:
                {
                  // The style of the standard content is already used in the note's body.
                  // Clear the text style to get the correct style for the note paragraph.
                  $this->closeTextStyle (false, false);
                  break;
                }
              }
              break;
            }
            case StyleTypePeripheral:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypePicture:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypePageBreak:
            {
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
            case StyleTypeTableElement:
            {
              $this->closeTextStyle (false, false);
              switch ($style['subtype'])
              {
                case TableElementSubtypeRow:
                {
                  $this->outputAsIs ($marker, $isOpeningMarker);
                  break;
                }
                case TableElementSubtypeHeading:
                case TableElementSubtypeCell:
                {
                  $this->openTextStyle ($style, false, false);
                  break;
                }
                default:
                {
                  $this->openTextStyle ($style, false, false);
                  break;
                }
              }
              break;
            }
            case StyleTypeWordlistElement:
            {
              if ($isOpeningMarker) {
                $this->openTextStyle ($style, false, false);
              } else {
                $this->closeTextStyle (false, false);
              }
              break;
            }
            default:
            {
              // This marker is known in the stylesheet, but not yet implemented here.
              $this->closeTextStyle (false, false);
              $this->outputAsIs ($marker, $isOpeningMarker);
              break;
            }
          }
        } else {
          // This is a marker unknown in the stylesheet.
          $this->closeTextStyle (false, false);
          $this->outputAsIs ($marker, $isOpeningMarker);
        }
      } else {
        // Here is no marker. Treat it as text.
        if ($this->noteOpened) {
          $this->addNoteText ($currentItem);
        } else {
          $this->addText ($currentItem);
        }
      }
    }
  }


  private function postprocess ()
  {
  }


  private function outputAsIs ($marker, $isOpeningMarker)
  {
    // Output the marker in monospace font.
    if ($isOpeningMarker) {
      // Add opening marker as it is.
      $this->newParagraph ("mono");
      $this->addText (Filter_Usfm::getOpeningUsfm ($marker));
    } else {
      // Add closing marker to existing paragraph.
      $this->addText (Filter_Usfm::getClosingUsfm ($marker));
    }
  }


  private function newElement ($element)
  {
    $element = $this->htmlDom->createElement ($element);
    return $element;
  }


  private function newParagraph ($style = "")
  {
    $this->currentPDomElement = $this->newElement ("p");
    if ($style != "") {
      $this->currentPDomElementNameNode = $this->currentPDomElement->setAttribute ("class", $style);
    }
    $this->bodyDomNode->appendChild ($this->currentPDomElement);
    $this->currentParagraphStyle = $style;
    $this->currentParagraphContent = "";
    $this->paragraphCount++;
    if ($this->paragraphCount > 1) {
      $this->textLength++;
    }
  }


  // This opens a text style.
  // $style: the array containing the style variables.
  // $note: boolean: Whether this refers to a note.
  // $embed: boolean: Whether to open embedded / nested style.
  private function openTextStyle ($style, $note, $embed)
  {
    $marker = $style["marker"];
    if ($note) {
      if (!$embed) $this->currentNoteTextStyles = array ();
      $this->currentNoteTextStyles [] = $marker;
    } else {
      if (!$embed) $this->currentTextStyles = array ();
      $this->currentTextStyles [] = $marker;
    }
  }


  // This closes any open text style.
  // $note: Whether this refers to a note.
  // $embed: boolean: Whether to close embedded character style.
  private function closeTextStyle ($note, $embed)
  {
    if ($note) {
      if (count ($this->currentNoteTextStyles)) array_pop ($this->currentNoteTextStyles);
      if (!$embed) $this->currentNoteTextStyles = array ();
    } else {
      if (count ($this->currentTextStyles)) array_pop ($this->currentTextStyles);
      if (!$embed) $this->currentTextStyles = array ();
    }
  }


  // This function adds text to the current paragraph.
  // $text: The text to add.
  private function addText ($text)
  {
    if ($text != "") {
      if (!isset ($this->currentPDomElement)) {
        $this->newParagraph ();
      }
      $spanDomElement = $this->newElement ("span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->currentPDomElement->appendChild ($spanDomElement);
      if (count ($this->currentTextStyles)) {
        // Take character style(s) as specified in this object.
        $spanDomElement->setAttribute ("class", implode (" ", $this->currentTextStyles));
      }
      $this->currentParagraphContent .= $text;
    }
    $this->textLength += mb_strlen ($text);
  }


  // This function adds a note to the current paragraph.
  // $citation: The text of the note citation.
  // $style: Style name for the paragraph in the note body.
  // $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
  private function addNote ($citation, $style, $endnote = false)
  {
    // Ensure that a paragraph is open, so that the note can be added to it.
    if (!isset ($this->currentPDomElement)) {
      $this->newParagraph ();
    }

    $this->noteCount++;

    // Add the link with all relevant data for the note citation.
    $reference = "#note" . $this->noteCount;
    $identifier = "citation" . $this->noteCount;
    $this->addLink ($this->currentPDomElement, $reference, $identifier, "superscript", $citation);

    // Open a paragraph element for the note body.
    $this->notePDomElement = $this->newElement ("p");
    $this->notesDomNode->appendChild ($this->notePDomElement);
    $this->notePDomElement->setAttribute ("class", $style);

    $this->closeTextStyle (true, false);

    // Add the link with all relevant data for the note body.
    $reference = "#citation" . $this->noteCount;
    $identifier = "note" . $this->noteCount;
    $this->addLink ($this->notePDomElement, $reference, $identifier, "", $citation);

    // Add a space.
    $this->addNoteText (" ");

    // Update the text length of the text body, excluding the note.
    $this->textLength += mb_strlen ($citation);
  }


  // This function adds text to the current footnote.
  // $text: The text to add.
  private function addNoteText ($text)
  {
    if ($text != "") {
      if (!isset ($this->notePDomElement)) {
        $this->addNote ("?", "");
      }
      $spanDomElement = $this->newElement ("span");
      $spanDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
      $this->notePDomElement->appendChild ($spanDomElement);
      if (count ($this->currentNoteTextStyles)) {
        // Take character style as specified in this object.
        $spanDomElement->setAttribute ("class", implode (" ", $this->currentNoteTextStyles));
      }
    }
  }


  // This function closes the current footnote.
  private function closeCurrentNote ()
  {
    $this->closeTextStyle (true, false);
    $this->notePDomElement = NULL;
  }


  // This adds a link.
  // $domNode: The DOM node where to add the link to.
  // $reference: The link's href, e.g. where it links to.
  // $identifier: The link's identifier. Others can link to it.
  // $style: The link text's style.
  // $text: The link's text.
  private function addLink ($domNode, $reference, $identifier, $style, $text)
  {
    $aDomElement = $this->newElement ("a");
    $domNode->appendChild ($aDomElement);
    $aDomElement->setAttribute ("href", $reference);
    $aDomElement->setAttribute ("id", $identifier);
    // The link itself, for the notes, is not editable, so it can be clicked.
    // It was disabled again due to Chrome removing content on backspace.
    // $aDomElement->setAttribute ("contenteditable", "false");
    if ($style != "") $aDomElement->setAttribute ("class", $style);
    $aDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
  }



}


?>
