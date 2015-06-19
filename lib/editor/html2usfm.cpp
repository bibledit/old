/*
 Copyright (©) 2003-2015 Teus Benschop.
 
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
#include <webserver/request.h>
#include <locale/translate.h>
#include <styles/logic.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <database/logs.h>


Editor_Html2Usfm::Editor_Html2Usfm (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Editor_Html2Usfm::~Editor_Html2Usfm ()
{
  //xmlDocDump (stdout, document);
  xmlFreeDoc (document);
}


void Editor_Html2Usfm::load (string html)
{
  // The web editor may insert non-breaking spaces. Convert them to normal ones.
  html = filter_string_str_replace ("&nbsp;", " ", html);
  
  // The web editor produces <hr> following the HTML specs, but Bibledit needs
  // <hr/> for its XML parser.
  html = filter_string_str_replace ("<hr>", "<hr/>", html);
  
  // The user may add several spaces in sequence. Convert them to single spaces.
  html = filter_string_str_replace ("   ", " ", html);
  html = filter_string_str_replace ("  ", " ", html);
  
  // DOMDocument deals well with imperfect markup, but it may throw warnings to the default error handler.
  // Therefore keep the errors separate.
  xmlGenericErrorFunc handler = (xmlGenericErrorFunc) error_handler;
  initGenericErrorDefaultFunc (&handler);
  
  // To help loadHTML() process utf8 correctly, set the correct meta tag before any other text.
  /*
  string prefix =
  "<!DOCTYPE html>\n"
  "<html>\n"
  "<head>\n"
  "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
  "</head>\n"
  "<body>\n";
  string suffix =
  "\n"
  "</body>\n"
  "</html>\n";
  string xml = prefix + html + suffix;
  htmlParserCtxtPtr context = htmlNewParserCtxt();
  document = htmlCtxtReadMemory (context, xml.c_str(), xml.length(), "", "UTF-8", HTML_PARSE_RECOVER);
  htmlFreeParserCtxt (context);
   */

  // On Android, the HTML parser fails. It returns a NULL document.
  // Therefore use the XML parser instead of the HTML one.
  string prefix =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<html>\n"
  "<head>\n"
  "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"></meta>\n"
  "</head>\n"
  "<body>\n";
  string suffix =
  "\n"
  "</body>\n"
  "</html>\n";
  string xml = prefix + html + suffix;
  xmlParserCtxtPtr context = xmlNewParserCtxt();
  document = xmlCtxtReadMemory (context, xml.c_str(), xml.length(), "", "UTF-8", XML_PARSE_RECOVER);
  xmlFreeParserCtxt (context);
}


void Editor_Html2Usfm::stylesheet (string stylesheet)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  styles.clear ();
  noteOpeners.clear ();
  characterStyles.clear ();
  vector <string> markers = request->database_styles()->getMarkers (stylesheet);
  // Load the style information into the object.
  for (string & marker : markers) {
    Database_Styles_Item style = request->database_styles()->getMarkerData (stylesheet, marker);
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
  // Walk the tree to retrieve the "p" elements, then process them.
  xmlNodePtr mainnode = xmlDocGetRootElement (document);
  mainnode = mainnode->xmlChildrenNode;
  while (mainnode != NULL) {
    if ((!xmlStrcmp (mainnode->name, (const xmlChar *)"body"))) {
      xmlNodePtr pnode = mainnode->xmlChildrenNode;
      while (pnode != NULL) {
        if ((!xmlStrcmp (pnode->name, (const xmlChar *)"p"))) {
          processNode (pnode);
        }
        pnode = pnode->next;
      }
    }
    mainnode = mainnode->next;
  }
}


string Editor_Html2Usfm::get ()
{
  // Generate the USFM as one string.
  string usfm = filter_string_implode (output, "\n");
  
  usfm = cleanUSFM (usfm);
  
  return usfm;
}


void Editor_Html2Usfm::processNode (xmlNodePtr node)
{
  switch (node->type) {
    case XML_ELEMENT_NODE:
    {
      openElementNode (node);
      processNodeChildren (node);
      closeElementNode (node);
      break;
    }
    case XML_ATTRIBUTE_NODE:
    {
      processAttributeNode (node);
      break;
    }
    case XML_TEXT_NODE:
    {
      processTextNode (node);
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
      string nodename ((char *) node->name);
      Database_Logs::log ("Unknown DOM node " + nodename + " while saving editor text");
      break;
    }
  }
}


void Editor_Html2Usfm::openElementNode (xmlNodePtr node)
{
  // The tag and class names of this element node.
  string tagName ((char *) node->name);
  string className;
  xmlChar * property = xmlGetProp (node, BAD_CAST "class");
  if (property) {
    className = (char *) property;
    xmlFree (property);
  }
  
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


void Editor_Html2Usfm::processNodeChildren (xmlNodePtr node)
{
  node = node->xmlChildrenNode;
  while (node != NULL) {
    processNode (node);
    node = node->next;
  }
}


void Editor_Html2Usfm::closeElementNode (xmlNodePtr node)
{
  // The tag and class names of this element node.
  string tagName ((char *) node->name);
  string className;
  xmlChar * property = xmlGetProp (node, BAD_CAST "class");
  if (property) {
    className = (char *) property;
    xmlFree (property);
  }
  
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


void Editor_Html2Usfm::processAttributeNode (xmlNodePtr node)
{
  string tagName ((char *) node->name);
  Database_Logs::log ("Unprocessed XML_ATTRIBUTE_NODE while saving editor text " + tagName);
}


void Editor_Html2Usfm::processTextNode (xmlNodePtr node)
{
  // Add the text to the current USFM line.
  xmlChar * contents = xmlNodeGetContent (node);
  if (contents) {
    string text ((char *) contents);
    currentLine += text;
    xmlFree (contents);
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


void Editor_Html2Usfm::processNoteCitation (xmlNodePtr node)
{
  // Remove the note citation from the text.
  xmlNodePtr child = node->xmlChildrenNode;
  while (child != NULL) {
    xmlNodePtr cache = child;
    child = child->next;
    xmlUnlinkNode (cache);
    xmlFree (cache);
  }
  
  // Get more information about the footnote to retrieve.
  string href;
  string id;
  xmlChar * property = xmlGetProp (node, BAD_CAST "href");
  if (property) {
    href = (char *) property;
    xmlFree (property);
    id = href.substr (1);
  }
  
  // Sample footnote body.
  // <p class="x"><a href="#citation1" id="note1">x</a><span> </span><span>+ 2 Joh. 1.1</span></p>
  // Retrieve the <a> element from it.
  // At first this was done through an XPath expression: 
  // http://www.grinninglizard.com/tinyxml2docs/index.html
  // But XPath crashes on Android.
  // Therefore now it iterates of all the nodes to find the required <a> element.
  xmlNodePtr aElement = get_note_pointer (xmlDocGetRootElement (document), id);
  if (aElement) {

    // It now has the 'a' element: Get its 'p' parent, and then remove that 'a' element.
    // So we remain with:
    // <p class="x"><span> </span><span>+ 2 Joh. 1.1</span></p>
    xmlNodePtr pElement = aElement->parent;
    xmlUnlinkNode (aElement);
    xmlFree (aElement);
    
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
    xmlUnlinkNode (pElement);
    xmlFree (pElement);
    
  } else {
    Database_Logs::log ("Discarding note with id " + id + " and href " + href);
  }
}


string Editor_Html2Usfm::cleanUSFM (string usfm)
{
  // The user may accidentally omit or erase the note caller.
  // The note caller is one character that immediately follows the note opener.
  // E.g.: \f + ...\f*.
  // Check for missing note caller, and if it's not there, add the default "+".
  // Also replace a double space after a note opener.
  for (string noteOpener : noteOpeners) {
    string opener = usfm_get_opening_usfm (noteOpener);
    usfm = filter_string_str_replace (opener + " ", opener, usfm);
    size_t pos = unicode_string_strpos (usfm, opener);
    while (pos != string::npos) {
      bool isClean = true;

      // Check that the character that follows the note opener is a non-space.
      size_t pos2 = pos + unicode_string_length (opener);
      string character = unicode_string_substr (usfm, pos2, 1);
      if (character == " ") isClean = false;
      
      // Check that the following character is not a space.
      pos2++;
      character = unicode_string_substr (usfm, pos2, 1);
      if (character != " ") isClean = false;
      
      // Fix the note caller if necessary.
      if (!isClean) {
        Database_Logs::log ("Fixing note caller in " + usfm);
        pos2--;
        usfm = unicode_string_substr (usfm, 0, pos2) + "+" + unicode_string_substr (usfm, pos2);
      }
      
      // Next iteration.
      pos = unicode_string_strpos (usfm, opener, pos + 5);
    }
  }
  
  return usfm;
}


void Editor_Html2Usfm::preprocess ()
{
  output.clear ();
  currentLine.clear ();
  mono = false;
  //document->encoding = BAD_CAST "UTF-8";
  //document->preserveWhiteSpace = false;
}


// Log errors to the logbook.
void Editor_Html2Usfm::error_handler (void *ctx, const char *msg, ...)
{
  if (ctx) {};
  char buf [256];
  va_list arg_ptr;
  va_start (arg_ptr, msg);
  vsnprintf (buf, 256, msg, arg_ptr);
  va_end (arg_ptr);
  string error (translate ("Saving Editor text") + ": ");
  error.append (buf);
  Database_Logs::log (error);
  return;
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
xmlNodePtr Editor_Html2Usfm::get_note_pointer (xmlNodePtr node, string id)
{
  if (node) {

    if (xmlStrEqual(node->name, BAD_CAST "a")) {
      string note_id;
      xmlChar * property = xmlGetProp (node, BAD_CAST "id");
      if (property) {
        note_id = (char *) property;
        xmlFree (property);
      }
      if (id == note_id) return node;
    }
    
    xmlNodePtr child = node->children;

    while (child != NULL) {
      xmlNodePtr note = get_note_pointer (child, id);
      if (note) return note;
      child = child->next;
    }
  }
  
  return NULL;
}


// This function takes the html from an editor that edits one verse,
// and converts it to USFM.
// It properly deals with cases when a verse does not start a new paragraph.
string editor_export_verse (void * webserver_request, string stylesheet, string html)
{
  // When the $html starts with a paragraph without a style,
  // put a recognizable style there.
  string style = "oneversestyle";
  size_t pos = html.find ("<p>");
  if (pos == 0) {
    html.insert (2, " class=\"" + style + "\"");
  }

  // Convert html to USFM.
  Editor_Html2Usfm editor_export (webserver_request);
  editor_export.load (html);
  editor_export.stylesheet (stylesheet);
  editor_export.run ();
  string usfm = editor_export.get ();

  // Remove that recognizable style converted to USFM.
  usfm = filter_string_str_replace ("\\" + style, "", usfm);
  usfm = filter_string_trim (usfm);
  
  return usfm;
}
