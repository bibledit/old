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
  navigationNewPassage ();
});


var resourceBook;
var resourceChapter;
var resourceVerse;
$.ajaxRequests = [];


function navigationNewPassage ()
{
  if (typeof navigationBook != 'undefined') {
    resourceBook = navigationBook;
    resourceChapter = navigationChapter;
    resourceVerse = navigationVerse;
  } else if (parent.window.navigationBook != 'undefined') {
    resourceBook = parent.window.navigationBook;
    resourceChapter = parent.window.navigationChapter;
    resourceVerse = parent.window.navigationVerse;
  } else {
    return;
  }
  if (resourceBook == undefined) return;
  $.ajaxRequests.abortAll ();
  for (i = 1; i <= resourceCount; i++) {
    $ ("#line" + i).show ();
    $ ("#name" + i).show ();
    $ ("#loading" + i).show ();
    $ ("#content" + i).empty ();
    resourceGet (i);
  }
}


function resourceGet (i)
{
  var request;
  request = $.ajax ({
    context: i,
    url: "get.php",
    type: "GET",
    data: { resource: i, book: resourceBook, chapter: resourceChapter, verse: resourceVerse },
    success: function (response) {
      var i = Number (this);
      if (response == "") {
        $ ("#line" + i).hide ();
        $ ("#name" + i).hide ();
      }
      $ ("#loading" + i).hide ();
      $ ("#content" + i).append (response);
      navigationSetup ();
    },
    error: function (jqXHR, textStatus, errorThrown) {
      var i = Number (this);
      resourceGet (i);
    },
    complete: function (jqXHR) {
      var index = $.ajaxRequests.indexOf (jqXHR);
      if (index > -1) {
        $.ajaxRequests.splice (index, 1);
      }      
    }
  });
  $.ajaxRequests.push (request);
}


// This function is used to abort pending ajax requests in case new resources are requested.
// Aborting pending requests makes the interface more responsive.
$.ajaxRequests.abortAll = function () {
  $ (this).each (function (idx, jqXHR) { 
    console.log (jqXHR);
    jqXHR.abort ();
  });
  $.ajaxRequests.length = 0
};
