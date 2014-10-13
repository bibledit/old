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

// This sample script displays the interlinear Bible from biblehub.com.
// Sample URL:
// http://biblehub.com/interlinear/genesis/1-1.htm



// This array maps the the book identifiers from bibledit-web
// to the book names as used by the website.
$bookConverter = array (
1 => "genesis",
2 => "exodus",
3 => "leviticus",
4 => "numbers",
5 => "deuteronomy",
6 => "joshua",
7 => "judges",
8 => "ruth",
9 => "1_samuel",
10 => "2_samuel",
11 => "1_kings",
12 => "2_kings",
13 => "1_chronicles",
14 => "2_chronicles",
15 => "ezra",
16 => "nehemiah",
17 => "esther",
18 => "job",
19 => "psalms",
20 => "proverbs",
21 => "ecclesiastes",
22 => "songs",
23 => "isaiah",
24 => "jeremiah",
25 => "lamentations",
26 => "ezekiel",
27 => "daniel",
28 => "hosea",
29 => "joel",
30 => "amos",
31 => "obadiah",
32 => "jonah",
33 => "micah",
34 => "nahum",
35 => "habakkuk",
36 => "zephaniah",
37 => "haggai",
38 => "zechariah",
39 => "malachi",
40 => "matthew",
41 => "mark",
42 => "luke",
43 => "john",
44 => "acts",
45 => "romans",
46 => "1_corinthians",
47 => "2_corinthians",
48 => "galatians",
49 => "ephesians",
50 => "philippians",
51 => "colossians",
52 => "1_thessalonians",
53 => "2_thessalonians",
54 => "1_timothy",
55 => "2_timothy",
56 => "titus",
57 => "philemon",
58 => "hebrews",
59 => "james",
60 => "1_peter",
61 => "2_peter",
62 => "1_john",
63 => "2_john",
64 => "3_john",
65 => "jude",
66 => "revelation"
);

$book = $bookConverter [$book];

$url = "http://biblehub.com/text/$book/$chapter-$verse.htm";

// Get the html from the server, and tidy it up.
@$html = file_get_contents ($url);
$clean = Tidy_Html::run ($html);
$tidied = explode ("\n", $clean);

$html = "";
$hit = false;
foreach ($tidied as $line) {
  if ($hit) {
    // This is the line we are now at:
    // </span><span class="greek">Βίβλος γενέσεως Ἰησοῦ Χριστοῦ, υἱοῦ Δαβὶδ, υἱοῦ Ἀβραάμ.</span></p>
    $line = trim ($line);
    $line = substr ($line, 7, 1000);
    $pos = strpos ($line, "</p>");
    $line = substr ($line, 0, $pos);
    $html = $line;
    $hit = false;
  }
  if (strpos ($line, "Scrivener") !== false) {
    $hit = true;
  }
}

if ($html == "") return;

$stylesheet = <<<EOD
<style>

@font-face {
font-family: Cardo;
src: url(../fonts/Cardo-Regular.ttf);
}

.greek {
font-family: Cardo;
font-size: large;
}

</style>
EOD;

$output = $stylesheet;
$output .= "\n";
$output .= $html;


?>
