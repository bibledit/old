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


/*
This generates a start menu.

It is based on an array of all possible menu entries.
It reads the access levels of those entries.
It only keeps those menu entries the currently logged-in user has access to.

It originally self-organized the entries such that the ones used most often came earlier in the menu.
But menu entries moving around does no good. It creates confusion instead.
Therefore it was removed again.
*/


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::GUEST_LEVEL);


@$url = $_GET ['url'];
if (isset ($url)) {
  // Dissect the $url, as it may contain a query component also.
  $path = parse_url ($url, PHP_URL_PATH);
  $query = parse_url ($url, PHP_URL_QUERY);
  // When a file with the $url and the .php extension exists, go there.
  // Else go to the given $url.
  $filename = realpath ("../$path.php");
  if (file_exists ($filename)) {
    $url = "$path.php";
    if ($query) {
      $url .= "?$query";
    }
  }
  Filter_Url::redirect ("../$url");
  die;
}


Assets_Page::header (Locale_Translate::_("Start"));
Assets_Page::footer ();


?>
