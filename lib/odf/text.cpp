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


#include <odf/text.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/archive.h>
#include <database/books.h>
#include <database/config/bible.h>


// Class for creating OpenDocument text documents.
// At first the ODF Toolkit was used. But the Java code to generate this became too big for the compiler.
// The other thing is that Java is slow as compared to this method employed here.


Odf_Text::Odf_Text (string bible_in)
{
  bible = bible_in;
  currentParagraphStyle.clear();
  currentParagraphContent.clear();
  currentTextStyle.clear();
  frameCount = 0;
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
  // Todo filter_url_rmdir (unpackedOdtFolder);
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
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "Standard");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:class", BAD_CAST "text");
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (officeStylesDomNode, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "Heading");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Standard");
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
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Standard");
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
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Standard");
    xmlNewProp (style_style, BAD_CAST "style:class", BAD_CAST "extra");
  }
  {
    xmlNodePtr style_style = xmlNewNode (NULL, BAD_CAST "style:style");
    xmlAddChild (officeStylesDomNode, style_style);
    xmlNewProp (style_style, BAD_CAST "style:name", BAD_CAST "Header_20_left");
    xmlNewProp (style_style, BAD_CAST "style:display-name", BAD_CAST "Header left");
    xmlNewProp (style_style, BAD_CAST "style:family", BAD_CAST "paragraph");
    xmlNewProp (style_style, BAD_CAST "style:parent-style-name", BAD_CAST "Standard");
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
    xmlNewProp (style_master_page, BAD_CAST "style:name", BAD_CAST "Standard");
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


/* Todo C++Port


public function newParagraph ($style = "Standard")
{
  $this->currentTextPDomElement = $this->contentDom->createElement ("text:p");
  $this->currentTextPDomElementNameNode = $this->currentTextPDomElement->setAttribute ("text:style-name", $style);
  $this->officeTextDomNode->appendChild ($this->currentTextPDomElement);
  $this->currentParagraphStyle = $style;
  $this->currentParagraphContent = "";
}


// This function adds text to the current paragraph.
// $text: The text to add.
public function addText ($text)
{
  // Bail out if there's no text.
  if ($text == "") return;

  // Ensure a paragraph has started.
  if (!isset ($this->currentTextPDomElement)) {
    $this->newParagraph ();
  }
  
  // Temporal styles array should have at least one style for the code below to work.
  $styles = $this->currentTextStyle;
  if (empty ($styles)) $styles [] = "";
  
  // Write a text span element, nesting the second and later ones.
  $domElement = $this->currentTextPDomElement;
  foreach ($styles as $style) {
    $textSpanDomElement = $this->contentDom->createElement ("text:span");
    $domElement->appendChild ($textSpanDomElement);
    if ($style) {
      $textSpanDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
    }
    $domElement = $textSpanDomElement;
  }
  $domElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
  
  // Update public paragraph text.
  $this->currentParagraphContent .= $text;
}


// This creates a heading with contents styled "Heading 1".
// $text: Contents.
public function newHeading1($text, $hide = false)
{
  $this->newNamedHeading ("Heading 1", $text, $hide);
}
*/

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

/*
// This applies a page break.
public function newPageBreak ()
{
  $this->newParagraph ("Page_20_Break");
  // Always make the paragraph null, because we don't want subsequent text to be added to this page break,
  // since it would be nearly invisible, and thus text would mysteriously get lost.
  unset ($this->currentTextPDomElement);
  $this->currentParagraphStyle = "";
  $this->currentParagraphContent = "";
}


// This creates a paragraph style.
// $name: the name of the style, e.g. 'p'.
// $dropcaps: If 0, there are no drop caps.
//            If greater than 0, it the number of characters in drop caps style.
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
    $styleTextPropertiesDomElement->setAttribute ("fo:font-style-asian", "italic");
    $styleTextPropertiesDomElement->setAttribute ("fo:font-style-complex", "italic");
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


// This updates the style name of the current paragraph.
// $name: the name of the style, e.g. 'p'.
public function updateCurrentParagraphStyle ($name)
{
  if (!isset ($this->currentTextPDomElement)) {
    $this->newParagraph ();
  }
  $this->currentTextPDomElement->removeAttributeNode ($this->currentTextPDomElementNameNode);
  $this->currentTextPDomElementNameNode = $this->currentTextPDomElement->setAttribute ("text:style-name", $this->convertStyleName ($name));
}


// This opens a text style.
// $style: the array containing the style variables.
// $note: Whether this refers to notes.
// $embed: boolean: Whether nest $style in an existing character style.
public function openTextStyle ($style, $note, $embed)
{
  $marker = $style["marker"];
  if (!in_array ($marker, $this->createdStyles)) {
    $italic = $style["italic"];
    $bold = $style["bold"];
    $underline = $style["underline"];
    $smallcaps = $style["smallcaps"];
    $superscript = $style["superscript"];
    $color = $style["color"];
    $this->createdStyles [] = $marker;

    // The style entry looks like this in styles.xml, e.g., for italic:
    // <style:style style:name="T1" style:family="text">
    // <style:text-properties fo:font-style="italic" style:font-style-asian="italic" style:font-style-complex="italic"/>
    // </style:style>
    $styleDomElement = $this->stylesDom->createElement ("style:style");
    $styleDomElement->setAttribute ("style:name", $this->convertStyleName ($marker));
    $styleDomElement->setAttribute ("style:display-name", $marker);
    $styleDomElement->setAttribute ("style:family", "text");
    $this->officeStylesDomNode->appendChild ($styleDomElement);

    $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
    $styleDomElement->appendChild ($styleTextPropertiesDomElement);

    // Italics, bold, underline, small caps can be ooitOff or ooitOn or ooitInherit or ooitToggle.
    // Not all features are implemented.
    if (($italic == ooitOn) || ($italic == ooitToggle)) {
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style", "italic");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style-asian", "italic");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style-complex", "italic");
    }
    if (($bold == ooitOn) || ($bold == ooitToggle)) {
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight", "bold");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-asian", "bold");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-complex", "bold");
    }
    if (($underline == ooitOn) || ($underline == ooitToggle)) {
      $styleTextPropertiesDomElement->setAttribute ("style:text-underline-style", "solid");
      $styleTextPropertiesDomElement->setAttribute ("style:text-underline-width", "auto");
      $styleTextPropertiesDomElement->setAttribute ("style:text-underline-color", "font-color");
    }
    if (($smallcaps == ooitOn) || ($smallcaps == ooitToggle)) {
      $styleTextPropertiesDomElement->setAttribute ("fo:font-variant", "small-caps");
    }

    if ($superscript) {
      //$styleTextPropertiesDomElement->setAttribute ("style:text-position", "super 58%");
      // If the percentage is not specified, an appropriate font height is used.
      $styleTextPropertiesDomElement->setAttribute ("style:text-position", "super");
      // The mere setting of the superscript value makes the font smaller. No need to set it manually.
      //$styleTextPropertiesDomElement->setAttribute ("fo:font-size", "87%");
      //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "87%");
      //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "87%");
    }

    if ($color != "000000") {
      $styleTextPropertiesDomElement->setAttribute ("fo:color", "#$color");
    }

  }

  if ($note) {
    if (!$embed) $this->currentNoteTextStyle = array ();
    $this->currentNoteTextStyle [] = $marker;
  } else {
    if (!$embed) $this->currentTextStyle = array ();
    $this->currentTextStyle [] = $marker;
  }
}



// This closes any open text style.
// $note: Whether this refers to notes.
// $embed: boolean: Whether to close embedded style.
public function closeTextStyle ($note, $embed)
{
  if ($note) {
    if (!$embed) $this->currentNoteTextStyle = array ();
    if (count ($this->currentNoteTextStyle)) array_pop ($this->currentNoteTextStyle);
  } else {
    if (!$embed) $this->currentTextStyle = array ();
    if (count ($this->currentTextStyle)) array_pop ($this->currentTextStyle);
  }
}



// This places text in a frame in OpenDocument.
// It does all the housekeeping to get it display properly.
// $text - the text to place in the frame.
// $style - the name of the style of the $text.
// $fontsize - given in points.
// $italic, $bold - boolean values.
public function placeTextInFrame ($text, $style, $fontsize, $italic, $bold)
{
  // Empty text is discarded.
  if ($text == "") return;

  // The frame goes in an existing paragraph (text:p) element, just like a 'text:span' element.
  // Ensure that a paragraph is open.
  if (!isset ($this->currentTextPDomElement)) {
    $this->newParagraph ();
  }

  // The frame looks like this, in content.xml:
  // <draw:frame draw:style-name="fr1" draw:name="frame1" text:anchor-type="paragraph" svg:y="0cm" fo:min-width="0.34cm" draw:z-index="0">
  //   <draw:text-box fo:min-height="0.34cm">
  //     <text:p text:style-name="c">1</text:p>
  //   </draw:text-box>
  // </draw:frame>
  $drawFrameDomElement = $this->contentDom->createElement ("draw:frame");
  $this->currentTextPDomElement->appendChild ($drawFrameDomElement);
  $drawFrameDomElement->setAttribute ("draw:style-name", "chapterframe");
  $this->frameCount++;
  $drawFrameDomElement->setAttribute ("draw:name", "frame" . $this->frameCount);
  $drawFrameDomElement->setAttribute ("text:anchor-type", "paragraph");
  $drawFrameDomElement->setAttribute ("svg:y", "0cm");
  $drawFrameDomElement->setAttribute ("fo:min-width", "0.34cm");
  $drawFrameDomElement->setAttribute ("draw:z-index", "0");

  $drawTextBoxDomElement = $this->contentDom->createElement ("draw:text-box");
  $drawFrameDomElement->appendChild ($drawTextBoxDomElement);
  $drawTextBoxDomElement->setAttribute ("fo:min-height", "0.34cm");

  $textPDomElement = $this->contentDom->createElement ("text:p");
  $drawTextBoxDomElement->appendChild ($textPDomElement);
  $textPDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
  $textPDomElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");

  // File styles.xml contains the appropriate styles for this frame and text box and paragraph.
  // Create the styles once for the whole document.
  if (!in_array ($style, $this->createdStyles)) {

    // The style for the text:p element looks like this:
    // <style:style style:name="c" style:family="paragraph">
    //   <style:paragraph-properties fo:text-align="justify" style:justify-single-word="false"/>
    //   <style:text-properties fo:font-size="24pt" fo:font-weight="bold" style:font-size-asian="24pt" style:font-weight-asian="bold" style:font-size-complex="24pt" style:font-weight-complex="bold"/>
    // </style:style>
    $styleDomElement = $this->stylesDom->createElement ("style:style");
    $this->officeStylesDomNode->appendChild ($styleDomElement);
    $styleDomElement->setAttribute ("style:name", $this->convertStyleName ($style));
    $styleDomElement->setAttribute ("style:family", "paragraph");

    $styleParagraphPropertiesDomElement = $this->stylesDom->createElement ("style:paragraph-properties");
    $styleDomElement->appendChild ($styleParagraphPropertiesDomElement);
    $styleParagraphPropertiesDomElement->setAttribute ("fo:text-align", "justify");
    $styleParagraphPropertiesDomElement->setAttribute ("style:justify-single-word", "false");

    $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
    $styleDomElement->appendChild ($styleTextPropertiesDomElement);
    $fontsize .= "pt";
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size", $fontsize);
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", $fontsize);
    $styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", $fontsize);
    if ($italic != ooitOff) {
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style", "italic");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style-asian", "italic");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-style-complex", "italic");
    }
    if ($bold != ooitOff) {
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight", "bold");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-asian", "bold");
      $styleTextPropertiesDomElement->setAttribute ("fo:font-weight-complex", "bold");
    }

    // The style for the draw:frame element looks like this:
    // <style:style style:name="chapterframe" style:family="graphic" style:parent-style-name="ChapterFrameParent">
    //   <style:graphic-properties fo:margin-left="0cm" fo:margin-right="0.199cm" fo:margin-top="0cm" fo:margin-bottom="0cm" style:vertical-pos="from-top" style:vertical-rel="paragraph-content" style:horizontal-pos="left" style:horizontal-rel="paragraph" fo:background-color="transparent" style:background-transparency="100%" fo:padding="0cm" fo:border="none" style:shadow="none" style:flow-with-text="true">
    //   <style:background-image/>
    //   </style:graphic-properties>
    // </style:style>
    $styleDomElement = $this->stylesDom->createElement ("style:style");
    $this->officeStylesDomNode->appendChild ($styleDomElement);
    $styleDomElement->setAttribute ("style:name", "chapterframe");
    $styleDomElement->setAttribute ("style:family", "graphic");

    $styleGraphicPropertiesDomElement = $this->stylesDom->createElement ("style:graphic-properties");
    $styleDomElement->appendChild ($styleGraphicPropertiesDomElement);
    $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-left", "0cm");
    $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-right", "0.2cm");
    $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-top", "0cm");
    $styleGraphicPropertiesDomElement->setAttribute ("fo:margin-bottom", "0cm");
    $styleGraphicPropertiesDomElement->setAttribute ("style:vertical-pos", "from-top");
    $styleGraphicPropertiesDomElement->setAttribute ("style:vertical-rel", "paragraph-content");
    $styleGraphicPropertiesDomElement->setAttribute ("style:horizontal-pos", "left");
    $styleGraphicPropertiesDomElement->setAttribute ("style:horizontal-rel", "paragraph");
    $styleGraphicPropertiesDomElement->setAttribute ("fo:background-color", "transparent");
    $styleGraphicPropertiesDomElement->setAttribute ("style:background-transparency", "100%");
    $styleGraphicPropertiesDomElement->setAttribute ("fo:padding", "0cm");
    $styleGraphicPropertiesDomElement->setAttribute ("fo:border", "none");
    $styleGraphicPropertiesDomElement->setAttribute ("style:shadow", "none");
    $styleGraphicPropertiesDomElement->setAttribute ("style:flow-with-text", "true");
  }

}


// This creates the superscript style.
public function createSuperscriptStyle ()
{
  // The style entry looks like this in styles.xml:
  // <style:style style:name="superscript" style:family="text">
  //   <style:text-properties style:text-position="super 58%"/>
  // </style:style>
  $styleDomElement = $this->stylesDom->createElement ("style:style");
  $styleDomElement->setAttribute ("style:name", "superscript");
  $styleDomElement->setAttribute ("style:family", "text");
  $this->officeStylesDomNode->appendChild ($styleDomElement);

  $styleTextPropertiesDomElement = $this->stylesDom->createElement ("style:text-properties");
  $styleDomElement->appendChild ($styleTextPropertiesDomElement);
  //$styleTextPropertiesDomElement->setAttribute ("style:text-position", "super 58%");
  // If the percentage is not specified, an appropriate font height is used.
  $styleTextPropertiesDomElement->setAttribute ("style:text-position", "super");
  // The mere setting of the superscript value makes the font smaller. No need to set it manually.
  //$styleTextPropertiesDomElement->setAttribute ("fo:font-size", "87%");
  //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-asian", "87%");
  //$styleTextPropertiesDomElement->setAttribute ("fo:font-size-complex", "87%");
}


// This function adds a note to the current paragraph.
// $caller: The text of the note caller, that is, the note citation.
// $style: Style name for the paragraph in the footnote body.
// $endnote: Whether this is a footnote and cross reference (false), or an endnote (true).
public function addNote ($caller, $style, $endnote = false)
{
  // Ensure that a paragraph is open, so that the note can be added to it.
  if (!isset ($this->currentTextPDomElement)) {
    $this->newParagraph ();
  }

  $textNoteDomElement = $this->contentDom->createElement ("text:note");
  $this->currentTextPDomElement->appendChild ($textNoteDomElement);
  $textNoteDomElement->setAttribute ("text:id", "ftn" . $this->noteCount);
  $this->noteCount++;
  if ($endnote) $noteclass = "endnote";
  else $noteclass = "footnote";
  $textNoteDomElement->setAttribute ("text:note-class", $noteclass);

  // The note citation, the 'caller' is normally in superscript in the OpenDocument.
  // The default values of the application are used. The Bibledit-Web stylesheet is not consulted.
  $textNoteCitationDomElement = $this->contentDom->createElement ("text:note-citation");
  $textNoteDomElement->appendChild ($textNoteCitationDomElement);
  $textNoteCitationDomElement->setAttribute ("text:label", htmlspecialchars ($caller, ENT_QUOTES, "UTF-8"));
  $textNoteCitationDomElement->nodeValue = htmlspecialchars ($caller, ENT_QUOTES, "UTF-8");

  $textNoteBodyDomElement = $this->contentDom->createElement ("text:note-body");
  $textNoteDomElement->appendChild ($textNoteBodyDomElement);

  $this->noteTextPDomElement = $this->contentDom->createElement ("text:p");
  $textNoteBodyDomElement->appendChild ($this->noteTextPDomElement);
  $this->noteTextPDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));

  $this->closeTextStyle (true, false);
}


// This function adds text to the current footnote.
// $text: The text to add.
public function addNoteText ($text)
{
  // Bail out if there's no text.
  if ($text == "") return;

  // Ensure a note has started.
  if (!isset ($this->noteTextPDomElement)) {
    $this->addNote ("?", "");
  }

  // Temporal styles array should have at least one style for the code below to work.
  $styles = $this->currentNoteTextStyle;
  if (empty ($styles)) $styles [] = "";

  // Write a text span element, nesting the second and later ones.
  $domElement = $this->noteTextPDomElement;
  foreach ($styles as $style) {
    $textSpanDomElement = $this->contentDom->createElement ("text:span");
    $domElement->appendChild ($textSpanDomElement);
    if ($style) {
      $textSpanDomElement->setAttribute ("text:style-name", $this->convertStyleName ($style));
    }
    $domElement = $textSpanDomElement;
  }
  $domElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
}


// This function closes the current footnote.
public function closeCurrentNote ()
{
  $this->closeTextStyle (true, false);
  $this->noteTextPDomElement = NULL;
}


// This creates a heading with styled content.
// $style: A style name.
// $text: Content.
private function newNamedHeading ($style, $text, $hide = false)
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
      if ($hide) {
        $styleTextPropertiesDomElement->setAttribute ("text:display", "none");
      }
      $styleDomElement->appendChild ($styleTextPropertiesDomElement);
    }
    $this->createdStyles [] = $style;
  }

  // Make paragraph null, so that adding subsequent text creates a new paragraph.
  unset ($this->currentTextPDomElement);
  $this->currentParagraphStyle = "";
  $this->currentParagraphContent = "";
}


// This converts the name of a style so that it is fit for use in OpenDocument files.
// E.g. 'Heading 1' becomes 'Heading_20_1'
// $style: Input
// It returns the converted style name.
private function convertStyleName ($style)
{
  $style = str_replace (" ", "_20_", $style);
  return $style;
}


*/

// This saves the OpenDocument to file
// $name: the name of the file to save to.
void Odf_Text::save (string name)
{
  // Create the content.xml file.
  // No formatting because some white space is processed. $this->contentDom->formatOutput = true;
  // Todo $string = $this->contentDom->save ($this->unpackedOdtFolder . "/content.xml");
  string contentXml = filter_url_create_path (unpackedOdtFolder, "content.xml");
  xmlSaveFormatFileEnc (contentXml.c_str(), contentDom, "UTF-8", 0);

  // Create the styles.xml file.
  // No formatting because some white space is processed. $this->stylesDom->formatOutput = true;
  // Todo $string = $this->stylesDom->save ($this->unpackedOdtFolder . "/styles.xml");
  string stylesXml = filter_url_create_path (unpackedOdtFolder, "styles.xml");
  xmlSaveFormatFileEnc (stylesXml.c_str(), stylesDom, "UTF-8", 0);

  // Save the OpenDocument file.
  string zippedfile = filter_archive_zip_folder (unpackedOdtFolder);
  filter_url_file_put_contents (name, filter_url_file_get_contents (zippedfile));
  filter_url_unlink (zippedfile);
}


