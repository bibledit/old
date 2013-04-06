/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_OT_QUOTATIONS_H
#define INCLUDED_OT_QUOTATIONS_H


#include "libraries.h"
#include "reference.h"
#include "mapping.h"


class OTQuotation
{
public:
  OTQuotation (int dummy);
  Reference reference;
  vector <Reference> referents;
  bool lxx;
private:
};


class OTQuotations
{
public:
  OTQuotations (int dummy);
  void read ();
  void get (Reference& reference, vector <Reference>& references, vector <ustring>& comments);
  vector <OTQuotation> quotations_nt_order;
  void produce_in_ot_order();
  vector <OTQuotation> quotations_ot_order;
private:
  ustring comment (const gchar * text, bool lxx);
};


#endif
