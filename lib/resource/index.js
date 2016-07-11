/*
Copyright (©) 2003-2016 Teus Benschop.

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
  if (swipe_operations) $ ("body").on ("swipeleft", resourceSwipeLeft);
  if (swipe_operations) $ ("body").on ("swiperight", resourceSwipeRight);
});


var resourceBook;
var resourceChapter;
var resourceVerse;
var resourceAjaxRequest;
var resourceDoing;
var resourceAborting = false;


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
  resourceAborting = true;
  try {
    resourceAjaxRequest.abort ();
  } catch (err) {
  }
  for (i = 1; i <= resourceCount; i++) {
    $ ("#line" + i).show ();
    $ ("#name" + i).show ();
    $ ("#loading" + i).show ();
    $ ("#content" + i).empty ();
  }
  resourceAborting = false;
  resourceDoing = 0;
  resourceGetOne ();
}


function resourceGetOne ()
{
  if (resourceAborting) return;
  resourceDoing++;
  if (resourceDoing > resourceCount) {
    return;
  }
  resourceAjaxRequest = $.ajax ({
    url: "get",
    type: "GET",
    data: { resource: resourceDoing, book: resourceBook, chapter: resourceChapter, verse: resourceVerse },
    success: function (response) {
      if (response == "") {
        $ ("#line" + resourceDoing).hide ();
        $ ("#name" + resourceDoing).hide ();
      } else {
        if (response.charAt (0) == "$") {
          $ ("#name" + resourceDoing).hide ();
          response = response.substring (1);
        }
        $ ("#content" + resourceDoing).append (response);
      }
      $ ("#loading" + resourceDoing).hide ();
      navigationSetup ();
    },
    error: function (jqXHR, textStatus, errorThrown) {
      resourceDoing--;
    },
    complete: function (jqXHR) {
      resourceGetOne ();
    }
  });
}


function resourceSwipeLeft (event)
{
  if (typeof navigateNextVerse != 'undefined') {
    navigateNextVerse (event);
  } else if (parent.window.navigateNextVerse != 'undefined') {
    parent.window.navigateNextVerse (event);
  }
}


function resourceSwipeRight (event)
{
  if (typeof navigatePreviousVerse != 'undefined') {
    navigatePreviousVerse (event);
  } else if (parent.window.navigatePreviousVerse != 'undefined') {
    parent.window.navigatePreviousVerse (event);
  }
}
