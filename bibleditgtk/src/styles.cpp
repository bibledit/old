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

#include "libraries.h"
#include "styles.h"
#include "stylesheetutils.h"

Styles::Styles(int dummy)
{
  myusfmstandard = NULL;
}

Styles::~Styles()
{
  for (unsigned int i = 0; i < usfms.size(); i++) {
    delete usfms[i];
  }
  if (myusfmstandard)
    delete myusfmstandard;
  for (unsigned int i = 0; i < stylesheets.size(); i++) {
    delete stylesheets[i];
  }
}

Usfm *Styles::usfm(const ustring & stylesheet)
{
  // If these styles have been loaded already, return a pointer to them.
  for (unsigned int i = 0; i < usfms.size(); i++) {
    if (stylesheet == usfms[i]->stylesheet) {
      return usfms[i];
    }
  }
  // If this stylesheet does not exist, create it.
  if (!stylesheet_exists(stylesheet)) {
    stylesheet_create_new(stylesheet, stFull);
  }
  // The styles were not loaded yet: create a new object and return a pointer to it.
  Usfm *usfm = new Usfm(stylesheet);
  usfms.push_back(usfm);
  return usfms[usfms.size() - 1];
}


USFMStandard * Styles::usfmstandard()
{
  // Create the object if it does not exist.
  if (myusfmstandard == NULL) {
    myusfmstandard = new USFMStandard (0);
  }
  // Give the object.
  return myusfmstandard;
}
  

Stylesheet * Styles::stylesheet (const ustring& name)
{
  // If this stylesheet has been loaded already, return a pointer to it.
  for (unsigned int i = 0; i < stylesheets.size(); i++) {
    if (name == stylesheets[i]->name) {
      return stylesheets[i];
    }
  }
  // If this stylesheet does not exist, create it.
  if (!stylesheet_exists(name)) {
    stylesheet_create_new(name, stFull);
  }
  // The stylesheet was not loaded yet. Return a pointer to a new one.
  Stylesheet *stylesheet = new Stylesheet(name);
  stylesheets.push_back(stylesheet);
  return stylesheets[stylesheets.size() - 1];
}


