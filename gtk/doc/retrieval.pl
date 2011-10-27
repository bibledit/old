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
use LWP::UserAgent;
use utf8;

my $ua = LWP::UserAgent->new;
my $response =
  $ua->get('https://sites.google.com/site/bibledit/system/feeds/sitemap');

$parser  = XML::LibXML->new();
$content = $parser->parse_string( $response->content );

@pages = $content->getElementsByTagName("loc");

foreach (@pages) {
    my $page_uri = $_->textContent();
    $page_uri =~ s/site\/bibledit/feeds\/content\/site\/bibledit?path=/;

    my $page_name = $_->textContent();
    $page_name =~
      s/https:\/\/sites.google.com\/site\/bibledit\/(.*)/site\/$1\.html/;

    my $page         = $ua->get($page_uri);
    my $page_parse   = $parser->parse_string( $page->content );
    my $page_content = "";

    if ( ( $page_parse->getElementsByTagName("div") )[1] ) {
        $page_content .=
          ( $page_parse->getElementsByTagName("div") )[1]->toString();
    }
    $page_content =~
      s/http:\/\/sites.google.com\/site\/bibledit\/(.*?)\"/SELFREFERENCE$1\.html\"/g;
    $page_content =~ s/<\/?div.*?>//g;
    $page_content =
        "<head\/><body><div id=\"menu\"></div><div id=\"content\"><h1>"
      . ( $page_parse->getElementsByTagName("title") )[1]->textContent()
      . "</h1>"
      . $page_content
      . "<\/div><\/body>";

    ($dir_name) = ( $page_name =~ /(.*?)\/[^\/]*?$/ );

    my $name = $content->createElement("name");
    $name->appendTextNode($page_name);
    my $dir  = $content->createElement("dir");
    my $html = $content->createElement("html");

    $html->appendWellBalancedChunk($page_content);
    $dir->appendTextNode($dir_name);

    $_->addSibling($dir);
    $_->addSibling($html);
    $_->addSibling($name);
}

open FILE, ">", "site.xml";
print FILE $content->toString();
close;

