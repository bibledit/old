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

// This sample script displays the text and the notes of the NET Bible.


// This array maps the the book identifiers from bibledit-web
// to the book names as used by the web service.
$bookConverter = array (
1 => "Gen",
2 => "Exd",
3 => "Lev",
4 => "Num",
5 => "Deu",
6 => "Jos",
7 => "Jdg",
8 => "Rth",
9 => "1Sa",
10 => "2Sa",
11 => "1Ki",
12 => "2Ki",
13 => "1Ch",
14 => "2Ch",
15 => "Ezr",
16 => "Neh",
17 => "Est",
18 => "Job",
19 => "Psa",
20 => "Pro",
21 => "Ecc",
22 => "Sgs",
23 => "Isa",
24 => "Jer",
25 => "Lam",
26 => "Eze",
27 => "Dan",
28 => "Hsa",
29 => "Joe",
30 => "Amo",
31 => "Oba",
32 => "Jon",
33 => "Mic",
34 => "Nah",
35 => "Hab",
36 => "Zep",
37 => "Hag",
38 => "Zec",
39 => "Mal",
40 => "Mat",
41 => "Mar",
42 => "Luk",
43 => "Jhn",
44 => "Act",
45 => "Rom",
46 => "1Cr",
47 => "2Cr",
48 => "Gal",
49 => "Eph",
50 => "Phl",
51 => "Col",
52 => "1Th",
53 => "2Th",
54 => "1Ti",
55 => "2Ti",
56 => "Tts",
57 => "Phm",
58 => "Hbr",
59 => "Jam",
60 => "1Pe",
61 => "2Pe",
62 => "1Jo",
63 => "2Jo",
64 => "3Jo",
65 => "Jud",
66 => "Rev"
);

$book = $bookConverter [$book];

$output = "";

$url = "http://www.blueletterbible.org/Bible.cfm?b=$book&c=$chapter&t=KJV&ss=1";
$url = str_replace (" ", "%20", $url);

$output .= "<a href=\"$url\" target=\"_blank\">KJV</a>";

$output .= " | ";

$url = "http://www.blueletterbible.org/Bible.cfm?b=$book&c=$chapter&t=WLC";
$url = str_replace (" ", "%20", $url);

$output .= "<a href=\"$url\" target=\"_blank\">WLC</a>";

$output .= " | ";

$url = "http://www.blueletterbible.org/Bible.cfm?b=$book&c=$chapter&t=mGNT";
$url = str_replace (" ", "%20", $url);

$output .= "<a href=\"$url\" target=\"_blank\">mGNT</a>";

?>
