/*
Copyright (©) 2003-2016 Teus Benschop.

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


#ifndef INCLUDED_USER_LOGIC_H
#define INCLUDED_USER_LOGIC_H


#include <config/libraries.h>


#define PLATFORM_WINDOWS 1
#define PLATFORM_ANDROID 2
#define PLATFORM_MAC 3
#define PLATFORM_LINUX 4
#define PLATFORM_IOS 5
#define PLATFORM_CHROMEOS 6
#define PLATFORM_CLOUD 7


void user_logic_software_updates_notify ();


#endif
