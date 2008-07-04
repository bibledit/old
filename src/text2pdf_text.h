/*
 ** Copyright (©) 2003-2008 Teus Benschop.
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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_USFM2PDF_TEXT_H
#define INCLUDED_USFM2PDF_TEXT_H

#include "libraries.h"

enum T2PTextType { tp2ttParagraph };

class T2PText
{
public:
  T2PText(T2PText * parent_in);
  virtual ~T2PText();
  T2PTextType type;
  ustring text;
private:
  T2PText * parent;
};

/*
class T2PReferenceArea : public T2PArea
{
public:
  T2PReferenceArea(PangoRectangle initial_rectangle, T2PArea * parent_in);
  virtual ~T2PReferenceArea();
private:
};
*/

#endif
