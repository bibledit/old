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

#ifndef INCLUDED_USFM2PDF_AREA_H
#define INCLUDED_USFM2PDF_AREA_H

#include "libraries.h"
#include <pango/pangocairo.h>

enum T2PAreaType { tp2atArea, tp2atReference, tp2atSingleColumn, tp2atBalancedColumn };

class T2PArea
{
public:
  T2PArea(PangoRectangle initial_rectangle, T2PArea * parent_in);
  virtual ~T2PArea();
  PangoRectangle rectangle;
  T2PAreaType type;
private:
  T2PArea * parent;
};

class T2PReferenceArea : public T2PArea
{
public:
  T2PReferenceArea(PangoRectangle initial_rectangle, T2PArea * parent_in);
  virtual ~T2PReferenceArea();
private:
};

class T2PPage
{
public:
  T2PPage(int page_number, int page_width, int page_height, int inside_margin, int outside_margin, int top_margin, int bottom_margin, int header_height, int footer_height);
  virtual ~T2PPage();
  T2PReferenceArea * header_reference_area;
  T2PReferenceArea * text_reference_area;
  T2PReferenceArea * footer_reference_area;
private:
  int number;
};

class T2PSingleColumn : public T2PArea
{
public:
  T2PSingleColumn(PangoRectangle initial_rectangle, T2PArea * parent_in);
  virtual ~T2PSingleColumn();
private:
};

class T2PBalancedColumn : public T2PArea
{
public:
  T2PBalancedColumn(PangoRectangle initial_rectangle, T2PArea * parent_in);
  virtual ~T2PBalancedColumn();
private:
  T2PSingleColumn * start_column;
  T2PSingleColumn * end_column;
};

#endif
