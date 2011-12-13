/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#ifndef INCLUDED_SETTINGS_H
#define INCLUDED_SETTINGS_H


#include "libraries.h"
#include "session.h"
#include "settings.h"
#include "generalconfig.h"
#include "projectconfig.h"


class Settings
{
public:
  Settings (bool save_on_destroy);
  ~Settings ();
  Session session;
  GeneralConfiguration genconfig;
  ProjectConfiguration * projectconfig (ustring project, bool save_on_destroy = true);
  void save ();
private:
  vector <ProjectConfiguration *> projectconfigurations;
  unsigned int event_id;
  static bool on_timeout(gpointer user_data);
  void timeout();
};


#endif
