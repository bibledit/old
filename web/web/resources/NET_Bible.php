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
1 => "Genesis",
2 => "Exodus",
3 => "Leviticus",
4 => "Numbers",
5 => "Deuteronomy",
6 => "Joshua",
7 => "Judges",
8 => "Ruth",
9 => "1 Samuel",
10 => "2 Samuel",
11 => "1 Kings",
12 => "2 Kings",
13 => "1 Chronicles",
14 => "2 Chronicles",
15 => "Ezra",
16 => "Nehemiah",
17 => "Esther",
18 => "Job",
19 => "Psalms",
20 => "Proverbs",
21 => "Ecclesiastes",
22 => "Song of Solomon",
23 => "Isaiah",
24 => "Jeremiah",
25 => "Lamentations",
26 => "Ezekiel",
27 => "Daniel",
28 => "Hosea",
29 => "Joel",
30 => "Amos",
31 => "Obadiah",
32 => "Jonah",
33 => "Micah",
34 => "Nahum",
35 => "Habakkuk",
36 => "Zephaniah",
37 => "Haggai",
38 => "Zechariah",
39 => "Malachi",
40 => "Matthew",
41 => "Mark",
42 => "Luke",
43 => "John",
44 => "Acts",
45 => "Romans",
46 => "1 Corinthians",
47 => "2 Corinthians",
48 => "Galatians",
49 => "Ephesians",
50 => "Philippians",
51 => "Colossians",
52 => "1 Thessalonians",
53 => "2 Thessalonians",
54 => "1 Timothy",
55 => "2 Timothy",
56 => "Titus",
57 => "Philemon",
58 => "Hebrews",
59 => "James",
60 => "1 Peter",
61 => "2 Peter",
62 => "1 John",
63 => "2 John",
64 => "3 John",
65 => "Jude",
66 => "Revelation"
);

$book = $bookConverter [$book];

$url = "https://net.bible.org/resource/netTexts/$book $chapter:$verse";
$url = str_replace (" ", "%20", $url);

@$text = file_get_contents ($url);

$output = $text;

$output .= "\n";

$url = "https://net.bible.org/resource/netNotes/$book $chapter:$verse";
$url = str_replace (" ", "%20", $url);

@$notes = file_get_contents ($url);

// The "bibleref" class experiences interference from other resources,
// so that the reference would become invisible.
// Remove this class, and the references will remain visible.
$notes = str_replace ('class="bibleref"', '', $notes);

if ($notes) $output .= $notes;

$output .= "\n";


?>
