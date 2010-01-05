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


#include "xetex.h"
#include "unixwrappers.h"
#include "gwrappers.h"
#include "directories.h"


// Tex editors: lyx, kile, texmaker


XeTeX::XeTeX(int dummy)
{
	create_work_area ();
}


XeTeX::~XeTeX()
{
}


void XeTeX::create_work_area ()
{
  working_directory = gw_build_filename (directories_get_temp (), "xetex");
	unix_rmdir (working_directory);
	gw_mkdir_with_parents (working_directory);
}



/*

Todo xetex


One dialog that does the settings. Several tabs could be used.
Information may be in the dialog that describe how to set certain things, such as font in the View menu, and
other settings in the stylesheet.

One routine, in file xetex, that does the conversion from our format to the format required by XeTeX, and writes the various formatting files.

O

task #8017: use/allow xetex as formatter

Remember that to do complex non-Roman scripts like I think Farsi is, you will
need the latest version of XeTeX. The one that gets install with the current
version of Ubuntu (9.10) may not be good enough. You can pull the most up to
date version from the SIL repo. (deb http://packages.sil.org/ubuntu karmic
main).

Beyond that, the main element you will need is the ptx2pdf macro package. The
one that Paratext uses should do everything most people need. We are working
on a branch of that here that does a little more. I don't know if I can
recommend that one or not. You probably just want to use the same set of
macros that Paratext uses to have some compatibility.

I suggest you don't try to do too much. Just try to give basic output on the
first pass. Give them a little and people will ask for more. Before you know
it, you will be developing a full-blown publishing application and you may not
want to necessarily go there, at least not yet.

The nice thing about going this route is that it leaves it open to
modifications at the local level. One of our support guys was able to get
Paratext 7 to give some output that was never intended because he had access
to the TeX code. We would want this for BE as well.

One hurdle you will need to overcome is the fact that you store all your data
in chapter files. TeX (XeTeX) and ptx2pdf will need the data in book files in
order to process. You will need to provide that some way.

Dennis














bug #27949: Farsi Right-to-Left Cross Referencing and Printing
1. When a printed pdf version of the text is made, whenever the footnotes and cross-references appear on the same page, they overlay on each other.

2. Also page numbering and chapter numbering in the header part of the printed pages are still in Latin format and not Farsi.

I have included 1John which has the references and footnotes on chapter 1. Also I have included a sample copy of the PDF output.





*/
