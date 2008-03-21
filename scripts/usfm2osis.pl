#!/usr/bin/perl

## USFM to OSIS (2.0) converter

## Licensed under the standard BSD license:

# Copyright (c) 2002,2003, CrossWire Bible Society <http://www.crosswire.org/>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of the CrossWire Bible Society nor the names of
#       its contributors may be used to endorse or promote products
#       derived from this software without specific prior written
#       permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## For general inquiries, comments, suggestions, bug reports, etc. email:
## sword-support@crosswire.org

#########################################################################

$version = "0.5.1";
$date = "2003-10-08";
$osisVersion = "2.0.1";

%OSISbook = (
# Theoretically, these are laid out according to <BooksPresent>, but I can really only guess without a spec
"" => "", "GEN" => "Gen", "EXO" => "Exod", "LEV" => "Lev", "NUM" => "Num",
 "DEU" => "Deut", "JOS" => "Josh", "JDG" => "Judg", "RUT" => "Ruth",
 "1SA" => "1Sam", "2SA" => "2Sam", "1KI" => "1Kgs", "2KI" => "2Kgs",
 "1CH" => "1Chr", "2CH" => "2Chr", "EZR" => "Ezra", "NEH" => "Neh",
 "EST" => "Esth", "JOB" => "Job", "PSA" => "Ps", "PRO" => "Prov",
 "ECC" => "Eccl", "SNG" => "Song", "ISA" => "Isa", "JER" => "Jer",
 "LAM" => "Lam", "EZK" => "Ezek", "DAN" => "Dan", "HOS" => "Hos",
 "JOL" => "Joel", "AMO" => "Amos", "OBA" => "Obad", "JON" => "Jonah",
 "MIC" => "Mic", "NAM" => "Nah", "HAB" => "Hab", "ZEP" => "Zeph",
 "HAG" => "Hag", "ZEC" => "Zech", "MAL" => "Mal", "MAT" => "Matt",
 "MRK" => "Mark", "LUK" => "Luke", "JHN" => "John", "ACT" => "Acts",
 "ROM" => "Rom", "1CO" => "1Cor", "2CO" => "2Cor", "GAL" => "Gal",
 "EPH" => "Eph", "PHP" => "Phil", "COL" => "Col", "1TH" => "1Thess",
 "2TH" => "2Thess", "1TI" => "1Tim", "2TI" => "2Tim", "TIT" => "Titus",
 "PHM" => "Phlm", "HEB" => "Heb", "JAS" => "Jas", "1PE" => "1Pet",
 "2PE" => "2Pet", "1JN" => "1John", "2JN" => "2John", "3JN" => "3John",
 "JUD" => "Jude", "REV" => "Rev", "TOB" => "Tob", "JDT" => "Jdt",
 "ESG" => "Esth", "WIS" => "Wis", "SIR" => "Sir", "BAR" => "Bar",
 "LJE" => "EpJer", "S3Y" => "PrAzar", "SUS" => "Sus", "BEL" => "Bel",
 "1MA" => "1Macc", "2MA" => "2Macc", "3MA" => "3Macc", "4MA" => "4Macc",
 "1ES" => "1Esd", "2ES" => "2Esd", "MAN" => "PrMan",
# Following this is just an uneducated guess
"PS2" => "Ps151", "ODA" => "Odes", "PSS" => "PssSol", "JSA" => "Josh",
 "JSB" => "Josh", "TBS" => "Tob", "SST" => "Sus", "DNT" => "Dan",
 "BLT" => "Bel", "ADE" => "AddEsth"
);

if (scalar(@ARGV) < 2) {
    print "usfm2osis.pl -- USFM to OSIS $osisVersion converter version $version ($date)\nSyntax: usfm2osis.pl <osisWork> [-o OSIS-file] <USFM filenames|wildcard>\n";
    exit (-1);
}

$osisWork = $ARGV[0];

if ($ARGV[1] eq "-o") {
    $outputFilename = "$ARGV[2];"
}
else {
    $outputFilename = "$osisWork.osis.xml";
}
open (OUTF, ">$outputFilename") or die "Could not open file $ARGV[2] for writing.";

if ($ARGV[1] eq "-o") {
    for ($i = 3; $i < scalar(@ARGV); $i++) {
	push(@files, $ARGV[$i]);
    }
}
else {
    for ($i = 1; $i < scalar(@ARGV); $i++) {
	push(@files, $ARGV[$i]);
    }
}

push (@outdata, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<osis xmlns=\"http://www.bibletechnologies.net/2003/OSIS/namespace\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.bibletechnologies.net/2003/OSIS/namespace osisCore.$osisVersion.xsd\">\n<osisText osisRefWork=\"Bible\" xml:lang=\"en\" osisIDWork=\"$osisWork\">\n<header>\n<work osisWork=\"$osisWork\"\/>\n<\/header>\n");

$tagStack = "<\/osisText><\/osis>";
$chapClose = "";
$versClose = "";

sub closeTag {
    $tag = @_[0];

    if ($tagStack =~ /$tag/) {
	$tagStack =~ s/(.*?$tag)//;
	$taglist = $1;
	$taglist =~ s/>/>\n/g;
	$taglist =~ s/(<\/\w+)\s+[^>]+>/$1>/g;
	return $taglist;
    }
    else {
	return:
    }
}

sub openTag {
    $tag = @_[0];
    $tagStack = $tag . $tagStack;
    return;
}

foreach $file (@files) {
    @filedata = `uconv -f windows-1252 -t utf-8 $file`;

    $ollevel = 0;
    $vers = 0;
    $chap = 0;
    $book = "";

    #encoding stuff
    for ($i = 0; $i < scalar(@filedata); $i++) {
	$line = @filedata[$i];
	$line =~ s/[\r\n]//g;

	### Basic XML entity encoding

	$line =~ s/&(?![a-zA-Z0-9])/&amp;/g;
	$line =~ s/<< ?/\@/g;
	$line =~ s/>>/\#/g;
	$line =~ s/</\$/g;
	$line =~ s/>/\%/g;

	$line =~ s/(\w)\'(\w)/$1ʼ$2/g;
	$line =~ s/\\fr 1\/2 \\fr\*/½/g;

	@filedata[$i] = $line;
    }

    for ($i = 0; $i < scalar(@filedata); $i++) {
	$line = @filedata[$i];

	$line =~ s/LORD/<divineName>Lord<\/divineName>/g;

	### File Identification

	# \id (book marker)
	if ($line =~ /^\\id\b\s*([^ ]*)/) {
	    $book = $OSISbook{$1};
	    $chap = 0;
	    if ($chapClose =~ "<verse") {
		push (@outdata, $verseClose); # close verse
		$verseClose = "";
	    }
	    if ($chapClose =~ "<chapter") {
		push (@outdata, $chapClose); # close chapter
		$chapClose = "";
	    }
	    push (@outdata, closeTag("<\/div type=\"book\">")); #close book
	    if ($book eq "") {
		$book = "UnknownUSFMBook";
	    }
	    push (@outdata, "<div type=\"book\" osisID=\"$book\">\n"); # open current book
	    openTag("<\/div type=\"book\">");
	    $line = "";
	}
	
	# \h (running header--discard)
	if ($line =~ /^\\h\b/) {
	    $line = "";
	}

	# \cc (concistent changes script--discard)
	if ($line =~ /^\\cc\b/) {
	    $line = "";
	}


	### Introduction

	# \it title
	if ($line =~ /^\\it\b\s*(.*)/) {
	    $line = "<div type=\"introduction\">\n<title>$1<\/title>";
	    openTag("<\/div>");
	}

	# \is introduction section title
	if ($line =~ /^\\is(\d*)\b\s*(.*)/) {
	    $level = $1;
	    if ($level eq "") {
		$level = "1";
	    }
	    $line = "<div type=\"section\"><title>$2<\/title>";
	    openTag("<\/div>");
	}
	
	# \iot introduction outline title
	if ($line =~ /^\\iot\b\s*(.*)/) {
	    $line = "<div type=\"outline\">\n<title>$1<\/title>";
	}
	
	# \io\d+ introduction outline item
	if ($line =~ /^\\io(\d+)\b\s*(.*)/) {
	    if ($ollevel == $1) {
		$line = "<item>$2<\/item>";
	    }
	    elsif ($ollevel > $1) {
		$line = "";
		while ($ollevel > $1) {
		    $line .= "<\/list>";
		    $ollevel--;
		}
		$line .= "<item>$2<\/item>";
	    }
	    elsif ($ollevel < $1) {
		$line = "";
		while ($ollevel < $1) {
		    $line .= "<list>\n";
		    $ollevel++;
		}
		$line .= "<item>$2<\/item>\n";
	    }
	
	    if (@filedata[$i+1] !~ /^\\io/) {
		while ($ollevel > 0) {
		    $line .= "\n<\/list>";
		    $ollevel--;
		}
		if ($ollevel == 0) {
		    $line .= "\n<\/div>";
		}
	    }
	}

	# \ip introduction paragraph
	if ($line =~ /^\\ip\b\s*(.*)/) {
	    $line = "<p>$1<\/p>";
	}


	### Chapters and Verses

	# \c chapter
	if ($line =~ /^\\c\b\s*([^ ]*)/) {
	    if ($1 ne "") {
		$chap = $1;
	    }
	    else {
		$chap++;
	    }

	    push (@outdata, $versClose);
	    $versClose = "";
	    push (@outdata, closeTag("<\/p>"));
	    if ($chapClose =~ "<chapter") {
		push (@outdata, $chapClose); # close previous chapter
		$chapClose = "";
	    } else {
		push (@outdata, closeTag("<\/div>")); # close introduction div
	    }

	    push (@outdata, "<chapter sID=\"$book.$chap\" osisID=\"$book.$chap\"\/>\n");
	    $chapClose = "<chapter eID=\"$book.$chap\"\/>\n";
	    $line =~ s/\\c\b\s*([^ ]*)//;
	}

	# \d majorSection
	if ($line =~ /^\\d\b\s*(.+)/) {
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, closeTag("<\/div type=\"majorSection\">"));
	    push (@outdata, "<div type=\"majorSection\">\n");
	    openTag("<\/div type=\"majorSection\">");
	    $line =~ s/\\d\b\s*(.+)/<title>$1<\/title>/;
	}

	# \s section
	if ($line =~ /^\\s\b\s*(.+)/) {
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, closeTag("<\/div type=\"section\">"));
	    push (@outdata, "<div type=\"section\">\n");
	    openTag("<\/div type=\"section\">");
	    $line =~ s/\\s\b\s*(.+)/<title>$1<\/title>/;
	    if ($line =~ /HEBREW TITLE/) {
		$line =~ s/<title>/<title type=\"psalm\">/;
	    }
	}

	# \ss subSection
	if ($line =~ /^\\ss\b\s*(.+)/) {
	    $line =~ s/\\ss\b\s*(.+)/<title>$1<\/title>/;
	}

	# \sss x-subsubSection
	if ($line =~ /^\\sss\b\s*(.+)/) {
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, closeTag("<\/div type=\"x=subSubSection\">"));
	    push (@outdata, "<div type=\"x-subSubSection\">\n");
	    openTag("<\/div type=\"x-subSubSection\">");
	    $line =~ s/\\sss\b\s*(.+)/<title>$1<\/title>/;
	}

	# \p paragraph
	if ($line =~ /^\\p\b\s*/) {
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, "<p>\n");
	    openTag("<\/p>");
	    $line =~ s/\\p\b\s*//;
	}

	# \v verse
	if ($line =~ /^\\v\b\s*(\d[^\\ ]*)?/) {
	    if ($1 ne "") {
		$vers = $1;
	    }
	    else {
		$vers++;
	    }

	    push (@outdata, $versClose);
	    $versClose = "";

	    if ($vers =~ /(\d+[^\\\- ]*)\-(\d+[^\\ ]*)/) {
		$vF = $1;
		$vT = $2;
		$vF =~ /^(\d+)/;
		$vFn = scalar($1);
		$vT =~ /^(\d+)/;
		$vTn = scalar($1);
		$osisID = "$book.$chap.$vF";
		if ($vTn > $vFn && $vFn > 0) {
		    for ($j = $vFn + 1; $j < $vTn; $j++) {
			$osisID .=" $book.$chap.$j";
		    }
		}
		$osisID .= " $book.$chap.$vT";
	    }
	    else {
		$osisID = "$book.$chap.$vers";
	    }
	    push (@outdata, "<verse sID=\"$osisID\" osisID=\"$osisID\"\/>\n");
	    $versClose = "<verse eID=\"$osisID\"\/>\n";
	    $line =~ s/\\v\b\s*(\d[^\\ ]*)? *//;
	}

	## Notes

	# \f note
	for ($j = 2; $j > 0; $j--) {
	    if ($line =~ /\\f\b\s*([^\s]+)\s*\\rf\s*([^\\]+)\\rf\*\s*/) {
		$nVal = $1;
		$scopeVal = $2;
		
		$scopeVal =~ s/://g;
		$scopeVal = "$book.$scopeVal";
		$scopeVal =~ s/(\d+)\.(\d[^\,]+)\,\s*(\d.+)/$1.$2 $book.$1.$3/;
		$scopeVal =~ s/(\d+)\.(\d[^\-]+)\-+\s*(\d.+)/$1.$2\-$book.$1.$3/;
		
		$line =~ s/\\f\b\s*([^\s]+)\s*\\rf\s*([^\\]+)\\rf\*\s*/<note n=\"$nVal\" annotateRef=\"$scopeVal\">/;
		$line =~ s/(<note [^>]+>)([A-Z][^a-z:]*?):/$1<catchWord>$2<\/catchWord>/g;
	    }
	}
	# \f hebrew title note
	if ($line =~ /\\f\b\s*([^\s]+)\s*HEBREW TITLE:\s*/) {
	    $nVal = $1;
	    $line =~ s/\\f\b\s*([^\s]+)\s*HEBREW TITLE:\s*/<note n=\"$nVal\">/;
	}
	# \f spare notes
	if ($line =~ /\\f\b\s*([^\s]+)\s*\*\s*/) {
	    $nVal = $1;
	    $line =~ s/\\f\b\s*([^\s]+)\s*\*\s*/<note n=\"$nVal\">/;
	    $line =~ s/(<note [^>]+>)([A-Z][^a-z:]*?):/$1<catchWord>$2<\/catchWord>/g;
	}
	
	# \x crossReference
	for ($j = 2; $j > 0; $j--) {
	    if ($line =~ /\\x\b\s*\\rf\s*([^\\]+)\\rf\*\s*/) {
		$scopeVal = $1;
		
		$scopeVal =~ s/://g;
		$scopeVal = "$book.$scopeVal";
		$scopeVal =~ s/(\d+)\.(\d[^\,]*)\,\s*(\d.*)/$1.$2 $book.$1.$3/;
		$scopeVal =~ s/(\d+)\.(\d[^\-]*)\-+\s*(\d.*)/$1.$2\-$book.$1.$3/;
		
		$line =~ s/\\x\b\s*\\rf\s*([^\\]+)\\rf\*\s*/<note type=\"crossReference\" annotateRef=\"$scopeVal\">/;
		$line =~ s/\[2\]\s*([^\[]+?)(\s*\[1\]|\\x\*)/<seg type=\"x-dc\">$1<\/seg>$2/g;
		$line =~ s/\[2\]\s*([^\[]+?)$/<seg type=\"x-dc\">$1<\/seg>/g;
		$line =~ s/\s*\[1\]//g;
		if ($line =~ /<note type=\"crossReference\" annotateRef=\"[^\"]+?\">\\bw/) {
		    $line =~ s/(<note type=\"crossReference\" annotateRef=\"[^\"]+?\")>\\bw (.+?) \\bw\* /$1 n=\"$2\">/;
		    $pa = $1;
		    $line =~ s/\\bw (.+?) \\bw\* /<\/note>\n$pa n=\"$1\">/g;
		}
		if ($line =~ /<\/seg> \\bw/) {
		    $line =~ /(<note type=\"crossReference\" annotateRef=\"[^\"]+?\")>/;
		    $pa = $1;
		    $line =~ s/\s*\\bw (.+?) \\bw\* /<\/note>\n$pa n=\"$1\">/g;
		}
	    }
	    
	}
	# \x hebrew title crossReference
	if ($line =~ /\\x\b\s*Title:\s*/) {
	    $nVal = $1;
	    $line =~ s/\\x\b\s*Title:\s*/<note type=\"crossReference\">/;
	}
	

	# \[fx]* note/crossReference closers
	if ($line =~ /\\[fx]\*/) {
	    $line =~ s/\\[fx]\*/<\/note>/g;
	}

	
	## Poetry

	# \q line
	if ($line =~ /^\\q/) {
	    if ($l != 1) {
		push (@outdata, "<lg>\n");
		$l = 1;
	    }
	    if ($line =~ /\\q(c|\d*)$/) {
		if ($1 eq "") {
		    $line = "<l>\n";
		}
		elsif ($1 eq "c") {
		    $line = "<l type=\"x-centered\">";
		}
		else {
		    $line = "<l level=\"$1\">\n";
		}
		@filedata[$i+1] .= "<\/l>";
		if (@filedata[$i+2] !~ /\\q/) {
		    @filedata[$i+1] .= "\n<\/lg>";
		    $l = 0;
		}
	    }
	    else {
		$line =~ s/\\q\b\s*(.+)/<l>$1<\/l>/;
		$line =~ s/\\q(\d+)\b\s*(.+)/<l level=\"$1\">$2<\/l>/;
		$line =~ s/\\qc\b\s*(.+)/<l type=\"x-centered\">$1<\/l>/;
		if (@filedata[$i+1] !~ /\\q/) {
		    $line .= "\n<\/lg>";
		    $l = 0;
		}
	    }
	}
	

	## Tables

	# \th table heading
	if ($line =~ /^\\t/) {
	    if ($line =~ /^\\th1\b\s*(.*)/) {
		if ($table != 1) {
		    push (@outdata, "<table>\n");
		    $table = 1;
		}
		$line = "<row><cell role=\"label\">$1<\/cell>\n";
	    }	
	    elsif ($line =~ /^\\th\d+\b\s*(.*)/) {
		$line = "<cell role=\"label\">$1<\/cell>\n";
	    }	
	    if ($line =~ /^\\tb1\b\s*(.*)/) {
		if ($table != 1) {
		    push (@outdata, "<table>\n");
		    $table = 1;
		}
		else {
		    push (@outdata, "<\/row>");
		}
		$line = "<row><cell>$1<\/cell>\n";
		if (@filedata[$i+1] !~ /\\tb/) {
		    $line .= "<\/row><\/table>\n";
		    $table = 0;
		}
	    }	
	    elsif ($line =~ /^\\tb\d+\b\s*(.*)/) {
		$line = "<cell>$1<\/cell>\n";
		if (@filedata[$i+1] !~ /\\tb/) {
		    $line .= "<\/row><\/table>\n";
		    $table = 0;
		}
	    }	
	}

	## Other

	# \ls list
	if ($line =~ /^\\ls\b\s*(.+)/) {
	    if ($ls != 1) {
		push (@outdata, "<list>\n");
		$ls = 1;
	    }
	    $line = "<item>$1<\/item>\n";
	    if (@filedata[$i+1] !~ /\\ls/) {
		$line .= "<\/list>";
		$ls = 0;
	    }
	}

	# \mt title
	if ($line =~ /^\\mt\b\s*(.+)/) {
	    $line = "<title type=\"main\">$1<\/title>";
	}

	# \st,\st2 title
	if ($line =~ /^\\st2?\b\s*(.+)/) {
	    $line = "<title type=\"continued\">$1<\/title>";
	}

	# \st3 title
	if ($line =~ /^\\st3\b\s*(.+)/) {
	    $line = "<title type=\"sub\">$1<\/title>";
	}

	# \r sub title
	if ($line =~ /^\\mr\b\s*(.+)/) {
	    $line = "<title type=\"sub\">$1<\/title>";
	}

	# \r parallel title
	if ($line =~ /^\\r\b\s*(.+)/) {
	    $line = "<title type=\"parallel\">$1<\/title>";
	}

	# \sp speaker
	if ($line =~ /^\\sp\b\s*(.+)/) {
	    $line = "<speaker>$1<\/speaker>";
	}

	# \itw italic word
	$line =~ s/\\itw\b\s*(.*?)\\itw\*/<hi type=\"italic\">$1<\/hi>/g;

	# \n superscripted verse number
	$line =~ s/\\n\b\s*(.*?)\\n\*\s*/<seg type=\"x-versenum\">$1<\/seg>/g;

	# remove unnecessary tags
	$line =~ s/\\b\b//;
	$line =~ s/\\m\b//;


	$line =~ s/\\bq\*/<\/p><\/q>/g;
	$line =~ s/\\bq\b\s*/<q type=\"block\"><p>/g;
	$line =~ s/\\pp/<\/p><p>/g;
	$line =~ s/\\in\*/<\/p><\/inscription>/g;
	$line =~ s/\\in\b\s*/<inscription><p>/g;
	
	
	if ($line !~ /^\s*$/) {
	    push (@outdata, "$line\n");
	}
    }

    close (INF);
}

push (@outdata, closeTag("<\/osis>"));

for ($i = 0; $i < scalar(@outdata); $i++) {
    @outdata[$i] =~ s/---/―/g;
    @outdata[$i] =~ s/--/—/g;
    @outdata[$i] =~ s/([es]ID=\"[^\" ]+) [^\"]*\"/$1\"/;
}

for ($i = 0; $i < scalar(@outdata); $i++) {
    if (@outdata[$i] !~ /^\s*$/) {
	@outdata[$i] =~ s/[\r\n]+/\n/g;
	@outdata[$i] =~ s/\n?$/\n/;
	print OUTF @outdata[$i];
    }
}

close (OUTF);

open (INF, "$outputFilename");
@filedata = <INF>;
close (INF);
open (OUTF, ">$outputFilename");
for ($i = 0; $i < scalar(@filedata); $i++) {
    if (@filedata[$i] =~ /^<\// && @filedata[$i-1] =~ /^<chapter.+\/>/) {
	$temp = @filedata[$i];
	@filedata[$i] = @filedata[$i-1];
	@filedata[$i-1] = $temp;
	$i -= 2;
    }
}
for ($i = 0; $i < scalar(@filedata); $i++) {
    $fullfile .= @filedata[$i];
}

$q = 1;

$fullfile =~ s/\$([^\%]+?)\%/"<q level=\"2\" sID=\"q2." . $q . "\"\/>" . $1 . "<q level=\"2\" eID=\"q2." . $q++ . "\"\/>"/eg;

$fullfile =~ s/\$/"<milestone type=\"cQuote\" subType=\"x-level-2\"\/>"/eg;

$q = 1;

while ($fullfile =~ /(\@[^\@\#]+?)\@([^\@\#]+?)\#(([^\@\#]+?\@[^\@\#]+?\#)+[^\@\#]+?\#)/) {
    $fullfile =~ s/(\@[^\@\#]+?)\@([^\@\#]+?)\#(([^\@\#]+?\@[^\@\#]+?\#)+[^\@\#]+?\#)/$1 . "<q level=\"1\" sID=\"q1." . $q . "\"\/>" . $2 . "<q level=\"1\" eID=\"q1." . $q++ . "\"\/>" . $3/eg;
}
while ($fullfile =~ /(\@[^\@\#]+?)\@([^\@\#]+?)\#([^\@\#]+?\#)/) {
    $fullfile =~ s/(\@[^\@\#]+?)\@([^\@\#]+?)\#([^\@\#]+?\#)/$1 . "<q level=\"1\" sID=\"q1." . $q . "\"\/>" . $2 . "<q level=\"1\" eID=\"q1." . $q++ . "\"\/>" . $3/eg;
}

$fullfile =~ s/\@([^\#]+?)\#/"<q level=\"1\" sID=\"q1." . $q . "\"\/>" . $1 . "<q level=\"1\" eID=\"q1." . $q++ . "\"\/>"/eg;
$fullfile =~ s/\@/"<milestone type=\"cQuote\" subType=\"x-level-1\"\/>"/eg;

$fullfile =~ s/\^/"<q level=\"1\" eID=\"q1." . $q++ . ".false\"\/>"/eg;



print OUTF $fullfile;
close (OUTF);
