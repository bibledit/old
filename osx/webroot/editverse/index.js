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
var verseVerseLoading;
var verseVerseLoaded;
var verseEditorChangedTimeout;
var verseLoadedText;
var verseIdChapter = 0;
var verseIdTimeout;
var verseReload = false;
var verseCaretPosition = 0;
var verseEditorTextChanged = false;
var verseSaveAsync;
var verseLoadAjaxRequest;


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
    verseVerseLoading = verseNavigationVerse;
    verseIdChapter = 0;
    $ ("#usfmeditor").focus;
    verseCaretPosition = getCaretPosition ();
    if (verseLoadAjaxRequest && verseLoadAjaxRequest.readystate != 4) {
      verseLoadAjaxRequest.abort();
    }
    verseLoadAjaxRequest = $.ajax ({
      url: "load",
      type: "GET",
      data: { bible: verseBible, book: verseBook, chapter: verseChapter, verse: verseVerseLoading },
      success: function (response) {
        // Checksumming.
        response = checksum_receive (response);
        if (response !== false) {
          $ ("#usfmeditor").empty ();
          $ ("#usfmeditor").append (response);
          verseEditorStatus (verseEditorVerseLoaded);
          verseLoadedText = response;
          verseVerseLoaded = verseVerseLoading;
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
    url: "save",
    type: "POST",
    async: verseSaveAsync,
    data: { bible: verseBible, book: verseBook, chapter: verseChapter, verse: verseVerseLoaded, usfm: usfm, checksum: checksum },
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


function verseEditorChanged (event)
{
  if (editKeysIgnoreForContentChange (event)) return;
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
    url: "../edit/id",
    type: "GET",
    data: { bible: verseBible, book: verseBook, chapter: verseChapter },
    cache: false,
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


