/*
Copyright (©) 2003-2015 Teus Benschop.

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


var lexiconBook;
var lexiconChapter;
var lexiconVerse;


function navigationNewPassage ()
{
  if (typeof navigationBook != 'undefined') {
    lexiconBook = navigationBook;
    lexiconChapter = navigationChapter;
    lexiconVerse = navigationVerse;
  } else if (parent.window.navigationBook != 'undefined') {
    lexiconBook = parent.window.navigationBook;
    lexiconChapter = parent.window.navigationChapter;
    lexiconVerse = parent.window.navigationVerse;
  } else {
    return;
  }
  if (lexiconBook == undefined) return;
  lexiconEnglish();
}


function lexiconEnglish ()
{
  $.ajax ({
    url: "english",
    type: "GET",
    data: { book: lexiconBook, chapter: lexiconChapter, verse: lexiconVerse },
    success: function (response) {
      var element = $ ("#english");
      element.empty ();
      element.append (response);
      element.off ("click");
      element.on ("click", lexiconDefinition);
    },
  });
}


function lexiconDefinition (event)
{
  event.preventDefault ();
  var href = lexiconBasename (event.target.href);
  $.ajax ({
  url: "definition",
  type: "GET",
  data: { id: href },
  success: function (response) {
    var element = $ ("#definition");
    element.empty ();
    element.append (response);
    element.off ("click");
    // Todo element.on ("click", lexiconDefinition);
  },
    });
}


function lexiconBasename (url)
{
  return url.substring(url.lastIndexOf('/') + 1);
}