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


var passagesTimeout;
var identifier;
var pollerTimeout;
var translationsTimeout;
var searchResultApplier;


$(document).ready (function () {
  $ ("#passages").focus ();
  $ ("#passages").on ("paste cut keydown", function (event) {
    if (passagesTimeout) clearTimeout (passagesTimeout);
    passagesTimeout = setTimeout (passagesChanged, 500);
  });
  $ ("#translations").on ("paste cut keydown", function (event) {
    if (translationsTimeout) clearTimeout (translationsTimeout);
    translationsTimeout = setTimeout (translationsChanged, 500);
  });
  rangy.init ();
  searchResultApplier = rangy.createCssClassApplier("searchResult");
});


function passagesChanged ()
{
  identifier = Math.floor (( Math.random () * 1000000) + 1000000);
  var passages = $('#passages').val();
  $.ajax ({
    url: "passages.php",
    type: "POST",
    data: { id: identifier, passages: passages },
    success: function (response) {
      $ ("#texts").empty ();
      $ ("#texts").append (response);
      translationsChanged ();
    },
    complete: function (xhr, status) {
      delayedPoll ();
    }
  });
}


function delayedPoll ()
{
  if (pollerTimeout) {
    clearTimeout (pollerTimeout);
  }
  pollerTimeout = setTimeout (poll, 1000);
}


function poll ()
{
  $.ajax ({
    url: "poll.php",
    type: "GET",
    data: { id: identifier },
    success: function (response) {
      if (response != "") {
        $ ("#texts").empty ();
        $ ("#texts").append (response);
        translationsChanged ();
      }
    },
    complete: function (xhr, status) {
      delayedPoll ();
    }
  });
}


function translationsChanged ()
{
  var range = rangy.createRange();
  var searchScopeRange = rangy.createRange();
  searchScopeRange.selectNodeContents (document.body);

  var options = {
      caseSensitive: false,
      wholeWordsOnly: false,
      withinRange: searchScopeRange,
      direction: "forward"
  };

  range.selectNodeContents(document.body);
  searchResultApplier.undoToRange(range);

  var translations = $('#translations').val();
  translations = $.trim (translations);
  if (translations == "") return;
  translations = translations.split ("\n");

  for (i in translations) {
    while (range.findText (translations [i], options)) {
      searchResultApplier.applyToRange(range);
      range.collapse (false);
    }
  }

}

