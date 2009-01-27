/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#include "text2pdf_utils.h"

#define CENTIMETERS_TO_INCHES_FACTOR 0.393700787        // 1 centimeter = 0.393700787 inches
#define INCHES_TO_POINTS_FACTOR 72.0

int centimeters_to_pango_units(double centimeters)
{
  int pango_units = int (centimeters * CENTIMETERS_TO_INCHES_FACTOR * INCHES_TO_POINTS_FACTOR * PANGO_SCALE);
  return pango_units;
}

int millimeters_to_pango_units(double millimeters)
{
  int pango_units = int (millimeters / 10 * CENTIMETERS_TO_INCHES_FACTOR * INCHES_TO_POINTS_FACTOR * PANGO_SCALE);
  return pango_units;
}

double pango_units_to_millimeters(int pango_units)
{
  double millimeters = double (pango_units) / PANGO_SCALE / INCHES_TO_POINTS_FACTOR / CENTIMETERS_TO_INCHES_FACTOR * 10;
  return millimeters;
}

double pango_units_to_points(int pango_units)
{
  double points = double (pango_units) / PANGO_SCALE;
  return points;
}
