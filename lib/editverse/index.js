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
  $ ("#usfmeditor").on ("paste cut keydown", verseEditorChanged);
  $ (window).on ("unload", verseEditorUnload);
  $ ("#usfmeditor").on ("paste", function (e) {
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    document.execCommand ("insertHTML", false, data);
  });
  verseIdPoller ();
  $ ("#usfmeditor").focus ();
});


var verseBible;
var verseBook;
var verseNavigationBook;
var verseChapter;
var verseNavigationChapter;
var verseVerse;
var verseNavigationVerse;
var verseEditorChangedTimeout;
var verseLoadedText;
var verseIdChapter = 0;
var verseIdTimeout;
var verseReload = false;
var verseCaretPosition = 0;
var versePreviousWidth;
var versePreviousHeight;
var verseEditorTextChanged = false;
var verseSaveAsync;


function navigationNewPassage ()
{
  if (typeof navigationBook != 'undefined') {
    verseNavigationBook = navigationBook;
    verseNavigationChapter = navigationChapter;
    verseNavigationVerse = navigationVerse;
  } else if (parent.window.navigationBook != 'undefined') {
    verseNavigationBook = parent.window.navigationBook;
    verseNavigationChapter = parent.window.navigationChapter;
    verseNavigationVerse = parent.window.navigationVerse;
  } else {
    return;
  }
  verseEditorSaveChapter ();
  verseReload = false;
  verseEditorLoadChapter ();
}


function verseEditorLoadChapter ()
{
  if ((verseNavigationBook != verseBook) || (verseNavigationChapter != verseChapter) || (verseNavigationVerse != verseVerse) || verseReload) {
    verseBible = navigationBible;
    verseBook = verseNavigationBook;
    verseChapter = verseNavigationChapter;
    verseVerse = verseNavigationVerse;
    verseIdChapter = 0;
    $ ("#usfmeditor").focus;
    verseCaretPosition = getCaretPosition ();
    $.ajax ({
      url: "load.php",
      type: "GET",
      data: { bible: verseBible, book: verseBook, chapter: verseChapter, verse: verseVerse },
      success: function (response) {
        // Checksumming.
        response = checksum_receive (response);
        if (response !== false) {
          $ ("#usfmeditor").empty ();
          $ ("#usfmeditor").append (response);
          verseEditorStatus (verseEditorVerseLoaded);
          verseLoadedText = response;
          if (verseReload) {
            positionCaret (verseCaretPosition);
          }
          verseReload = false;
        } else {
          // Checksum error: Reload.
          verseReload = true;
          verseEditorLoadChapter ();
        }
      },
    });
  }
}


function verseEditorUnload ()
{
  verseEditorSaveChapter (true);
}


function verseEditorSaveChapter (sync)
{
  if (!verseEditorWriteAccess) return;
  verseEditorTextChanged = false;
  if (!verseBible) return;
  if (!verseBook) return;
  var usfm = $ ("#usfmeditor").text ();
  if (usfm == verseLoadedText) return;
  verseEditorStatus (verseEditorVerseSaving);
  verseLoadedText = usfm;
  verseIdChapter = 0;
  verseSaveAsync = true;
  if (sync) verseSaveAsync = false;
  var checksum = checksum_get (usfm);
  $.ajax ({
    url: "save.php",
    type: "POST",
    async: verseSaveAsync,
    data: { bible: verseBible, book: verseBook, chapter: verseChapter, verse: verseVerse, usfm: usfm, checksum: checksum },
    error: function (jqXHR, textStatus, errorThrown) {
      verseEditorStatus (verseEditorChapterRetrying);
      verseLoadedText = "";
      verseEditorChanged ();
      if (!verseSaveAsync) verseEditorSaveChapter (true);
    },
    success: function (response) {
      verseEditorStatus (response);
      verseSaveAsync = true;
    },
  });
}


function verseEditorChanged ()
{
  verseEditorTextChanged = true;
  if (verseEditorChangedTimeout) {
    clearTimeout (verseEditorChangedTimeout);
  }
  verseEditorChangedTimeout = setTimeout (verseEditorSaveChapter, 1000);
}


function verseEditorStatus (text)
{
  $ ("#usfmstatus").empty ();
  $ ("#usfmstatus").append (text);
}


function verseIdPoller ()
{
  if (verseIdTimeout) {
    clearTimeout (verseIdTimeout);
  }
  verseIdTimeout = setTimeout (verseEditorPollId, 1000);
}


function verseEditorPollId ()
{
  $.ajax ({
    url: "id.php",
    type: "GET",
    data: { bible: verseBible, book: verseBook, chapter: verseChapter },
    success: function (response) {
      if (verseIdChapter != 0) {
        if (response != verseIdChapter) {
          verseReload = true;
          verseEditorLoadChapter ();
          verseIdChapter = 0;
        }
      }
      verseIdChapter = response;
    },
    complete: function (xhr, status) {
      verseIdPoller ();
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


