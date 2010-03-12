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


#include "parallel_passages.h"
#include "ot-quotations.h"
#include "settings.h"
#include "ot-nt-parallels.h"


void parallel_passages_retrieve(Reference& reference, vector <Reference>& references, vector <ustring>& comments)
{
  OTQuotations otquotations(0);
  otquotations.get(reference, references, comments);

  vector <Reference> references2;
  vector <ustring > comments2;
  OtNtParallels otntparallels(0);
  otntparallels.get(reference, references2, comments2);

  for (unsigned int i = 0; i < references2.size(); i++) {
    references.push_back(references2[i]);
    comments.push_back(comments2[i]);
  }
}
