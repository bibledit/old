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
  rangy.init ();
  navigationNewPassage ();
  $ ("#usfmeditor").on ("paste cut keydown", usfmEditorChanged);
  $ (window).on ("unload", usfmEditorUnload ());
  $ ("#usfmeditor").on ("paste", function (e) {
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    document.execCommand ("insertHTML", false, data);
  });
  usfmIdPoller ();
  $ ("#usfmeditor").on ("paste cut click", usfmCaretChanged);
  $ ("#usfmeditor").on ("keydown", usfmHandleKeyDown);
  $ ("#usfmeditor").focus ();
  $ (window).on ("focus", usfmWindowFocused);
});


var usfmBible;
var usfmBook;
var usfmNavigationBook;
var usfmChapter;
var usfmNavigationChapter;
var usfmNavigationVerse;
var usfmEditorChangedTimeout;
var usfmLoadedText;
var usfmIdChapter = 0;
var usfmIdTimeout;
var usfmCaretTimeout;
var usfmReload = false;
var usfmCaretPosition = 0;
var usfmClarifierTimeout;
var usfmPreviousCaretTop;
var usfmPreviousWidth;
var usfmPreviousHeight;
var usfmEditorTextChanged = false;
var usfmSaveAsync;


function navigationNewPassage ()
{
  if (typeof navigationBook != 'undefined') {
    usfmNavigationBook = navigationBook;
    usfmNavigationChapter = navigationChapter;
    usfmNavigationVerse = navigationVerse;
  } else if (parent.window.navigationBook != 'undefined') {
    usfmNavigationBook = parent.window.navigationBook;
    usfmNavigationChapter = parent.window.navigationChapter;
    usfmNavigationVerse = parent.window.navigationVerse;
  } else {
    return;
  }
  usfmEditorSaveChapter ();
  usfmReload = false;
  usfmEditorLoadChapter ();
  positionCaretViaAjax ();
}


function usfmEditorLoadChapter ()
{
  if ((usfmNavigationBook != usfmBook) || (usfmNavigationChapter != usfmChapter) || usfmReload) {
    usfmBible = navigationBible;
    usfmBook = usfmNavigationBook;
    usfmChapter = usfmNavigationChapter;
    usfmIdChapter = 0;
    $ ("#usfmeditor").focus;
    usfmCaretPosition = getCaretPosition ();
    $.ajax ({
      url: "load.php",
      type: "GET",
      data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter },
      success: function (response) {
        // Checksumming.
        response = checksum_receive (response);
        if (response !== false) {
          $ ("#usfmeditor").empty ();
          $ ("#usfmeditor").append (response);
          usfmEditorStatus (usfmEditorChapterLoaded);
          usfmLoadedText = response;
          if (usfmReload) {
            positionCaret (usfmCaretPosition);
          } else {
            positionCaretViaAjax ();
          }
          usfmReload = false;
        } else {
          // Checksum error: Reload.
          usfmReload = true;
          usfmEditorLoadChapter ();
        }
      },
    });
  }
}


function usfmEditorUnload ()
{
  usfmEditorSaveChapter (true);
}


function usfmEditorSaveChapter (sync)
{
  if (!usfmEditorWriteAccess) return;
  usfmEditorTextChanged = false;
  if (!usfmBible) return;
  if (!usfmBook) return;
  var usfm = $ ("#usfmeditor").text ();
  if (usfm == usfmLoadedText) return;
  usfmEditorStatus (usfmEditorChapterSaving);
  usfmLoadedText = usfm;
  usfmIdChapter = 0;
  usfmSaveAsync = true;
  if (sync) usfmSaveAsync = false;
  var checksum = checksum_get (usfm);
  $.ajax ({
    url: "save.php",
    type: "POST",
    async: usfmSaveAsync,
    data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter, usfm: usfm, checksum: checksum },
    error: function (jqXHR, textStatus, errorThrown) {
      usfmEditorStatus (usfmEditorChapterRetrying);
      usfmLoadedText = "";
      usfmEditorChanged ();
      if (!usfmSaveAsync) usfmEditorSaveChapter (true);
    },
    success: function (response) {
      usfmEditorStatus (response);
      usfmSaveAsync = true;
    },
  });
}


function usfmEditorChanged ()
{
  usfmEditorTextChanged = true;
  if (usfmEditorChangedTimeout) {
    clearTimeout (usfmEditorChangedTimeout);
  }
  usfmEditorChangedTimeout = setTimeout (usfmEditorSaveChapter, 1000);
  restartCaretClarifier ();
}


function usfmEditorStatus (text)
{
  $ ("#usfmstatus").empty ();
  $ ("#usfmstatus").append (text);
}


function usfmIdPoller ()
{
  if (usfmIdTimeout) {
    clearTimeout (usfmIdTimeout);
  }
  usfmIdTimeout = setTimeout (usfmEditorPollId, 1000);
}


function usfmEditorPollId ()
{
  $.ajax ({
    url: "id.php",
    type: "GET",
    data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter },
    success: function (response) {
      if (usfmIdChapter != 0) {
        if (response != usfmIdChapter) {
          usfmReload = true;
          usfmEditorLoadChapter ();
          usfmIdChapter = 0;
        }
      }
      usfmIdChapter = response;
    },
    complete: function (xhr, status) {
      usfmIdPoller ();
    }
  });
}


function usfmCaretChanged ()
{
  if (usfmCaretTimeout) {
    clearTimeout (usfmCaretTimeout);
  }
  usfmCaretTimeout = setTimeout (usfmHandleCaret, 500);
  restartCaretClarifier ();
}


function usfmHandleKeyDown (event)
{
  // Ctrl-G: No action.
  if ((event.ctrlKey == true) && (event.keyCode == 71)) {
    return;
  }
  usfmCaretChanged ();
}


function usfmHandleCaret ()
{
  if (usfmEditorTextChanged) {
    usfmCaretChanged ();
    return;
  }
  if ($ ("#usfmeditor").is (":focus")) {
    var offset = getCaretPosition ();
    $.ajax ({
      url: "offset.php",
      type: "GET",
      data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter, offset: offset },
      success: function (response) {
      },
      error: function (jqXHR, textStatus, errorThrown) {
        // On (network) failure, reschedule the update.
        usfmCaretChanged ();
      }
    });
  }
}


function positionCaretViaAjax ()
{
  $ ("#usfmeditor").focus ();
  $.ajax ({
    url: "focus.php",
    type: "GET",
    data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter },
    success: function (response) {
      response = $.parseJSON (response);
      var start = response ["start"];
      var end = response ["end"];
      var offset = getCaretPosition ();
      if ((offset < start) || (offset > end)) {
        positionCaret (start + 3);
      }
      restartCaretClarifier ();
    }
  });
}


function getCaretPosition ()
{
  var position = undefined;
  if ($ ("#usfmeditor").is (":focus")) {
    var sel = rangy.getSelection ();
    var range = sel.getRangeAt(0);
    position = range.startOffset;
  }
  return position;
}


function positionCaret (position)
{
  $ ("#usfmeditor").focus ();
  var currentPosition = getCaretPosition ();
  if (currentPosition == undefined) return;
  if (position == undefined) return;
  var selection = rangy.getSelection ();
  selection.move ("character", position - currentPosition);
}


function usfmWindowFocused ()
{
  positionCaretViaAjax ();
}


function restartCaretClarifier ()
{
  if (usfmClarifierTimeout) {
    clearTimeout (usfmClarifierTimeout);
  }
  usfmClarifierTimeout = setTimeout (clarifyCaret, 100);
}


function getSelectionCoordinates() {
  var x = 0, y = 0;
  var sel = document.selection, range;
  if (sel) {
    if (sel.type != "Control") {
      range = sel.createRange();
      range.collapse(true);
      x = range.boundingLeft;
      y = range.boundingTop;
    }
  } else if (window.getSelection) {
    sel = window.getSelection();
    if (sel.rangeCount) {
      range = sel.getRangeAt(0).cloneRange();
      if (range.getClientRects) {
        range.collapse(true);
        var rect = range.getClientRects()[0];
        x = rect.left;
        y = rect.top;
      }
    }
  }
  return { x: x, y: y };
}


function clarifyCaret ()
{
  var scrolltop = $ ("body").scrollTop ();
  var coordinates = getSelectionCoordinates ();
  var caretTop = coordinates.y + scrolltop;
  if (caretTop == usfmPreviousCaretTop) return;
  usfmPreviousCaretTop = caretTop;
  var viewportHeight = $(window).height ();
  $ ("body").animate ({ scrollTop: caretTop - (viewportHeight / 2) }, 500);
  var barOffset = $ ("#caretbar").offset ().top;
  $ ("#caretbar").empty ();
  $ ("#caretbar").prepend ("<span><mark>￫</mark></span>");
  var barTop = barOffset + $ ("#caretbar").height ();
  while (barTop <= caretTop) {
    $ ("#caretbar").prepend ("\n");
    barTop = barOffset + $ ("#caretbar").height ();
  }
  $ ("#caretbar").prepend ("\n");
}


