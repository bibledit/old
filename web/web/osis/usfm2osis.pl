#!/usr/bin/perl

## USFM to OSIS (2.1.1) converter

## Licensed under the standard BSD license:

# Copyright (c) 2002-2012 CrossWire Bible Society <http://www.crosswire.org/>
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

use utf8;

# Stores the script version and date
$version = "1.7.3";

$date = '$Date$';
$rev = '$Rev$';

$date =~ s/^.+?(\d{4}-\d{2}-\d{2}).+/$1/;
$rev =~ s/^.+?(\d+).+/$1/g;

# Sets the version of OSIS used in the OSIS header
$osisVersion = "2.1.1";
# Stores the USFM Version
$usfmVersion = "2.2"; # The USFM reference document can be found at http://www.ubs-icap.org/chm/usfm/2.2/ (as of 2009-10-26)

# This is the hash which maps the conversion of USFM book abbreviations to OSIS book abbreviations. ***I would like to add the ability to access an external file to provide options for other languages. In other words, in preparing a USFM file for conversion, a separate file could be created which could be used to map the conversion of abbreviated book names in other languages to OSIS. This would be especially useful for cross-references, but I haven't figured out how to do it yet.
%OSISbook = (
# Theoretically, these are laid out according to <BooksPresent>, but I can really only guess without a spec ***Need to check
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
 "ESG" => "EsthGr", "WIS" => "Wis", "SIR" => "Sir", "BAR" => "Bar",
 "LJE" => "EpJer", "S3Y" => "PrAzar", "SUS" => "Sus", "BEL" => "Bel",
 "1MA" => "1Macc", "2MA" => "2Macc", "3MA" => "3Macc", "4MA" => "4Macc",
 "1ES" => "1Esd", "2ES" => "2Esd", "MAN" => "PrMan",
# Following this is just an uneducated guess
 "PS2" => "Ps151", "ODA" => "Odes", "PSS" => "PssSol", "JSA" => "Josh",
 "JSB" => "Josh", "TBS" => "Tob", "SST" => "Sus", "DNT" => "Dan",
 "BLT" => "Bel", "ADE" => "AddEsth"
 );

$divOpen=false;

$wj = 0; # number of \wj's we've encountered

# Generates a list of available encodings.
use Encode;
@encodingList = Encode->encodings(":all");
foreach $enc (@encodingList) {
    $encodings .= "$enc, ";
}
$encodings =~ s/\, $//;

# Syntax instructions
if (scalar(@ARGV) < 2) {
    print "\nusfm2osis.pl -- USFM $usfmVersion to OSIS $osisVersion converter version $version\nRevision $rev ($date)\nSyntax: usfm2osis.pl <osisWork> [-o OSIS-file] [-e USFM encoding] [-t module type] <USFM filenames|wildcard>\n";
    print "- Arguments in braces < > are required. Arguments in brackets [ ] are optional.\n";
    print "- The osisWork is a short name with no spaces which will identify your module.\n";
    print "- If no -o option is specified for the output filename, the default output file is: \n\tosisWork.osis.xml.\n";
    print "- Supported encodings include:\n\t$encodings\n";
    print "- If the encoding is omitted, utf8 is the default value.\n";
    print "- The script can currently produce valid OSIS for usfm bibles and nearly valid OSIS for usfm commentaries. To set the module type use '-t' with either 'bible' or 'comment'. On ommision 'bible' is assumed.\n";
    print "- USFM filenames with the SFM extension can be accessed using a wildcard: \n\t*.SFM\n";
    print "As an example, if you want to generate the osisWork <bible> and your USFM files are encoded in utf8, located in the /Bible folder relative to this script with the file extension SFM, enter:\n\tperl usfm2osis.pl bible Bible/*.SFM\n\n";
    exit (-1);
}

$osisWork = @ARGV[0];

$nextarg = 1;

if (@ARGV[$nextarg] eq "-o") {
    $outputFilename = "@ARGV[$nextarg+1]";
    $nextarg += 2;
}
else {
    $outputFilename = "$osisWork.osis.xml";
}
open (OUTF, , ">:utf8", "$outputFilename") or die "Could not open file @ARGV[2] for writing.";

if (@ARGV[$nextarg] eq "-e") {
    $inputEncoding = "@ARGV[$nextarg+1]";
    $nextarg += 2;
}
else {
    $inputEncoding = "utf8";
}

if (@ARGV[$nextarg] eq "-t") {
    $moduleType = "@ARGV[$nextarg+1]";
    $moduleType = lc($moduleType);
    $nextarg += 2;
}
else {
    $moduleType = "bible";
}

$encFound = 0;
foreach $enc (@encodingList) {
    if ($enc eq $inputEncoding) {
	$encFound = 1;
    }
}
if ($encFound == 0) {
    die "Encoding $inputEncoding not supported.\nSupported encodings include:\n\t$encodings\n";
}
else {
    print "Encoding \"$inputEncoding\" is supported.\n"
}

for (; $nextarg < scalar(@ARGV); $nextarg++) {
    push(@files, @ARGV[$nextarg]);
}

push (@outdata, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<osis xmlns=\"http://www.bibletechnologies.net/2003/OSIS/namespace\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.bibletechnologies.net/2003/OSIS/namespace http://www.bibletechnologies.net/osisCore.$osisVersion.xsd\">\n<osisText osisRefWork=\"Bible\" xml:lang=\"en\" osisIDWork=\"$osisWork\">\n<header>\n<work osisWork=\"$osisWork\"\/>\n<\/header>\n");

$tagStack = "<\/osisText><\/osis>";
$chapClose = "";
$versClose = "";

sub closeTag {
    $tag = @_[0];

    if ($tagStack =~ /$tag/) {
	$tagStack =~ s/^(.*?$tag)//;
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
    print "Processing $file.\n";
    open (SFM, "$file");
    my @filedata = "";
    while (<SFM>) {
	my $sfline;
	$sfline = decode($inputEncoding, $_);
	push (@filedata, $sfline);
    }
    close (SFM);

    for ($i = 0; $i < scalar(@filedata);$i++) {
	$line = @filedata[$i];
	
	### Trim leading/trailing whitespace
#	$line =~ s/[\r\n]+//g;
	$line =~ s/\s*$//;
	$line =~ s/^\s*//;

	if ($line !~ /^\s*$/) {
	    if ($line !~ /^\\/) {	
		@filedata[$i-1] .= " $line";
		splice(@filedata, $i, 1);
		$i--
	    }
	    else {
		@filedata[$i] = $line;
	    }
	}	
    }
    for ($i = 0; $i < scalar(@filedata);$i++) {
	if (@filedata[$i] =~ /(.+) (\\v\b\s*.*)/) {
	    splice(@filedata, $i+1, 0, $2);
	    @filedata[$i] = $1;
	}
    }
    
    $ollevel = 0;
    $vers = 0;
    $chap = 0;
    $book = "";
    # Sets the initial value for the attribute "n" in footnotes.
    $nFN = 0;
    $xFN = 0;
    # Creates array for the attribute "n" in cross-references
    @nCR = (a .. z);
    # Sets the initial value for the attribute "n" in cross-references.
    $nCR = @nCR [0];

    #delete BOM, if present
    @filedata[1] =~ s/^﻿//;

    #encoding stuff
    for ($i = 0; $i < scalar(@filedata); $i++) {
	$line = @filedata[$i];

	### Basic XML entity encoding
	$line =~ s/&(?![a-zA-Z0-9])/&amp;/g;
#	$line =~ s/<< ?/\@/g;
#	$line =~ s/>>/\#/g;
#	$line =~ s/</\$/g;
#	$line =~ s/>/\%/g;

	# $line =~ s/(\w)\'(\w)/"$1" . chr(0x2019) . "$2"/eg;
	$line =~ s/\\fr 1\/2 \\fr\*/chr(0xBD)/eg;

	@filedata[$i] = $line;
    }

    for ($i = 0; $i < scalar(@filedata); $i++) {
	$line = @filedata[$i];

	### File Identification--Markers Supported: \id, \h, \ide, \sts, \rem, \toc1, \toc2, \toc3

	if ($line =~ /\\v\b\s*(\d+)\,(\d+)/) {
	    if ($1 + 1 == $2) {
		$line =~ s/\\v\b\s*(\d+)\,(\d+)/\\v $1\-$2/;
	    }
	}
	$line =~ s/\\v\b\s+(\d+)(\-\d+|\s*\\v\b\s+\d+)\s*\\v\b\s+(\d+)/\\v $1\-$3/;
	$line =~ s/\\v\b\s+(\d+)\s*\\v\b\s+(\d+\-)?(\d+)/\\v $1\-$3/;
	$line =~ s/^\\(p[is]|mi)\b/\\p/;
	$line =~ s/^\\li\b/\\p/; #\li isn't part of USFM, so we'll make it \p

	# \id (book marker)
	if ($line =~ /^\\id\b\s*([^ ]*)/) {
	    $book = $OSISbook{$1};
	    $chap = 0;
	    if ($versClose =~ /<verse/) {
		push (@outdata, $versClose); # close verse
		$versClose = "";
	    }
#	    push (@outdata, closeTag("<\/div[^>]*?>")); # close section
	    if ($chapClose =~ /<chapter/) {
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

	# \ide Encoding (discard)
	if ($line =~ /^\\ide\b/) {
	    $line = "";
	}

	# \sts Status (discard)
	if ($line =~ /^\\sts\b/) {
	    $line = "";
	}

	# \rem Comments from translator (discard)
	if ($line =~ /^\\rem\b/) {
	    $line = "";
	}
	
	# \restore Version control comments from translator, not a formal USFM marker but used by paratext (discard)
	if ($line =~ /^\\restore\b/) {
	    $line = "";
	}

	# \toc1 Table of Contents (discard)
	if ($line =~ /^\\toc\d\b/) {
	    $line = "";
	}

	### Introduction--Markers Supported: \imt#, \is#, \iot, \io#, \ip, \ie, \ib, \ior..\ior*
	#### Markers Not Yet Supported: \ipi, \im, \imi, \ipq, \imq, \ipr, \iq#, \ili, \iex, \imte

	# \imt major title
	if ($line =~ /^\\imt\b\s*(.+)/) {
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
	if ($line =~ /^\\io(\b\s*.*)/) {
	    $line = "\\io1$1";
	    }
	
	if ($line =~ /^\\io(\d+)\b\s*(.*)/) {
	    if ($ollevel == $1) {
		$line = "<item>$2<\/item>";
	    }
	    elsif ($ollevel > $1) {
		$line = "";
		while ($ollevel > $1) {
		    $line .= "<\/list><\/item>\n";
		    $ollevel--;
		}
		$line .= "<item>$2<\/item>";
	    }
	    elsif ($ollevel < $1) {
		$line = "";
		if ($ollevel != 0) {
		    $line .= "<item>";
		}
		while ($ollevel < $1) {
		    $line .= "<list>\n";
		    $ollevel++;
		}
		$line .= "<item>$2<\/item>\n";
	    }

	    if (@filedata[$i+1] !~ /^\\io/) {
		while ($ollevel > 0) {
		    $line .= "\n<\/list>";
		    if ($ollevel > 1) {$line .= "<\/item>";}
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
	
	# \im introduction paragraph ('left flush' - NOT IMPLENTED FLUSH)
	if ($line =~ /^\\im\b\s*(.*)/) {
	    $line = "<p>$1<\/p>";
	}
	
	# \im introduction quotation (implemented as ordinary quotation)
	if ($line =~ /^\\imq\b\s*(.*)/) {
	    $line = "<q>$1<\/q>";
	}
	
	
	# \iq line (including \iq#), adapted from \q (see below), needs more clean-up
	if ($line =~ /^\\iq/) {
	    if ($l != 1) {
		push (@outdata, "<lg>\n");
		$l = 1;
	    }
	    if ($line =~ /\\iq(\d*)$/) {
		if ($1 eq "") {
		    $line = "<l>\n";
		}
		else {
		    $line = "<l level=\"$1\">\n";
		}
		@filedata[$i+1] .= "<\/l>";
		if (@filedata[$i+2] !~ /\\iq(?!t)/) {
		    @filedata[$i+1] .= "\n<\/lg>";
		    $l = 0;
		}
	    }
	    else {
		$line =~ s/\\iq\b\s*(.+)/<l>$1<\/l>/;
		$line =~ s/\\iq(\d+)\b\s*(.+)/<l level=\"$1\">$2<\/l>/;
		if (@filedata[$i+1] !~ /\\iq(?![ta])/) {
		    $line .= "\n<\/lg>";
		    $l = 0;
		}
	    }
	}
	# \ib introduction blank line
	
	if ($line =~ /^\\ib$/) {
	    $line = "<lb/>";
	}

	# \ior..\ior* introduction reference
	# These ranges are usually massive. I quite likely would prefer to wipe the tag and leave the references as plan text
	# $line =~ s/\\ior( .*?)\\ior\*/$1/g;
	
	$line =~ s/\\ior( .*?)\\ior\*/<reference>$1<\/reference>/g;
	
	
        # \ie introduction end (discard)
	if ($line =~ /^\\ie\b/) {
	    $line = "";
	}
	
	

	### Titles, Headings, and Labels (elsewhere?)--Markers Supported: \d, \ms#, \s#, \mt#, \r, \sp, \rq..\rq*
	#### Markers Not Yet Supported: \mte#, \mr, \sr 

	# \ms majorSection
	if ($line =~ /^\\ms\d?\b\s*(.*)/) {
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, closeTag("<\/div type=\"majorSection\">"));
	    push (@outdata, "<div type=\"majorSection\">\n");
	    openTag("<\/div type=\"majorSection\">");
	    $line =~ s/\\ms\d?\b\s*(.+)/<title>$1<\/title>/;
	    $line =~ s/\\ms\d?\b\s*//;
	}
	# \d canonical title
	if ($line =~ /^\\d\b\s*(.+)?(\\d\*)?/) {
	    push (@outdata, closeTag("<\/p>"));
	    $line =~ s/\\d\b\s*(.+)/<title type=\"psalm\">$1<\/title>/;
	}

	# \s \s1 section (From Chapters and Verses)
	if ($line =~ /^\\s1?\b\s*(.*)/) {
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, closeTag("<\/div type=\"section\">"));
	    push (@outdata, "<div type=\"section\">\n");
	    openTag("<\/div type=\"section\">");
	    $line =~ s/\\s1?\b\s*(.+)/<title>$1<\/title>/;
	    $line =~ s/\\s1?\b\s*//;
	    if ($line =~ /HEBREW TITLE/) {
		$line =~ s/<title>/<title type=\"psalm\" canonical=\"true\">/;
	    }
	}

	# \ss \s2 subSection (From Chapters and Verses)
	if ($line =~ /^\\s[s2]\b\s*(.*)/) {
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, closeTag("<\/div type=\"subSection\">"));
	    push (@outdata, "<div type=\"subSection\">\n");
	    openTag("<\/div type=\"subSection\">");
	    $line =~ s/\\s[s2]\b\s*(.+)/<title>$1<\/title>/;
	    $line =~ s/\\s[s2]\b\s*//;
	}

	# \sss \s3 x-subSubSection (From Chapters and Verses)
	# This will also handle deeper levels (4+) of subsections.
	if ($line =~ /^\\s(ss|\d+)\b\s*(.*)/) {
	    my $ssLvl = $1;
	    my $ssType = "";
	    while ($ssLvl > 2) {
		$ssLvl--;
		$ssType .= "Sub";
	    }
	    $ssType = "x-sub" . $ssType . "Section";
	    push (@outdata, closeTag("<\/p>"));
	    push (@outdata, closeTag("<\/div type=\"$ssType\">"));
	    push (@outdata, "<div type=\"$ssType\">\n");
	    openTag("<\/div type=\"$ssType\">");
	    $line =~ s/\\s(ss|\d+)\b\s*(.+)/<title>$2<\/title>/;
	    $line =~ s/\\s(ss|\d+)\b\s*//;
	}

	# \mt\mt1 title
	if ($line =~ /^\\mt[1234]?\b\s*(.+)/) {
	    $line = "<title type=\"main\">$1<\/title>";
	}

	# \mt2 title
	if ($line =~ /^\\mt2\b\s*(.+)/) {
	    $line = "<title type=\"continued\">$1<\/title>";
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

	# \rq..\rq* inline reference
	$line =~ s/\\rq( .*?)\\rq\*/<reference>$1<\/reference>/g;
	
	### Chapters and Verses--Markers Supported: \c, \v, \vp...\vp*, \cl
	#### Markers Not Yet Supported: \ca...\ca*, \cp, \cd, \va...\va*

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
	    if ($moduleType eq "bible") {
	        push (@outdata, closeTag("<\/p>"))
	        }
	    if ($chapClose =~ /<chapter/) {
		push (@outdata, $chapClose); # close previous chapter
		$chapClose = "";
	    } else {
		push (@outdata, closeTag("<\/div>")); # close introduction div
	    }

	    push (@outdata, "<chapter sID=\"$book.$chap\" osisID=\"$book.$chap\"\/>\n");
	    $chapClose = "<chapter eID=\"$book.$chap\"\/>\n";
	    $line =~ s/\\c\b\s*([^ ]*)//;
	}

	# \cl chapter label
	if ($line =~ /^\\cl\b\s*(.*)/) {
	    $line = "<title>$1<\/title>";
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
	    $divOpen=false;
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
	    if ($moduleType eq "bible") {
        	    push (@outdata, "<verse sID=\"$osisID\" osisID=\"$osisID\"\/>\n");
	            $versClose = "<verse eID=\"$osisID\"\/>\n";
	            $line =~ s/\\v\b\s*(\d[^\\ ]*)? *//;
	            }
            elsif ($moduleType eq "comment") {
                    closeTag("<\/p>");
                    push (@outdata, "<div type=\"section\" annotateType=\"commentary\" annotateRef=\"$osisID\">\n");
	            $versClose = "<\/p>\n<\/div>\n";
	            $line =~ s/\\v\b\s*(\d[^\\ ]*)? *//;
	            $divOpen=true;
	            }
            else { print "usfm2osis.pl supports only the module types \"bible\" and \"comment\" \n";
                    exit 1;
            }
	}

        # \vp...\vp# published verse numbers
	$line =~ s/\\vp\*\s*//g;
	$line =~ s/\\vp\b\s*(\d+[a-z]?|[a-z])\s*/<seg type="verseNumber">$1<\/seg>/g;

	### Paragraphs--Markers Supported: \p, \b, \m, \nb, \cls
	#### Markers Not Yet Supported: \pmo, \pm, \pmc, \pmr, \pi#, \mi, \li#, \pc, \pr, \ph#

	# Hack to solve an issue in a module that used <R> for linebreaks in the usfm files--may be commented out (not USFM 2.1)
	# $line =~ s/\\lb\*/<lb \/>/g;

	# \p paragraph (From Chapters and Verses)
	if ($line =~ /^\\p\b\s*/) {
	    if ((($moduleType eq "comment") && ($divOpen))||($moduleType eq "bible")){
	        push (@outdata, closeTag("<\/p>"));
	        }
	    push (@outdata, "<p>\n");
	    openTag("<\/p>");
	    $line =~ s/\\p\b\s*//;
	}

	# \pc paragraph centered (From Chapters and Verses)
	if ($line =~ /^\\pc\b\s*/) {
	    if ((($moduleType eq "comment") && ($divOpen))||($moduleType eq "bible")){
	        push (@outdata, closeTag("<\/p>"));
	        }
	    push (@outdata, "<p type=\"x-center\">\n");
	    openTag("<\/p>");
	    $line =~ s/\\pc\b\s*//;
	}

	# \mi2 paragraph flush left, no indentation
	if ($line =~ /^\\mi2\b\s*/) {
	    if ((($moduleType eq "comment") && ($divOpen))||($moduleType eq "bible")){
	        push (@outdata, closeTag("<\/p>"));
	        }
	    push (@outdata, "<p type=\"x-noindent\">\n");
	    openTag("<\/p>");
	    $line =~ s/\\mi2\b\s*//;
	}
	# \cls paragraph (From Chapters and Verses)
	if ($line =~ /^\\cls\b\s*/) {
	    if ((($moduleType eq "comment") && ($divOpen))||($moduleType eq "bible")){
	        push (@outdata, closeTag("<\/closer>"));
	        }
	    push (@outdata, "<closer>\n");
	    openTag("<\/closer>");
	    $line =~ s/\\cls\b\s*//;
	}

	# \b
	$line =~ s/\\b\b/<lb\/>/;
	# \m
	$line =~ s/\\m\b//;
	# \nb
	$line =~ s/\\nb\b//;

	### Poetry--Markers Supported: \q#, \qs...\qs*, \qr, \qc, \qac...\qac*, \qa, \qm#
	#### Markers Not Yet Supported: [none]

	# \qt...\qt*, OT quotation (handle early)
	$line =~ s/\\qt\b\s*(.*?)\\qt\*/<seg type="otPassage">$1<\/seg>/g;

	# \qa, acrostic heading
	$line =~ s/^\\qa\b\s*(.*)/<title type=\"acrostic\">$1<\/title>/g;

	# \qac...\qac*, acrostic character style (used within a line)
	$line =~ s/\\qac\b\s*(.*?)\\qac\*/<hi type="acrostic">$1<\/hi>/g;	
	
	# \q line (including \q#, \qr, \qc, and \qs...\qs*)
	if ($line =~ /^\\q/) {
	    if ($l != 1) {
		push (@outdata, "<lg>\n");
		$l = 1;
	    }
	    if ($line =~ /\\qm?(c|r|\d*)$/) {
		if ($1 eq "") {
		    $line = "<l>\n";
		}
		elsif ($1 eq "c") {
		    $line = "<l type=\"x-centered\">";
		}
		elsif ($1 eq "r") {
		    $line = "<l type=\"x-right\">";
		}
		else {
		    $line = "<l level=\"$1\">\n";
		}
		@filedata[$i+1] .= "<\/l>";
		if (@filedata[$i+2] !~ /\\q(?!t)/) {
		    @filedata[$i+1] .= "\n<\/lg>";
		    $l = 0;
		}
	    }
	    else {
		$line =~ s/\\q\b\s*(.+)/<l>$1<\/l>/;
		$line =~ s/\\qm?(\d+)\b\s*(.+)/<l level=\"$1\">$2<\/l>/;
		$line =~ s/\\qc\b\s*(.+)/<l type=\"x-centered\">$1<\/l>/;
		$line =~ s/\\qr\b\s*(.+)/<l type=\"x-right\">$1<\/l>/;
		$line =~ s/\\qs\b\s*(.+?)\s*\\qs\*/<l type="selah">$1<\/l>/;
		if (@filedata[$i+1] !~ /\\q(?![ta])/) {
		    $line .= "\n<\/lg>";
		    $l = 0;
		}
	    }
	    $line =~ s/\s*\\qs\b\s*(.+?)\s*\\qs\*/<\/l>\n<l type="selah">$1/;
	}
	$line =~ s/\s*\\qs\b\s*(.+?)\s*\\qs\*\s*/<lg><l type="selah">$1<\/l><\/lg>/;

	### Tables--Markers Supported: \tr, \th#, \tc#, \tcr#
	####Markers Not Yet Supported: \thr#

	# \th table heading
	if ($line =~ /^\\t/) {
	    if ($line =~ /^\\tr\b\s*(\\th.*)/) {
		$line = "$1";
		if ($table != 1) {
		    push (@outdata, "<table>\n");
		    $table = 1;
		}
		$line =~ s/\\th\d?\b\s*(.+?)\s*(?=(\\th|$))/<cell role=\"label\">$1<\/cell>/g;
		$line = "<row>$line<\/row>";
	    }

	    if ($line =~ /^\\tr\b\s*(\\tc.*)/) {
		$line = $1;
		if ($table != 1) {
		    push (@outdata, "<table>\n");
		    $table = 1;
		}
		$line =~ s/\\tcr?\d?\b\s*(.+?)\s*(?=(\\tc|$))/<cell>$1<\/cell>/g;
		$line = "<row>$line<\/row>";
		if (@filedata[$i+1] !~ /\\tr/) {
		    $line .= "<\/table>\n";
		    $table = 0;
		}
	    }

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

	sub parseRef {
	    $ref = @_[0];

	    $ref =~ s/[:\.]\s*$//;
	    $ref =~ s/:/\./g;
	    $ref = "$book.$ref";
	    $ref =~ s/(\d+)\.(\d[^\,]+)\-(\d+)/$1.$2-$book.$1.$3/;
	    $ref =~ s/(\d+)\.(\d[^\-]+)\-+\s*(\d.+)/$1.$2\-$book.$1.$3/;

	    return $ref;
	}

	### Footnotes--Markers Supported: \fk, \fq, \f...\f*, \fv, \ft, \fqa
	####Markers Not Yet Supported: \fe...\fe*, \fr, \fl, \fp, \fdc...\fdc*, \fm...\fm*

	sub footnoteHandler {
	    $note = @_[0];
	    $note = "<note>$note</note>";

	    # \fk Catch Words
	    $note =~ s/\\fk\s(.+?)(\s*)\\fk\*/\\fX<catchWord>$1<\/catchWord>\\fX$2/g;
	    $note =~ s/\\fk\s(.+?)(\s*)(?=\\f)/\\fX<catchWord>$1<\/catchWord>$2\\fX/g;
	    $note =~ s/\\fk\*/\\fX/g;

	    # \fq Quotations in Footnotes
	    # CCL--I don't know the difference, aside from length, between catch words and quotations in footnotes. It may vary by document.
	    $note =~ s/\\fq\s(.+?)(\s*)\\fq\*/\\fX<catchWord>$1<\/catchWord>\\fX$2/g;
	    $note =~ s/\\fq\s(.+?)(\s*)(?=\\f)/\\fX<catchWord>$1<\/catchWord>$2\\fX/g;
	    $note =~ s/\\fq\*/\\fX/g;

	    # \fqa Alternate translations in Footnotes
	    $note =~ s/\\fqa\s(.+?)\\fqa\*/\\fX<rdg type=\"alternate\">$1<\/rdg>\\fX/g;
	    $note =~ s/\\fqa\s(.+?)(?=\\f)/\\fX<rdg type=\"alternate\">$1<\/rdg>\\fX/g;
	    $note =~ s/\\fqa\*/\\fX/g;

	    # \fv Footnote verse number
	    $note =~ s/\\fv\s(.+?)\\fv\*/\\fX<reference osisID=\"$book.$chap.$1\">$1<\/reference>\\fX/g;
	    $note =~ s/\\fv\s*(\d+)\b\s*(?=\\f)/\\fX<reference osisID=\"$book.$chap.$1\">$1<\/reference>\\fX/g;
	    $note =~ s/\\fv\*/\\fX/g;

	    # \fr Footnote origin reference (the verse where the fn appears)
	    while ($note =~ /\\fr\s*(.+?)\s*(?=\\f)/) {
		$sourceVal = parseRef($1);
		$nFN++;
#		$note =~ s/\\fr\s*(.+?)\s*(?=\\f)//;
		$note =~ s/\\fr\s*//;
		$note =~ s/<note>/<note n="$nFN">/;
	    }

	    # \ft Footnote text
	    $note =~ s/\\ft\s//g;
	    $note =~ s/\\ft\*//g;

	    # \f* Footnote closer
	    $note =~ s/\s*\\f\*//;

	    # \f Footnote opener
	    $note =~ s/\\fe?\b\s*([^\s]\s*)?//;

	    # \fX was inserted above to mark former locations of various already-handled markers, which can now be removed
	    $note =~ s/\\fX//g;

	    return $note;
	}

	$line =~ s/(\\f\b.+?\\f\*)/footnoteHandler($1)/eg;

	### Crossreferences--Markers Supported: \x + \xo...\x*, \xk, \xq, \xt
	#### Markers Not Yet Supported: \xdc...\xdc*
	sub xrefHandler {
	    $xref = @_[0];
	    $xref = "<note type=\"crossReference\">$xref</note>";

	    # \xk Catch Words
	    $xref =~ s/\\xk\s(.+?)(\s*)\\xk\*/<catchWord>$1<\/catchWord>$2/g;
	    $xref =~ s/\\xk\s(.+?)(\s*)(?=\\x)/<catchWord>$1<\/catchWord>$2/g;
	    $xref =~ s/\\xk\*//g;

	    # \xq Quotations in Footnotes
	    # CCL--I don't know the difference, aside from length, between catch words and quotations in footnotes. It may vary by document.
	    $xref =~ s/\\xq\s(.+?)(\s*)\\xq\*/<catchWord>$1<\/catchWord>$2/g;
	    $xref =~ s/\\xq\s(.+?)(\s*)(?=\\x)/<catchWord>$1<\/catchWord>$2/g;
	    $xref =~ s/\\xq\*//g;

	    # \xo Footnote origin reference (the verse where the fn appears)
	    while ($xref =~ /\\xo\s*(.+?)\s*(?=\\x)/) {
		$sourceVal = parseRef($1);
		$xFN++;
#		$xref =~ s/\\xo\s*(.+?)\s*(?=\\x)//;
		$xref =~ s/\\xo\s*//;
		$xref =~ s/<note type=\"crossReference\">/<note type=\"crossReference\" n="$xFN">/;
	    }

	    # \xt Crossref itself
	    $xref =~ s/\\xt\s(.+?)\\xt\*/<reference>$1<\/reference>/g;
	    $xref =~ s/\\xt\s(.+?)(?=\\x)/<reference>$1<\/reference>/g;
	    $xref =~ s/\\xt\*//g;

	    # \x* Footnote closer
	    $xref =~ s/\\x\*//;

	    # \x Footnote opener
	    $xref =~ s/\\x\b\s*([^\s]\s*)?//;

	    return $xref;
	}

	$line =~ s/(\\x\b.+?\\x\*)/xrefHandler($1)/eg;


	# crossReference osisRef=""
	$line =~ s/<reference osisRef="">([^<]+)<\/reference>/<reference osisRef="$1">$1<\/reference>/g;
	$line =~ s/osisRef="\s/osisRef="\s/g;
	$line =~ s/\s">/">/g;
	$line =~ s/<reference osisRef="([^\s\"]+)\s/<reference osisRef="$1\./g; # Changes space after book name to a period

	$line =~ s/<reference osisRef="([^\"]+):([^\"]+)"/<reference osisRef="$1\.$2"/g; # Gen 1:1
	$line =~ s/<reference osisRef="([^\.\"]+)\.(\d+)\.(\d+)-(\d+)"/<reference osisRef="$1\.$2\.$3-$1\.$2\.$4"/g; # Gen 1:1-2
	$line =~ s/<reference osisRef="([^\.\"]+).(\d+):(\d+)-(\d+).(\d+)"/<reference osisRef="$1\.$2\.$3-$1\.$4\.$5"/g; # Gen 1:1-2:3
	$line =~ s/<reference osisRef="([^\.\"]+)\.(\d+)\.([^\"]+)">([^<]+)<\/reference>; <reference osisRef="(\d+)\.(\d+)"/<reference osisRef="$1\.$2\.$3">$4<\/reference>; <reference osisRef="$1\.$5\.$6"/g; # Gen. 1:1, 2:3
	$line =~ s/<reference osisRef="([^\.\"]+)\.(\d+)\.([^\"]+)">([^<]+)<\/reference>, <reference osisRef="(\d+)"/<reference osisRef="$1\.$2\.$3">$4<\/reference>, <reference osisRef="$1\.$2\.$5"/g; # Gen. 1:1, 3
	$line =~ s/<reference osisRef="([^\"\.]+)\.(\d+)"/<reference osisRef="$1\.1\.$2"/g; # Jude 1

	### Special Text and Character Styles--Markers Supported: \it...\it*, \nd...\nd*, \pn...\pn*, \tl...\tl*, \qt...\qt*, \add...\add*, \pb, \bk...\bk*, \sc..\sc*, \bd...\bd*, \wj...\wj*
	#### Markers Not Yet Supported: Special Text:  \k...\k*, \lit, \ord...\ord*, \sig...\sig*,; Character Styling: \em...\em*,  \bdit...\bdit*, \no...\no*; Spacing and Breaks: !$, //; Special Features: \fig...\fig*, \ndx...\ndx*, \pro...\pro*, \w...\w*, \wg...\wg*, \wh...\wh*

	# \dc...\dc*, inserted deuterocanonical text
	$line =~ s/\\dc\b\s*(.*?)\\dc\*/<transChange type=\"added\" editions=\"dc\">$1<\/transChange>/g;

	# \it...\it*, italic text
	$line =~ s/\\it\b\s*(.*?)\\it\*/<hi type=\"italic\">$1<\/hi>/g;
	
	# \bd...\bd*, bold text
	$line =~ s/\\bd\b\s*(.*?)\\bd\*/<hi type=\"bold\">$1<\/hi>/g;

	# \bk...\bk*, book name in text
	$line =~ s/\\bk\b\s*(.*?)\\bk\*/<hi type=\"italic\">$1<\/hi>/g;

	# \sc...\sc*, small-caps character style (used within a line)
	$line =~ s/\\sc\b\s*(.*?)\\sc\*/<hi type="small-caps">$1<\/hi>/g;	
	
	# \nd...\nd*, Divine Name
	$line =~ s/\\nd\b\s*(.*?)\\nd\*/<divineName>$1<\/divineName>/g;

	# \pn...\pn*, Proper name
	$line =~ s/\\pn\b\s*(.*?)\\pn\*/<name>$1<\/name>/g;

	# \tl...\tl*, Foreign Langauge (treated here merely as transliterated text)
	$line =~ s/\\tl\b\s*(.*?)\\tl\*/<hi type="italic">$1<\/hi>/g;

	# \sls...\sls*, Text from alternative text source
	$line =~ s/\\sls\b\s*(.*?)\\sls\*/<hi type="italic">$1<\/hi>/g;

	# \add...\add*, text added for translation purposes
	$line =~ s/\\add\b\s*(.*?)\\add\*/<transChange type=\"added\">$1<\/transChange>/g;

	# \wj...\wj*, Words of Jesus
	sub wjCount {
	    if (@_[0] eq "") {
		$wj++;
		return "sID=\"q.$wj\" ";
	    }
	    return "eID=\"q.$wj\" ";
	}
	#$line =~ s/\\wj\b\s*(.*?)\\wj\*/<q who="Jesus" marker="">$1<\/q>/g;
	$line =~ s/\\wj\b(\*?)\s*/"<q " . wjCount($1) . "who=\"Jesus\" marker=\"\"\/>"/eg;

	# \pb, page break
	$line =~ s/\\pb\b/<milestone type=\"pb\"\/>/g;

	### Other (probably non-standard) items
	### Markers Supported: \zelastic

	# \zelastic, elastic height marker for typesetting
	$line =~ s/\\zelastic\b//g;

	$line =~ s/_/ /g;

### End USFM 2.1 Items

	if ($line !~ /^\s*$/) {
	    push (@outdata, "$line\n");
	}
    }
}

if ($versClose =~ /<verse/) {
    push (@outdata, $versClose); # close verse
    $versClose = "";
}
if ($chapClose =~ /<chapter/) {
    push (@outdata, $chapClose); # close chapter
    $chapClose = "";
}
push (@outdata, closeTag("<\/osis>"));

for ($i = 0; $i < scalar(@outdata); $i++) {
    #@outdata[$i] =~ s/---/â€•/g; # m-dash
    #@outdata[$i] =~ s/--/â€”/g; # n-dash
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

print "Doing some cleanup.\n";

open (INF, "<:utf8", "$outputFilename");
@filedata = <INF>;
close (INF);
open (OUTF, ">:utf8", "$outputFilename");

# bubble chapter down
for ($i = 0; $i < scalar(@filedata); $i++) {
    if (@filedata[$i] =~ /^<\// && @filedata[$i-1] =~ /^<chapter.+\/>/) {
	$temp = @filedata[$i];
	@filedata[$i] = @filedata[$i-1];
	@filedata[$i-1] = $temp;
	$i -= 2;
    }
}

# bubble verse end up
# CCL--this may require further attention, but works for the present
for ($i = 0; $i < scalar(@filedata); $i++) {
    if (@filedata[$i-1] =~ /^(<title|<\/?div|<\/?p)/ && @filedata[$i] =~ /^<verse eID.+\/>/) {
	$temp = @filedata[$i];
	@filedata[$i] = @filedata[$i-1];
	@filedata[$i-1] = $temp;
	$i -= 2;
    }
}

for ($i = 0; $i < scalar(@filedata); $i++) {
    $fullfile .= @filedata[$i];
}
$fullfile =~ s/<\/osisText>\n<\/osis>\n(<chapter eID[^>]+>)/$1\n<\/osisText>\n<\/osis>/mg; #swap the chapter back up one before the osisText closer
$fullfile =~ s/<\/div>\n(<chapter eID[^>]+>)/$1\n<\/div>/mg; #swap the chapter back up one before the book closer

#print "Tagging quotations.\n";

#$q = 1;

#$fullfile =~ s/\$([^\%]+?)\%/"<q level=\"2\" sID=\"q2." . $q . "\"\/>" . $1 . "<q level=\"2\" eID=\"q2." . $q++ . "\"\/>"/eg;

#$fullfile =~ s/\$/"<milestone type=\"cQuote\" subType=\"x-level-2\"\/>"/eg;

#$q = 1;

#while ($fullfile =~ /(\@[^\@\#]+?)\@([^\@\#]+?)\#(([^\@\#]+?\@[^\@\#]+?\#)+[^\@\#]+?\#)/) {
#    $fullfile =~ s/(\@[^\@\#]+?)\@([^\@\#]+?)\#(([^\@\#]+?\@[^\@\#]+?\#)+[^\@\#]+?\#)/$1 . "<q level=\"1\" sID=\"q1." . $q . "\"\/>" . $2 . "<q level=\"1\" eID=\"q1." . $q++ . "\"\/>" . $3/eg;
#}
#while ($fullfile =~ /(\@[^\@\#]+?)\@([^\@\#]+?)\#([^\@\#]+?\#)/) {
#    $fullfile =~ s/(\@[^\@\#]+?)\@([^\@\#]+?)\#([^\@\#]+?\#)/$1 . "<q level=\"1\" sID=\"q1." . $q . "\"\/>" . $2 . "<q level=\"1\" eID=\"q1." . $q++ . "\"\/>" . $3/eg;
#}

#$fullfile =~ s/\@([^\#]+?)\#/"<q level=\"1\" sID=\"q1." . $q . "\"\/>" . $1 . "<q level=\"1\" eID=\"q1." . $q++ . "\"\/>"/eg;
#$fullfile =~ s/\@/"<milestone type=\"cQuote\" subType=\"x-level-1\"\/>"/eg;

#$fullfile =~ s/\^/"<q level=\"1\" eID=\"q1." . $q++ . ".false\"\/>"/eg;

print OUTF $fullfile;
close (OUTF);

print "All done! OSIS file: $outputFilename\n";
