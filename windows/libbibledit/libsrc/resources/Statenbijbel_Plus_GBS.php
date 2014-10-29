<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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

// This script displays the Statenbijbel from the Dutch GBS.
// It also includes headers, introductions, and notes.
// Books OT: http://gbsdigitaal.nl/Data/Statenvertaling/1.json
// Books NT: http://gbsdigitaal.nl/Data/Statenvertaling/2.json
// Genesis intro http://gbsdigitaal.nl/Data/Statenvertaling/1/1/1.json
// Genesis 1 http://gbsdigitaal.nl/Data/Statenvertaling/1/1/2.json
// Matthew 1 http://gbsdigitaal.nl/Data/Statenvertaling/2/40/2.json

include_once ("gbsdigitaalplus.php");

// Old (1) or New (2) Testament.
$testament = $book >= 40 ? 2 : 1;

// Introduction is chapter 1 in the URL.
// Chapter 1 of the text is chapter 2 in the URL.
$urlchapter = $chapter + 1;

$url = "http://gbsdigitaal.nl/Data/Statenvertaling/$testament/$book/$urlchapter.json";

$output = gbsdigitaalplus ($url, $chapter, $verse);

?>
