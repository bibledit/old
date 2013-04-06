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

// The code was inspired by software copyrighted (©) 1992 Free Software Foundation, Inc.


#ifndef	FNMATCH_H
#define	FNMATCH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Bits set in the FLAGS argument to `fnmatch'.  */
#undef FNM_PATHNAME
#define	FNM_PATHNAME	(1 << 0)/* No wildcard can ever match `/'.  */
#undef FNM_NOESCAPE
#define	FNM_NOESCAPE	(1 << 1)/* Backslashes don't quote special chars.  */
#undef FNM_PERIOD
#define	FNM_PERIOD	(1 << 2)/* Leading `.' is matched only explicitly.  */
#undef FNM_FLAGS
#define	FNM_FLAGS	(FNM_PATHNAME|FNM_NOESCAPE|FNM_PERIOD)

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#undef FNM_NOMATCH
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
int myfnmatch (const char *pattern, const char *string, int flags);

#ifdef __cplusplus
}
#endif

#endif	/* fnmatch.h */
