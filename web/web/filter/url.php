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


class Filter_Url
{


  // This function redirects the browser to $path.
  // $path is relative to the URL of the current script.
  // E.g.: the current script is http://localhost/bibledit-web
  // and the $path is index.php
  // then the browser is instructed to redirect to 
  // http://localhost/bibledit-web/index.php.
  public static function redirect ($path)
  {
    // E.g. http or https.
    if (isset ($_SERVER['HTTPS']) && !empty ($_SERVER['HTTPS']) && $_SERVER ['HTTPS'] != 'off') $scheme = 'https';
    else $scheme = 'http';
    
    // E.g. localhost
    @$host = $_SERVER ["SERVER_NAME"];
    
    @$port = $_SERVER ["SERVER_PORT"];
    if ($port == 80) $port = "";
    if ($port == 443) $port = "";
    if ($port) $port = ":$port";
    
    // E.g. /bibledit-web/
    $self = $_SERVER ["PHP_SELF"];
    
    // Whether this script is called by directory, or by e.g. index.php.
    @$extension = pathinfo ($self, PATHINFO_EXTENSION);
    if ($extension == "php") $self = dirname ($self);

    // Ensure slash at the end.
    if (substr ($self, -1) != "/") $self = $self .= "/";
    
    $location = $scheme . "://" . $host . $port . $self . $path;
    
    // A location header needs to contain an absolute url, like http://localhost/some.php.
    // See 14.30 in the specification http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html.
    header("Location: $location");
  }


}

?>
