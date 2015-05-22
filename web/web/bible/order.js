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


$(document).ready (function () {
  $( "#sortable" ).sortable({ update: sorted });
  $( "#sortable" ).disableSelection();
  $ (".drag").hover (function () {
    $(this).css('cursor','move');
  }, function () {
    $(this).css ('cursor','auto');
  });
});


var sorted = function (event, ui) {
  var list = new Array ();
  $ ("#sortable").children ().each (function (i) {
    var data = $ (this.childNodes [1]);
    list.push (data.text ());
  });
  $.post ("order.php?bible=" + orderBible, {'order': list.toString()});
}
