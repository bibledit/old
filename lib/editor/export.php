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


class Editor_Export
{

  // DOMDocument holding the html.
  private $document;
  // Style information.
  private $styles;
  // Output USFM.
  private $output;
  // Growing current USFM line.
  private $currentLine;
  // Monospace font.
  private $mono;
  // Markers which should not have endmarkers, e.g. \v does not have \v*
  private $suppressEndMarkers;
  // Note openers.
  private $noteOpeners;
  // Active character styles.
  private $characterStyles;
  // Note processing flag.
  private $processingNote = false;


  private static $instance;

  private function __construct() 
  {
  }

  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Editor_Export ();
    }
    return self::$instance;
  }


  public function load ($html)
  {
    // The online editor may insert non-breaking spaces. Convert them to normal ones.
    $html = str_replace ('&nbsp;', ' ', $html);

    // The user may add several spaces in sequence. Convert them to single spaces.
    $html = str_replace ('   ', ' ', $html);
    $html = str_replace ('  ', ' ', $html);

    // Create new DOMDocument.
    $this->document = new DOMDocument ();

    // DOMDocument deals well with imperfect markup, but it throws warnings to the default error handler.
    // Therefore keep the errors separate.
    libxml_use_internal_errors (true);

    // To help loadHTML() process utf8 correctly, set the correct meta tag before any other text.
$prefix = <<<'EOD'
<!doctype html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body>
EOD;
$suffix = <<<'EOD'
</body>
</html>
EOD;
    $this->document->loadHTML ($prefix . $html . $suffix);

    // Log any errors to the logbook.
    $errors = libxml_get_errors ();
    if (count ($errors)) {
      $database_logs = Database_Logs::getInstance ();
      foreach ($errors as $error) {
        $level = "";
        switch ($error->level) {
          case LIBXML_ERR_WARNING:
            $level = "Warning";
            break;
           case LIBXML_ERR_ERROR:
            $level = "Error";
            break;
          case LIBXML_ERR_FATAL:
            $level = "Fatal";
            break;
        }
        $database_logs->log ("Saving Editor text: $level: " . $error->message);
      }
      libxml_clear_errors();
    }
  }


  public function stylesheet ($stylesheet)
  {
    $this->styles = array ();
    $this->noteOpeners = array ();
    $this->characterStyles = array ();
    $styles_logic = Styles_Logic::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $markers = $database_styles->getMarkers ($stylesheet);
    // Load the style information into the object.
    foreach ($markers as $marker) {
      $style = $database_styles->getMarkerData ($stylesheet, $marker);
      $this->styles [$marker] = $style;
      // Get markers with should not have endmarkers.
      $suppress = false;
      $type = $style ['type'];
      $subtype = $style ['subtype'];
      if ($type == StyleTypeVerseNumber) $suppress = true;
      if ($type == StyleTypeFootEndNote) {
        $suppress = true;
        if ($subtype == FootEndNoteSubtypeFootnote) $this->noteOpeners [] = $marker;
        if ($subtype == FootEndNoteSubtypeEndnote) $this->noteOpeners [] = $marker;
        if ($subtype == FootEndNoteSubtypeContentWithEndmarker) $suppress = false;
        if ($subtype == FootEndNoteSubtypeParagraph) $suppress = false;
      }
      if ($type == StyleTypeCrossreference) {
        $suppress = true;
        if ($subtype == CrossreferenceSubtypeCrossreference) $this->noteOpeners [] = $marker;
        if ($subtype == CrossreferenceSubtypeContentWithEndmarker) $suppress = false;
      }
      if ($type == StyleTypeTableElement) $suppress = true;
      if ($suppress) $this->suppressEndMarkers [] = $marker;
    }
  }


  public function run ()
  {
    $this->preprocess ();
    $this->process ();
    $this->postprocess ();
  }


  private function preprocess ()
  {
    $this->output = array ();
    $this->currentLine = "";
    $this->mono = false;
    $this->document->encoding = 'UTF-8';
    $this->document->preserveWhiteSpace = false;
  }


  private function process ()
  {
    $DOMNodeList = $this->document->getElementsByTagName ("p");
    foreach ($DOMNodeList as $DOMNode) {
      $this->processNode ($DOMNode);
    }
  }


  private function postprocess ()
  {
    // Flush any last USFM line being built.
    $this->flushLine ();
  }


  private function processNode ($node)
  {
    switch ($node->nodeType) {
      case XML_ELEMENT_NODE:
      {
        $this->openElementNode ($node);
        $this->processNodeChildren ($node);
        $this->closeElementNode ($node);
        break;
      }
      case XML_ATTRIBUTE_NODE:
      {
        $this->processAttributeNode ($node);
        break;
      }
      case XML_TEXT_NODE:
      {
        $this->processTextNode ($node);
        break;
      }
      case XML_CDATA_SECTION_NODE:
      case XML_ENTITY_REF_NODE:
      case XML_ENTITY_NODE:
      case XML_PI_NODE:
      case XML_COMMENT_NODE:
      case XML_DOCUMENT_NODE:
      case XML_DOCUMENT_TYPE_NODE:
      case XML_DOCUMENT_FRAG_NODE:
      case XML_NOTATION_NODE:
      {
        break;
      }
      default:
      {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ("Unknown DOM node $node while saving editor text");
        break;
      }
    }
  }


  private function openElementNode ($node)
  {
    // The tag name of this element node.
    $tagName = $node->tagName;
    $class = $node->getAttribute ('class');

    switch ($tagName) {

      case 'p':
      {
        // While editing, it may occur that the p element does not have a class.
        // Use the 'p' class in such cases.
        if ($class == "") $class = "p";
        switch ($class) {
          // Class 'mono': The editor has the full USFM in the text.
          case 'mono':
          {
            $this->mono = true;
            break;
          }
          // Start the USFM line with a marker with the class name.
          default:
          {
            $this->currentLine .= Filter_Usfm::getOpeningUsfm ($class);
            break;
          }
        }
        break;
      }

      case 'span':
      {
        switch ($class) {
          // Handle the verse.
          case 'v':
          {
            $this->flushLine ();
            $this->openInline ($class);
            break;
          }
          // Normal text is wrapped in elements without a class attribute.
          case '':
          {
            break;
          }
          // Handle remaining class attributes for inline text.
          default:
          {
            $this->openInline ($class);
            break;
          }
        }
        break;
      }

      case 'a':
      {
        $this->processNoteCitation ($node);
        break;
      }

      default:
      {
        break;
      }
    }
  }


  private function processNodeChildren ($node)
  {
    if ($node->hasChildNodes ()) {
      foreach ($node->childNodes as $childnode) {
        $this->processNode ($childnode);
      }
    }
  }


  private function closeElementNode ($node)
  {
    // The tag name of this element node.
    $tagName = $node->tagName;
    $class = $node->getAttribute ('class');

    switch ($tagName) {

      case 'p':
      {
        // While editing it happens that the p element does not have a class.
        // Use the 'p' class in such cases.
        if ($class == "") $class = "p";

        if (in_array ($class, $this->noteOpeners)) {
          // Deal with note closers.
          $this->currentLine .= Filter_Usfm::getClosingUsfm ($class);
        } else {
          // Normally a p element closes the USFM line.
          $this->flushLine ();
          $this->mono = false;
          // Clear active character styles clear active character styles.
          $this->characterStyles = array ();
        }
        break;
      }

      case 'span':
      {
        // Do nothing for monospace elements, because the USFM would be the text nodes only.
        if ($this->mono) break;
        // Do nothing without a class.
        if ($class == "") break;
        // Do nothing if no endmarkers are supposed to be produced.
        if (in_array ($class, $this->suppressEndMarkers)) break;
        // Add closing USFM, optionally closing embedded tags in reverse order.
        $classes = explode (" ", $class);
        $this->characterStyles = array_diff ($this->characterStyles, $classes);
        $classes = array_reverse ($classes);
        foreach ($classes as $offset => $class) {
          $embedded = (count ($classes) > 1) && ($offset == 0);
          if (!empty ($this->characterStyles)) $embedded = true;
          $this->currentLine .= Filter_Usfm::getClosingUsfm ($class, $embedded);
        }
        break;
      }

      case 'a':
      {
        // Do nothing for note citations in the text.
        break;
      }

      default:
      {
        break;
      }
    }
  }



  private function processAttributeNode ($node)
  {
    $database_logs = Database_Logs::getInstance ();
    $database_logs->log ("Unprocessed XML_ATTRIBUTE_NODE while saving editor text");
  }


  private function processTextNode ($node)
  {
    // Add the text to the current USFM line.
    $text = $node->wholeText;
    $this->currentLine .= $text;
  }


  private function flushLine ()
  {
    if ($this->currentLine) {
      // Trim so that '\p ' becomes '\p', for example.
      $this->currentLine = trim ($this->currentLine);
      $this->output [] = $this->currentLine;
      $this->currentLine = '';
    }
  }


  private function openInline ($class)
  {
    // It has been observed that the <span> elements of the character styles may be embedded, like so:
    // The <span class="add">
    //   <span class="nd">Lord God</span> 
    // is calling</span> you</span><span>.</span>
    $classes = explode (" ", $class);
    foreach ($classes as $offset => $class) {
      $embedded = (count ($this->characterStyles) + $offset) > 0;
      $this->currentLine .= Filter_Usfm::getOpeningUsfm ($class, $embedded);
    }
    // Store active character styles in some cases.
    $store = true;
    if (in_array ($class, $this->suppressEndMarkers)) $store = false;
    if ($this->processingNote) $store = false;
    if ($store) {
      $this->characterStyles = array_merge ($this->characterStyles, $classes);
    }
  }


  private function processNoteCitation ($node)
  {
    // The note citation in the text will have the "1" or the "2", and so on, till "9". Remove it.
    if ($node->hasChildNodes ()) {
      foreach ($node->childNodes as $childnode) {
        $node->removeChild ($childnode);
      }
    }

    // Get more information about the footnote to retrieve.
    $href = $node->getAttribute ("href");
    $id = substr ($href, 1);

    // Sample footnote body.
    // <p class="x"><a href="#citation1" id="note1">x</a><span> </span><span>+ 2 Joh. 1.1</span></p>

    // XPath to retrieve the note contents.
    $xpath = new DOMXPath ($this->document);
    $path = "//a[@id='$id']";
    $nodeList = $xpath->query ($path);

    // There should be only one relevant note node.
    if ($nodeList->length != 1) {
      $database_logs = Database_Logs::getInstance ();
      $database_logs->log ("Discarding note with id $id and href $href");
      return;
    }

    // Get the 'a' element, its 'p' parent, and then remove that 'a' element.
    // So we remain with:
    // <p class="x"><span> </span><span>+ 2 Joh. 1.1</span></p>
    $aElement = $nodeList->item (0);
    $pElement = $aElement->parentNode;
    $pElement->removeChild ($aElement);

    // Preserve active character styles in the main text, and reset them for the note.
    $characterStyles = $this->characterStyles;
    $this->characterStyles = array ();

    // Process this 'p' element.
    $this->processingNote = true;
    $this->processNode ($pElement);
    $this->processingNote = false;
    
    // Restore the active character styles for the main text.
    $this->characterStyles = $characterStyles;

    // Remove this element so it can't be processed again.
    $parentNode = $pElement->parentNode;
    $parentNode->removeChild ($pElement);
  }


  public function get ()
  {
    // Generate the USFM as one string.
    $usfm = implode ("\n", $this->output);

    $usfm = $this->cleanUSFM ($usfm);

    return $usfm;
  }


  private function cleanUSFM ($usfm)
  {
    // The user may accidentally omit or erase the note caller.
    // The note caller is one character that immediately follows the note opener.
    // E.g.: \f + ...\f*.
    // Check for missing note caller, and if it's not there, add the default "+".
    // Also replace a double space after a note opener.
    foreach ($this->noteOpeners as $noteOpener) {
      $opener = Filter_Usfm::getOpeningUsfm ($noteOpener);
      $usfm = str_replace ($opener . " ", $opener, $usfm);
      $pos = mb_strpos ($usfm, $opener);
      while ($pos !== false) {

        $isClean = true;

        // Check that the character that follows the note opener is a non-space.
        $pos2 = $pos + mb_strlen ($opener);
        $character = mb_substr ($usfm, $pos2, 1);
        if ($character == " ") $isClean = false;

        // Check that the following character is not a space.
        $pos2++;
        $character = mb_substr ($usfm, $pos2, 1);
        if ($character != " ") $isClean = false;

        // Fix the note caller is necessary.
        if (!$isClean) {
          $database_logs = Database_Logs::getInstance ();
          $database_logs->log ("Fixing note caller in $usfm");
          $pos2--;
          $usfm = mb_substr ($usfm, 0, $pos2) . "+" . mb_substr ($usfm, $pos2);
        }

        // Next iteration.
        $pos = mb_strpos ($usfm, $opener, $pos + 5);
      }
    }

    return $usfm;
  }


}


?>
