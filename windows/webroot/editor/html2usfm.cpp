/*
 Copyright (©) 2003-2016 Teus Benschop.
 
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


#include <editor/html2usfm.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/usfm.h>
#include <locale/translate.h>
#include <styles/logic.h>
#include <database/logs.h>
#include <pugixml/utils.h>


void Editor_Html2Usfm::load (string html)
{
  // The web editor may insert non-breaking spaces. Convert them to normal ones.
  html = filter_string_str_replace (non_breaking_space_entity (), " ", html);
  
  // The web editor produces <hr> and other elements following the HTML specs,
  // but Bibledit's XML parser needs <hr/> and similar elements.
  html = html2xml (html);
  
  // The user may add several spaces in sequence. Convert them to single spaces.
  html = filter_string_str_replace ("   ", " ", html);
  html = filter_string_str_replace ("  ", " ", html);
  
  string xml = "<body>\n" + html + "\n</body>";
  // Parse document with important option, so it parses the white space in the following:
  //   <node> </node>
  // This is significant for, for example, the space after verse numbers, among others.
  xml_parse_result result = document.load_string (xml.c_str(), parse_ws_pcdata_single);
  // Log parsing errors.
  pugixml_utils_error_logger (&result, xml);
}


void Editor_Html2Usfm::stylesheet (string stylesheet)
{
  styles.clear ();
  noteOpeners.clear ();
  characterStyles.clear ();
  Database_Styles database_styles;
  vector <string> markers = database_styles.getMarkers (stylesheet);
  // Load the style information into the object.
  for (string & marker : markers) {
    Database_Styles_Item style = database_styles.getMarkerData (stylesheet, marker);
    styles [marker] = style;
    // Get markers with should not have endmarkers.
    bool suppress = false;
    int type = style.type;
    int subtype = style.subtype;
    if (type == StyleTypeVerseNumber) suppress = true;
    if (type == StyleTypeFootEndNote) {
      suppress = true;
      if (subtype == FootEndNoteSubtypeFootnote) noteOpeners.insert (marker);
      if (subtype == FootEndNoteSubtypeEndnote) noteOpeners.insert (marker);
      if (subtype == FootEndNoteSubtypeContentWithEndmarker) suppress = false;
      if (subtype == FootEndNoteSubtypeParagraph) suppress = false;
    }
    if (type == StyleTypeCrossreference) {
      suppress = true;
      if (subtype == CrossreferenceSubtypeCrossreference) noteOpeners.insert (marker);
      if (subtype == CrossreferenceSubtypeContentWithEndmarker) suppress = false;
    }
    if (type == StyleTypeTableElement) suppress = true;
    if (suppress) suppressEndMarkers.insert (marker);
  }
}


void Editor_Html2Usfm::run ()
{
  preprocess ();
  process ();
  postprocess ();
}


void Editor_Html2Usfm::process ()
{
  // Walk the children to retrieve the "p" elements, then process them.
  xml_node body = document.first_child ();
  for (xml_node node : body.children()) {
    // Do not process the notes <div>,
    // because it is at the end of the text body,
    // and data has already been gleaned from it.
    string id = node.attribute ("id").value ();
    if (id == "notes") continue;
    // Process the node.
    processNode (node);
  }
}


string Editor_Html2Usfm::get ()
{
  // Generate the USFM as one string.
  string usfm = filter_string_implode (output, "\n");
  
  usfm = cleanUSFM (usfm);
  
  return usfm;
}


void Editor_Html2Usfm::processNode (xml_node node)
{
  switch (node.type ()) {
    case node_element:
    {
      openElementNode (node);
      for (xml_node child : node.children()) {
        processNode (child);
      }
      closeElementNode (node);
      break;
    }
    case node_pcdata:
    {
      // Add the text to the current USFM line.
      string text = node.text ().get ();
      currentLine += text;
      break;
    }
    default:
    {
      string nodename = node.name ();
      Database_Logs::log ("Unknown XML node " + nodename + " while saving editor text");
      break;
    }
  }
}


void Editor_Html2Usfm::openElementNode (xml_node node)
{
  // The tag and class names of this element node.
  string tagName = node.name ();
  string className = node.attribute ("class").value ();
  
  if (tagName == "p")
  {
    // While editing, it may occur that the p element does not have a class.
    // Use the 'p' class in such cases.
    if (className.empty ()) className = "p";
    if (className == "mono") {
      // Class 'mono': The editor has the full USFM in the text.
      mono = true;
    } else {
      // Start the USFM line with a marker with the class name.
      currentLine += usfm_get_opening_usfm (className);
    }
  }
  
  if (tagName == "span")
  {
    if (className == "v")  {
      // Handle the verse.
      flushLine ();
      openInline (className);
    }
    else if (className.empty ()) {
      // Normal text is wrapped in elements without a class attribute.
    } else {
      // Handle remaining class attributes for inline text.
      openInline (className);
    }
  }
  
  if (tagName == "a")
  {
    processNoteCitation (node);
  }
}


void Editor_Html2Usfm::closeElementNode (xml_node node)
{
  // The tag and class names of this element node.
  string tagName = node.name ();
  string className = node.attribute ("class").value ();
  
  if (tagName == "p")
  {
    // While editing it happens that the p element does not have a class.
    // Use the 'p' class in such cases.
    if (className == "") className = "p";
    
    if (noteOpeners.find (className) != noteOpeners.end()) {
      // Deal with note closers.
      currentLine += usfm_get_closing_usfm (className);
    } else {
      // Normally a p element closes the USFM line.
      flushLine ();
      mono = false;
      // Clear active character styles.
      characterStyles.clear();
    }
  }
  
  if (tagName == "span")
  {
    // Do nothing for monospace elements, because the USFM would be the text nodes only.
    if (mono) return;
    // Do nothing without a class.
    if (className.empty()) return;
    // Do nothing if no endmarkers are supposed to be produced.
    if (suppressEndMarkers.find (className) != suppressEndMarkers.end()) return;
    // Add closing USFM, optionally closing embedded tags in reverse order.
    vector <string> classes = filter_string_explode (className, ' ');
    characterStyles = filter_string_array_diff (characterStyles, classes);
    reverse (classes.begin(), classes.end());
    for (unsigned int offset = 0; offset < classes.size(); offset++) {
      bool embedded = (classes.size () > 1) && (offset == 0);
      if (!characterStyles.empty ()) embedded = true;
      currentLine += usfm_get_closing_usfm (classes [offset], embedded);
    }
  }
  
  if (tagName == "a")
  {
    // Do nothing for note citations in the text.
  }
}


void Editor_Html2Usfm::openInline (string className)
{
  // It has been observed that the <span> elements of the character styles may be embedded, like so:
  // The <span class="add">
  //   <span class="nd">Lord God</span>
  // is calling</span> you</span><span>.</span>
  vector <string> classes = filter_string_explode (className, ' ');
  for (unsigned int offset = 0; offset < classes.size(); offset++) {
    bool embedded = (characterStyles.size () + offset) > 0;
    currentLine += usfm_get_opening_usfm (classes[offset], embedded);
  }
  // Store active character styles in some cases.
  bool store = true;
  if (suppressEndMarkers.find (className) != suppressEndMarkers.end ()) store = false;
  if (processingNote) store = false;
  if (store) {
    characterStyles.insert (characterStyles.end(), classes.begin(), classes.end());
  }
}


void Editor_Html2Usfm::processNoteCitation (xml_node node)
{
  // Remove the note citation from the text.
  // It means that this:
  //   <a href="#note1" id="citation1" class="superscript">x</a>
  // becomes this:
  //   <a href="#note1" id="citation1" class="superscript" />
  xml_node child = node.first_child ();
  node.remove_child (child);

  // Get more information about the footnote to retrieve.
  string href = node.attribute ("href").value ();
  string id = href.substr (1);

  // Sample footnote body.
  // <p class="x"><a href="#citation1" id="note1">x</a><span> </span><span>+ 2 Joh. 1.1</span></p>
  // Retrieve the <a> element from it.
  // At first this was done through an XPath expression:
  // http://www.grinninglizard.com/tinyxml2docs/index.html
  // But XPath crashes on Android with libxml2.
  // Therefore now it iterates of all the nodes to find the required <a> element.
  // (After moving to pugixml, the XPath expression could have been used again, but this was not done.)
  xml_node aElement = get_note_pointer (document.first_child (), id);
  if (aElement) {

    // It now has the 'a' element: Get its 'p' parent, and then remove that 'a' element.
    // So we remain with:
    // <p class="x"><span> </span><span>+ 2 Joh. 1.1</span></p>
    xml_node pElement = aElement.parent ();
    pElement.remove_child (aElement);
    
    // Preserve active character styles in the main text, and reset them for the note.
    vector <string> preservedCharacterStyles = characterStyles;
    characterStyles.clear();
    
    // Process this 'p' element.
    processingNote = true;
    processNode (pElement);
    processingNote = false;
    
    // Restore the active character styles for the main text.
    characterStyles = preservedCharacterStyles;
    
    // Remove this element so it can't be processed again.
    xml_node div_notes = pElement.parent ();
    div_notes.remove_child (pElement);
    
  } else {
    Database_Logs::log ("Discarding note with id " + id + " and href " + href);
  }
}


string Editor_Html2Usfm::cleanUSFM (string usfm)
{
  // Replace a double space after a note opener.
  for (string noteOpener : noteOpeners) {
    string opener = usfm_get_opening_usfm (noteOpener);
    usfm = filter_string_str_replace (opener + " ", opener, usfm);
  }

  // Done.
  return usfm;
}


void Editor_Html2Usfm::preprocess ()
{
  output.clear ();
  currentLine.clear ();
  mono = false;
}


void Editor_Html2Usfm::flushLine ()
{
  if (!currentLine.empty ()) {
    // Trim so that '\p ' becomes '\p', for example.
    currentLine = filter_string_trim (currentLine);
    output.push_back (currentLine);
    currentLine.clear ();
  }
}


void Editor_Html2Usfm::postprocess ()
{
  // Flush any last USFM line being built.
  flushLine ();
}


// Retrieves a pointer to a relevant footnote element in the XML.
// Sample footnote element:
// <a href="#citation1" id="note1">x</a>
xml_node Editor_Html2Usfm::get_note_pointer (xml_node node, string id)
{
  if (node) {

    string name = node.name ();
    if (name == "a") {
      string note_id = node.attribute ("id").value ();
      if (id == note_id) return node;
    }
    
    for (xml_node child : node.children ()) {
      xml_node note = get_note_pointer (child, id);
      if (note) return note;
    }

  }
  
  xml_node null_node;
  return null_node;
}


// This function takes the html from an editor that edits one verse,
// and converts it to USFM.
// It properly deals with cases when a verse does not start a new paragraph.
string editor_export_verse (string stylesheet, string html)
{
  // When the $html starts with a paragraph without a style,
  // put a recognizable style there.
  string style = "oneversestyle";
  size_t pos = html.find ("<p>");
  if (pos == 0) {
    html.insert (2, " class=\"" + style + "\"");
  }

  // Convert html to USFM.
  Editor_Html2Usfm editor_export;
  editor_export.load (html);
  editor_export.stylesheet (stylesheet);
  editor_export.run ();
  string usfm = editor_export.get ();

  // Remove that recognizable style converted to USFM.
  usfm = filter_string_str_replace ("\\" + style, "", usfm);
  usfm = filter_string_trim (usfm);
  
  return usfm;
}
