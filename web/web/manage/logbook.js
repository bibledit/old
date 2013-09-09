/*
Copyright (©) 2003-2013 Teus Benschop.

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
$(window).load (function () {
  $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 3000);
});

var getMore = function () 
{
  $.get ("logbook.php?id=" + lastID, function (response) {
    if (response == "") {
      setTimeout (getMore, 2000);
    } else {
      response = $.parseJSON (response);
      lastID = response ["id"];
      $ ("div").append ($ ("<p>" + response ["entry"] + "</p>"));
      $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 100);
      setTimeout (getMore, 100);
    }
  });  
}

setTimeout (getMore, 3000);
