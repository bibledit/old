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

  oneverseBindUnselectable ();
  $ ("#stylebutton").on ("click", oneverseStylesButtonHandler);
  $ (window).on ("keydown", oneverseWindowKeyHandler);

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


/*
 
 Section for the new Passage event from the Navigator.
 
*/


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


/*
 
Section for editor load and save.
 
*/


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


/*
 
Portion dealing with triggers for editor's content change.
 
*/


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


/*
 
Section for user interface updates and feedback.
 
*/


function oneverseEditorStatus (text)
{
  $ ("#onestatus").empty ();
  $ ("#onestatus").append (text);
}


/*
 
Section for polling the server for updates on the loaded chapter.
 
*/


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


/*
 
Section for getting and setting the caret position.

*/


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


/*
 
Section for loading prefix and suffix.

*/


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


/*

Section for scrolling the editable portion into the center.

*/


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


/*
 
Section for the styles handling.
 
*/


function oneverseStylesButtonHandler ()
{
  if (!oneverseEditorWriteAccess) return;
  $.get ("/edit/styles", function (response) {
    oneverseShowResponse (response);
    oneverseBindUnselectable ();
    oneverseDynamicClickHandlers ();
  });
  return false;
}


function oneverseBindUnselectable ()
{
  var elements = $ (".unselectable");
  elements.off ("mousedown");
  elements.on ("mousedown", function (event) {
    event.preventDefault();
  });
}


function oneverseShowResponse (response)
{
  if (!oneverseEditorWriteAccess) return;
  $ ("#stylebutton").hide ();
  $ ("#nostyles").hide ();
  var area = $ ("#stylesarea");
  area.empty ();
  area.append (response);
}


function oneverseClearStyles ()
{
  var area = $ ("#stylesarea");
  area.empty ();
  $ ("#stylebutton").show ();
  $ ("#nostyles").show ();
}


function oneverseDynamicClickHandlers ()
{
  var elements = $ ("#stylesarea > a");
  elements.on ("click", function (event) {
    event.preventDefault();
    oneverseClearStyles ();
    $ ("#editor").focus ();
    var href = event.currentTarget.href;
    href = href.substring (href.lastIndexOf ('/') + 1);
    if (href == "cancel") return;
    if (href == "all") {
      oneverseDisplayAllStyles ();
    } else {
      oneverseRequestStyle (href);
    }
  });
  
  $ ("#styleslist").on ("change", function (event) {
    var selection = $ ("#styleslist option:selected").text ();
    var style = selection.substring (0, selection.indexOf (" "));
    event.preventDefault();
    oneverseClearStyles ();
    $ ("#editor").focus ();
    oneverseRequestStyle (style);
  });
}


function oneverseRequestStyle (style)
{
  $.get ("/edit/styles?style=" + style, function (response) {
    response = response.split ("\n");
    var style = response [0];
    var action = response [1];
    if (action == "p") {
      oneverseApplyParagraphStyle (style);
      oneverseEditorChanged ();
    } else if (action == 'c') {
      oneverseApplyCharacterStyle (style);
      oneverseEditorChanged ();
    } else if (action == 'n') {
      applyNotesStyle (style); // Todo rename / implement.
      oneverseEditorChanged ();
    } else if (action == "m") {
      oneverseApplyMonoStyle (style);
      oneverseEditorChanged ();
    }
  });
}


function oneverseDisplayAllStyles ()
{
  $.get ("/edit/styles?all=", function (response) {
    oneverseShowResponse (response);
    oneverseBindUnselectable ();
    oneverseDynamicClickHandlers ();
  });
}


function oneverseApplyParagraphStyle (style)
{
  if (!oneverseEditorWriteAccess) return;
  $ ("#editor").focus ();
  var parent = rangy.getSelection().anchorNode.parentElement;
  parent = $ (parent);
  var paragraph = parent.closest ("p");
  paragraph.removeClass ();
  paragraph.addClass (style);
}


function oneverseApplyCharacterStyle (style)
{
  if (!oneverseEditorWriteAccess) return;
  $ ("#editor").focus ();
  var cssApplier = rangy.createCssClassApplier (style);
  cssApplier.toggleSelection ();
}


function oneverseApplyMonoStyle (style)
{
  if (!oneverseEditorWriteAccess) return;
  
  $ ("#editor").focus ();
  
  var parent = rangy.getSelection().anchorNode.parentElement;
  parent = $ (parent);
  var paragraph = parent.closest ("p");
  
  paragraph.removeClass ();
  paragraph.addClass ("mono");
  
  var text = paragraph.text ();
  
  var char = text.substring (0, 1);
  if (char == "\\") {
    text = text.substring (1, 10000);
    var pos = text.indexOf (' ');
    text = text.substring (pos + 1, 10000);
  }
  text = "\\" + style + " " + text;
  paragraph.text (text);
}


/*
 
Section with window events and their basic handlers.
 
*/


function oneverseWindowKeyHandler (event)
{
  if (!oneverseEditorWriteAccess) return;
  // Ctrl-S: Style.
  if ((event.ctrlKey == true) && (event.keyCode == 83)) {
    oneverseStylesButtonHandler ();
    return false;
  }
  // Escape.
  if (event.keyCode == 27) {
    oneverseClearStyles ();
  }
}


