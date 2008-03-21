/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "hyphenate.h"


/*
Some information about how XEP does hyphenation follows below:


The entity for a nonbreaking hyphen is as in these names: 
's&#8209;Gravenhage or 's&#x2011;Gravenhage.

There are hyphenation files available from Tex/LaTex, with rules in them.
But bibledit does not use them.

In XEP every paragraph that gets hyphenated automatically needs: hyphenate="true"

XEP 4.4 uses Unicode soft hyphen characters (U+00AD) (&#173;) to mark possible 
hyphenation points. These characters can be contained in the source XSL FO 
document (e.g. come from an external hyphenation software). XEP can also add 
them inside: it contains a hyphenator class that inserts soft hyphens to all 
text data before they are passed to the formatter.
Bibledit only uses the soft hyphen character.


Unicode has a non-breaking hyphen character - &#x2011; - This is &#8209;.
There is also the normal hyphen, &#8208;. 
This can be used in combination with the soft hyphen, so it only breaks once.


Generally, to hyphenate, in the .fo file, set hyphenate="false", and use soft 
hyphens.


*/
