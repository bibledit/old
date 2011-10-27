# Copyright (©) 2011 Peter von Kaehne.
#  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#  
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

use XML::LibXML;
use File::Path qw(make_path remove_tree);
use File::Copy;
use HTML::Tidy;

$file = `cat site.xml`;

my $tidy = HTML::Tidy->new(
    {
        output_xhtml => 1,
        tidy_mark    => 0,
        indent	     => 1,
    }
);

$parser  = XML::LibXML->new();

$content = $parser->parse_string($file);
@pages   = $content->getElementsByTagName("url");

my %menu = ();

# Creating and cleaning a list of all pages under a common path

## first create hashed list - this could be done better with a hash of anonymous arrays, but I do not understand them well enough 

foreach (@pages) {
    $menu{ ( $_->getElementsByTagName("dir") )[0]->textContent() } .=
      ( ( $_->getElementsByTagName("html") )[0]->getElementsByTagName("h1") )
      [0]->textContent() . "AAAA"
      . ( $_->getElementsByTagName("name") )[0]->textContent()."ZZZZ";
}

## then clean each list up

foreach $key ( keys %menu ) {
    @value = split( "ZZZZ", $menu{$key} );
    @value = sort (@value);
    foreach (@value) {
        @item = split( "AAAA", $_ );
        $_ = "<li><a href=\"$item[1]\">$item[0]</a></li>";
    }
    $menu{$key} = join( "", @value );
    $menu{$key} =~ s/href=\"[^"].*?\/([0-9,a-z,A-Z,-]+\.html)\"/href=\"$1\"/g;
}

# Build the page:

foreach (@pages) {

    my $dir   = ( $_->getElementsByTagName("dir") )[0]->textContent();
    my $name  = ( $_->getElementsByTagName("name") )[0]->textContent();
    my $title = ( $_->getElementsByTagName("h1") )[0]->textContent();
    my $index;
    my $depth;

## calculating the page's depth
    { my $temp = $dir;
      $temp =~ s/[^\/]//g;
      $depth = '../' x length($temp);
    }    
## creating a path to the index page (home.html)
    
    $index = "<li><a href=\"".$depth."home.html\">1 Bibledit</a></li>";

## identfiying and creating a link to the parent
    
    my $parent;


    if ( $dir eq "site" ) {
        $parent = "";
    }
    else {
        $parent = $dir;
        $parent =~ s/.*?\/([a-z,A-Z,0-9,\-]+?)$/$1/;
        $parent =
            "<li><a href=\"../" 
          . $parent
          . ".html\">"
          . ucfirst($parent)
          . "</a></li>";
    }

## link to self

    my $home = $name;
    $home =~ s/[^\/]*\///g;

# list of children pages

    $children = $name;
    $children =~ s/\.html//;
    if ( $menu{$children} ) {
        $children = "<ul>" . $menu{$children} . "</ul>";
        $children =~ s/href=\"/href=\"$home\//g;
        $children =~ s/\.html\//\//g;
    }
    else {
        $children = "";
    }

# list of sibling pages

    my $siblings =
      $menu{ ( $_->getElementsByTagName("dir") )[0]->textContent() };

    $siblings =~ s/$dir\///g;
    $siblings =~ s/<a[^>]*?>$title<\/a>/$title.$children/e;

# creating a unified menu and appending it in the right place

    my $menu = $index . $parent . "<hr/>" . $siblings;

    if ($menu) {
        ( $_->getElementsByTagName("div") )[0]->appendWellBalancedChunk($menu);
    }
# fix internal links
    {	my @links = $_->getElementsByTagName("a");
        foreach (@links) {
          my $ref = $_->getAttribute("href");
          $ref =~ s/SELFREFERENCE/$depth/;
          $_->setAttribute("href",$ref);
        }
    }

# adding the license and a link to the stylesheet
    ( $_->getElementsByTagName("head") )[0]->appendWellBalancedChunk("<link href=\"".$depth."bibledit.css\" rel=\"stylesheet\" type=\"text/css\"\/>");
    $license = `cat license`;
    ( $_->getElementsByTagName("head") )[0]->appendWellBalancedChunk($license);


}

    

# creating and writing the finalised page

foreach (@pages) {
    my $dir  = ( $_->getElementsByTagName("dir") )[0];
    my $name = ( $_->getElementsByTagName("name") )[0];
    my $html = ( $_->getElementsByTagName("html") )[0];
    make_path( $dir->textContent() );

    open FILE, ">", $name->textContent();
    print FILE $tidy->clean( $html->toString() );
    close FILE;
}

# putting the style sheet in the correct place

copy ("bibledit.css","site/bibledit.css");
