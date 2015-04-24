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
  $ ("#oneeditor").on ("paste cut keydown", oneverseEditorChanged);
  $ (window).on ("unload", oneverseEditorUnload);
  $ ("#oneeditor").on ("paste", function (e) {
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    document.execCommand ("insertHTML", false, data);
  });
  oneverseIdPoller ();
  $ ("#oneeditor").focus ();
});


var oneverseBible;
var oneverseBook;
var oneverseNavigationBook;
var oneverseChapter;
var oneverseNavigationChapter;
var oneverseVerse;
var oneverseNavigationVerse;
var oneverseVerseLoading;
var oneverseVerseLoaded;
var oneverseEditorChangedTimeout;
var oneverseLoadedText;
var oneverseIdChapter = 0;
var oneverseIdTimeout;
var oneverseReload = false;
var oneverseEditorTextChanged = false;
var oneverseSaveAsync;
var oneverseLoadAjaxRequest;


function navigationNewPassage ()
{
  if (typeof navigationBook != 'undefined') {
    oneverseNavigationBook = navigationBook;
    oneverseNavigationChapter = navigationChapter;
    oneverseNavigationVerse = navigationVerse;
  } else if (parent.window.navigationBook != 'undefined') {
    oneverseNavigationBook = parent.window.navigationBook;
    oneverseNavigationChapter = parent.window.navigationChapter;
    oneverseNavigationVerse = parent.window.navigationVerse;
  } else {
    return;
  }
  oneverseEditorSaveVerse ();
  oneverseReload = false;
  oneverseEditorLoadVerse ();
}


function oneverseEditorLoadVerse ()
{
  if ((oneverseNavigationBook != oneverseBook) || (oneverseNavigationChapter != oneverseChapter) || (oneverseNavigationVerse != oneverseVerse) || oneverseReload) {
    oneverseBible = navigationBible;
    oneverseBook = oneverseNavigationBook;
    oneverseChapter = oneverseNavigationChapter;
    oneverseVerse = oneverseNavigationVerse;
    oneverseVerseLoading = oneverseNavigationVerse;
    oneverseIdChapter = 0;
    $ ("#oneeditor").focus;
    if (oneverseLoadAjaxRequest && oneverseLoadAjaxRequest.readystate != 4) {
      oneverseLoadAjaxRequest.abort();
    }
    oneverseLoadAjaxRequest = $.ajax ({
      url: "load",
      type: "GET",
      data: { bible: oneverseBible, book: oneverseBook, chapter: oneverseChapter, verse: oneverseVerseLoading, part: "verse" },
      success: function (response) {
        // Checksumming.
        response = checksum_receive (response);
        if (response !== false) {
          $ ("#oneeditor").empty ();
          $ ("#oneeditor").append (response);
          oneverseVerseLoaded = oneverseVerseLoading;
          oneverseEditorStatus (oneverseEditorVerseLoaded);
          oneverseLoadedText = response;
          oneverseReload = false;
        } else {
          // Checksum error: Reload.
          oneverseReload = true;
          oneverseEditorLoadVerse ();
        }
      },
    });
  }
  oneverseEditorLoadPrefix ();
  oneverseEditorLoadSuffix ();
}


function oneverseEditorUnload ()
{
  oneverseEditorSaveVerse (true);
}


function oneverseEditorSaveVerse (sync)
{
  if (!oneverseEditorWriteAccess) return;
  oneverseEditorTextChanged = false;
  if (!oneverseBible) return;
  if (!oneverseBook) return;
  var html = $ ("#oneeditor").html ();
  if (html == oneverseLoadedText) return;
  oneverseEditorStatus (oneverseEditorVerseSaving);
  oneverseLoadedText = html;
  oneverseIdChapter = 0;
  oneverseSaveAsync = true;
  if (sync) oneverseSaveAsync = false;
  var checksum = checksum_get (html);
  $.ajax ({
    url: "save",
    type: "POST",
    async: oneverseSaveAsync,
    data: { bible: oneverseBible, book: oneverseBook, chapter: oneverseChapter, verse: oneverseVerseLoaded, html: html, checksum: checksum },
    error: function (jqXHR, textStatus, errorThrown) {
      oneverseEditorStatus (oneverseEditorChapterRetrying);
      oneverseLoadedText = "";
      oneverseEditorChanged ();
      if (!oneverseSaveAsync) oneverseEditorSaveVerse (true);
    },
    success: function (response) {
      oneverseEditorStatus (response);
      oneverseSaveAsync = true;
    },
  });
}


function oneverseEditorChanged (event)
{
  if (!oneverseEditorWriteAccess) return;
  if (editKeysIgnoreForContentChange (event)) return;
  oneverseEditorTextChanged = true;
  if (oneverseEditorChangedTimeout) {
    clearTimeout (oneverseEditorChangedTimeout);
  }
  oneverseEditorChangedTimeout = setTimeout (oneverseEditorSaveVerse, 1000);
}


function oneverseEditorStatus (text)
{
  $ ("#onestatus").empty ();
  $ ("#onestatus").append (text);
}


function oneverseIdPoller ()
{
  if (oneverseIdTimeout) {
    clearTimeout (oneverseIdTimeout);
  }
  oneverseIdTimeout = setTimeout (oneverseEditorPollId, 1000);
}


function oneverseEditorPollId ()
{
  $.ajax ({
    url: "../edit/id",
    type: "GET",
    data: { bible: oneverseBible, book: oneverseBook, chapter: oneverseChapter },
    success: function (response) {
      if (oneverseIdChapter != 0) {
        if (response != oneverseIdChapter) {
          oneverseReload = true;
          oneverseEditorLoadVerse ();
          oneverseIdChapter = 0;
        }
      }
      oneverseIdChapter = response;
    },
    complete: function (xhr, status) {
      oneverseIdPoller ();
    }
  });
}


function getCaretPosition ()
{
  var position = undefined;
  if ($ ("#oneeditor").is (":focus")) {
    var sel = rangy.getSelection ();
    var range = sel.getRangeAt(0);
    position = range.startOffset;
  }
  return position;
}


function positionCaret (position)
{
  $ ("#oneeditor").focus ();
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


// Loading prefix and suffix.


var oneverseEditorLoadPrefixRetry;
var oneverseEditorLoadSuffixRetry;


function oneverseEditorLoadPrefix ()
{
  $.ajax ({
    url: "load",
    type: "GET",
    data: { bible: oneverseBible, book: oneverseBook, chapter: oneverseChapter, verse: oneverseVerse, part: "prefix" },
    success: function (response) {
      $ ("#oneprefix").empty ();
      $ ("#oneprefix").append (response);
      oneverseScrollVerseIntoView ();
      oneversePositionCaret ();
    },
    error: function (jqXHR, textStatus, errorThrown) {
      oneverseEditorLoadPrefixRetry ();
    },
  });
}


function oneverseEditorLoadSuffix ()
{
  $.ajax ({
    url: "load",
    type: "GET",
    data: { bible: oneverseBible, book: oneverseBook, chapter: oneverseChapter, verse: oneverseVerse, part: "suffix" },
    success: function (response) {
      $ ("#onesuffix").empty ();
      $ ("#onesuffix").append (response);
    },
    error: function (jqXHR, textStatus, errorThrown) {
      oneverseEditorLoadSuffixRetry ();
    },
  });
}


function oneverseEditorLoadPrefixRetry ()
{
  if (oneverseEditorLoadPrefixRetry) {
    clearTimeout (oneverseEditorLoadPrefixRetry);
  }
  oneverseEditorLoadPrefixRetry = setTimeout (oneverseEditorLoadPrefix, 500);
}


function oneverseEditorLoadSuffixRetry ()
{
  if (oneverseEditorLoadSuffixRetry) {
    clearTimeout (oneverseEditorLoadSuffixRetry);
  }
  oneverseEditorLoadSuffixRetry = setTimeout (oneverseEditorLoadSuffix, 500);
}


// Scrolling the editable portion into the center.


function oneverseScrollVerseIntoView ()
{
  $ ("body").stop ();
  var element = $ ("#oneeditor");
  var offset = element.offset ();
  var verseTop = offset.top;
  var viewportHeight = $(window).height ();
  var scrollTo = verseTop - (viewportHeight / 2);
  $ ("body").animate ({ scrollTop: scrollTo }, 500);
}


// Place the caret at the start of the text.


function oneversePositionCaret ()
{
  $ ("#oneeditor").focus ();
  setTimeout (oneversePositionCaretTimeout, 100);
}


function oneversePositionCaretTimeout ()
{
  var selection = rangy.getSelection ();
  selection.move ("character", 4);
}
