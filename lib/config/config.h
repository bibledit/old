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


#ifndef INCLUDED_CONFIG_CONFIG_H
#define INCLUDED_CONFIG_CONFIG_H


#define HAVE_UPLOAD 1


#define MAX_PARALLEL_TASKS 10


#ifdef HAVE_WINDOWS
#define HAVE_CLIENT 1
#define HAVE_PARATEXT 1
#undef MAX_PARALLEL_TASKS
#define MAX_PARALLEL_TASKS 1
#endif


#ifdef HAVE_ANDROID
#define HAVE_CLIENT 1
#undef MAX_PARALLEL_TASKS
#define MAX_PARALLEL_TASKS 3
#define HAVE_BARE_BROWSER 1
#endif


#ifdef HAVE_MAC
#define HAVE_CLIENT 1
#define HAVE_BARE_BROWSER 1
#endif


#ifdef HAVE_LINUX
#define HAVE_CLIENT 1
#define HAVE_PARATEXT 1
#endif


#ifdef HAVE_IOS
#define HAVE_CLIENT 1
#undef MAX_PARALLEL_TASKS
#define MAX_PARALLEL_TASKS 3
#define HAVE_BARE_BROWSER 1
#endif


#ifdef HAVE_CHROMEOS
#define HAVE_CLIENT 1
#undef MAX_PARALLEL_TASKS
#define MAX_PARALLEL_TASKS 3
#endif


#endif
