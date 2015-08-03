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


#include <odf/text.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/archive.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <styles/logic.h>


// Class for creating OpenDocument text documents.
// At first the ODF Toolkit was used. But the Java code to generate this became too big for the compiler.
// The other thing is that Java is slow as compared to this method employed here.


Odf_Text::Odf_Text (string bible_in)
{
  bible = bible_in;
  currentTextPDomElement = NULL;
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
  currentTextStyle.clear();
  frameCount = 0;
  noteTextPDomElement = NULL;
  noteCount = 0;
  currentNoteTextStyle.clear();

  // Unpack the .odt template.
  string templateOdf = filter_url_create_root_path ("odf", "template.odt");
  unpackedOdtFolder = filter_archive_unzip (templateOdf);
  filter_url_rmdir (filter_url_create_path (unpackedOdtFolder, "Configurations2"));
  
  initialize_content_xml ();
  initialize_styles_xml ();
}


Odf_Text::~Odf_Text ()
{
  xmlFreeDoc (contentDom);
  xmlFreeDoc (stylesDom);
  filter_url_rmdir (unpackedOdtFolder);
}


// Build the default content.xml for the template.
void Odf_Text::initialize_content_xml ()
{
  contentDom = xmlNewDoc (BAD_CAST "1.0");

  xmlNodePtr rootnode = xmlNewNode (NULL, BAD_CAST "office:document-content");
  xmlDocSetRootElement (contentDom, rootnode);

  xmlNewProp (rootnode, BAD_CAST "xmlns:office", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:style", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:text", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:table", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:draw", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:fo", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:xlink", BAD_CAST "http://www.w3.org/1999/xlink");
  xmlNewProp (rootnode, BAD_CAST "xmlns:dc", BAD_CAST "http://purl.org/dc/elements/1.1/");
  xmlNewProp (rootnode, BAD_CAST "xmlns:meta", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:number", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:svg", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:chart", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:dr3d", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:math", BAD_CAST "http://www.w3.org/1998/Math/MathML");
  xmlNewProp (rootnode, BAD_CAST "xmlns:form", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:script", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:script:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:ooo", BAD_CAST "http://openoffice.org/2004/office");
  xmlNewProp (rootnode, BAD_CAST "xmlns:ooow", BAD_CAST "http://openoffice.org/2004/writer");
  xmlNewProp (rootnode, BAD_CAST "xmlns:oooc", BAD_CAST "http://openoffice.org/2004/calc");
  xmlNewProp (rootnode, BAD_CAST "xmlns:dom", BAD_CAST "http://www.w3.org/2001/xml-events");
  xmlNewProp (rootnode, BAD_CAST "xmlns:xforms", BAD_CAST "http://www.w3.org/2002/xforms");
  xmlNewProp (rootnode, BAD_CAST "xmlns:xsd", BAD_CAST "http://www.w3.org/2001/XMLSchema");
  xmlNewProp (rootnode, BAD_CAST "xmlns:xsi", BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
  xmlNewProp (rootnode, BAD_CAST "xmlns:rpt", BAD_CAST "http://openoffice.org/2005/report");
  xmlNewProp (rootnode, BAD_CAST "xmlns:of", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:of:1.2");
  xmlNewProp (rootnode, BAD_CAST "xmlns:xhtml", BAD_CAST "http://www.w3.org/1999/xhtml");
  xmlNewProp (rootnode, BAD_CAST "xmlns:grddl", BAD_CAST "http://www.w3.org/2003/g/data-view#");
  xmlNewProp (rootnode, BAD_CAST "xmlns:tableooo", BAD_CAST "http://openoffice.org/2009/table");
  xmlNewProp (rootnode, BAD_CAST "xmlns:field", BAD_CAST "urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:formx", BAD_CAST "urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:css3t", BAD_CAST "http://www.w3.org/TR/css3-text/");
  xmlNewProp (rootnode, BAD_CAST "office:version", BAD_CAST "1.2");
  xmlNewProp (rootnode, BAD_CAST "grddl:transformation", BAD_CAST "http://docs.oasis-open.org/office/1.2/xslt/odf2rdf.xsl");

  xmlNodePtr office_scripts = xmlNewNode (NULL, BAD_CAST "office:scripts");
  xmlAddChild (rootnode, office_scripts);

  xmlNodePtr office_font_face_decls = xmlNewNode (NULL, BAD_CAST "office:font-face-decls");
  xmlAddChild (rootnode, office_font_face_decls);
  {
    xmlNodePtr childnode = NULL;

    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Lohit Hindi1");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Lohit Hindi'");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Times New Roman");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Times New Roman'");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "roman");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Arial");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "Arial");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "swiss");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Droid Sans Fallback");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Droid Sans Fallback'");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "system");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Lohit Hindi");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Lohit Hindi'");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "system");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  }

  xmlNodePtr office_automatic_styles = xmlNewNode (NULL, BAD_CAST "office:automatic-styles");
  xmlAddChild (rootnode, office_automatic_styles);
  {
    xmlNodePtr style_style = NULL;
    style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (office_automatic_styles, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "P1");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Header");
    {
      xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
      xmlAddChild (style_style, style_paragraph_properties);
      {
        xmlNodePtr style_tab_stops = xmlNewNode (NULL, BAD_CAST "style:tab-stops");
        xmlAddChild (style_paragraph_properties, style_tab_stops);
        {
          xmlNodePtr style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST "9.005cm");
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "center");
          style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST "17.986cm");
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "right");
        }
      }
    }
    style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (office_automatic_styles, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "P2");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Header_20_left");
    {
      xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
      xmlAddChild (style_style, style_paragraph_properties);
      {
        xmlNodePtr style_tab_stops = xmlNewNode (NULL, BAD_CAST "style:tab-stops");
        xmlAddChild (style_paragraph_properties, style_tab_stops);
        {
          xmlNodePtr style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST "8.193cm");
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "center");
          style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST "17.963cm");
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "right");
        }
      }
    }
  }

  xmlNodePtr office_body = xmlNewNode (NULL, BAD_CAST "office:body");
  xmlAddChild (rootnode, office_body);
  {
    officeTextDomNode = xmlNewNode (NULL, BAD_CAST "office:text");
    xmlAddChild (office_body, officeTextDomNode);
    {
      xmlNodePtr text_sequence_decls = xmlNewNode (NULL, BAD_CAST "text:sequence-decls");
      xmlAddChild (officeTextDomNode, text_sequence_decls);
      {
        xmlNodePtr text_sequence_decl = xmlNewNode (NULL, BAD_CAST "text:sequence-decl");
        xmlAddChild (text_sequence_decls, text_sequence_decl);
        xmlNewProp (text_sequence_decl, BAD_CAST "text:display-outline-level", BAD_CAST "0");
        xmlNewProp (text_sequence_decl, BAD_CAST "text:name", BAD_CAST "Illustration");
        text_sequence_decl = xmlNewNode (NULL, BAD_CAST "text:sequence-decl");
        xmlAddChild (text_sequence_decls, text_sequence_decl);
        xmlNewProp (text_sequence_decl, BAD_CAST "text:display-outline-level", BAD_CAST "0");
        xmlNewProp (text_sequence_decl, BAD_CAST "text:name", BAD_CAST "Table");
        text_sequence_decl = xmlNewNode (NULL, BAD_CAST "text:sequence-decl");
        xmlAddChild (text_sequence_decls, text_sequence_decl);
        xmlNewProp (text_sequence_decl, BAD_CAST "text:display-outline-level", BAD_CAST "0");
        xmlNewProp (text_sequence_decl, BAD_CAST "text:name", BAD_CAST "Text");
        text_sequence_decl = xmlNewNode (NULL, BAD_CAST "text:sequence-decl");
        xmlAddChild (text_sequence_decls, text_sequence_decl);
        xmlNewProp (text_sequence_decl, BAD_CAST "text:display-outline-level", BAD_CAST "0");
        xmlNewProp (text_sequence_decl, BAD_CAST "text:name", BAD_CAST "Drawing");
      }
      // The default text:p element is not added so the document appears empty.
    }
  }
}


// Build the default styles.xml for the template.
void Odf_Text::initialize_styles_xml ()
{
  stylesDom = xmlNewDoc (BAD_CAST "1.0");

  xmlNodePtr rootnode = xmlNewNode (NULL, BAD_CAST "office:document-styles");
  xmlDocSetRootElement (stylesDom, rootnode);

  xmlNewProp (rootnode, BAD_CAST "xmlns:office", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:style", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:text", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:table", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:draw", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:fo", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:xlink", BAD_CAST "http://www.w3.org/1999/xlink");
  xmlNewProp (rootnode, BAD_CAST "xmlns:dc", BAD_CAST "http://purl.org/dc/elements/1.1/");
  xmlNewProp (rootnode, BAD_CAST "xmlns:meta", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:number", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:svg", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:chart", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:dr3d", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:math", BAD_CAST "http://www.w3.org/1998/Math/MathML");
  xmlNewProp (rootnode, BAD_CAST "xmlns:form", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:script", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:script:1.0");
  xmlNewProp (rootnode, BAD_CAST "xmlns:ooo", BAD_CAST "http://openoffice.org/2004/office");
  xmlNewProp (rootnode, BAD_CAST "xmlns:ooow", BAD_CAST "http://openoffice.org/2004/writer");
  xmlNewProp (rootnode, BAD_CAST "xmlns:oooc", BAD_CAST "http://openoffice.org/2004/calc");
  xmlNewProp (rootnode, BAD_CAST "xmlns:dom", BAD_CAST "http://www.w3.org/2001/xml-events");
  xmlNewProp (rootnode, BAD_CAST "xmlns:rpt", BAD_CAST "http://openoffice.org/2005/report");
  xmlNewProp (rootnode, BAD_CAST "xmlns:of", BAD_CAST "urn:oasis:names:tc:opendocument:xmlns:of:1.2");
  xmlNewProp (rootnode, BAD_CAST "xmlns:xhtml", BAD_CAST "http://www.w3.org/1999/xhtml");
  xmlNewProp (rootnode, BAD_CAST "xmlns:grddl", BAD_CAST "http://www.w3.org/2003/g/data-view#");
  xmlNewProp (rootnode, BAD_CAST "xmlns:tableooo", BAD_CAST "http://openoffice.org/2009/table");
  xmlNewProp (rootnode, BAD_CAST "xmlns:css3t", BAD_CAST "http://www.w3.org/TR/css3-text/");
  xmlNewProp (rootnode, BAD_CAST "office:version", BAD_CAST "1.2");
  xmlNewProp (rootnode, BAD_CAST "grddl:transformation", BAD_CAST "http://docs.oasis-open.org/office/1.2/xslt/odf2rdf.xsl");

  xmlNodePtr office_font_face_decls = xmlNewNode (NULL, BAD_CAST "office:font-face-decls");
  xmlAddChild (rootnode, office_font_face_decls);
  {
    xmlNodePtr childnode = NULL;

    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Lohit Hindi1");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Lohit Hindi'");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Times New Roman");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Times New Roman'");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "roman");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Arial");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "Arial");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "swiss");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Droid Sans Fallback");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Droid Sans Fallback'");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "system");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  
    childnode = xmlNewNode (NULL, BAD_CAST "style:font-face");
    xmlAddChild (office_font_face_decls, childnode);
    xmlNewProp (childnode, BAD_CAST "style:name", BAD_CAST "Lohit Hindi");
    xmlNewProp (childnode, BAD_CAST "svg:font-family", BAD_CAST "'Lohit Hindi'");
    xmlNewProp (childnode, BAD_CAST "style:font-family-generic", BAD_CAST "system");
    xmlNewProp (childnode, BAD_CAST "style:font-pitch", BAD_CAST "variable");
  }

  officeStylesDomNode = xmlNewNode (NULL, BAD_CAST "office:styles");
  xmlAddChild (rootnode, officeStylesDomNode);
  {
    xmlNodePtr style_default_style = xmlNewNode (NULL, BAD_CAST "style:default-style");
    xmlAddChild (officeStylesDomNode, style_default_style);
    xmlNewProp (style_default_style, BAD_CAST "style:family", BAD_CAST "graphic");
    xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
    xmlAddChild (style_default_style, style_paragraph_properties);
    xmlNodePtr style_text_properties = xmlNewNode (NULL, BAD_CAST "style:text-properties");
    xmlAddChild (style_default_style, style_text_properties);
  }
  {
    xmlNodePtr style_default_style = xmlNewNode (NULL, BAD_CAST "style:default-style");
    xmlAddChild (officeStylesDomNode, style_default_style);
    xmlNewProp (style_default_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
    xmlAddChild (style_default_style, style_paragraph_properties);
    xmlNodePtr style_text_properties = xmlNewNode (NULL, BAD_CAST "style:text-properties");
    xmlAddChild (style_default_style, style_text_properties);
  }
  {
    xmlNodePtr style_default_style = xmlNewNode (NULL, BAD_CAST "style:default-style");
    xmlAddChild (officeStylesDomNode, style_default_style);
    xmlNewProp (style_default_style, BAD_CAST "style:family", BAD_CAST "table");
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (officeStylesDomNode, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST styles_logic_standard_sheet ().c_str());
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:class", BAD_CAST "text");
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (officeStylesDomNode, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "Heading");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST styles_logic_standard_sheet ().c_str());
    xmlNewProp (style_style, BAD_CAST "style:next-style-name", BAD_CAST "Text_20_body");
    xmlNewProp (style_style, BAD_CAST "style:class", BAD_CAST "text");
    {
      xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
      xmlAddChild (style_style, style_paragraph_properties);
      xmlNewProp (style_paragraph_properties, BAD_CAST "fo:margin-top", BAD_CAST "0.423cm");
      xmlNewProp (style_paragraph_properties, BAD_CAST "fo:margin-bottom", BAD_CAST "0.212cm");
      xmlNewProp (style_paragraph_properties, BAD_CAST "fo:keep-with-next", BAD_CAST "always");
    }
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (officeStylesDomNode, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "Text_20_body");
    xmlNewProp (style_style, BAD_CAST "style:display-name", BAD_CAST "Text body");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST styles_logic_standard_sheet ().c_str());
    xmlNewProp (style_style, BAD_CAST "style:class", BAD_CAST "text");
    {
      xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
      xmlAddChild (style_style, style_paragraph_properties);
      xmlNewProp (style_paragraph_properties, BAD_CAST "fo:margin-top", BAD_CAST "0cm");
      xmlNewProp (style_paragraph_properties, BAD_CAST "fo:margin-bottom", BAD_CAST "0.212cm");
    }
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (officeStylesDomNode, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "Header");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST styles_logic_standard_sheet ().c_str());
    xmlNewProp (style_style, BAD_CAST "style:class", BAD_CAST "extra");
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (officeStylesDomNode, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "Header_20_left");
    xmlNewProp (style_style, BAD_CAST "style:display-name", BAD_CAST "Header left");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST styles_logic_standard_sheet ().c_str());
    xmlNewProp (style_style, BAD_CAST "style:class", BAD_CAST "extra");
  }

  // Update the tab-stops in the header style. The tab stops depend on page and margin dimensions.
  int centerPosition = convert_to_int (Database_Config_Bible::getPageWidth (bible)) - convert_to_int (Database_Config_Bible::getInnerMargin (bible)) - convert_to_int (Database_Config_Bible::getOuterMargin (bible));

  xmlNodePtr office_automatic_styles = xmlNewNode (NULL, BAD_CAST "office:automatic-styles");
  xmlAddChild (rootnode, office_automatic_styles);
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (office_automatic_styles, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "MP1");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Header");
    {
      xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
      xmlAddChild (style_style, style_paragraph_properties);
      {
        xmlNodePtr style_tab_stops = xmlNewNode (NULL, BAD_CAST "style:tab-stops");
        xmlAddChild (style_paragraph_properties, style_tab_stops);
        centerPosition /= 2;
        {
          xmlNodePtr style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST convert_to_string (convert_to_string (centerPosition) + "mm").c_str());
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "center");
        }
        {
          xmlNodePtr style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST convert_to_string (convert_to_string (centerPosition * 2) + "mm").c_str());
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "right");
        }
      }
    }
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (office_automatic_styles, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "MP2");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Header_20_left");
    {
      xmlNodePtr style_paragraph_properties = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
      xmlAddChild (style_style, style_paragraph_properties);
      {
        xmlNodePtr style_tab_stops = xmlNewNode (NULL, BAD_CAST "style:tab-stops");
        xmlAddChild (style_paragraph_properties, style_tab_stops);
        {
          xmlNodePtr style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST convert_to_string (convert_to_string (centerPosition) + "mm").c_str());
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "center");
        }
        {
          xmlNodePtr style_tab_stop = xmlNewNode (NULL, BAD_CAST "style:tab-stop");
          xmlAddChild (style_tab_stops, style_tab_stop);
          xmlNewProp (style_tab_stop, BAD_CAST "style:position", BAD_CAST convert_to_string (convert_to_string (centerPosition * 2) + "mm").c_str());
          xmlNewProp (style_tab_stop, BAD_CAST "style:type", BAD_CAST "right");
        }
      }
    }
  }
  {
    xmlNodePtr style_page_layout = xmlNewNode (NULL, BAD_CAST "style:page-layout");
    xmlAddChild (office_automatic_styles, style_page_layout);
    xmlNewProp (style_page_layout, BAD_CAST "style:name", BAD_CAST "Mpm1");
    xmlNewProp (style_page_layout, BAD_CAST "style:page-usage", BAD_CAST "mirrored");
    {
      xmlNodePtr style_page_layout_properties = xmlNewNode (NULL, BAD_CAST "style:page-layout-properties");
      xmlAddChild (style_page_layout, style_page_layout_properties);
      // Take the page size and margins from the Bible's settings.
      xmlNewProp (style_page_layout_properties, BAD_CAST "fo:page-width", BAD_CAST convert_to_string (Database_Config_Bible::getPageWidth (bible) + "mm").c_str());
      xmlNewProp (style_page_layout_properties, BAD_CAST "fo:page-height", BAD_CAST convert_to_string (Database_Config_Bible::getPageHeight (bible) + "mm").c_str());
      xmlNewProp (style_page_layout_properties, BAD_CAST "style:num-format", BAD_CAST "1");
      xmlNewProp (style_page_layout_properties, BAD_CAST "style:print-orientation", BAD_CAST "portrait");
      xmlNewProp (style_page_layout_properties, BAD_CAST "fo:margin-top", BAD_CAST convert_to_string (Database_Config_Bible::getTopMargin (bible) + "mm").c_str());
      xmlNewProp (style_page_layout_properties, BAD_CAST "fo:margin-bottom", BAD_CAST convert_to_string (Database_Config_Bible::getBottomMargin (bible) + "mm").c_str());
      xmlNewProp (style_page_layout_properties, BAD_CAST "fo:margin-left", BAD_CAST convert_to_string (Database_Config_Bible::getInnerMargin (bible) + "mm").c_str());
      xmlNewProp (style_page_layout_properties, BAD_CAST "fo:margin-right", BAD_CAST convert_to_string (Database_Config_Bible::getOuterMargin (bible) + "mm").c_str());
      xmlNewProp (style_page_layout_properties, BAD_CAST "style:writing-mode", BAD_CAST "lr-tb");
      xmlNewProp (style_page_layout_properties, BAD_CAST "style:footnote-max-height", BAD_CAST "0cm");
      {
        xmlNodePtr style_footnote_sep = xmlNewNode (NULL, BAD_CAST "style:footnote-sep");
        xmlAddChild (style_page_layout_properties, style_footnote_sep);
        xmlNewProp (style_footnote_sep, BAD_CAST "style:width", BAD_CAST "0.018cm");
        xmlNewProp (style_footnote_sep, BAD_CAST "style:distance-before-sep", BAD_CAST "0.101cm");
        xmlNewProp (style_footnote_sep, BAD_CAST "style:distance-after-sep", BAD_CAST "0.101cm");
        xmlNewProp (style_footnote_sep, BAD_CAST "style:adjustment", BAD_CAST "left");
        xmlNewProp (style_footnote_sep, BAD_CAST "style:rel-width", BAD_CAST "25%");
        xmlNewProp (style_footnote_sep, BAD_CAST "style:color", BAD_CAST "#000000");
      }
    }
    {
      xmlNodePtr style_header_style = xmlNewNode (NULL, BAD_CAST "style:header-style");
      xmlAddChild (style_page_layout, style_header_style);
      {
        xmlNodePtr style_header_footer_properties = xmlNewNode (NULL, BAD_CAST "style:header-footer-properties");
        xmlAddChild (style_header_style, style_header_footer_properties);
        xmlNewProp (style_header_footer_properties, BAD_CAST "fo:min-height", BAD_CAST "0.799cm");
        xmlNewProp (style_header_footer_properties, BAD_CAST "fo:margin-left", BAD_CAST "0cm");
        xmlNewProp (style_header_footer_properties, BAD_CAST "fo:margin-right", BAD_CAST "0cm");
        xmlNewProp (style_header_footer_properties, BAD_CAST "fo:margin-bottom", BAD_CAST "0.3cm");
        xmlNewProp (style_header_footer_properties, BAD_CAST "style:dynamic-spacing", BAD_CAST "false");
      }
      xmlNodePtr style_footer_style = xmlNewNode (NULL, BAD_CAST "style:footer-style");
      xmlAddChild (style_page_layout, style_footer_style);
    }
  }
  {
    // Do not write the date style for the running headers, so that it takes the default style.
    /*
    xmlNodePtr number_date_style = xmlNewNode (NULL, BAD_CAST "number:date-style");
    xmlAddChild (office_automatic_styles, number_date_style);
    xmlNewProp (number_date_style, BAD_CAST "style:name", BAD_CAST "N81");
    {
      xmlNodePtr number_day = xmlNewNode (NULL, BAD_CAST "number:day");
      xmlAddChild (number_date_style, number_day);
      xmlNodePtr number_text = xmlNewNode (NULL, BAD_CAST "number:text");
      xmlAddChild (number_date_style, number_text);
      {
        xmlNodePtr textnode = xmlNewText(BAD_CAST ".");
        xmlAddChild (number_text, textnode);
      }
      xmlNodePtr number_month = xmlNewNode (NULL, BAD_CAST "number:month");
      xmlAddChild (number_date_style, number_month);
      {
        xmlNewProp (number_month, BAD_CAST "number:style", BAD_CAST "long");
        xmlNewProp (number_month, BAD_CAST "number:textual", BAD_CAST "true");
      }
      number_text = xmlNewNode (NULL, BAD_CAST "number:text");
      xmlAddChild (number_date_style, number_text);
      xmlNodePtr number_year = xmlNewNode (NULL, BAD_CAST "number:year");
      xmlAddChild (number_date_style, number_year);
      {
        xmlNewProp (number_year, BAD_CAST "number:style", BAD_CAST "long");
      }
    }
    */
  }
  xmlNodePtr office_master_styles = xmlNewNode (NULL, BAD_CAST "office:master-styles");
  xmlAddChild (rootnode, office_master_styles);
  {
    xmlNodePtr style_master_page = xmlNewNode (NULL, BAD_CAST "style:master-page");
    xmlAddChild (office_master_styles, style_master_page);
    xmlNewProp (style_master_page, BAD_CAST "style:name", BAD_CAST styles_logic_standard_sheet ().c_str());
    xmlNewProp (style_master_page, BAD_CAST "style:page-layout-name", BAD_CAST "Mpm1");
    {
      xmlNodePtr style_header = xmlNewNode (NULL, BAD_CAST "style:header");
      xmlAddChild (style_master_page, style_header);
      {
        xmlNodePtr text_p = xmlNewNode (NULL, BAD_CAST "text:p");
        xmlAddChild (style_header, text_p);
        xmlNewProp (text_p, BAD_CAST "text:style-name", BAD_CAST "MP1");
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:page-number");
          xmlAddChild (text_p, node);
          xmlNewProp (node, BAD_CAST "text:select-page", BAD_CAST "current");
          xmlNodePtr textnode = xmlNewText(BAD_CAST "1");
          xmlAddChild (node, textnode);
        }
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:tab");
          xmlAddChild (text_p, node);
        }
        // Whether and how to put the date in the running headers.
        if (Database_Config_Bible::getDateInHeader (bible)) {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:date");
          xmlAddChild (text_p, node);
          xmlNewProp (node, BAD_CAST "style:data-style-name", BAD_CAST "N81");
          xmlNewProp (node, BAD_CAST "text:date-value", BAD_CAST "");
          xmlNodePtr textnode = xmlNewText(BAD_CAST "");
          xmlAddChild (node, textnode);
        }
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:tab");
          xmlAddChild (text_p, node);
        }
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:chapter");
          xmlAddChild (text_p, node);
          xmlNewProp (node, BAD_CAST "text:display", BAD_CAST "name");
          xmlNewProp (node, BAD_CAST "text:outline-level", BAD_CAST "1");
        }
      }
      xmlNodePtr style_header_left = xmlNewNode (NULL, BAD_CAST "style:header-left");
      xmlAddChild (style_master_page, style_header_left);
      {
        xmlNodePtr text_p = xmlNewNode (NULL, BAD_CAST "text:p");
        xmlAddChild (style_header_left, text_p);
        xmlNewProp (text_p, BAD_CAST "text:style-name", BAD_CAST "MP2");
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:chapter");
          xmlAddChild (text_p, node);
          xmlNewProp (node, BAD_CAST "text:display", BAD_CAST "name");
          xmlNewProp (node, BAD_CAST "text:outline-level", BAD_CAST "1");
        }
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:tab");
          xmlAddChild (text_p, node);
        }
        // Whether and how to put the date in the running headers.
        if (Database_Config_Bible::getDateInHeader (bible)) {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:date");
          xmlAddChild (text_p, node);
          xmlNewProp (node, BAD_CAST "style:data-style-name", BAD_CAST "N81");
          xmlNewProp (node, BAD_CAST "text:date-value", BAD_CAST "");
          xmlNodePtr textnode = xmlNewText(BAD_CAST "");
          xmlAddChild (node, textnode);
        }
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:tab");
          xmlAddChild (text_p, node);
        }
        {
          xmlNodePtr node = xmlNewNode (NULL, BAD_CAST "text:page-number");
          xmlAddChild (text_p, node);
          xmlNewProp (node, BAD_CAST "text:select-page", BAD_CAST "current");
          xmlNodePtr textnode = xmlNewText(BAD_CAST "1");
          xmlAddChild (node, textnode);
        }
      }
    }
  }
}


void Odf_Text::newParagraph (string style)
{
  currentTextPDomElement = xmlNewNode (NULL, BAD_CAST "text:p");
  currentTextPDomElementNameNode = xmlNewProp (currentTextPDomElement, BAD_CAST "text:style-name", BAD_CAST style.c_str());
  xmlAddChild (officeTextDomNode, currentTextPDomElement);
  currentParagraphStyle = style;
  currentParagraphContent.clear();
}


// This function adds text to the current paragraph.
// $text: The text to add.
void Odf_Text::addText (string text)
{
  // Bail out if there's no text.
  if (text.empty()) return;

  // Ensure a paragraph has started.
  if (!currentTextPDomElement) newParagraph ();
  
  // Temporal styles array should have at least one style for the code below to work.
  vector <string> styles (currentTextStyle.begin (), currentTextStyle.end ());
  if (styles.empty()) styles.push_back ("");
  
  // Write a text span element, nesting the second and later ones.
  xmlNodePtr domElement = currentTextPDomElement;
  for (string style : styles) {
    xmlNodePtr textSpanDomElement = xmlNewNode (NULL, BAD_CAST "text:span");
    xmlAddChild (domElement, textSpanDomElement);
    if (!style.empty ()) {
      xmlNewProp (textSpanDomElement, BAD_CAST "text:style-name", BAD_CAST convertStyleName (style).c_str());
    }
    domElement = textSpanDomElement;
  }
  xmlNodePtr textnode = xmlNewText(BAD_CAST filter_string_sanitize_html (text).c_str());
  xmlAddChild (domElement, textnode);
  
  // Update public paragraph text.
  currentParagraphContent += text;
}


// This creates a heading with contents styled "Heading 1".
// $text: Contents.
void Odf_Text::newHeading1 (string text, bool hide)
{
  newNamedHeading ("Heading 1", text, hide);
}


// This creates the page break style.
void Odf_Text::createPageBreakStyle ()
{
  // This is how the style looks in styles.xml:
  // <style:style style:display-name="Page Break" style:family="paragraph" style:name="Page_20_Break">
  // <style:paragraph-properties fo:break-after="page" fo:line-height="0.05cm" fo:margin-bottom="0cm" fo:margin-top="0cm"/>
  // <style:text-properties fo:font-size="2pt" style:font-size-asian="2pt" style:font-size-complex="2pt"/>
  // </style:style>
  xmlNodePtr styleDomElement = xmlNewNode (NULL, BAD_CAST "style:style");
  xmlNewProp (styleDomElement, BAD_CAST "style:name", BAD_CAST "Page_20_Break");
  xmlNewProp (styleDomElement, BAD_CAST "style:display-name", BAD_CAST "Page Break");
  xmlNewProp (styleDomElement, BAD_CAST "style:family", BAD_CAST "paragraph");
  xmlAddChild (officeStylesDomNode, styleDomElement);

  xmlNodePtr styleParagraphPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:break-after", BAD_CAST "page");
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:line-height", BAD_CAST "0.05cm");
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:margin-bottom", BAD_CAST "0cm");
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:margin-top", BAD_CAST "0cm");
  xmlAddChild (styleDomElement, styleParagraphPropertiesDomElement);

  xmlNodePtr styleTextPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:text-properties");
  xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size", BAD_CAST "2pt");
  xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-asian", BAD_CAST "2pt");
  xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-complex", BAD_CAST "2pt");
  xmlAddChild (styleDomElement, styleTextPropertiesDomElement);
}


// This applies a page break.
void Odf_Text::newPageBreak ()
{
  newParagraph ("Page_20_Break");
  // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
  // since it would be nearly invisible, and thus text would mysteriously get lost.
  currentTextPDomElement = NULL;
  currentParagraphStyle.clear ();
  currentParagraphContent.clear ();
}


// This creates a paragraph style.
// $name: the name of the style, e.g. 'p'.
// $dropcaps: If 0, there are no drop caps.
//            If greater than 0, it the number of characters in drop caps style.
void Odf_Text::createParagraphStyle (string name, float fontsize, int italic, int bold, int underline, int smallcaps, int alignment, float spacebefore, float spaceafter, float leftmargin, float rightmargin, float firstlineindent, bool keepWithNext, int dropcaps)
{
  // It looks like this in styles.xml:
  // <style:style style:display-name="p_c1" style:family="paragraph" style:name="p_c1">
  //   <style:paragraph-properties fo:margin-bottom="0mm" fo:margin-left="0mm" fo:margin-right="0mm" fo:margin-top="0mm" fo:text-align="justify" fo:text-indent="0mm"/>
  //     <style:drop-cap style:distance="0.15cm" style:length="1" style:lines="2"/>
  //   <style:paragraph-properties>
  //   <style:text-properties fo:font-size="12pt" style:font-size-asian="12pt" style:font-size-complex="12pt"/>
  // </style:style>
  xmlNodePtr styleDomElement = xmlNewNode (NULL, BAD_CAST "style:style");
  xmlNewProp (styleDomElement, BAD_CAST "style:name", BAD_CAST convertStyleName (name).c_str());
  xmlNewProp (styleDomElement, BAD_CAST "style:display-name", BAD_CAST name.c_str());
  xmlNewProp (styleDomElement, BAD_CAST "style:family", BAD_CAST "paragraph");
  xmlAddChild (officeStylesDomNode, styleDomElement);

  xmlNodePtr styleParagraphPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
  xmlAddChild (styleDomElement, styleParagraphPropertiesDomElement);

  xmlNodePtr styleTextPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:text-properties");
  xmlAddChild (styleDomElement, styleTextPropertiesDomElement);

  string sfontsize = convert_to_string (fontsize) + "pt";
  xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size", BAD_CAST sfontsize.c_str());
  xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-asian", BAD_CAST sfontsize.c_str());
  xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-complex", BAD_CAST sfontsize.c_str());

  // Italics, bold, underline, small caps can be either ooitOff or ooitOn for a paragraph.
  if (italic != ooitOff) {
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style", BAD_CAST "italic");
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style-asian", BAD_CAST "italic");
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style-complex", BAD_CAST "italic");
  }
  if (bold != ooitOff) {
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight", BAD_CAST "bold");
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-asian", BAD_CAST "bold");
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-complex", BAD_CAST "bold");
  }
  if (underline != ooitOff) {
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-underline-style", BAD_CAST "solid");
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-underline-width", BAD_CAST "auto");
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-underline-color", BAD_CAST "font-color");
  }
  if (smallcaps != ooitOff) {
    xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-variant", BAD_CAST "small-caps");
  }

  // Text alignment can be: AlignmentLeft, AlignmentCenter, AlignmentRight, AlignmentJustify.
  string alignmenttext = "";
  switch (alignment) {
    case AlignmentLeft:    alignmenttext = "start"; break;
    case AlignmentCenter:  alignmenttext = "center"; break;
    case AlignmentRight:   alignmenttext = "end"; break;
    case AlignmentJustify: alignmenttext = "justify"; break;
  }
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:text-align", BAD_CAST alignmenttext.c_str());
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "style:justify-single-word", BAD_CAST "false");

  // Paragraph measurements; given in mm.
  string sspacebefore = convert_to_string (spacebefore) + "mm";
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:margin-top", BAD_CAST sspacebefore.c_str());
  string sspaceafter = convert_to_string (spaceafter) + "mm";
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:margin-bottom", BAD_CAST sspaceafter.c_str());
  string sleftmargin = convert_to_string (leftmargin) + "mm";
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:margin-left", BAD_CAST sleftmargin.c_str());
  string srightmargin = convert_to_string (rightmargin) + "mm";
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:margin-right", BAD_CAST srightmargin.c_str());
  string sfirstlineindent = convert_to_string (firstlineindent) + "mm";
  xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:text-indent", BAD_CAST sfirstlineindent.c_str());

  if (keepWithNext) {
    xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:keep-together", BAD_CAST "always");
    xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:keep-with-next", BAD_CAST "always");
  }

  if (dropcaps > 0) {
    // E.g.: <style:drop-cap style:lines="2" style:length="2" style:distance="0.15cm"/>
    string length = convert_to_string (dropcaps);
    xmlNodePtr styleDropCapDomElement = xmlNewNode (NULL, BAD_CAST "style:drop-cap");
    xmlAddChild (styleParagraphPropertiesDomElement, styleDropCapDomElement);
    xmlNewProp (styleDropCapDomElement, BAD_CAST "style:lines", BAD_CAST "2");
    xmlNewProp (styleDropCapDomElement, BAD_CAST "style:length", BAD_CAST length.c_str());
    xmlNewProp (styleDropCapDomElement, BAD_CAST "style:distance", BAD_CAST "0.15cm");
  }
}


// This updates the style name of the current paragraph.
// $name: the name of the style, e.g. 'p'.
void Odf_Text::updateCurrentParagraphStyle (string name)
{
  if (!currentTextPDomElement) newParagraph ();
  xmlRemoveProp (currentTextPDomElementNameNode);
  currentTextPDomElementNameNode = xmlNewProp (currentTextPDomElement, BAD_CAST "text:style-name", BAD_CAST convertStyleName (name).c_str());
}


// This opens a text style.
// $style: the object with the style variables.
// $note: Whether this refers to notes.
// $embed: boolean: Whether nest $style in an existing character style.
void Odf_Text::openTextStyle (Database_Styles_Item style, bool note, bool embed)
{
  string marker = style.marker;
  if (find (createdStyles.begin(), createdStyles.end(), marker) == createdStyles.end()) {
    int italic = style.italic;
    int bold = style.bold;
    int underline = style.underline;
    int smallcaps = style.smallcaps;
    int superscript = style.superscript;
    string color = style.color;
    createdStyles.push_back (marker);

    // The style entry looks like this in styles.xml, e.g., for italic:
    // <style:style style:name="T1" style:family="text">
    // <style:text-properties fo:font-style="italic" style:font-style-asian="italic" style:font-style-complex="italic"/>
    // </style:style>
    xmlNodePtr styleDomElement = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlNewProp (styleDomElement, BAD_CAST "style:name", BAD_CAST convertStyleName (marker).c_str());
    xmlNewProp (styleDomElement, BAD_CAST "style:display-name", BAD_CAST marker.c_str());
    xmlNewProp (styleDomElement, BAD_CAST "style:family", BAD_CAST "text");
    xmlAddChild (officeStylesDomNode, styleDomElement);

    xmlNodePtr styleTextPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:text-properties");
    xmlAddChild (styleDomElement, styleTextPropertiesDomElement);

    // Italics, bold, underline, small caps can be ooitOff or ooitOn or ooitInherit or ooitToggle.
    // Not all features are implemented.
    if ((italic == ooitOn) || (italic == ooitToggle)) {
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style", BAD_CAST "italic");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style-asian", BAD_CAST "italic");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style-complex", BAD_CAST "italic");
    }
    if ((bold == ooitOn) || (bold == ooitToggle)) {
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight", BAD_CAST "bold");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-asian", BAD_CAST "bold");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-complex", BAD_CAST "bold");
    }
    if ((underline == ooitOn) || (underline == ooitToggle)) {
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-underline-style", BAD_CAST "solid");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-underline-width", BAD_CAST "auto");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-underline-color", BAD_CAST "font-color");
    }
    if ((smallcaps == ooitOn) || (smallcaps == ooitToggle)) {
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-variant", BAD_CAST "small-caps");
    }

    if (superscript) {
      //$styleTextPropertiesDomElement->setAttribute ("style:text-position", "super 58%");
      // If the percentage is not specified, an appropriate font height is used.
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-position", BAD_CAST "super");
      // The mere setting of the superscript value makes the font smaller. No need to set it manually.
      //$styleTextPropertiesDomElement->setAttribute ("fo:font-size", "87%");
      //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "87%");
      //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "87%");
    }

    if (color != "#000000") {
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:color", BAD_CAST color.c_str());
    }

  }

  if (note) {
    if (!embed) currentNoteTextStyle.clear();
    currentNoteTextStyle.push_back (marker);
  } else {
    if (!embed) currentTextStyle.clear ();
    currentTextStyle.push_back (marker);
  }
}


// This closes any open text style.
// $note: Whether this refers to notes.
// $embed: boolean: Whether to close embedded style.
void Odf_Text::closeTextStyle (bool note, bool embed)
{
  if (note) {
    if (!embed) currentNoteTextStyle.clear();
    if (!currentNoteTextStyle.empty ()) currentNoteTextStyle.pop_back ();
  } else {
    if (!embed) currentTextStyle.clear();
    if (!currentTextStyle.empty()) currentTextStyle.pop_back ();
  }
}


// This places text in a frame in OpenDocument.
// It does all the housekeeping to get it display properly.
// $text - the text to place in the frame.
// $style - the name of the style of the $text.
// $fontsize - given in points.
// $italic, $bold - integer values.
void Odf_Text::placeTextInFrame (string text, string style, float fontsize, int italic, int bold)
{
  // Empty text is discarded.
  if (text.empty ()) return;

  // The frame goes in an existing paragraph (text:p) element, just like a 'text:span' element.
  // Ensure that a paragraph is open.
  if (!currentTextPDomElement) newParagraph ();

  // The frame looks like this, in content.xml:
  // <draw:frame draw:style-name="fr1" draw:name="frame1" text:anchor-type="paragraph" svg:y="0cm" fo:min-width="0.34cm" draw:z-index="0">
  //   <draw:text-box fo:min-height="0.34cm">
  //     <text:p text:style-name="c">1</text:p>
  //   </draw:text-box>
  // </draw:frame>
  xmlNodePtr drawFrameDomElement = xmlNewNode (NULL, BAD_CAST "draw:frame");
  xmlAddChild (currentTextPDomElement, drawFrameDomElement);
  xmlNewProp (drawFrameDomElement, BAD_CAST "draw:style-name", BAD_CAST "chapterframe");
  frameCount++;
  xmlNewProp (drawFrameDomElement, BAD_CAST "draw:name", BAD_CAST convert_to_string ("frame" + convert_to_string (frameCount)).c_str());
  xmlNewProp (drawFrameDomElement, BAD_CAST "text:anchor-type", BAD_CAST "paragraph");
  xmlNewProp (drawFrameDomElement, BAD_CAST "svg:y", BAD_CAST "0cm");
  xmlNewProp (drawFrameDomElement, BAD_CAST "fo:min-width", BAD_CAST "0.34cm");
  xmlNewProp (drawFrameDomElement, BAD_CAST "draw:z-index", BAD_CAST "0");

  xmlNodePtr drawTextBoxDomElement = xmlNewNode (NULL, BAD_CAST "draw:text-box");
  xmlAddChild (drawFrameDomElement, drawTextBoxDomElement);
  xmlNewProp (drawTextBoxDomElement, BAD_CAST "fo:min-height", BAD_CAST "0.34cm");

  xmlNodePtr textPDomElement = xmlNewNode (NULL, BAD_CAST "text:p");
  xmlAddChild (drawTextBoxDomElement, textPDomElement);
  xmlNewProp (textPDomElement, BAD_CAST "text:style-name", BAD_CAST convertStyleName (style).c_str());
  xmlNodePtr textnode = xmlNewText(BAD_CAST filter_string_sanitize_html (text).c_str());
  xmlAddChild (textPDomElement, textnode);

  // File styles.xml contains the appropriate styles for this frame and text box and paragraph.
  // Create the styles once for the whole document.
  if (find (createdStyles.begin(), createdStyles.end (), style) == createdStyles.end()) {
    createdStyles.push_back (style);
    
    {
      // The style for the text:p element looks like this:
      // <style:style style:name="c" style:family="paragraph">
      //   <style:paragraph-properties fo:text-align="justify" style:justify-single-word="false"/>
      //   <style:text-properties fo:font-size="24pt" fo:font-weight="bold" style:font-size-asian="24pt" style:font-weight-asian="bold" style:font-size-complex="24pt" style:font-weight-complex="bold"/>
      // </style:style>
      xmlNodePtr styleDomElement = xmlNewNode (NULL, BAD_CAST "style:style");
      xmlAddChild (officeStylesDomNode, styleDomElement);
      xmlNewProp (styleDomElement, BAD_CAST "style:name", BAD_CAST convertStyleName (style).c_str());
      xmlNewProp (styleDomElement, BAD_CAST "style:family", BAD_CAST "paragraph");
  
      xmlNodePtr styleParagraphPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:paragraph-properties");
      xmlAddChild (styleDomElement, styleParagraphPropertiesDomElement);
      xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "fo:text-align", BAD_CAST "justify");
      xmlNewProp (styleParagraphPropertiesDomElement, BAD_CAST "style:justify-single-word", BAD_CAST "false");
  
      xmlNodePtr styleTextPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:text-properties");
      xmlAddChild (styleDomElement, styleTextPropertiesDomElement);
      string sfontsize = convert_to_string (fontsize) + "pt";
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size", BAD_CAST sfontsize.c_str());
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-asian", BAD_CAST sfontsize.c_str());
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-complex", BAD_CAST sfontsize.c_str());
      if (italic != ooitOff) {
        xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style", BAD_CAST "italic");
        xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style-asian", BAD_CAST "italic");
        xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-style-complex", BAD_CAST "italic");
      }
      if (bold != ooitOff) {
        xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight", BAD_CAST "bold");
        xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-asian", BAD_CAST "bold");
        xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-complex", BAD_CAST "bold");
      }
    }
    {
      // The style for the draw:frame element looks like this:
      // <style:style style:name="chapterframe" style:family="graphic" style:parent-style-name="ChapterFrameParent">
      //   <style:graphic-properties fo:margin-left="0cm" fo:margin-right="0.199cm" fo:margin-top="0cm" fo:margin-bottom="0cm" style:vertical-pos="from-top" style:vertical-rel="paragraph-content" style:horizontal-pos="left" style:horizontal-rel="paragraph" fo:background-color="transparent" style:background-transparency="100%" fo:padding="0cm" fo:border="none" style:shadow="none" style:flow-with-text="true">
      //   <style:background-image/>
      //   </style:graphic-properties>
      // </style:style>
      xmlNodePtr styleDomElement = xmlNewNode (NULL, BAD_CAST "style:style");
      xmlAddChild (officeStylesDomNode, styleDomElement);
      xmlNewProp (styleDomElement, BAD_CAST "style:name", BAD_CAST "chapterframe");
      xmlNewProp (styleDomElement, BAD_CAST "style:family", BAD_CAST "graphic");
  
      xmlNodePtr styleGraphicPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:graphic-properties");
      xmlAddChild (styleDomElement, styleGraphicPropertiesDomElement);
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "fo:margin-left", BAD_CAST "0cm");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "fo:margin-right", BAD_CAST "0.2cm");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "fo:margin-top", BAD_CAST "0cm");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "fo:margin-bottom", BAD_CAST "0cm");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "style:vertical-pos", BAD_CAST "from-top");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "style:vertical-rel", BAD_CAST "paragraph-content");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "style:horizontal-pos", BAD_CAST "left");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "style:horizontal-rel", BAD_CAST "paragraph");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "fo:background-color", BAD_CAST "transparent");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "style:background-transparency", BAD_CAST "100%");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "fo:padding", BAD_CAST "0cm");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "fo:border", BAD_CAST "none");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "style:shadow", BAD_CAST "none");
      xmlNewProp (styleGraphicPropertiesDomElement, BAD_CAST "style:flow-with-text", BAD_CAST "true");
    }
  }

}


// This creates the superscript style.
void Odf_Text::createSuperscriptStyle ()
{
  // The style entry looks like this in styles.xml:
  // <style:style style:name="superscript" style:family="text">
  //   <style:text-properties style:text-position="super 58%"/>
  // </style:style>
  xmlNodePtr styleDomElement = xmlNewNode (NULL, BAD_CAST "style:style");
  xmlNewProp (styleDomElement, BAD_CAST "style:name", BAD_CAST "superscript");
  xmlNewProp (styleDomElement, BAD_CAST "style:family", BAD_CAST "text");
  xmlAddChild (officeStylesDomNode, styleDomElement);

  xmlNodePtr styleTextPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:text-properties");
  xmlAddChild (styleDomElement, styleTextPropertiesDomElement);
  //$styleTextPropertiesDomElement->setAttribute ("style:text-position", "super 58%");
  // If the percentage is not specified, an appropriate font height is used.
  xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "style:text-position", BAD_CAST "super");
  // Setting the superscript attribute automatically makes the font smaller. No need to set it manually.
  //$styleTextPropertiesDomElement->setAttribute ("fo:font-size", "87%");
  //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "87%");
  //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "87%");
}


// This function adds a note to the current paragraph.
// $caller: The text of the note caller, that is, the note citation.
// $style: Style name for the paragraph in the footnote body.
// $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
void Odf_Text::addNote (string caller, string style, bool endnote)
{
  // Ensure that a paragraph is open, so that the note can be added to it.
  if (!currentTextPDomElement) newParagraph ();

  xmlNodePtr textNoteDomElement = xmlNewNode (NULL, BAD_CAST "text:note");
  xmlAddChild (currentTextPDomElement, textNoteDomElement);
  xmlNewProp (textNoteDomElement, BAD_CAST "text:id", BAD_CAST convert_to_string ("ftn" + convert_to_string (noteCount)).c_str());
  noteCount++;
  string noteclass;
  if (endnote) noteclass = "endnote";
  else noteclass = "footnote";
  xmlNewProp (textNoteDomElement, BAD_CAST "text:note-class", BAD_CAST noteclass.c_str());

  // The note citation, the 'caller' is normally in superscript in the OpenDocument.
  // The default values of the application are used. The Bibledit stylesheet is not consulted.
  xmlNodePtr textNoteCitationDomElement = xmlNewNode (NULL, BAD_CAST "text:note-citation");
  xmlAddChild (textNoteDomElement, textNoteCitationDomElement);
  xmlNewProp (textNoteCitationDomElement, BAD_CAST "text:label", BAD_CAST filter_string_sanitize_html (caller).c_str());
  xmlNodePtr textnode = xmlNewText(BAD_CAST filter_string_sanitize_html (caller).c_str());
  xmlAddChild (textNoteCitationDomElement, textnode);

  xmlNodePtr textNoteBodyDomElement = xmlNewNode (NULL, BAD_CAST "text:note-body");
  xmlAddChild (textNoteDomElement, textNoteBodyDomElement);

  noteTextPDomElement = xmlNewNode (NULL, BAD_CAST "text:p");
  xmlAddChild (textNoteBodyDomElement, noteTextPDomElement);
  xmlNewProp (noteTextPDomElement, BAD_CAST "text:style-name", BAD_CAST convertStyleName (style).c_str());

  closeTextStyle (true, false);
}


// This function adds text to the current footnote.
// $text: The text to add.
void Odf_Text::addNoteText (string text)
{
  // Bail out if there's no text.
  if (text == "") return;

  // Ensure a note has started.
  if (!noteTextPDomElement) addNote ("?", "");

  // Temporal styles array should have at least one style for the code below to work.
  vector <string> styles (currentNoteTextStyle.begin(), currentNoteTextStyle.end());
  if (styles.empty ()) styles.push_back ("");

  // Write a text span element, nesting the second and later ones.
  xmlNodePtr domElement = noteTextPDomElement;
  for (string style : styles) {
    xmlNodePtr textSpanDomElement = xmlNewNode (NULL, BAD_CAST "text:span");
    xmlAddChild (domElement, textSpanDomElement);
    if (!style.empty()) {
      xmlNewProp (textSpanDomElement, BAD_CAST "text:style-name", BAD_CAST convertStyleName (style).c_str());
    }
    domElement = textSpanDomElement;
  }
  xmlNodePtr textnode = xmlNewText(BAD_CAST filter_string_sanitize_html (text).c_str());
  xmlAddChild (domElement, textnode);
}


// This function closes the current footnote.
void Odf_Text::closeCurrentNote ()
{
  closeTextStyle (true, false);
  noteTextPDomElement = NULL;
}


// This creates a heading with styled content.
// $style: A style name.
// $text: Content.
void Odf_Text::newNamedHeading (string style, string text, bool hide)
{
  // Heading looks like this in content.xml:
  // <text:h text:style-name="Heading_20_1" text:outline-level="1">Text</text:h>
  xmlNodePtr textHDomElement = xmlNewNode (NULL, BAD_CAST "text:h");
  xmlNewProp (textHDomElement, BAD_CAST "text:style-name", BAD_CAST convertStyleName (style).c_str());
  xmlNewProp (textHDomElement, BAD_CAST "text:outline-level", BAD_CAST "1");
  xmlAddChild (officeTextDomNode, textHDomElement);
  xmlNodePtr textnode = xmlNewText(BAD_CAST filter_string_sanitize_html (text).c_str());
  xmlAddChild (textHDomElement, textnode);

  // Heading style looks like this in styles.xml:
  // <style:style style:name="Heading_20_1" style:display-name="Heading 1" style:family="paragraph" style:parent-style-name="Heading" style:next-style-name="Text_20_body" style:default-outline-level="1" style:class="text">
  // <style:text-properties fo:font-size="115%" fo:font-weight="bold" style:font-size-asian="115%" style:font-weight-asian="bold" style:font-size-complex="115%" style:font-weight-complex="bold"/>
  // </style:style>
  // Create the style if it does not yet exist.
  if (find (createdStyles.begin(), createdStyles.end (), style) == createdStyles.end()) {
    xmlNodePtr styleDomElement = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlNewProp (styleDomElement, BAD_CAST "style:name", BAD_CAST convertStyleName (style).c_str());
    xmlNewProp (styleDomElement, BAD_CAST "style:display-name", BAD_CAST style.c_str());
    xmlNewProp (styleDomElement, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (styleDomElement, BAD_CAST "style:parent-style-name", BAD_CAST "Heading");
    xmlNewProp (styleDomElement, BAD_CAST "style:next-style-name", BAD_CAST "Text_20_body");
    xmlNewProp (styleDomElement, BAD_CAST "style:default-outline-level", BAD_CAST "1");
    xmlNewProp (styleDomElement, BAD_CAST "style:class", BAD_CAST "text");
    xmlAddChild (officeStylesDomNode, styleDomElement);
    {
      xmlNodePtr styleTextPropertiesDomElement = xmlNewNode (NULL, BAD_CAST "style:text-properties");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size", BAD_CAST "115%");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight", BAD_CAST "bold");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-asian", BAD_CAST "115%");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-asian", BAD_CAST "bold");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-size-complex", BAD_CAST "115%");
      xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "fo:font-weight-complex", BAD_CAST "bold");
      if (hide) {
        xmlNewProp (styleTextPropertiesDomElement, BAD_CAST "text:display", BAD_CAST "none");
      }
      xmlAddChild (styleDomElement, styleTextPropertiesDomElement);
    }
    createdStyles.push_back (style);
  }

  // Make paragraph null, so that adding subsequent text creates a new paragraph.
  currentTextPDomElement = NULL;
  currentParagraphStyle.clear ();
  currentParagraphContent.clear ();
}


// This converts the name of a style so that it is fit for use in OpenDocument files.
// E.g. 'Heading 1' becomes 'Heading_20_1'
// $style: Input
// It returns the converted style name.
string Odf_Text::convertStyleName (string style)
{
  style = filter_string_str_replace (" ", "_20_", style);
  return style;
}


// This saves the OpenDocument to file
// $name: the name of the file to save to.
void Odf_Text::save (string name)
{
  // Create the content.xml file.
  // No formatting because some white space is processed. $this->contentDom->formatOutput = true;
  string contentXml = filter_url_create_path (unpackedOdtFolder, "content.xml");
  xmlSaveFormatFileEnc (contentXml.c_str(), contentDom, "UTF-8", 0);

  // Create the styles.xml file.
  // No formatting because some white space is processed. $this->stylesDom->formatOutput = true;
  string stylesXml = filter_url_create_path (unpackedOdtFolder, "styles.xml");
  xmlSaveFormatFileEnc (stylesXml.c_str(), stylesDom, "UTF-8", 0);

  // Save the OpenDocument file.
  string zippedfile = filter_archive_zip_folder (unpackedOdtFolder);
  filter_url_file_put_contents (name, filter_url_file_get_contents (zippedfile));
  filter_url_unlink (zippedfile);
}


