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


#include <html/header.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/url.h>
#include <database/books.h>
#include <html/text.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <locale/translate.h>


// Class for creating a html Bible header with breadcrumbs and search box.


Html_Header::Html_Header (void * html_text)
{
  htmlText = html_text;
}


Html_Header::~Html_Header ()
{
}


void Html_Header::searchBackLink (string url, string text)
{
  searchBackLinkUrl = url;
  searchBackLinkText = text;
}


void Html_Header::create (const vector <pair <string, string> > & breadcrumbs)
{
  Html_Text * html_text = (Html_Text *) htmlText;
  xmlNodePtr tableElement = html_text->newTable ();
  xmlNodePtr tableRowElement = html_text->newTableRow (tableElement);
  xmlNodePtr tableDataElement = html_text->newTableData (tableRowElement);
  bool crumbAdded = false;
  for (auto breadcrumb : breadcrumbs) {
    if (crumbAdded) {
      xmlNodePtr spanElement = xmlNewNode (NULL, BAD_CAST "span");
      xmlNodePtr textnode = xmlNewText(BAD_CAST "»");
      xmlAddChild (spanElement, textnode);
      xmlAddChild (tableDataElement, spanElement);
    }
    html_text->addLink (tableDataElement, breadcrumb.second, "", breadcrumb.first, "", ' ' + breadcrumb.first + ' ');
    crumbAdded = true;
  }
  tableDataElement = html_text->newTableData (tableRowElement, true);
  xmlNodePtr formElement = xmlNewNode (NULL, BAD_CAST "form");
  xmlAddChild (tableDataElement, formElement);
  xmlNewProp (formElement, BAD_CAST "action", BAD_CAST "/webbible/search");
  xmlNewProp (formElement, BAD_CAST "method", BAD_CAST "GET");
  xmlNewProp (formElement, BAD_CAST "name", BAD_CAST "search");
  xmlNewProp (formElement, BAD_CAST "id", BAD_CAST "search");
  xmlNodePtr inputElement = xmlNewNode (NULL, BAD_CAST "input");
  xmlAddChild (formElement, inputElement);
  xmlNewProp (inputElement, BAD_CAST "name", BAD_CAST "q");
  xmlNewProp (inputElement, BAD_CAST "type", BAD_CAST "text");
  xmlNewProp (inputElement, BAD_CAST "placeholder", BAD_CAST translate("Search the Bible").c_str());
  inputElement = xmlNewNode (NULL, BAD_CAST "input");
  xmlAddChild (formElement, inputElement);
  xmlNewProp (inputElement, BAD_CAST "type", BAD_CAST "image");
  xmlNewProp (inputElement, BAD_CAST "name", BAD_CAST "search");
  xmlNewProp (inputElement, BAD_CAST "src", BAD_CAST "lens.png");
  inputElement = xmlNewNode (NULL, BAD_CAST "input");
  xmlAddChild (formElement, inputElement);
  xmlNewProp (inputElement, BAD_CAST "type", BAD_CAST "hidden");
  xmlNewProp (inputElement, BAD_CAST "name", BAD_CAST "url");
  xmlNewProp (inputElement, BAD_CAST "value", BAD_CAST searchBackLinkUrl.c_str ());
  inputElement = xmlNewNode (NULL, BAD_CAST "input");
  xmlAddChild (formElement, inputElement);
  xmlNewProp (inputElement, BAD_CAST "type", BAD_CAST "hidden");
  xmlNewProp (inputElement, BAD_CAST "name", BAD_CAST "text");
  xmlNewProp (inputElement, BAD_CAST "value", BAD_CAST searchBackLinkText.c_str ());
}

