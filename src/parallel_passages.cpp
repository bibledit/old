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
#include "references.h"
#include "settings.h"
#include "ot-nt-parallels.h"


void parallel_passages_display(Reference & reference, GtkListStore * liststore, GtkWidget * treeview, GtkTreeViewColumn * treecolumn)
{
  vector < Reference > parallels1;
  vector < ustring > comments1;
  OTQuotations otquotations(0);
  otquotations.get(reference, parallels1, comments1);

  vector < Reference > parallels2;
  vector < ustring > comments2;
  OtNtParallels otntparallels(0);
  otntparallels.get(reference, parallels2, comments2);

  for (unsigned int i = 0; i < parallels2.size(); i++) {
    parallels1.push_back(parallels2[i]);
    comments1.push_back(comments2[i]);
  }

  /* Todo  here.
  References references(liststore, treeview, treecolumn);
  references.set_references(parallels1, comments1);
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  references.fill_store(projectconfig->language_get());
  */

}
