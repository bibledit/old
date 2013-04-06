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

// The code was copied from software written and copyrighted by the gtkmm Development Team


#ifndef INCLUDED_USTRING_UTILITY_H
#define INCLUDED_USTRING_UTILITY_H

#include "ustring.h"
#include <glib.h>

// Helper to deal with memory allocated
// by GLib functions in an exception-safe manner.
template <typename T>
class GlibScopedPtr
{
private:
  T* ptr_;
  GlibScopedPtr(const GlibScopedPtr<T>&);
  GlibScopedPtr<T>& operator=(const GlibScopedPtr<T>&);

public:
  GlibScopedPtr()                 : ptr_ (0)   {}
  explicit GlibScopedPtr(T* ptr)  : ptr_ (ptr) {}
  ~GlibScopedPtr()                { g_free(ptr_); }
  T*  get() const             { return ptr_;  }
  T** addr()                  { return &ptr_; }
};

// Removes the const nature of a ptr
template <class T>
inline T* unconst(const T* t)
  { return const_cast<T*>(t); }

// Convert const gchar* to ustring, while treating NULL as empty string.
inline
ustring convert_const_gchar_ptr_to_ustring(const char* str)
{
  return (str) ? ustring(str) : ustring();
}

// Convert const gchar* to std::string, while treating NULL as empty string.
inline
std::string convert_const_gchar_ptr_to_stdstring(const char* str)
{
  return (str) ? std::string(str) : std::string();
}

// Convert a non-const gchar* return value to ustring, freeing it too.
inline
ustring convert_return_gchar_ptr_to_ustring(char* str)
{
  return (str) ? ustring(GlibScopedPtr<char>(str).get())
               : ustring();
}

// Convert a non-const gchar* return value to std::string, freeing it too.
inline
std::string convert_return_gchar_ptr_to_stdstring(char* str)
{
  return (str) ? std::string(GlibScopedPtr<char>(str).get())
               : std::string();
}

// Append type_name to dest, while replacing special characters with '+'.
void append_canonical_typename(std::string& dest, const char* type_name);

#endif /* INCLUDED_USTRING_UTILITY_H */

