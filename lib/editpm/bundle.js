(function e(t,n,r){function s(o,u){if(!n[o]){if(!t[o]){var a=typeof require=="function"&&require;if(!u&&a)return a(o,!0);if(i)return i(o,!0);var f=new Error("Cannot find module '"+o+"'");throw f.code="MODULE_NOT_FOUND",f}var l=n[o]={exports:{}};t[o][0].call(l.exports,function(e){var n=t[o][1][e];return s(n?n:e)},l,l.exports,e,t,n,r)}return n[o].exports}var i=typeof require=="function"&&require;for(var o=0;o<r.length;o++)s(r[o]);return s})({1:[function(require,module,exports){
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


const {EditorState} = require("prosemirror-state")
const {MenuBarEditorView} = require("prosemirror-menu")
const {DOMParser, Schema} = require("prosemirror-model")
const {schema: baseSchema} = require("prosemirror-schema-basic")
const {addListNodes} = require("prosemirror-schema-list")
const {exampleSetup} = require("prosemirror-example-setup")


const schema = new Schema ({
  nodes: addListNodes(baseSchema.nodeSpec, "paragraph block*", "block"),
  marks: baseSchema.markSpec
})


var editorView;


/*
 const {EditorState} = require("prosemirror-state")
 const {EditorView} = require("prosemirror-view")
 const {MenuBarEditorView} = require("prosemirror-menu")
 const {DOMParser, Schema} = require("prosemirror-model")
 const {schema: baseSchema} = require("prosemirror-schema-basic")
 const history = require("prosemirror-history")
 const {keymap} = require("prosemirror-keymap")
 const {addListNodes} = require("prosemirror-schema-list")
 const {exampleSetup} = require("prosemirror-example-setup")
 
 
 const schema = new Schema({
 nodes: addListNodes(baseSchema.nodeSpec, "paragraph block*", "block"),
 marks: baseSchema.markSpec
 })
 
 
 let view = new MenuBarEditorView (
 document.getElementById ("editor"),
 {
 state: EditorState.create({
 doc: DOMParser.fromSchema (schema).parse (content),
 //schema: schema,
 plugins: [
 //history.history (),
 //keymap ({
 //"Mod-z": history.undo,
 //"Mod-Shift-z": history.redo
 //}),
 exampleSetup ({schema})
 ]
 }),
 onAction: function(action) {
 view.updateState (view.state.applyAction (action))
 }
 }
 )
 
 
 // Focus the editor (but for some reason it does not work).
 view.focus ();
 document.getElementById ("editor").focus ();
 */



$ (document).ready (function ()
{
  // Make the editor's menu to never scroll out of view.
  var bar = $ ("#editorheader").remove ();
  $ ("#workspacemenu").append (bar);

  editorView = new MenuBarEditorView (document.querySelector ("#editor"), {
    state: EditorState.create ({
      doc: DOMParser.fromSchema (schema).parse (content),
      plugins: exampleSetup ({schema})
    }),
    onAction (action) { view.updateState (view.editor.state.applyAction (action)) },
    onFocus () {
    }
  })
  
  navigationNewPassage ();
  
  /*

  $ ("#editor").on ("paste cut keydown", editorContentChanged);
  $ (window).on ("unload", editorUnload);

  $ ("#editor").on ("focus", editorWindowFocused);

  $ ("#editor").on ("paste", editorPaste);

  editorIdPollerTimeoutStart ();

  $ ("#editor").on ("click", editorCaretChangedByMouse);
  $ ("#editor").on ("keydown", editorCaretChangedByKeyboard);

  $ ("#editor").focus ();

  editorBindUnselectable ();
  $ ("#stylebutton").on ("click", editorStylesButtonHandler);
  $ (window).on ("keydown", editorWindowKeyHandler);

  positionCaretViaAjax ();
  
  if (swipe_operations) {
    $ ("body").swipe ( {
      swipeLeft:function (event, direction, distance, duration, fingerCount, fingerData) {
        editorSwipeLeft (event);
      },
      swipeRight:function (event, direction, distance, duration, fingerCount, fingerData) {
        editorSwipeRight (event);
      }
    });
  }
  
  if (unsentBibleDataTimeoutWarning) notifyItError (unsentBibleDataTimeoutWarning);
   */
});


/*

Section for the new Passage event from the Navigator.

*/


var editorNavigationBook;
var editorNavigationChapter;
var editorNavigationVerse;


function navigationNewPassage ()
{
  console.log (navigationBook); 
  if (typeof navigationBook != 'undefined') {
    editorNavigationBook = navigationBook;
    editorNavigationChapter = navigationChapter;
    editorNavigationVerse = navigationVerse;
  } else if (parent.window.navigationBook != 'undefined') {
    editorNavigationBook = parent.window.navigationBook;
    editorNavigationChapter = parent.window.navigationChapter;
    editorNavigationVerse = parent.window.navigationVerse;
  } else {
    return;
  }

  if ((editorNavigationBook != editorLoadedBook) || (editorNavigationChapter != editorLoadedChapter)) {
    //editorSaveChapter ();
    editorLoadChapter (false);
  } else {
    //editorScheduleCaretPositioning ();
  }
}


/*

Section for editor load and save.

Notes:
* It remembers the Bible, book, and chapter loaded.
  The reason for remembering these is, among others, that the active Bible / book / chapter
  on the server may change due to user actions, so when saving this chapter to the server,
  it passes the correct Bible / book / chapter to the server along with the updated text.
* While loading the chapter, do not set the "contenteditable" to false, then to true,
  because Google Chrome gets confused then. This was seen on version 33.
  Other types of browsers and other versions were not tested on this behaviour.

*/


var editorLoadedBible;
var editorLoadedBook;
var editorLoadedChapter;
var editorReferenceText;
var editorTextChanged = false;
var editorCaretPosition = 0;
var editorSaveAsync;
var editorSaving = false;


function editorLoadChapter (reload)
{
  editorLoadedBible = navigationBible;
  editorLoadedBook = editorNavigationBook;
  editorLoadedChapter = editorNavigationChapter;
  editorChapterIdOnServer = 0;
  editorCaretPosition = getCaretPosition ();
  editorCaretInitialized = false;
  $.ajax ({
    url: "load",
    type: "GET",
    data: { bible: editorLoadedBible, book: editorLoadedBook, chapter: editorLoadedChapter },
    success: function (response) {
      editorWriteAccess = checksum_readwrite (response);
      var contenteditable = ($ ("#editor").attr('contenteditable') === 'true');
      if (editorWriteAccess != contenteditable) $ ("#editor").attr('contenteditable', editorWriteAccess);
      // Checksumming.
      response = checksum_receive (response);
      if (response !== false) {
        // Only load new text when it is different.
        console.log (response); // Todo
        if (response != editorGetHtml ()) {
          $ ("#editor").empty ();
          $ ("#editor").append (response);
          editorStatus (editorChapterLoaded);
        }
        editorReferenceText = response;
        if (reload) {
          positionCaret (editorCaretPosition);
        } else {
        }
        editorScheduleCaretPositioning ();
      } else {
        // Checksum error: Reload.
        editorLoadChapter (false);
      }
      editorCaretInitialized = false;
    },
  });
}


function editorSaveChapter (sync)
{
  if (editorSaving) {
    editorContentChangedTimeoutStart ();
    return;
  }
  if (!editorWriteAccess) return;
  editorTextChanged = false;
  if (!editorLoadedBible) return;
  if (!editorLoadedBook) return;
  var html = editorGetHtml ();
  if (html == editorReferenceText) return;
  editorStatus (editorChapterSaving);
  editorReferenceText = html;
  editorChapterIdOnServer = 0;
  editorIdPollerTimeoutStop ();
  editorSaveAsync = true;
  if (sync) editorSaveAsync = false;
  var encodedHtml = filter_url_plus_to_tag (html);
  var checksum = checksum_get (encodedHtml);
  editorSaving = true;
  $.ajax ({
    url: "save",
    type: "POST",
    async: editorSaveAsync,
    data: { bible: editorLoadedBible, book: editorLoadedBook, chapter: editorLoadedChapter, html: encodedHtml, checksum: checksum },
    success: function (response) {
      editorStatus (response);
      if (response == editorChapterReformat) {
        editorLoadChapter (true);
      }
    },
    error: function (jqXHR, textStatus, errorThrown) {
      editorStatus (editorChapterRetrying);
      editorReferenceText = "";
      editorContentChanged ();
      if (!editorSaveAsync) editorSaveChapter (true);
    },
    complete: function (xhr, status) {
      editorIdPollerTimeoutStart ();
      editorSaveAsync = true;
      editorSaving = false;
    },
  });
}


function editorGetHtml ()
{
  var html = $ ("#editor").html ();
  // Remove verse focus class name, if it is:
  // * the only class name.
  html = html.split (' class="focusedverse"').join ('');
  // * between two more class names.
  html = html.split (' focusedverse ').join ('');
  // * the first class name.
  html = html.split ('focusedverse ').join ('');
  // * the last class name.
  html = html.split (' focusedverse').join ('');
  return html;
}


/*

Portion dealing with triggers for editor's content change.

*/


var editorContentChangedTimeoutId;


function editorContentChanged (event)
{
  if (!editorWriteAccess) return;
  if (editKeysIgnoreForContentChange (event)) return;
  editorTextChanged = true;
  editorContentChangedTimeoutStart ();
}


function editorContentChangedTimeoutStart ()
{
  if (editorContentChangedTimeoutId) clearTimeout (editorContentChangedTimeoutId);
  editorContentChangedTimeoutId = setTimeout (editorSaveChapter, 1000);
}


function editorUnload ()
{
  editorSaveChapter (true);
}


/*

Section for polling the server for updates on the loaded chapter.

*/


var editorChapterIdOnServer = 0;
var editorChapterIdPollerTimeoutId;


function editorIdPollerTimeoutStart ()
{
  editorIdPollerTimeoutStop ();
  editorChapterIdPollerTimeoutId = setTimeout (editorEditorPollId, 1000);
}


function editorIdPollerTimeoutStop ()
{
  if (editorChapterIdPollerTimeoutId) clearTimeout (editorChapterIdPollerTimeoutId);
}


function editorEditorPollId ()
{
  $.ajax ({
    url: "id",
    type: "GET",
    data: { bible: editorLoadedBible, book: editorLoadedBook, chapter: editorLoadedChapter },
    success: function (response) {
      if (!editorSaving) {
        if (editorChapterIdOnServer != 0) {
          if (response != editorChapterIdOnServer) {
            editorLoadChapter (true);
            editorChapterIdOnServer = 0;
          }
        }
        editorChapterIdOnServer = response;
      }
    },
    complete: function (xhr, status) {
      editorIdPollerTimeoutStart ();
    }
  });
}


/*

Section responding to a moved caret.

*/


var editorCaretMovedTimeoutId;
var editorCaretMovedAjaxRequest;
var editorCaretMovedAjaxOffset;
var editorCaretInitialized = false;


function editorCaretChangedByMouse (event)
{
  if (editorPositionCaretProgrammatically) return;
  editorCaretMovedTimeoutStart ();
}


function editorCaretChangedByKeyboard (event)
{
  if (editKeysIgnoreForCaretChange (event)) return;
  if (editorPositionCaretProgrammatically) return;
  editorCaretMovedTimeoutStart ();
}


function editorCaretMovedTimeoutStart ()
{
  if (editorCaretMovedTimeoutId) clearTimeout (editorCaretMovedTimeoutId);
  editorCaretMovedTimeoutId = setTimeout (editorHandleCaretMoved, 200);
}


function editorHandleCaretMoved ()
{
  // If the text in the editor has been changed, and therefore not saved,
  // or is being saved now, postpone handling the moved caret.
  // It is important to also delay the handler "while" the text is being saved,
  // because there have been situations that javascript initiated the save operation,
  // shortly after that the moved caret handler via AJAX,
  // and that the server completed processing the caret handler before it had completed the save operations,
  // which led to inaccurate caret position calculations, which caused symptoms like a "jumping caret".
  if (editorTextChanged || editorSaving) {
    editorCaretMovedTimeoutStart ();
    return;
  }
  
  // If the caret has not yet been positioned, postpone the action.
  if (!editorCaretInitialized) {
    editorCaretMovedTimeoutStart ();
    positionCaretViaAjax ();
    return;
  }

  if ($ ("#editor").is (":focus")) {
    // Cancel any previous ajax request that might still be incompleted.
    // This is to avoid the caret jumping on slower or unstable connections.
    if (editorCaretMovedAjaxRequest && editorCaretMovedAjaxRequest.readystate != 4) {
      editorCaretMovedAjaxRequest.abort();
    }
    // Record the offset of the caret at the start of the ajax request.
    editorCaretMovedAjaxOffset = getCaretPosition ();
    // Initiate a new ajax request.
    editorCaretMovedAjaxRequest = $.ajax ({
      url: "offset",
      type: "GET",
      data: { bible: editorLoadedBible, book: editorLoadedBook, chapter: editorLoadedChapter, offset: editorCaretMovedAjaxOffset },
      success: function (response) {
        if (response != "") {
          var offset = getCaretPosition ();
          // Take action only when the caret is still at the same position as it was when this ajax request was initiated.
          if (offset == editorCaretMovedAjaxOffset) {
            // Set the verse correct immediately, rather than waiting on the Navigator signal that likely will come later.
            // This fixes a clicking / scrolling problem.
            editorNavigationVerse = response;
            editorScheduleWindowScrolling ();
          } else {
            // Caret was moved during this AJAX operation: Reschedule it.
            editorCaretMovedTimeoutStart ();
          }
        }
      },
      error: function (jqXHR, textStatus, errorThrown) {
        // On (network) failure, reschedule the update.
        editorCaretMovedTimeoutStart ();
      }
    });
  } else {
    editorCaretMovedTimeoutStart ();
  }

  editorActiveStylesFeedback ();
}


/*

Section with window events and their basic handlers.

*/


function editorWindowKeyHandler (event)
{
  if (!editorWriteAccess) return;
  // Ctrl-S: Style.
  if ((event.ctrlKey == true) && (event.keyCode == 83)) {
    editorStylesButtonHandler ();
    return false;
  }
  // Escape.
  if (event.keyCode == 27) {
    editorClearStyles ();
  }
}


function editorWindowFocused ()
{
  editorCaretMovedTimeoutStart ();
}


/*

Section for user interface updates and feedback.

*/


function editorStatus (text)
{
  $ ("#editorstatus").empty ();
  $ ("#editorstatus").append (text);
  editorSelectiveNotification (text);
}


function editorActiveStylesFeedback ()
{
  var editor = $ ("#editor");
  if (editor.is (":focus")) {
    var parent = rangy.getSelection().anchorNode.parentElement;
    parent = $ (parent);
    var paragraph = parent.closest ("p");
    var pname = paragraph.attr ('class');
    var span = parent.closest ("span");
    var cname = span.attr ("class");
    if (cname == undefined) cname = "";
    cname = cname.replace ("focusedverse", "");
    var element = $ ("#activestyles");
    element.text (pname + " " + cname);
  }
}


function editorSelectiveNotification (message)
{
  if (message == editorChapterLoaded) return;
  if (message == editorChapterSaving) return;
  if (message == editorChapterSaved) return;
  if (message == editorChapterReformat) return;
  notifyItError (message);
}


/*

Section for getting and setting the caret position.

*/


function getCaretPosition ()
{
  var position = undefined;
  var editor = $ ("#editor");
  if (editor.is (":focus")) {
    var element = editor.get (0);
    position = getCaretCharacterOffsetWithin (element);
  }
  return position;
}


function getCaretCharacterOffsetWithin (element)
{
  var caretOffset = 0;
  if (typeof window.getSelection != "undefined") {
    var range = window.getSelection().getRangeAt(0);
    var preCaretRange = range.cloneRange();
    preCaretRange.selectNodeContents(element);
    preCaretRange.setEnd(range.endContainer, range.endOffset);
    caretOffset = preCaretRange.toString().length;
  } else if (typeof document.selection != "undefined" && document.selection.type != "Control") {
    var textRange = document.selection.createRange();
    var preCaretTextRange = document.body.createTextRange();
    preCaretTextRange.moveToElementText(element);
    preCaretTextRange.setEndPoint("EndToEnd", textRange);
    caretOffset = preCaretTextRange.text.length;
  }
  return caretOffset;
}


function positionCaret (position)
{
  if (position == undefined) return;
  editorPositionCaretCount = 3;
  editorPositionCaretOffset = position;
  editorPositionCaretExecute ();
}

var editorPositionCaretCount = 0;
var editorPositionCaretOffset = 0;
var editorPositionCaretProgrammatically = false;

function editorPositionCaretExecute ()
{
  // Positioning often is not accurate the first time, so do it several times if needed.
  if (editorPositionCaretCount > 0) {
    var currentPosition = getCaretPosition ();
    if (currentPosition == undefined) return;
    editorPositionCaretProgrammatically = true;
    var selection = rangy.getSelection ();
    var desiredPosition = parseInt (editorPositionCaretOffset);
    // Fix bug that it jumps to the previous verse through positioning the cursor slightly off-location.
    if (editorPositionCaretCount == 3) desiredPosition += 5;
    selection.move ("character", desiredPosition - currentPosition);
    currentPosition = getCaretPosition ();
    editorPositionCaretProgrammatically = false;
    if (editorPositionCaretOffset == currentPosition) return;
    editorPositionCaretCount--;
    setTimeout (editorPositionCaretExecute, 10);
  }
}


var editorPositionCaretViaAjaxTimerId;


function positionCaretViaAjax ()
{
  // Very frequent focus calls have been seen in some browsers, so they are filtered here.
  if (editorPositionCaretViaAjaxTimerId) clearTimeout (editorPositionCaretViaAjaxTimerId);
  editorPositionCaretViaAjaxTimerId = setTimeout (positionCaretViaAjaxExecute, 100);
}


function positionCaretViaAjaxExecute ()
{
  if (isNoVerseBook (editorLoadedBook)) return;
  $.ajax ({
    url: "focus",
    type: "GET",
    data: { bible: editorLoadedBible, book: editorLoadedBook, chapter: editorLoadedChapter },
    success: function (response) {
      if (response != "") {
        response = response.split ("\n");
        var start = response [0];
        var end = response [1];
        var offset = getCaretPosition ();
        if ((offset < start) || (offset > end)) {
          positionCaret (start);
        }
        editorCaretInitialized = true;
      }
      editorScheduleWindowScrolling ();
    },
    error: function (jqXHR, textStatus, errorThrown) {
      // Network error: Reschedule.
      editorScheduleCaretPositioning ();
    },
    complete: function (xhr, status) {
    }
  });
}


/*

Section for scrolling the caret into view.

*/


function editorScrollVerseIntoView ()
{
  if (isNoVerseBook (editorLoadedBook)) return;

  var verses = [0];
  var navigated = false;
  $ ("#editor > p span").each (function (index) {
    var element = $(this);
    if (element.hasClass ("v")) {
      verses = usfm_get_verse_numbers (element[0].textContent);
      if (verses.indexOf (parseInt (editorNavigationVerse)) >= 0) {
        if (navigated == false) {
          var verseTop = element.offset().top;
          var workspaceHeight = $("#workspacewrapper").height();
          var currentScrollTop = $("#workspacewrapper").scrollTop();
          var scrollTo = verseTop - (workspaceHeight * verticalCaretPosition / 100) + currentScrollTop;
          var lowerBoundary = currentScrollTop - (workspaceHeight / 10);
          var upperBoundary = currentScrollTop + (workspaceHeight / 10);
          if ((scrollTo < lowerBoundary) || (scrollTo > upperBoundary)) {
            $("#workspacewrapper").animate({ scrollTop: scrollTo }, 500);
          }
          navigated = true;
        }
      }
    }
    if (verses.indexOf (parseInt (editorNavigationVerse)) >= 0) element.addClass ("focusedverse");
    else element.removeClass ("focusedverse");

  });
  if (editorNavigationVerse == 0) {
    $ ("#workspacewrapper").animate ({ scrollTop: 0 }, 500);
  }
}


/*

Section for the styles handling.

*/


function editorStylesButtonHandler ()
{
  if (!editorWriteAccess) return;
  $.ajax ({
    url: "/edit/styles",
    type: "GET",
    cache: false,
    success: function (response) {
      editorShowResponse (response);
      editorBindUnselectable ();
      dynamicClickHandlers ();
    },
  });
  return false;
}


function editorBindUnselectable ()
{
  var elements = $ (".unselectable");
  elements.off ("mousedown");
  elements.on ("mousedown", function (event) {
    event.preventDefault();
  });
}


function editorShowResponse (response)
{
  if (!editorWriteAccess) return;
  $ ("#stylebutton").hide ();
  $ ("#nostyles").hide ();
  var area = $ ("#stylesarea");
  area.empty ();
  area.append (response);
}


function editorClearStyles ()
{
  var area = $ ("#stylesarea");
  area.empty ();
  $ ("#stylebutton").show ();
  $ ("#nostyles").show ();
}


function dynamicClickHandlers ()
{
  var elements = $ ("#stylesarea > a");
  elements.on ("click", function (event) {
    event.preventDefault();
    editorClearStyles ();
    //$ ("#editor").focus ();
    var href = event.currentTarget.href;
    href = href.substring (href.lastIndexOf ('/') + 1);
    if (href == "cancel") return;
    if (href == "all") {
      displayAllStyles ();
    } else {
      requestStyle (href);
    }
  });

  $ ("#styleslist").on ("change", function (event) {
    var selection = $ ("#styleslist option:selected").text ();
    var style = selection.substring (0, selection.indexOf (" "));
    event.preventDefault();
    editorClearStyles ();
    //$ ("#editor").focus ();
    requestStyle (style);
  });
}


function requestStyle (style)
{
  $.ajax ({
    url: "/edit/styles",
    type: "GET",
    data: { style: style },
    cache: false,
    success: function (response) {
      response = response.split ("\n");
      var style = response [0];
      var action = response [1];
      if (action == "p") {
        applyParagraphStyle (style);
        editorContentChanged ();
      } else if (action == 'c') {
        applyCharacterStyle (style);
        editorContentChanged ();
      } else if (action == 'n') {
        applyNotesStyle (style);
        editorContentChanged ();
      } else if (action == "m") {
        applyMonoStyle (style);
        editorContentChanged ();
      }
    },
  });
}


function displayAllStyles ()
{
  $.ajax ({
    url: "styles",
    type: "GET",
    data: { all: "" },
    cache: false,
    success: function (response) {
      editorShowResponse (response);
      editorBindUnselectable ();
      dynamicClickHandlers ();
    },
  });
}


function applyParagraphStyle (style)
{
  if (!editorWriteAccess) return;
  //$ ("#editor").focus ();
  var parent = rangy.getSelection().anchorNode.parentElement;
  parent = $ (parent);
  var paragraph = parent.closest ("p");
  paragraph.removeClass ();
  paragraph.addClass (style);
}


function applyCharacterStyle (style)
{
  if (!editorWriteAccess) return;
  //$ ("#editor").focus ();
  var classApplier = rangy.createClassApplier (style);
  classApplier.toggleSelection ();
}


function applyMonoStyle (style)
{
  if (!editorWriteAccess) return;

  //$ ("#editor").focus ();

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

Section for the notes.

*/


var editorInsertedNotesCount = 0;


function applyNotesStyle (style)
{
  if (!editorWriteAccess) return;

  //$ ("#editor").focus ();

  // Check for / append notes section.
  var notes = $ ("#notes");
  if (notes.length == 0) {
    $ ("#editor").append ('<div id="notes"><hr></div>');
  }

  // Get a new node identifier based on the local time.
  var date = new Date();
  var noteId = date.getTime();

  var caller = (editorInsertedNotesCount % 9) + 1;
  
  // Insert note caller at caret.
  var html = '<a href="#note' + noteId + '" id="citation' + noteId + '" class="superscript">' + caller + '</a>';
  insertHtmlAtCaret (html);
  
  // Append note text to notes section.
  html = '<p class="' + style + '"><a href="#citation' + noteId + '" id="note' + noteId + '">' + caller + '</a><span> </span><span>+ </span><span> </span></p>';
  $ ("#notes").append (html);
  
  editorInsertedNotesCount++;
}


function insertHtmlAtCaret (html) 
{
  var sel, range;
  if (window.getSelection) {
    // IE9 and non-IE
    sel = window.getSelection();
    if (sel.getRangeAt && sel.rangeCount) {
      range = sel.getRangeAt(0);
      range.deleteContents();

      // Range.createContextualFragment() would be useful here but is
      // only relatively recently standardized and is not supported in
      // some browsers (IE9, for one)
      var el = document.createElement("div");
      el.innerHTML = html;
      var frag = document.createDocumentFragment(), node, lastNode;
      while ( (node = el.firstChild) ) {
        lastNode = frag.appendChild(node);
      }
      var firstNode = frag.firstChild;
      range.insertNode(frag);
      
      // Preserve the selection
      if (lastNode) {
        range = range.cloneRange();
        range.setStartAfter(lastNode);
        range.collapse(true);
        sel.removeAllRanges();
        sel.addRange(range);
      }
    }
  } else if ((sel = document.selection) && sel.type != "Control") {
    // IE < 9
    var originalRange = sel.createRange();
    originalRange.collapse(true);
    sel.createRange().pasteHTML(html);
  }
}


/*

Section for the clipboard.

*/


function editorPaste (event)
{
  var data = event.originalEvent.clipboardData.getData ('Text');
  event.preventDefault();
  document.execCommand ("insertHTML", false, data);
}


/*

Caret positioning and subsequent window scrolling.

The purpose of this section is to coordinate the two events,
so that caret positioning is done first, 
and window scrolling last.

*/


var editorPositioningTimerId;
var editorPendingCaretPositioning = false;
var editorPendingWindowScrolling = false;


function editorScheduleCaretPositioning ()
{
  editorPendingCaretPositioning = true;
  editorPendingWindowScrolling = false;
  editorPositioningTimerStart ();
}


function editorScheduleWindowScrolling ()
{
  editorPendingWindowScrolling = true;
  editorPositioningTimerStart ();
}


function editorPositioningTimerStart ()
{
  if (editorPositioningTimerId) clearTimeout (editorPositioningTimerId);
  editorPositioningTimerId = setTimeout (editorPositioningRun, 200);
}


function editorPositioningRun ()
{
  if (editorPendingCaretPositioning) {
    positionCaretViaAjax ();
    editorPendingCaretPositioning = false;
    editorPendingWindowScrolling = false;
  }
  if (editorPendingWindowScrolling) {
    editorPendingWindowScrolling = false;
    editorScrollVerseIntoView ();
  }
}



// Debug



function editorLog (msg)
{
  var date = new Date();
  var seconds = date.getSeconds();
  var milliseconds = date.getMilliseconds();
  console.log (seconds + " " + milliseconds + ": " + msg);
}


/*
 
 Section for swipe navigation.
 
 */


function editorSwipeLeft (event)
{
  if (typeof navigateNextChapter != 'undefined') {
    navigateNextChapter (event);
  } else if (parent.window.navigateNextChapter != 'undefined') {
    parent.window.navigateNextChapter (event);
  }
}


function editorSwipeRight (event)
{
  if (typeof navigatePreviousChapter != 'undefined') {
    navigatePreviousChapter (event);
  } else if (parent.window.navigatePreviousChapter != 'undefined') {
    parent.window.navigatePreviousChapter (event);
  }
}

},{"prosemirror-example-setup":7,"prosemirror-menu":18,"prosemirror-model":26,"prosemirror-schema-basic":34,"prosemirror-schema-list":35,"prosemirror-state":37}],2:[function(require,module,exports){
//Copyright (C) 2012 Kory Nunn

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/*

    This code is not formatted for readability, but rather run-speed and to assist compilers.

    However, the code's intention should be transparent.

    *** IE SUPPORT ***

    If you require this library to work in IE7, add the following after declaring crel.

    var testDiv = document.createElement('div'),
        testLabel = document.createElement('label');

    testDiv.setAttribute('class', 'a');
    testDiv['className'] !== 'a' ? crel.attrMap['class'] = 'className':undefined;
    testDiv.setAttribute('name','a');
    testDiv['name'] !== 'a' ? crel.attrMap['name'] = function(element, value){
        element.id = value;
    }:undefined;


    testLabel.setAttribute('for', 'a');
    testLabel['htmlFor'] !== 'a' ? crel.attrMap['for'] = 'htmlFor':undefined;



*/

(function (root, factory) {
    if (typeof exports === 'object') {
        module.exports = factory();
    } else if (typeof define === 'function' && define.amd) {
        define(factory);
    } else {
        root.crel = factory();
    }
}(this, function () {
    var fn = 'function',
        obj = 'object',
        nodeType = 'nodeType',
        textContent = 'textContent',
        setAttribute = 'setAttribute',
        attrMapString = 'attrMap',
        isNodeString = 'isNode',
        isElementString = 'isElement',
        d = typeof document === obj ? document : {},
        isType = function(a, type){
            return typeof a === type;
        },
        isNode = typeof Node === fn ? function (object) {
            return object instanceof Node;
        } :
        // in IE <= 8 Node is an object, obviously..
        function(object){
            return object &&
                isType(object, obj) &&
                (nodeType in object) &&
                isType(object.ownerDocument,obj);
        },
        isElement = function (object) {
            return crel[isNodeString](object) && object[nodeType] === 1;
        },
        isArray = function(a){
            return a instanceof Array;
        },
        appendChild = function(element, child) {
          if(!crel[isNodeString](child)){
              child = d.createTextNode(child);
          }
          element.appendChild(child);
        };


    function crel(){
        var args = arguments, //Note: assigned to a variable to assist compilers. Saves about 40 bytes in closure compiler. Has negligable effect on performance.
            element = args[0],
            child,
            settings = args[1],
            childIndex = 2,
            argumentsLength = args.length,
            attributeMap = crel[attrMapString];

        element = crel[isElementString](element) ? element : d.createElement(element);
        // shortcut
        if(argumentsLength === 1){
            return element;
        }

        if(!isType(settings,obj) || crel[isNodeString](settings) || isArray(settings)) {
            --childIndex;
            settings = null;
        }

        // shortcut if there is only one child that is a string
        if((argumentsLength - childIndex) === 1 && isType(args[childIndex], 'string') && element[textContent] !== undefined){
            element[textContent] = args[childIndex];
        }else{
            for(; childIndex < argumentsLength; ++childIndex){
                child = args[childIndex];

                if(child == null){
                    continue;
                }

                if (isArray(child)) {
                  for (var i=0; i < child.length; ++i) {
                    appendChild(element, child[i]);
                  }
                } else {
                  appendChild(element, child);
                }
            }
        }

        for(var key in settings){
            if(!attributeMap[key]){
                element[setAttribute](key, settings[key]);
            }else{
                var attr = attributeMap[key];
                if(typeof attr === fn){
                    attr(element, settings[key]);
                }else{
                    element[setAttribute](attr, settings[key]);
                }
            }
        }

        return element;
    }

    // Used for mapping one kind of attribute to the supported version of that in bad browsers.
    crel[attrMapString] = {};

    crel[isElementString] = isElement;

    crel[isNodeString] = isNode;

    if(typeof Proxy !== 'undefined'){
        crel.proxy = new Proxy(crel, {
            get: function(target, key){
                !(key in crel) && (crel[key] = crel.bind(null, key));
                return crel[key];
            }
        });
    }

    return crel;
}));

},{}],3:[function(require,module,exports){
// Extending unicode characters. A series of a non-extending char +
// any number of extending chars is treated as a single unit as far
// as editing and measuring is concerned. This is not fully correct,
// since some scripts/fonts/browsers also treat other configurations
// of code points as a group.
var extendingChar = /[\u0300-\u036f\u0483-\u0489\u0591-\u05bd\u05bf\u05c1\u05c2\u05c4\u05c5\u05c7\u0610-\u061a\u064b-\u065e\u0670\u06d6-\u06dc\u06de-\u06e4\u06e7\u06e8\u06ea-\u06ed\u0711\u0730-\u074a\u07a6-\u07b0\u07eb-\u07f3\u0816-\u0819\u081b-\u0823\u0825-\u0827\u0829-\u082d\u0900-\u0902\u093c\u0941-\u0948\u094d\u0951-\u0955\u0962\u0963\u0981\u09bc\u09be\u09c1-\u09c4\u09cd\u09d7\u09e2\u09e3\u0a01\u0a02\u0a3c\u0a41\u0a42\u0a47\u0a48\u0a4b-\u0a4d\u0a51\u0a70\u0a71\u0a75\u0a81\u0a82\u0abc\u0ac1-\u0ac5\u0ac7\u0ac8\u0acd\u0ae2\u0ae3\u0b01\u0b3c\u0b3e\u0b3f\u0b41-\u0b44\u0b4d\u0b56\u0b57\u0b62\u0b63\u0b82\u0bbe\u0bc0\u0bcd\u0bd7\u0c3e-\u0c40\u0c46-\u0c48\u0c4a-\u0c4d\u0c55\u0c56\u0c62\u0c63\u0cbc\u0cbf\u0cc2\u0cc6\u0ccc\u0ccd\u0cd5\u0cd6\u0ce2\u0ce3\u0d3e\u0d41-\u0d44\u0d4d\u0d57\u0d62\u0d63\u0dca\u0dcf\u0dd2-\u0dd4\u0dd6\u0ddf\u0e31\u0e34-\u0e3a\u0e47-\u0e4e\u0eb1\u0eb4-\u0eb9\u0ebb\u0ebc\u0ec8-\u0ecd\u0f18\u0f19\u0f35\u0f37\u0f39\u0f71-\u0f7e\u0f80-\u0f84\u0f86\u0f87\u0f90-\u0f97\u0f99-\u0fbc\u0fc6\u102d-\u1030\u1032-\u1037\u1039\u103a\u103d\u103e\u1058\u1059\u105e-\u1060\u1071-\u1074\u1082\u1085\u1086\u108d\u109d\u135f\u1712-\u1714\u1732-\u1734\u1752\u1753\u1772\u1773\u17b7-\u17bd\u17c6\u17c9-\u17d3\u17dd\u180b-\u180d\u18a9\u1920-\u1922\u1927\u1928\u1932\u1939-\u193b\u1a17\u1a18\u1a56\u1a58-\u1a5e\u1a60\u1a62\u1a65-\u1a6c\u1a73-\u1a7c\u1a7f\u1b00-\u1b03\u1b34\u1b36-\u1b3a\u1b3c\u1b42\u1b6b-\u1b73\u1b80\u1b81\u1ba2-\u1ba5\u1ba8\u1ba9\u1c2c-\u1c33\u1c36\u1c37\u1cd0-\u1cd2\u1cd4-\u1ce0\u1ce2-\u1ce8\u1ced\u1dc0-\u1de6\u1dfd-\u1dff\u200c\u200d\u20d0-\u20f0\u2cef-\u2cf1\u2de0-\u2dff\u302a-\u302f\u3099\u309a\ua66f-\ua672\ua67c\ua67d\ua6f0\ua6f1\ua802\ua806\ua80b\ua825\ua826\ua8c4\ua8e0-\ua8f1\ua926-\ua92d\ua947-\ua951\ua980-\ua982\ua9b3\ua9b6-\ua9b9\ua9bc\uaa29-\uaa2e\uaa31\uaa32\uaa35\uaa36\uaa43\uaa4c\uaab0\uaab2-\uaab4\uaab7\uaab8\uaabe\uaabf\uaac1\uabe5\uabe8\uabed\udc00-\udfff\ufb1e\ufe00-\ufe0f\ufe20-\ufe26\uff9e\uff9f]/

exports.isExtendingChar = function(ch) {
  return ch.charCodeAt(0) >= 768 && extendingChar.test(ch)
}

exports.isExtendingCharAt = function(string, pos) {
  return string.charCodeAt(pos) >= 768 && extendingChar.test(string.charAt(pos))
}

},{}],4:[function(require,module,exports){
var ref = require("extending-char");
var isExtendingChar = ref.isExtendingChar;

var nonASCIISingleCaseWordChar = /[\u00df\u0587\u0590-\u05f4\u0600-\u06ff\u3040-\u309f\u30a0-\u30ff\u3400-\u4db5\u4e00-\u9fcc\uac00-\ud7af]/

function isWordChar(ch) {
  return /\w/.test(ch) || ch > "\x80" && (ch < "\ud800" || ch > "\udfff") &&
    (isExtendingChar(ch) || ch.toUpperCase() != ch.toLowerCase() || nonASCIISingleCaseWordChar.test(ch))
}
exports.isWordChar = isWordChar

// Get the category of a given character. Either a "space",
// a character that can be part of a word ("word"), or anything else ("other").
function charCategory(ch) {
  return /\s/.test(ch) ? "space" : isWordChar(ch) ? "word" : "other"
}
exports.charCategory = charCategory

},{"extending-char":3}],5:[function(require,module,exports){
var ref = require("prosemirror-transform");
var joinPoint = ref.joinPoint;
var canJoin = ref.canJoin;
var findWrapping = ref.findWrapping;
var liftTarget = ref.liftTarget;
var canSplit = ref.canSplit;
var ReplaceAroundStep = ref.ReplaceAroundStep;
var ref$1 = require("prosemirror-model");
var Slice = ref$1.Slice;
var Fragment = ref$1.Fragment;
var ref$2 = require("prosemirror-state");
var Selection = ref$2.Selection;
var TextSelection = ref$2.TextSelection;
var NodeSelection = ref$2.NodeSelection;
var isSelectable = ref$2.isSelectable;
var ref$3 = require("extending-char");
var isExtendingCharAt = ref$3.isExtendingCharAt;

var ref$4 = require("./platform");
var ios = ref$4.ios;
var mac = ref$4.mac;
var ref$5 = require("./char");
var charCategory = ref$5.charCategory;

// :: (EditorState, ?(action: Action)) → bool
// Delete the selection, if there is one.
function deleteSelection(state, onAction) {
  if (state.selection.empty) { return false }
  if (onAction) { onAction(state.tr.replaceSelection().scrollAction()) }
  return true
}
exports.deleteSelection = deleteSelection

// :: (EditorState, ?(action: Action)) → bool
// If the selection is empty and at the start of a textblock, move
// that block closer to the block before it, by lifting it out of its
// parent or, if it has no parent it doesn't share with the node
// before it, moving it into a parent of that node, or joining it with
// that.
function joinBackward(state, onAction) {
  var ref = state.selection;
  var $head = ref.$head;
  var empty = ref.empty;
  if (!empty || $head.parentOffset > 0) { return false }

  // Find the node before this one
  var before, cut
  for (var i = $head.depth - 1; !before && i >= 0; i--) { if ($head.index(i) > 0) {
    cut = $head.before(i + 1)
    before = $head.node(i).child($head.index(i) - 1)
  } }

  // If there is no node before this, try to lift
  if (!before) {
    var range = $head.blockRange(), target = range && liftTarget(range)
    if (target == null) { return false }
    if (onAction) { onAction(state.tr.lift(range, target).scrollAction()) }
    return true
  }

  // If the node below has no content and the node above is
  // selectable, delete the node below and select the one above.
  if (before.isLeaf && isSelectable(before) && $head.parent.content.size == 0) {
    if (onAction) {
      var tr = state.tr.delete(cut, cut + $head.parent.nodeSize)
      tr.setSelection(new NodeSelection(tr.doc.resolve(cut - before.nodeSize)))
      onAction(tr.scrollAction())
    }
    return true
  }

  // If the node doesn't allow children, delete it
  if (before.isLeaf) {
    if (onAction) { onAction(state.tr.delete(cut - before.nodeSize, cut).scrollAction()) }
    return true
  }

  // Apply the joining algorithm
  return deleteBarrier(state, cut, onAction)
}
exports.joinBackward = joinBackward

// :: (EditorState, ?(action: Action)) → bool
// If the selection is empty and the cursor is at the end of a
// textblock, move the node after it closer to the node with the
// cursor (lifting it out of parents that aren't shared, moving it
// into parents of the cursor block, or joining the two when they are
// siblings).
function joinForward(state, onAction) {
  var ref = state.selection;
  var $head = ref.$head;
  var empty = ref.empty;
  if (!empty || $head.parentOffset < $head.parent.content.size) { return false }

  // Find the node after this one
  var after, cut
  for (var i = $head.depth - 1; !after && i >= 0; i--) {
    var parent = $head.node(i)
    if ($head.index(i) + 1 < parent.childCount) {
      after = parent.child($head.index(i) + 1)
      cut = $head.after(i + 1)
    }
  }

  // If there is no node after this, there's nothing to do
  if (!after) { return false }

  // If the node doesn't allow children, delete it
  if (after.isLeaf) {
    if (onAction) { onAction(state.tr.delete(cut, cut + after.nodeSize).scrollAction()) }
    return true
  }
  // Apply the joining algorithm
  return deleteBarrier(state, cut, onAction)
}
exports.joinForward = joinForward

// :: (EditorState, ?(action: Action)) → bool
// Delete the character before the cursor, if the selection is empty
// and the cursor isn't at the start of a textblock.
function deleteCharBefore(state, onAction) {
  var ref = state.selection;
  var $head = ref.$head;
  var empty = ref.empty;
  if (!empty || $head.parentOffset == 0) { return false }
  if (onAction) {
    var dest = moveBackward($head, "char")
    onAction(state.tr.delete(dest, $head.pos).scrollAction())
  }
  return true
}
exports.deleteCharBefore = deleteCharBefore

// :: (EditorState, ?(action: Action)) → bool
// Delete the word before the cursor, if the selection is empty and
// the cursor isn't at the start of a textblock.
function deleteWordBefore(state, onAction) {
  var ref = state.selection;
  var $head = ref.$head;
  var empty = ref.empty;
  if (!empty || $head.parentOffset == 0) { return false }
  if (onAction)
    { onAction(state.tr.delete(moveBackward($head, "word"), $head.pos).scrollAction()) }
  return true
}
exports.deleteWordBefore = deleteWordBefore

// :: (EditorState, ?(action: Action)) → bool
// Delete the character after the cursor, if the selection is empty
// and the cursor isn't at the end of its textblock.
function deleteCharAfter(state, onAction) {
  var ref = state.selection;
  var $head = ref.$head;
  var empty = ref.empty;
  if (!empty || $head.parentOffset == $head.parent.content.size) { return false }
  if (onAction)
    { onAction(state.tr.delete($head.pos, moveForward($head, "char")).scrollAction()) }
  return true
}
exports.deleteCharAfter = deleteCharAfter

// :: (EditorState, ?(action: Action)) → bool
// Delete the word after the cursor, if the selection is empty and the
// cursor isn't at the end of a textblock.
function deleteWordAfter(state, onAction) {
  var ref = state.selection;
  var $head = ref.$head;
  var empty = ref.empty;
  if (!empty || $head.parentOffset == $head.parent.content.size) { return false }
  if (onAction)
    { onAction(state.tr.delete($head.pos, moveForward($head, "word")).scrollAction()) }
  return true
}
exports.deleteWordAfter = deleteWordAfter

// :: (EditorState, ?(action: Action)) → bool
// Join the selected block or, if there is a text selection, the
// closest ancestor block of the selection that can be joined, with
// the sibling above it.
function joinUp(state, onAction) {
  var ref = state.selection;
  var node = ref.node;
  var from = ref.from;
  var point
  if (node) {
    if (node.isTextblock || !canJoin(state.doc, from)) { return false }
    point = from
  } else {
    point = joinPoint(state.doc, from, -1)
    if (point == null) { return false }
  }
  if (onAction) {
    var tr = state.tr.join(point)
    if (state.selection.node) { tr.setSelection(new NodeSelection(tr.doc.resolve(point - state.doc.resolve(point).nodeBefore.nodeSize))) }
    onAction(tr.scrollAction())
  }
  return true
}
exports.joinUp = joinUp

// :: (EditorState, ?(action: Action)) → bool
// Join the selected block, or the closest ancestor of the selection
// that can be joined, with the sibling after it.
function joinDown(state, onAction) {
  var node = state.selection.node, nodeAt = state.selection.from
  var point = joinPointBelow(state)
  if (!point) { return false }
  if (onAction) {
    var tr = state.tr.join(point)
    if (node) { tr.setSelection(new NodeSelection(tr.doc.resolve(nodeAt))) }
    onAction(tr.scrollAction())
  }
  return true
}
exports.joinDown = joinDown

// :: (EditorState, ?(action: Action)) → bool
// Lift the selected block, or the closest ancestor block of the
// selection that can be lifted, out of its parent node.
function lift(state, onAction) {
  var ref = state.selection;
  var $from = ref.$from;
  var $to = ref.$to;
  var range = $from.blockRange($to), target = range && liftTarget(range)
  if (target == null) { return false }
  if (onAction) { onAction(state.tr.lift(range, target).scrollAction()) }
  return true
}
exports.lift = lift

// :: (EditorState, ?(action: Action)) → bool
// If the selection is in a node whose type has a truthy
// [`code`](#model.NodeSpec.code) property in its spec, replace the
// selection with a newline character.
function newlineInCode(state, onAction) {
  var ref = state.selection;
  var $from = ref.$from;
  var $to = ref.$to;
  var node = ref.node;
  if (node) { return false }
  if (!$from.parent.type.spec.code || $to.pos >= $from.end()) { return false }
  if (onAction) { onAction(state.tr.insertText("\n").scrollAction()) }
  return true
}
exports.newlineInCode = newlineInCode

// :: (EditorState, ?(action: Action)) → bool
// If a block node is selected, create an empty paragraph before (if
// it is its parent's first child) or after it.
function createParagraphNear(state, onAction) {
  var ref = state.selection;
  var $from = ref.$from;
  var $to = ref.$to;
  var node = ref.node;
  if (!node || !node.isBlock) { return false }
  var type = $from.parent.defaultContentType($to.indexAfter())
  if (!type || !type.isTextblock) { return false }
  if (onAction) {
    var side = ($from.parentOffset ? $to : $from).pos
    var tr = state.tr.insert(side, type.createAndFill())
    tr.setSelection(new TextSelection(tr.doc.resolve(side + 1)))
    onAction(tr.scrollAction())
  }
  return true
}
exports.createParagraphNear = createParagraphNear

// :: (EditorState, ?(action: Action)) → bool
// If the cursor is in an empty textblock that can be lifted, lift the
// block.
function liftEmptyBlock(state, onAction) {
  var ref = state.selection;
  var $head = ref.$head;
  var empty = ref.empty;
  if (!empty || $head.parent.content.size) { return false }
  if ($head.depth > 1 && $head.after() != $head.end(-1)) {
    var before = $head.before()
    if (canSplit(state.doc, before)) {
      if (onAction) { onAction(state.tr.split(before).scrollAction()) }
      return true
    }
  }
  var range = $head.blockRange(), target = range && liftTarget(range)
  if (target == null) { return false }
  if (onAction) { onAction(state.tr.lift(range, target).scrollAction()) }
  return true
}
exports.liftEmptyBlock = liftEmptyBlock

// :: (EditorState, ?(action: Action)) → bool
// Split the parent block of the selection. If the selection is a text
// selection, also delete its content.
function splitBlock(state, onAction) {
  var ref = state.selection;
  var $from = ref.$from;
  var $to = ref.$to;
  var node = ref.node;
  if (node && node.isBlock) {
    if (!$from.parentOffset || !canSplit(state.doc, $from.pos)) { return false }
    if (onAction) { onAction(state.tr.split($from.pos).scrollAction()) }
    return true
  }

  if (onAction) {
    var atEnd = $to.parentOffset == $to.parent.content.size
    var tr = state.tr.delete($from.pos, $to.pos)
    var deflt = $from.depth == 0 ? null : $from.node(-1).defaultContentType($from.indexAfter(-1))
    var types = atEnd ? [{type: deflt}] : null
    var can = canSplit(tr.doc, $from.pos, 1, types)
    if (!types && !can && canSplit(tr.doc, $from.pos, 1, [{type: deflt}])) {
      types = [{type: deflt}]
      can = true
    }
    if (can) {
      tr.split($from.pos, 1, types)
      if (!atEnd && !$from.parentOffset && $from.parent.type != deflt &&
          $from.node(-1).canReplace($from.index(-1), $from.indexAfter(-1), Fragment.from(deflt.create(), $from.parent)))
        { tr.setNodeType($from.before(), deflt) }
    }
    onAction(tr.scrollAction())
  }
  return true
}
exports.splitBlock = splitBlock

// :: (EditorState, ?(action: Action)) → bool
// Move the selection to the node wrapping the current selection, if
// any. (Will not select the document node.)
function selectParentNode(state, onAction) {
  var sel = state.selection, pos
  if (sel.node) {
    if (!sel.$from.depth) { return false }
    pos = sel.$from.before()
  } else {
    var same = sel.$head.sameDepth(sel.$anchor)
    if (same == 0) { return false }
    pos = sel.$head.before(same)
  }
  if (onAction) { onAction(new NodeSelection(state.doc.resolve(pos)).action()) }
  return true
}
exports.selectParentNode = selectParentNode

function deleteBarrier(state, cut, onAction) {
  var $cut = state.doc.resolve(cut), before = $cut.nodeBefore, after = $cut.nodeAfter, conn, match
  if (canJoin(state.doc, cut)) {
    if (onAction) {
      var tr = state.tr.join(cut)
      if (tr.steps.length && before.content.size == 0 && !before.sameMarkup(after) &&
          $cut.parent.canReplace($cut.index() - 1, $cut.index()))
        { tr.setNodeType(cut - before.nodeSize, after.type, after.attrs) }
      onAction(tr.scrollAction())
    }
    return true
  } else if (after.isTextblock && $cut.parent.canReplace($cut.index(), $cut.index() + 1) &&
             (conn = (match = before.contentMatchAt(before.childCount)).findWrapping(after.type, after.attrs)) &&
             match.matchType((conn[0] || after).type, (conn[0] || after).attrs).validEnd()) {
    if (onAction) {
      var end = cut + after.nodeSize, wrap = Fragment.empty
      for (var i = conn.length - 1; i >= 0; i--)
        { wrap = Fragment.from(conn[i].type.create(conn[i].attrs, wrap)) }
      wrap = Fragment.from(before.copy(wrap))
      var tr$1 = state.tr.step(new ReplaceAroundStep(cut - 1, end, cut, end, new Slice(wrap, 1, 0), conn.length, true))
      var joinAt = end + 2 * conn.length
      if (canJoin(tr$1.doc, joinAt)) { tr$1.join(joinAt) }
      onAction(tr$1.scrollAction())
    }
    return true
  } else {
    var selAfter = Selection.findFrom($cut, 1)
    var range = selAfter.$from.blockRange(selAfter.$to), target = range && liftTarget(range)
    if (target == null) { return false }
    if (onAction) { onAction(state.tr.lift(range, target).scrollAction()) }
    return true
  }
}

// Get an offset moving backward from a current offset inside a node.
function moveBackward($pos, by) {
  if (by != "char" && by != "word")
    { throw new RangeError("Unknown motion unit: " + by) }

  var parent = $pos.parent, offset = $pos.parentOffset

  var cat = null, counted = 0, pos = $pos.pos
  for (;;) {
    if (offset == 0) { return pos }
    var ref = parent.childBefore(offset);
    var start = ref.offset;
    var node = ref.node;
    if (!node) { return pos }
    if (!node.isText) { return cat ? pos : pos - 1 }

    if (by == "char") {
      for (var i = offset - start; i > 0; i--) {
        if (!isExtendingCharAt(node.text, i - 1))
          { return pos - 1 }
        offset--
        pos--
      }
    } else if (by == "word") {
      // Work from the current position backwards through text of a singular
      // character category (e.g. "cat" of "#!*") until reaching a character in a
      // different category (i.e. the end of the word).
      for (var i$1 = offset - start; i$1 > 0; i$1--) {
        var nextCharCat = charCategory(node.text.charAt(i$1 - 1))
        if (cat == null || counted == 1 && cat == "space") { cat = nextCharCat }
        else if (cat != nextCharCat) { return pos }
        offset--
        pos--
        counted++
      }
    }
  }
}

function moveForward($pos, by) {
  if (by != "char" && by != "word")
    { throw new RangeError("Unknown motion unit: " + by) }

  var parent = $pos.parent, offset = $pos.parentOffset, pos = $pos.pos

  var cat = null, counted = 0
  for (;;) {
    if (offset == parent.content.size) { return pos }
    var ref = parent.childAfter(offset);
    var start = ref.offset;
    var node = ref.node;
    if (!node) { return pos }
    if (!node.isText) { return cat ? pos : pos + 1 }

    if (by == "char") {
      for (var i = offset - start; i < node.text.length; i++) {
        if (!isExtendingCharAt(node.text, i + 1))
          { return pos + 1 }
        offset++
        pos++
      }
    } else if (by == "word") {
      for (var i$1 = offset - start; i$1 < node.text.length; i$1++) {
        var nextCharCat = charCategory(node.text.charAt(i$1))
        if (cat == null || counted == 1 && cat == "space") { cat = nextCharCat }
        else if (cat != nextCharCat) { return pos }
        offset++
        pos++
        counted++
      }
    }
  }
}

// Parameterized commands

function joinPointBelow(state) {
  var ref = state.selection;
  var node = ref.node;
  var to = ref.to;
  if (node) { return canJoin(state.doc, to) ? to : null }
  else { return joinPoint(state.doc, to, 1) }
}

// :: (NodeType, ?Object) → (state: EditorState, onAction: ?(action: Action)) → bool
// Wrap the selection in a node of the given type with the given
// attributes.
function wrapIn(nodeType, attrs) {
  return function(state, onAction) {
    var ref = state.selection;
    var $from = ref.$from;
    var $to = ref.$to;
    var range = $from.blockRange($to), wrapping = range && findWrapping(range, nodeType, attrs)
    if (!wrapping) { return false }
    if (onAction) { onAction(state.tr.wrap(range, wrapping).scrollAction()) }
    return true
  }
}
exports.wrapIn = wrapIn

// :: (NodeType, ?Object) → (state: EditorState, onAction: ?(action: Action)) → bool
// Returns a command that tries to set the textblock around the
// selection to the given node type with the given attributes.
function setBlockType(nodeType, attrs) {
  return function(state, onAction) {
    var ref = state.selection;
    var $from = ref.$from;
    var $to = ref.$to;
    var node = ref.node;
    var depth
    if (node) {
      depth = $from.depth
    } else {
      if (!$from.depth || $to.pos > $from.end()) { return false }
      depth = $from.depth - 1
    }
    var target = node || $from.parent
    if (!target.isTextblock || target.hasMarkup(nodeType, attrs)) { return false }
    var index = $from.index(depth)
    if (!$from.node(depth).canReplaceWith(index, index + 1, nodeType)) { return false }
    if (onAction) {
      var where = $from.before(depth + 1)
      onAction(state.tr
               .clearMarkupFor(where, nodeType, attrs)
               .setNodeType(where, nodeType, attrs)
               .scrollAction())
    }
    return true
  }
}
exports.setBlockType = setBlockType

function markApplies(doc, from, to, type) {
  var can = false
  doc.nodesBetween(from, to, function (node) {
    if (can) { return false }
    can = node.isTextblock && node.contentMatchAt(0).allowsMark(type)
  })
  return can
}

// :: (MarkType, ?Object) → (state: EditorState, onAction: ?(action: Action)) → bool
// Create a command function that toggles the given mark with the
// given attributes. Will return `false` when the current selection
// doesn't support that mark. This will remove the mark if any marks
// of that type exist in the selection, or add it otherwise. If the
// selection is empty, this applies to the [stored
// marks](#state.EditorState.storedMarks) instead of a range of the
// document.
function toggleMark(markType, attrs) {
  return function(state, onAction) {
    var ref = state.selection;
    var empty = ref.empty;
    var from = ref.from;
    var to = ref.to;
    if (!markApplies(state.doc, from, to, markType)) { return false }
    if (onAction) {
      if (empty) {
        if (markType.isInSet(state.storedMarks || state.doc.marksAt(from)))
          { onAction({type: "removeStoredMark", markType: markType}) }
        else
          { onAction({type: "addStoredMark", mark: markType.create(attrs)}) }
      } else {
        if (state.doc.rangeHasMark(from, to, markType))
          { onAction(state.tr.removeMark(from, to, markType).scrollAction()) }
        else
          { onAction(state.tr.addMark(from, to, markType.create(attrs)).scrollAction()) }
      }
    }
    return true
  }
}
exports.toggleMark = toggleMark

// :: (...[(EditorState, ?(action: Action)) → bool]) → (EditorState, ?(action: Action)) → bool
// Combine a number of command functions into a single function (which
// calls them one by one until one returns true).
function chainCommands() {
  var commands = [], len = arguments.length;
  while ( len-- ) commands[ len ] = arguments[ len ];

  return function(state, onAction) {
    for (var i = 0; i < commands.length; i++)
      { if (commands[i](state, onAction)) { return true } }
    return false
  }
}
exports.chainCommands = chainCommands

// :: Object
// A basic keymap containing bindings not specific to any schema.
// Binds the following keys (when multiple commands are listed, they
// are chained with [`chainCommands`](#commands.chainCommands):
//
// * **Enter** to `newlineInCode`, `createParagraphNear`, `liftEmptyBlock`, `splitBlock`
// * **Backspace** to `deleteSelection`, `joinBackward`, `deleteCharBefore`
// * **Mod-Backspace** to `deleteSelection`, `joinBackward`, `deleteWordBefore`
// * **Delete** to `deleteSelection`, `joinForward`, `deleteCharAfter`
// * **Mod-Delete** to `deleteSelection`, `joinForward`, `deleteWordAfter`
// * **Alt-ArrowUp** to `joinUp`
// * **Alt-ArrowDown** to `joinDown`
// * **Mod-BracketLeft** to `lift`
// * **Escape** to `selectParentNode`
var baseKeymap = {
  "Enter": chainCommands(newlineInCode, createParagraphNear, liftEmptyBlock, splitBlock),

  "Backspace": ios ? chainCommands(deleteSelection, joinBackward) : chainCommands(deleteSelection, joinBackward, deleteCharBefore),
  "Mod-Backspace": chainCommands(deleteSelection, joinBackward, deleteWordBefore),
  "Delete": chainCommands(deleteSelection, joinForward, deleteCharAfter),
  "Mod-Delete": chainCommands(deleteSelection, joinForward, deleteWordAfter),

  "Alt-ArrowUp": joinUp,
  "Alt-ArrowDown": joinDown,
  "Mod-BracketLeft": lift,
  "Escape": selectParentNode
}

if (mac) {
  var extra = {
    "Ctrl-h": baseKeymap["Backspace"],
    "Alt-Backspace": baseKeymap["Cmd-Backspace"],
    "Ctrl-d": baseKeymap["Delete"],
    "Ctrl-Alt-Backspace": baseKeymap["Cmd-Delete"],
    "Alt-Delete": baseKeymap["Cmd-Delete"],
    "Alt-d": baseKeymap["Cmd-Delete"]
  }
  for (var prop in extra) { baseKeymap[prop] = extra[prop] }
}

exports.baseKeymap = baseKeymap

},{"./char":4,"./platform":6,"extending-char":3,"prosemirror-model":26,"prosemirror-state":37,"prosemirror-transform":42}],6:[function(require,module,exports){
// declare global: os, navigator

var mac = typeof navigator != "undefined" ? /Mac/.test(navigator.platform)
          : typeof os != "undefined" ? os.platform() == "darwin" : false
exports.mac = mac

var ios = typeof navigator != "undefined" && /AppleWebKit/.test(navigator.userAgent) && /Mobile\/\w+/.test(navigator.userAgent)
exports.ios = ios

},{}],7:[function(require,module,exports){
var ref = require("prosemirror-inputrules");
var blockQuoteRule = ref.blockQuoteRule;
var orderedListRule = ref.orderedListRule;
var bulletListRule = ref.bulletListRule;
var codeBlockRule = ref.codeBlockRule;
var headingRule = ref.headingRule;
var inputRules = ref.inputRules;
var allInputRules = ref.allInputRules;
var ref$1 = require("prosemirror-keymap");
var keymap = ref$1.keymap;
var ref$2 = require("prosemirror-history");
var history = ref$2.history;
var ref$3 = require("prosemirror-commands");
var baseKeymap = ref$3.baseKeymap;
var ref$4 = require("prosemirror-state");
var Plugin = ref$4.Plugin;

var ref$5 = require("./menu");
var buildMenuItems = ref$5.buildMenuItems;
exports.buildMenuItems = buildMenuItems
var ref$6 = require("./keymap");
var buildKeymap = ref$6.buildKeymap;
exports.buildKeymap = buildKeymap

// !! This module exports helper functions for deriving a set of basic
// menu items, input rules, or key bindings from a schema. These
// values need to know about the schema for two reasons—they need
// access to specific instances of node and mark types, and they need
// to know which of the node and mark types that they know about are
// actually present in the schema.
//
// The `exampleSetup` plugin ties these together into a plugin that
// will automatically enable this basic functionality in an editor.

// :: (Object) → [Plugin]
// A convenience plugin that bundles together a simple menu with basic
// key bindings, input rules, and styling for the example schema.
// Probably only useful for quickly setting up a passable
// editor—you'll need more control over your settings in most
// real-world situations.
//
//   options::- The following options are recognized:
//
//     schema:: Schema
//     The schema to generate key bindings and menu items for.
//
//     mapKeys:: ?Object
//     Can be used to [adjust](#example-setup.buildKeymap) the key bindings created.
function exampleSetup(options) {
  var deps = [
    inputRules({rules: allInputRules.concat(buildInputRules(options.schema))}),
    keymap(buildKeymap(options.schema, options.mapKeys)),
    keymap(baseKeymap)
  ]
  if (options.history !== false) { deps.push(history()) }

  return deps.concat(new Plugin({
    props: {
      class: function () { return "ProseMirror-example-setup-style"; },
      menuContent: buildMenuItems(options.schema).fullMenu,
      floatingMenu: true
    }
  }))
}
exports.exampleSetup = exampleSetup

// :: (Schema) → [InputRule]
// A set of input rules for creating the basic block quotes, lists,
// code blocks, and heading.
function buildInputRules(schema) {
  var result = [], type
  if (type = schema.nodes.blockquote) { result.push(blockQuoteRule(type)) }
  if (type = schema.nodes.ordered_list) { result.push(orderedListRule(type)) }
  if (type = schema.nodes.bullet_list) { result.push(bulletListRule(type)) }
  if (type = schema.nodes.code_block) { result.push(codeBlockRule(type)) }
  if (type = schema.nodes.heading) { result.push(headingRule(type, 6)) }
  return result
}
exports.buildInputRules = buildInputRules

},{"./keymap":8,"./menu":9,"prosemirror-commands":5,"prosemirror-history":11,"prosemirror-inputrules":12,"prosemirror-keymap":16,"prosemirror-state":37}],8:[function(require,module,exports){
var ref = require("prosemirror-commands");
var wrapIn = ref.wrapIn;
var setBlockType = ref.setBlockType;
var chainCommands = ref.chainCommands;
var newlineInCode = ref.newlineInCode;
var toggleMark = ref.toggleMark;
var ref$1 = require("prosemirror-schema-table");
var selectNextCell = ref$1.selectNextCell;
var selectPreviousCell = ref$1.selectPreviousCell;
var ref$2 = require("prosemirror-schema-list");
var wrapInList = ref$2.wrapInList;
var splitListItem = ref$2.splitListItem;
var liftListItem = ref$2.liftListItem;
var sinkListItem = ref$2.sinkListItem;
var ref$3 = require("prosemirror-history");
var undo = ref$3.undo;
var redo = ref$3.redo;

var mac = typeof navigator != "undefined" ? /Mac/.test(navigator.platform) : false

// :: (Schema, ?Object) → Object
// Inspect the given schema looking for marks and nodes from the
// basic schema, and if found, add key bindings related to them.
// This will add:
//
// * **Mod-b** for toggling [strong](#schema-basic.StrongMark)
// * **Mod-i** for toggling [emphasis](#schema-basic.EmMark)
// * **Mod-`** for toggling [code font](#schema-basic.CodeMark)
// * **Ctrl-Shift-0** for making the current textblock a paragraph
// * **Ctrl-Shift-1** to **Ctrl-Shift-Digit6** for making the current
//   textblock a heading of the corresponding level
// * **Ctrl-Shift-Backslash** to make the current textblock a code block
// * **Ctrl-Shift-8** to wrap the selection in an ordered list
// * **Ctrl-Shift-9** to wrap the selection in a bullet list
// * **Ctrl->** to wrap the selection in a block quote
// * **Enter** to split a non-empty textblock in a list item while at
//   the same time splitting the list item
// * **Mod-Enter** to insert a hard break
// * **Mod-_** to insert a horizontal rule
//
// You can suppress or map these bindings by passing a `mapKeys`
// argument, which maps key names (say `"Mod-B"` to either `false`, to
// remove the binding, or a new key name string.
function buildKeymap(schema, mapKeys) {
  var keys = {}, type
  function bind(key, cmd) {
    if (mapKeys) {
      var mapped = mapKeys[key]
      if (mapped === false) { return }
      if (mapped) { key = mapped }
    }
    keys[key] = cmd
  }

  bind("Mod-z", undo)
  bind("Mod-y", redo)

  if (type = schema.marks.strong)
    { bind("Mod-b", toggleMark(type)) }
  if (type = schema.marks.em)
    { bind("Mod-i", toggleMark(type)) }
  if (type = schema.marks.code)
    { bind("Mod-`", toggleMark(type)) }

  if (type = schema.nodes.bullet_list)
    { bind("Shift-Ctrl-8", wrapInList(type)) }
  if (type = schema.nodes.ordered_list)
    { bind("Shift-Ctrl-9", wrapInList(type)) }
  if (type = schema.nodes.blockquote)
    { bind("Ctrl->", wrapIn(type)) }
  if (type = schema.nodes.hard_break) {
    var br = type, cmd = chainCommands(newlineInCode, function (state, onAction) {
      onAction(state.tr.replaceSelection(br.create()).scrollAction())
      return true
    })
    bind("Mod-Enter", cmd)
    bind("Shift-Enter", cmd)
    if (mac) { bind("Ctrl-Enter", cmd) }
  }
  if (type = schema.nodes.list_item) {
    bind("Enter", splitListItem(type))
    bind("Mod-[", liftListItem(type))
    bind("Mod-]", sinkListItem(type))
  }
  if (type = schema.nodes.paragraph)
    { bind("Shift-Ctrl-0", setBlockType(type)) }
  if (type = schema.nodes.code_block)
    { bind("Shift-Ctrl-\\", setBlockType(type)) }
  if (type = schema.nodes.heading)
    { for (var i = 1; i <= 6; i++) { bind("Shift-Ctrl-" + i, setBlockType(type, {level: i})) } }
  if (type = schema.nodes.horizontal_rule) {
    var hr = type
    bind("Mod-_", function (state, onAction) {
      onAction(state.tr.replaceSelection(hr.create()).scrollAction())
      return true
    })
  }

  if (schema.nodes.table_row) {
    bind("Tab", selectNextCell)
    bind("Shift-Tab", selectPreviousCell)
  }
  return keys
}
exports.buildKeymap = buildKeymap

},{"prosemirror-commands":5,"prosemirror-history":11,"prosemirror-schema-list":35,"prosemirror-schema-table":36}],9:[function(require,module,exports){
var ref = require("prosemirror-menu");
var wrapItem = ref.wrapItem;
var blockTypeItem = ref.blockTypeItem;
var Dropdown = ref.Dropdown;
var DropdownSubmenu = ref.DropdownSubmenu;
var joinUpItem = ref.joinUpItem;
var liftItem = ref.liftItem;
var selectParentNodeItem = ref.selectParentNodeItem;
var undoItem = ref.undoItem;
var redoItem = ref.redoItem;
var icons = ref.icons;
var MenuItem = ref.MenuItem;
var ref$1 = require("prosemirror-schema-table");
var createTable = ref$1.createTable;
var addColumnBefore = ref$1.addColumnBefore;
var addColumnAfter = ref$1.addColumnAfter;
var removeColumn = ref$1.removeColumn;
var addRowBefore = ref$1.addRowBefore;
var addRowAfter = ref$1.addRowAfter;
var removeRow = ref$1.removeRow;
var ref$2 = require("prosemirror-commands");
var toggleMark = ref$2.toggleMark;
var ref$3 = require("prosemirror-schema-list");
var wrapInList = ref$3.wrapInList;
var ref$4 = require("./prompt");
var TextField = ref$4.TextField;
var openPrompt = ref$4.openPrompt;

// Helpers to create specific types of items

function canInsert(state, nodeType, attrs) {
  var $from = state.selection.$from
  for (var d = $from.depth; d >= 0; d--) {
    var index = $from.index(d)
    if ($from.node(d).canReplaceWith(index, index, nodeType, attrs)) { return true }
  }
  return false
}

function insertImageItem(nodeType) {
  return new MenuItem({
    title: "Insert image",
    label: "Image",
    select: function select(state) { return canInsert(state, nodeType) },
    run: function run(state, _, view) {
      var ref = state.selection;
      var node = ref.node;
      var from = ref.from;
      var to = ref.to;
      var attrs = nodeType && node && node.type == nodeType && node.attrs
      openPrompt({
        title: "Insert image",
        fields: {
          src: new TextField({label: "Location", required: true, value: attrs && attrs.src}),
          title: new TextField({label: "Title", value: attrs && attrs.title}),
          alt: new TextField({label: "Description",
                              value: attrs ? attrs.title : state.doc.textBetween(from, to, " ")})
        },
        // FIXME this (and similar uses) won't have the current state
        // when it runs, leading to problems in, for example, a
        // collaborative setup
        callback: function callback(attrs) {
          view.props.onAction(view.state.tr.replaceSelection(nodeType.createAndFill(attrs)).action())
        }
      })
    }
  })
}

function positiveInteger(value) {
  if (!/^[1-9]\d*$/.test(value)) { return "Should be a positive integer" }
}

function insertTableItem(tableType) {
  return new MenuItem({
    title: "Insert a table",
    run: function run(_, _a, view) {
      openPrompt({
        title: "Insert table",
        fields: {
          rows: new TextField({label: "Rows", validate: positiveInteger}),
          cols: new TextField({label: "Columns", validate: positiveInteger})
        },
        callback: function callback(ref) {
          var rows = ref.rows;
          var cols = ref.cols;

          view.props.onAction(view.state.tr.replaceSelection(createTable(tableType, +rows, +cols)).scrollAction())
        }
      })
    },
    select: function select(state) {
      var $from = state.selection.$from
      for (var d = $from.depth; d >= 0; d--) {
        var index = $from.index(d)
        if ($from.node(d).canReplaceWith(index, index, tableType)) { return true }
      }
      return false
    },
    label: "Table"
  })
}

function cmdItem(cmd, options) {
  var passedOptions = {
    label: options.title,
    run: cmd,
    select: function select(state) { return cmd(state) }
  }
  for (var prop in options) { passedOptions[prop] = options[prop] }
  return new MenuItem(passedOptions)
}

function markActive(state, type) {
  var ref = state.selection;
  var from = ref.from;
  var to = ref.to;
  var empty = ref.empty;
  if (empty) { return type.isInSet(state.storedMarks || state.doc.marksAt(from)) }
  else { return state.doc.rangeHasMark(from, to, type) }
}

function markItem(markType, options) {
  var passedOptions = {
    active: function active(state) { return markActive(state, markType) }
  }
  for (var prop in options) { passedOptions[prop] = options[prop] }
  return cmdItem(toggleMark(markType), passedOptions)
}

function linkItem(markType) {
  return markItem(markType, {
    title: "Add or remove link",
    icon: icons.link,
    run: function run(state, onAction, view) {
      if (markActive(state, markType)) {
        toggleMark(markType)(state, onAction)
        return true
      }
      openPrompt({
        title: "Create a link",
        fields: {
          href: new TextField({
            label: "Link target",
            required: true,
            clean: function (val) {
              if (!/^https?:\/\//i.test(val))
                { val = 'http://' + val }
              return val
            }
          }),
          title: new TextField({label: "Title"})
        },
        callback: function callback(attrs) {
          toggleMark(markType, attrs)(view.state, view.props.onAction)
        }
      })
    }
  })
}

function wrapListItem(nodeType, options) {
  return cmdItem(wrapInList(nodeType, options.attrs), options)
}

// :: (Schema) → Object
// Given a schema, look for default mark and node types in it and
// return an object with relevant menu items relating to those marks:
//
// **`toggleStrong`**`: MenuItem`
//   : A menu item to toggle the [strong mark](#schema-basic.StrongMark).
//
// **`toggleEm`**`: MenuItem`
//   : A menu item to toggle the [emphasis mark](#schema-basic.EmMark).
//
// **`toggleCode`**`: MenuItem`
//   : A menu item to toggle the [code font mark](#schema-basic.CodeMark).
//
// **`toggleLink`**`: MenuItem`
//   : A menu item to toggle the [link mark](#schema-basic.LinkMark).
//
// **`insertImage`**`: MenuItem`
//   : A menu item to insert an [image](#schema-basic.Image).
//
// **`wrapBulletList`**`: MenuItem`
//   : A menu item to wrap the selection in a [bullet list](#schema-list.BulletList).
//
// **`wrapOrderedList`**`: MenuItem`
//   : A menu item to wrap the selection in an [ordered list](#schema-list.OrderedList).
//
// **`wrapBlockQuote`**`: MenuItem`
//   : A menu item to wrap the selection in a [block quote](#schema-basic.BlockQuote).
//
// **`makeParagraph`**`: MenuItem`
//   : A menu item to set the current textblock to be a normal
//     [paragraph](#schema-basic.Paragraph).
//
// **`makeCodeBlock`**`: MenuItem`
//   : A menu item to set the current textblock to be a
//     [code block](#schema-basic.CodeBlock).
//
// **`insertTable`**`: MenuItem`
//   : An item to insert a [table](#schema-table).
//
// **`addRowBefore`**, **`addRowAfter`**, **`removeRow`**, **`addColumnBefore`**, **`addColumnAfter`**, **`removeColumn`**`: MenuItem`
//   : Table-manipulation items.
//
// **`makeHead[N]`**`: MenuItem`
//   : Where _N_ is 1 to 6. Menu items to set the current textblock to
//     be a [heading](#schema-basic.Heading) of level _N_.
//
// **`insertHorizontalRule`**`: MenuItem`
//   : A menu item to insert a horizontal rule.
//
// The return value also contains some prefabricated menu elements and
// menus, that you can use instead of composing your own menu from
// scratch:
//
// **`insertMenu`**`: Dropdown`
//   : A dropdown containing the `insertImage` and
//     `insertHorizontalRule` items.
//
// **`typeMenu`**`: Dropdown`
//   : A dropdown containing the items for making the current
//     textblock a paragraph, code block, or heading.
//
// **`fullMenu`**`: [[MenuElement]]`
//   : An array of arrays of menu elements for use as the full menu
//     for, for example the [menu bar](#menu.MenuBarEditorView).
function buildMenuItems(schema) {
  var r = {}, type
  if (type = schema.marks.strong)
    { r.toggleStrong = markItem(type, {title: "Toggle strong style", icon: icons.strong}) }
  if (type = schema.marks.em)
    { r.toggleEm = markItem(type, {title: "Toggle emphasis", icon: icons.em}) }
  if (type = schema.marks.code)
    { r.toggleCode = markItem(type, {title: "Toggle code font", icon: icons.code}) }
  if (type = schema.marks.link)
    { r.toggleLink = linkItem(type) }

  if (type = schema.nodes.image)
    { r.insertImage = insertImageItem(type) }
  if (type = schema.nodes.bullet_list)
    { r.wrapBulletList = wrapListItem(type, {
      title: "Wrap in bullet list",
      icon: icons.bulletList
    }) }
  if (type = schema.nodes.ordered_list)
    { r.wrapOrderedList = wrapListItem(type, {
      title: "Wrap in ordered list",
      icon: icons.orderedList
    }) }
  if (type = schema.nodes.blockquote)
    { r.wrapBlockQuote = wrapItem(type, {
      title: "Wrap in block quote",
      icon: icons.blockquote
    }) }
  if (type = schema.nodes.paragraph)
    { r.makeParagraph = blockTypeItem(type, {
      title: "Change to paragraph",
      label: "Plain"
    }) }
  if (type = schema.nodes.code_block)
    { r.makeCodeBlock = blockTypeItem(type, {
      title: "Change to code block",
      label: "Code"
    }) }
  if (type = schema.nodes.heading)
    { for (var i = 1; i <= 10; i++)
      { r["makeHead" + i] = blockTypeItem(type, {
        title: "Change to heading " + i,
        label: "Level " + i,
        attrs: {level: i}
      }) } }
  if (type = schema.nodes.horizontal_rule) {
    var hr = type
    r.insertHorizontalRule = new MenuItem({
      title: "Insert horizontal rule",
      label: "Horizontal rule",
      select: function select(state) { return canInsert(state, hr) },
      run: function run(state, onAction) { onAction(state.tr.replaceSelection(hr.create()).action()) }
    })
  }
  if (type = schema.nodes.table)
    { r.insertTable = insertTableItem(type) }
  if (type = schema.nodes.table_row) {
    r.addRowBefore = cmdItem(addRowBefore, {title: "Add row before"})
    r.addRowAfter = cmdItem(addRowAfter, {title: "Add row after"})
    r.removeRow = cmdItem(removeRow, {title: "Remove row"})
    r.addColumnBefore = cmdItem(addColumnBefore, {title: "Add column before"})
    r.addColumnAfter = cmdItem(addColumnAfter, {title: "Add column after"})
    r.removeColumn = cmdItem(removeColumn, {title: "Remove column"})
  }

  var cut = function (arr) { return arr.filter(function (x) { return x; }); }
  r.insertMenu = new Dropdown(cut([r.insertImage, r.insertHorizontalRule, r.insertTable]), {label: "Insert"})
  r.typeMenu = new Dropdown(cut([r.makeParagraph, r.makeCodeBlock, r.makeHead1 && new DropdownSubmenu(cut([
    r.makeHead1, r.makeHead2, r.makeHead3, r.makeHead4, r.makeHead5, r.makeHead6
  ]), {label: "Heading"})]), {label: "Type..."})
  var tableItems = cut([r.addRowBefore, r.addRowAfter, r.removeRow, r.addColumnBefore, r.addColumnAfter, r.removeColumn])
  if (tableItems.length)
    { r.tableMenu = new Dropdown(tableItems, {label: "Table"}) }

  r.inlineMenu = [cut([r.toggleStrong, r.toggleEm, r.toggleCode, r.toggleLink]), [r.insertMenu]]
  r.blockMenu = [cut([r.typeMenu, r.tableMenu, r.wrapBulletList, r.wrapOrderedList, r.wrapBlockQuote, joinUpItem,
                      liftItem, selectParentNodeItem])]
  r.fullMenu = r.inlineMenu.concat(r.blockMenu).concat([[undoItem, redoItem]])

  return r
}
exports.buildMenuItems = buildMenuItems

},{"./prompt":10,"prosemirror-commands":5,"prosemirror-menu":18,"prosemirror-schema-list":35,"prosemirror-schema-table":36}],10:[function(require,module,exports){
var prefix = "ProseMirror-prompt"

function openPrompt(options) {
  var wrapper = document.body.appendChild(document.createElement("div"))
  wrapper.className = prefix

  var mouseOutside = function (e) { if (!wrapper.contains(e.target)) { close() } }
  setTimeout(function () { return window.addEventListener("mousedown", mouseOutside); }, 50)
  var close = function () {
    window.removeEventListener("mousedown", mouseOutside)
    if (wrapper.parentNode) { wrapper.parentNode.removeChild(wrapper) }
  }

  var domFields = []
  for (var name in options.fields) { domFields.push(options.fields[name].render()) }

  var submitButton = document.createElement("button")
  submitButton.type = "submit"
  submitButton.className = prefix + "-submit"
  submitButton.textContent = "OK"
  var cancelButton = document.createElement("button")
  cancelButton.type = "button"
  cancelButton.className = prefix + "-cancel"
  cancelButton.textContent = "Cancel"
  cancelButton.addEventListener("click", close)

  var form = wrapper.appendChild(document.createElement("form"))
  if (options.title) { form.appendChild(document.createElement("h5")).textContent = options.title }
  domFields.forEach(function (field) {
    form.appendChild(document.createElement("div")).appendChild(field)
  })
  var buttons = form.appendChild(document.createElement("div"))
  buttons.className = prefix + "-buttons"
  buttons.appendChild(submitButton)
  buttons.appendChild(document.createTextNode(" "))
  buttons.appendChild(cancelButton)

  var box = wrapper.getBoundingClientRect()
  wrapper.style.top = ((window.innerHeight - box.height) / 2) + "px"
  wrapper.style.left = ((window.innerWidth - box.width) / 2) + "px"

  var submit = function () {
    var params = getValues(options.fields, domFields)
    if (params) {
      close()
      options.callback(params)
    }
  }

  form.addEventListener("submit", function (e) {
    e.preventDefault()
    submit()
  })

  form.addEventListener("keydown", function (e) {
    if (e.keyCode == 27) {
      e.preventDefault()
      close()
    } else if (e.keyCode == 13 && !(e.ctrlKey || e.metaKey || e.shiftKey)) {
      e.preventDefault()
      submit()
    } else if (e.keyCode == 9) {
      window.setTimeout(function () {
        if (!wrapper.contains(document.activeElement)) { close() }
      }, 500)
    }
  })

  var input = form.elements[0]
  if (input) { input.focus() }
}
exports.openPrompt = openPrompt

function getValues(fields, domFields) {
  var result = Object.create(null), i = 0
  for (var name in fields) {
    var field = fields[name], dom = domFields[i++]
    var value = field.read(dom), bad = field.validate(value)
    if (bad) {
      reportInvalid(dom, bad)
      return null
    }
    result[name] = field.clean(value)
  }
  return result
}

function reportInvalid(dom, message) {
  // FIXME this is awful and needs a lot more work
  var parent = dom.parentNode
  var msg = parent.appendChild(document.createElement("div"))
  msg.style.left = (dom.offsetLeft + dom.offsetWidth + 2) + "px"
  msg.style.top = (dom.offsetTop - 5) + "px"
  msg.className = "ProseMirror-invalid"
  msg.textContent = message
  setTimeout(function () { return parent.removeChild(msg); }, 1500)
}

// ::- The type of field that `FieldPrompt` expects to be passed to it.
var Field = function Field(options) { this.options = options };

// render:: (state: EditorState, props: Object) → dom.Node
// Render the field to the DOM. Should be implemented by all subclasses.

// :: (dom.Node) → any
// Read the field's value from its DOM node.
Field.prototype.read = function read (dom) { return dom.value };

// :: (any) → ?string
// A field-type-specific validation function.
Field.prototype.validateType = function validateType (_value) {};

Field.prototype.validate = function validate (value) {
  if (!value && this.options.required)
    { return "Required field" }
  return this.validateType(value) || (this.options.validate && this.options.validate(value))
};

Field.prototype.clean = function clean (value) {
  return this.options.clean ? this.options.clean(value) : value
};
exports.Field = Field

// ::- A field class for single-line text fields.
var TextField = (function (Field) {
  function TextField () {
    Field.apply(this, arguments);
  }

  if ( Field ) TextField.__proto__ = Field;
  TextField.prototype = Object.create( Field && Field.prototype );
  TextField.prototype.constructor = TextField;

  TextField.prototype.render = function render () {
    var input = document.createElement("input")
    input.type = "text"
    input.placeholder = this.options.label
    input.value = this.options.value || ""
    input.autocomplete = "off"
    return input
  };

  return TextField;
}(Field));
exports.TextField = TextField


// ::- A field class for dropdown fields based on a plain `<select>`
// tag. Expects an option `options`, which should be an array of
// `{value: string, label: string}` objects, or a function taking a
// `ProseMirror` instance and returning such an array.
var SelectField = (function (Field) {
  function SelectField () {
    Field.apply(this, arguments);
  }

  if ( Field ) SelectField.__proto__ = Field;
  SelectField.prototype = Object.create( Field && Field.prototype );
  SelectField.prototype.constructor = SelectField;

  SelectField.prototype.render = function render () {
    var this$1 = this;

    var select = document.createElement("select")
    this.options.options.forEach(function (o) {
      var opt = select.appendChild(document.createElement("option"))
      opt.value = o.value
      opt.selected = o.value == this$1.options.value
      opt.label = o.label
    })
    return select
  };

  return SelectField;
}(Field));
exports.SelectField = SelectField

},{}],11:[function(require,module,exports){
var RopeSequence = require("rope-sequence")
var ref = require("prosemirror-transform");
var Mapping = ref.Mapping;
var ref$1 = require("prosemirror-state");
var Selection = ref$1.Selection;
var Plugin = ref$1.Plugin;
var PluginKey = ref$1.PluginKey;

// ProseMirror's history isn't simply a way to roll back to a previous
// state, because ProseMirror supports applying changes without adding
// them to the history (for example during collaboration).
//
// To this end, each 'Branch' (one for the undo history and one for
// the redo history) keeps an array of 'Items', which can optionally
// hold a step (an actual undoable change), and always hold a position
// map (which is needed to move changes below them to apply to the
// current document).
//
// An item that has both a step and a selection JSON representation is
// the start of an 'event' — a group of changes that will be undone
// or redone at once. (It stores only the JSON, since that way we don't
// have to provide a document until the selection is actually applied,
// which is useful when compressing.)

// Used to schedule history compression
var max_empty_items = 500

var Branch = function Branch(items, eventCount) {
  this.items = items
  this.eventCount = eventCount
};

// : (Node, bool, ?Item) → ?{transform: Transform, selection: Object}
// Pop the latest event off the branch's history and apply it
// to a document transform.
Branch.prototype.popEvent = function popEvent (state, preserveItems) {
    var this$1 = this;

  if (this.eventCount == 0) { return null }

  var end = this.items.length
  for (;; end--) {
    var next = this$1.items.get(end - 1)
    if (next.selection) { --end; break }
  }

  var remap, mapFrom
  if (preserveItems) {
    remap = this.remapping(end, this.items.length)
    mapFrom = remap.maps.length
  }
  var transform = state.tr
  var selection, remaining
  var addAfter = [], addBefore = []

  this.items.forEach(function (item, i) {
    if (!item.step) {
      if (!remap) {
        remap = this$1.remapping(end, i + 1)
        mapFrom = remap.maps.length
      }
      mapFrom--
      addBefore.push(item)
      return
    }

    if (remap) {
      addBefore.push(new Item(item.map))
      var step = item.step.map(remap.slice(mapFrom)), map

      if (step && transform.maybeStep(step).doc) {
        map = transform.mapping.maps[transform.mapping.maps.length - 1]
        addAfter.push(new Item(map, null, null, addAfter.length + addBefore.length))
      }
      mapFrom--
      if (map) { remap.appendMap(map, mapFrom) }
    } else {
      transform.maybeStep(item.step)
    }

    if (item.selection) {
      selection = remap ? Selection.mapJSON(item.selection, remap.slice(mapFrom)) : item.selection
      remaining = new Branch(this$1.items.slice(0, end).append(addBefore.reverse().concat(addAfter)), this$1.eventCount - 1)
      return false
    }
  }, this.items.length, 0)

  return {remaining: remaining, transform: transform, selection: selection}
};

// : (Transform, Selection, Object)
// Create a new branch with the given transform added.
Branch.prototype.addTransform = function addTransform (transform, selection, histOptions) {
  var newItems = [], eventCount = this.eventCount + (selection ? 1 : 0)
  var oldItems = this.items, lastItem = !histOptions.preserveItems && oldItems.length ? oldItems.get(oldItems.length - 1) : null

  for (var i = 0; i < transform.steps.length; i++) {
    var step = transform.steps[i].invert(transform.docs[i])
    var item = new Item(transform.mapping.maps[i], step, selection), merged = void 0
    if (merged = lastItem && lastItem.merge(item)) {
      item = merged
      if (i) { newItems.pop() }
      else { oldItems = oldItems.slice(0, oldItems.length - 1) }
    }
    newItems.push(item)
    selection = null
    if (!histOptions.preserveItems) { lastItem = item }
  }
  var overflow = this.eventCount - histOptions.depth
  if (overflow > DEPTH_OVERFLOW) { oldItems = cutOffEvents(oldItems, overflow) }
  return new Branch(oldItems.append(newItems), eventCount)
};

Branch.prototype.remapping = function remapping (from, to) {
  var maps = [], mirrors = []
  this.items.forEach(function (item, i) {
    if (item.mirrorOffset != null) {
      var mirrorPos = i - item.mirrorOffset
      if (mirrorPos >= from) { mirrors.push(maps.length - item.mirrorOffset, maps.length) }
    }
    maps.push(item.map)
  }, from, to)
  return new Mapping(maps, mirrors)
};

Branch.prototype.addMaps = function addMaps (array) {
  if (this.eventCount == 0) { return this }
  return new Branch(this.items.append(array.map(function (map) { return new Item(map); })), this.eventCount)
};

// : ([StepMap], Transform, [number])
// When the collab module receives remote changes, the history has
// to know about those, so that it can adjust the steps that were
// rebased on top of the remote changes, and include the position
// maps for the remote changes in its array of items.
Branch.prototype.rebased = function rebased (rebasedTransform, rebasedCount) {
  if (!this.eventCount) { return this }

  var rebasedItems = [], start = this.items.length - rebasedCount, startPos = 0
  if (start < 0) {
    startPos = -start
    start = 0
  }

  var mapping = rebasedTransform.mapping
  var newUntil = rebasedTransform.steps.length

  var iRebased = startPos
  this.items.forEach(function (item) {
    var pos = mapping.getMirror(iRebased++)
    if (pos == null) { return }
    newUntil = Math.min(newUntil, pos)
    var map = mapping.maps[pos]
    if (item.step) {
      var step = rebasedTransform.steps[pos].invert(rebasedTransform.docs[pos])
      var selection = item.selection && Selection.mapJSON(item.selection, mapping.slice(iRebased - 1, pos))
      rebasedItems.push(new Item(map, step, selection))
    } else {
      rebasedItems.push(new Item(map))
    }
  }, start)

  var newMaps = []
  for (var i = rebasedCount; i < newUntil; i++)
    { newMaps.push(new Item(mapping.maps[i])) }
  var items = this.items.slice(0, start).append(newMaps).append(rebasedItems)
  var branch = new Branch(items, this.eventCount) // FIXME might update event count
  if (branch.emptyItemCount() > max_empty_items)
    { branch = branch.compress(this.items.length - rebasedItems.length) }
  return branch
};

Branch.prototype.emptyItemCount = function emptyItemCount () {
  var count = 0
  this.items.forEach(function (item) { if (!item.step) { count++ } })
  return count
};

// Compressing a branch means rewriting it to push the air (map-only
// items) out. During collaboration, these naturally accumulate
// because each remote change adds one. The `upto` argument is used
// to ensure that only the items below a given level are compressed,
// because `rebased` relies on a clean, untouched set of items in
// order to associate old items with rebased steps.
Branch.prototype.compress = function compress (upto) {
    if ( upto === void 0 ) upto = this.items.length;

  var remap = this.remapping(0, upto), mapFrom = remap.maps.length
  var items = [], events = 0
  this.items.forEach(function (item, i) {
    if (i >= upto) {
      items.push(item)
    } else if (item.step) {
      var step = item.step.map(remap.slice(mapFrom)), map = step && step.getMap()
      mapFrom--
      if (map) { remap.appendMap(map, mapFrom) }
      if (step) {
        var selection = item.selection && Selection.mapJSON(item.selection, remap.slice(mapFrom))
        if (selection) { events++ }
        var newItem = new Item(map.invert(), step, selection), merged, last = items.length - 1
        if (merged = items.length && items[last].merge(newItem))
          { items[last] = merged }
        else
          { items.push(newItem) }
      }
    } else if (item.map) {
      mapFrom--
    }
  }, this.items.length, 0)
  return new Branch(RopeSequence.from(items.reverse()), events)
};

Branch.empty = new Branch(RopeSequence.empty, 0)

function cutOffEvents(items, n) {
  var cutPoint
  items.forEach(function (item, i) {
    if (item.selection && (--n == 0)) {
      cutPoint = i
      return false
    }
  })
  return items.slice(cutPoint)
}

var Item = function Item(map, step, selection, mirrorOffset) {
  this.map = map
  this.step = step
  this.selection = selection
  this.mirrorOffset = mirrorOffset
};

Item.prototype.merge = function merge (other) {
  if (this.step && other.step && !other.selection) {
    var step = other.step.merge(this.step)
    if (step) { return new Item(step.getMap().invert(), step, this.selection) }
  }
};

// The value of the state field that tracks undo/redo history for that
// state. Will be stored in the plugin state when the history plugin
// is active.
var HistoryState = function HistoryState(done, undone, prevMap, prevTime) {
  this.done = done
  this.undone = undone
  this.prevMap = prevMap
  this.prevTime = prevTime
};
exports.HistoryState = HistoryState

var DEPTH_OVERFLOW = 20

// : (EditorState, Transform, Selection, Object)
// Record a transformation in undo history.
function recordTransform(history, selection, action, options) {
  var transform = action.transform
  if (action.historyState) {
    return action.historyState
  } else if (transform.steps.length == 0) {
    return history
  } else if (action.addToHistory !== false) {
    // Group transforms that occur in quick succession into one event.
    var newGroup = history.prevTime < (action.time || 0) - options.newGroupDelay ||
        !isAdjacentToLastStep(transform, history.prevMap, history.done)
    return new HistoryState(history.done.addTransform(transform, newGroup ? selection.toJSON() : null, options),
                            Branch.empty, transform.mapping.maps[transform.steps.length - 1], action.time)
  } else if (action.rebased) {
    // Used by the collab module to tell the history that some of its
    // content has been rebased.
    return new HistoryState(history.done.rebased(transform, action.rebased),
                            history.undone.rebased(transform, action.rebased),
                            history.prevMap && transform.mapping.maps[transform.steps.length - 1], history.prevTime)
  } else {
    return new HistoryState(history.done.addMaps(transform.mapping.maps),
                            history.undone.addMaps(transform.mapping.maps),
                            history.prevMap, history.prevTime)
  }
}

function isAdjacentToLastStep(transform, prevMap, done) {
  if (!prevMap) { return false }
  var firstMap = transform.mapping.maps[0], adjacent = false
  if (!firstMap) { return true }
  firstMap.forEach(function (start, end) {
    done.items.forEach(function (item) {
      if (item.step) {
        prevMap.forEach(function (_start, _end, rStart, rEnd) {
          if (start <= rEnd && end >= rStart) { adjacent = true }
        })
        return false
      } else {
        start = item.map.invert().map(start, -1)
        end = item.map.invert().map(end, 1)
      }
    }, done.items.length, 0)
  })
  return adjacent
}

// : (EditorState, bool, Object) → Object
// Apply the latest event from one branch to the document and optionally
// shift the event onto the other branch. Returns true when an event could
// be shifted.
function histAction(history, state, redo) {
  var histOptions = historyKey.get(state).options.config
  var pop = (redo ? history.undone : history.done).popEvent(state, histOptions.preserveItems)

  var selectionBefore = state.selection
  var selection = Selection.fromJSON(pop.transform.doc, pop.selection)
  var added = (redo ? history.done : history.undone).addTransform(pop.transform, selectionBefore.toJSON(), histOptions)

  var newHist = new HistoryState(redo ? added : pop.remaining, redo ? pop.remaining : added, null, 0)
  return pop.transform.action({selection: selection, historyState: newHist, scrollIntoView: true})
}

var historyKey = new PluginKey("history")

// :: (?Object) → Plugin
// Returns a plugin that enables the undo history for an editor.
//
//   config::-
//   Supports the following configuration options:
//
//     depth:: ?number
//     The amount of history events that are collected before the
//     oldest events are discarded. Defaults to 100.
//
//     newGroupDelay:: number
//     The delay between changes after which a new group should be
//     started. Defaults to 500 (milliseconds). Note that when changes
//     aren't adjacent, a new group is always started.
//
//     preserveItems:: ?bool
//     Whether to preserve the steps exactly as they came in. **Must**
//     be true when using the history together with the collaborative
//     editing plugin, to allow syncing the history when concurrent
//     changes come in. Defaults to false.
function history(config) {
  config = {depth: config && config.depth || 100,
            preserveItems: !!(config && config.preserveItems),
            newGroupDelay: config && config.newGroupDelay || 500}
  return new Plugin({
    key: historyKey,

    state: {
      init: function init() {
        return new HistoryState(Branch.empty, Branch.empty, null, 0)
      },
      applyAction: function applyAction(action, hist, state) {
        if (action.type == "transform")
          { return recordTransform(hist, state.selection, action, config) }
        if (action.type == "historyClose")
          { return new HistoryState(hist.done, hist.undone, null, 0) }
        return hist
      }
    },

    config: config
  })
}
exports.history = history

// :: (state: EditorState, onAction: ?(action: Action)) → bool
// A command function that undoes the last change, if any.
function undo(state, onAction) {
  var hist = historyKey.getState(state)
  if (!hist || hist.undoDepth == 0) { return false }
  if (onAction) { onAction(histAction(hist, state, false)) }
  return true
}
exports.undo = undo

// :: (state: EditorState, onAction: ?(action: Action)) → bool
// A command function that redoes the last undone change, if any.
function redo(state, onAction) {
  var hist = historyKey.getState(state)
  if (!hist || hist.redoDepth == 0) { return false }
  if (onAction) { onAction(histAction(hist, state, true)) }
  return true
}
exports.redo = redo

// :: (EditorState) → number
// The amount of undoable events available in a given state.
function undoDepth(state) {
  var hist = historyKey.getState(state)
  return hist ? hist.done.eventCount : 0
}
exports.undoDepth = undoDepth

// :: (EditorState) → number
// The amount of redoable events available in a given editor state.
function redoDepth(state) {
  var hist = historyKey.getState(state)
  return hist ? hist.undone.eventCount : 0
}
exports.redoDepth = redoDepth

},{"prosemirror-state":37,"prosemirror-transform":42,"rope-sequence":60}],12:[function(require,module,exports){
;var assign;
((assign = require("./inputrules"), exports.InputRule = assign.InputRule, exports.inputRules = assign.inputRules))
;var assign$1;
((assign$1 = require("./rules"), exports.emDash = assign$1.emDash, exports.ellipsis = assign$1.ellipsis, exports.openDoubleQuote = assign$1.openDoubleQuote, exports.closeDoubleQuote = assign$1.closeDoubleQuote, exports.openSingleQuote = assign$1.openSingleQuote, exports.closeSingleQuote = assign$1.closeSingleQuote, exports.smartQuotes = assign$1.smartQuotes, exports.allInputRules = assign$1.allInputRules))
;var assign$2;
((assign$2 = require("./util"), exports.wrappingInputRule = assign$2.wrappingInputRule, exports.textblockTypeInputRule = assign$2.textblockTypeInputRule, exports.blockQuoteRule = assign$2.blockQuoteRule, exports.orderedListRule = assign$2.orderedListRule, exports.bulletListRule = assign$2.bulletListRule, exports.codeBlockRule = assign$2.codeBlockRule, exports.headingRule = assign$2.headingRule))

},{"./inputrules":13,"./rules":14,"./util":15}],13:[function(require,module,exports){
var ref = require("prosemirror-state");
var Plugin = ref.Plugin;

// ::- Input rules are regular expressions describing a piece of text
// that, when typed, causes something to happen. This might be
// changing two dashes into an emdash, wrapping a paragraph starting
// with `"> "` into a blockquote, or something entirely different.
var InputRule = function InputRule(match, handler) {
  this.match = match
  this.handler = typeof handler == "string" ? stringHandler(handler) : handler
};
exports.InputRule = InputRule

function stringHandler(string) {
  return function(state, match, start, end) {
    var insert = string
    if (match[1]) {
      var offset = match[0].lastIndexOf(match[1])
      insert += match[0].slice(offset + match[1].length)
      start += offset
      var cutOff = start - end
      if (cutOff > 0) {
        insert = match[0].slice(offset - cutOff, offset) + insert
        start = end
      }
    }
    var marks = state.doc.marksAt(start)
    return state.tr.replaceWith(start, end, state.schema.text(insert, marks))
  }
}

var MAX_MATCH = 100

// :: (config: {rules: [InputRule]}) → Plugin
// Create an input rules plugin. When enabled, it will cause text
// input that matches any of the given rules to trigger the rule's
// action, and binds the backspace key, when applied directly after an
// input rule triggered, to undo the rule's effect.
function inputRules(ref) {
  var rules = ref.rules;

  return new Plugin({
    state: {
      init: function init() { return null },
      applyAction: function applyAction(action, prev) {
        if (action.type == "transform") { return action.fromInputRule }
        if (action.type == "selection") { return null }
        return prev
      }
    },

    props: {
      handleTextInput: function handleTextInput(view, from, to, text) {
        var state = view.state, $from = state.doc.resolve(from)
        var textBefore = $from.parent.textBetween(Math.max(0, $from.parentOffset - MAX_MATCH), $from.parentOffset,
                                                  null, "\ufffc") + text
        for (var i = 0; i < rules.length; i++) {
          var match = rules[i].match.exec(textBefore)
          var transform = match && rules[i].handler(state, match, from - (match[0].length - text.length), to)
          if (!transform) { continue }
          view.props.onAction(transform.action({fromInputRule: {transform: transform, from: from, to: to, text: text}}))
          return true
        }
        return false
      },

      handleKeyDown: function handleKeyDown(view, event) {
        if (event.keyCode == 8) { return maybeUndoInputRule(view.state, view.props.onAction, this.getState(view.state)) }
        return false
      }
    }
  })
}
exports.inputRules = inputRules

function maybeUndoInputRule(state, onAction, undoable) {
  if (!undoable) { return false }
  var tr = state.tr, toUndo = undoable.transform
  for (var i = toUndo.steps.length - 1; i >= 0; i--)
    { tr.step(toUndo.steps[i].invert(toUndo.docs[i])) }
  var marks = tr.doc.marksAt(undoable.from)
  tr.replaceWith(undoable.from, undoable.to, state.schema.text(undoable.text, marks))
  onAction(tr.action())
  return true
}

},{"prosemirror-state":37}],14:[function(require,module,exports){
var ref = require("./inputrules");
var InputRule = ref.InputRule;

// :: InputRule Converts double dashes to an emdash.
var emDash = new InputRule(/--$/, "—")
exports.emDash = emDash
// :: InputRule Converts three dots to an ellipsis character.
var ellipsis = new InputRule(/\.\.\.$/, "…")
exports.ellipsis = ellipsis
// :: InputRule “Smart” opening double quotes.
var openDoubleQuote = new InputRule(/(?:^|[\s\{\[\(\<'"\u2018\u201C])(")$/, "“")
exports.openDoubleQuote = openDoubleQuote
// :: InputRule “Smart” closing double quotes.
var closeDoubleQuote = new InputRule(/"$/, "”")
exports.closeDoubleQuote = closeDoubleQuote
// :: InputRule “Smart” opening single quotes.
var openSingleQuote = new InputRule(/(?:^|[\s\{\[\(\<'"\u2018\u201C])(')$/, "‘")
exports.openSingleQuote = openSingleQuote
// :: InputRule “Smart” closing single quotes.
var closeSingleQuote = new InputRule(/'$/, "’")
exports.closeSingleQuote = closeSingleQuote

// :: [InputRule] Smart-quote related input rules.
var smartQuotes = [openDoubleQuote, closeDoubleQuote, openSingleQuote, closeSingleQuote]
exports.smartQuotes = smartQuotes

// :: [InputRule] All schema-independent input rules defined in this module.
var allInputRules = [emDash, ellipsis].concat(smartQuotes)
exports.allInputRules = allInputRules

},{"./inputrules":13}],15:[function(require,module,exports){
var ref = require("./inputrules");
var InputRule = ref.InputRule;
var ref$1 = require("prosemirror-transform");
var findWrapping = ref$1.findWrapping;
var canJoin = ref$1.canJoin;

// :: (RegExp, NodeType, ?union<Object, ([string]) → ?Object>, ?([string], Node) → bool) → InputRule
// Build an input rule for automatically wrapping a textblock when a
// given string is typed. The `regexp` argument is
// directly passed through to the `InputRule` constructor. You'll
// probably want the regexp to start with `^`, so that the pattern can
// only occur at the start of a textblock.
//
// `nodeType` is the type of node to wrap in. If it needs attributes,
// you can either pass them directly, or pass a function that will
// compute them from the regular expression match.
//
// By default, if there's a node with the same type above the newly
// wrapped node, the rule will try to [join](#transform.Transform.join) those
// two nodes. You can pass a join predicate, which takes a regular
// expression match and the node before the wrapped node, and can
// return a boolean to indicate whether a join should happen.
function wrappingInputRule(regexp, nodeType, getAttrs, joinPredicate) {
  return new InputRule(regexp, function (state, match, start, end) {
    var attrs = getAttrs instanceof Function ? getAttrs(match) : getAttrs
    var tr = state.tr.delete(start, end)
    var $start = tr.doc.resolve(start), range = $start.blockRange(), wrapping = range && findWrapping(range, nodeType, attrs)
    if (!wrapping) { return null }
    tr.wrap(range, wrapping)
    var before = tr.doc.resolve(start - 1).nodeBefore
    if (before && before.type == nodeType && canJoin(tr.doc, start - 1) &&
        (!joinPredicate || joinPredicate(match, before)))
      { tr.join(start - 1) }
    return tr
  })
}
exports.wrappingInputRule = wrappingInputRule

// :: (RegExp, NodeType, ?union<Object, ([string]) → ?Object>) → InputRule
// Build an input rule that changes the type of a textblock when the
// matched text is typed into it. You'll usually want to start your
// regexp with `^` to that it is only matched at the start of a
// textblock. The optional `getAttrs` parameter can be used to compute
// the new node's attributes, and works the same as in the
// `wrappingInputRule` function.
function textblockTypeInputRule(regexp, nodeType, getAttrs) {
  return new InputRule(regexp, function (state, match, start, end) {
    var $start = state.doc.resolve(start)
    var attrs = getAttrs instanceof Function ? getAttrs(match) : getAttrs
    if (!$start.node(-1).canReplaceWith($start.index(-1), $start.indexAfter(-1), nodeType, attrs)) { return null }
    return state.tr
      .delete(start, end)
      .setBlockType(start, start, nodeType, attrs)
  })
}
exports.textblockTypeInputRule = textblockTypeInputRule


// :: (NodeType) → InputRule
// Given a blockquote node type, returns an input rule that turns `"> "`
// at the start of a textblock into a blockquote.
function blockQuoteRule(nodeType) {
  return wrappingInputRule(/^\s*> $/, nodeType)
}
exports.blockQuoteRule = blockQuoteRule

// :: (NodeType) → InputRule
// Given a list node type, returns an input rule that turns a number
// followed by a dot at the start of a textblock into an ordered list.
function orderedListRule(nodeType) {
  return wrappingInputRule(/^(\d+)\. $/, nodeType, function (match) { return ({order: +match[1]}); },
                           function (match, node) { return node.childCount + node.attrs.order == +match[1]; })
}
exports.orderedListRule = orderedListRule

// :: (NodeType) → InputRule
// Given a list node type, returns an input rule that turns a bullet
// (dash, plush, or asterisk) at the start of a textblock into a
// bullet list.
function bulletListRule(nodeType) {
  return wrappingInputRule(/^\s*([-+*]) $/, nodeType)
}
exports.bulletListRule = bulletListRule

// :: (NodeType) → InputRule
// Given a code block node type, returns an input rule that turns a
// textblock starting with three backticks into a code block.
function codeBlockRule(nodeType) {
  return textblockTypeInputRule(/^```$/, nodeType)
}
exports.codeBlockRule = codeBlockRule

// :: (NodeType, number) → InputRule
// Given a node type and a maximum level, creates an input rule that
// turns up to that number of `#` characters followed by a space at
// the start of a textblock into a heading whose level corresponds to
// the number of `#` signs.
function headingRule(nodeType, maxLevel) {
  return textblockTypeInputRule(new RegExp("^(#{1," + maxLevel + "}) $"),
                                nodeType, function (match) { return ({level: match[1].length}); })
}
exports.headingRule = headingRule

},{"./inputrules":13,"prosemirror-transform":42}],16:[function(require,module,exports){
var keyName = require("w3c-keyname")
var ref = require("prosemirror-state");
var Plugin = ref.Plugin;

// declare global: navigator

var mac = typeof navigator != "undefined" ? /Mac/.test(navigator.platform) : false

function normalizeKeyName(name) {
  var parts = name.split(/-(?!$)/), result = parts[parts.length - 1]
  if (result == "Space") { result = " " }
  var alt, ctrl, shift, meta
  for (var i = 0; i < parts.length - 1; i++) {
    var mod = parts[i]
    if (/^(cmd|meta|m)$/i.test(mod)) { meta = true }
    else if (/^a(lt)?$/i.test(mod)) { alt = true }
    else if (/^(c|ctrl|control)$/i.test(mod)) { ctrl = true }
    else if (/^s(hift)?$/i.test(mod)) { shift = true }
    else if (/^mod$/i.test(mod)) { if (mac) { meta = true; } else { ctrl = true } }
    else { throw new Error("Unrecognized modifier name: " + mod) }
  }
  if (alt) { result = "Alt-" + result }
  if (ctrl) { result = "Ctrl-" + result }
  if (meta) { result = "Meta-" + result }
  if (shift) { result = "Shift-" + result }
  return result
}

function normalize(map) {
  var copy = Object.create(null)
  for (var prop in map) { copy[normalizeKeyName(prop)] = map[prop] }
  return copy
}

function modifiers(name, event, shift) {
  if (event.altKey) { name = "Alt-" + name }
  if (event.ctrlKey) { name = "Ctrl-" + name }
  if (event.metaKey) { name = "Meta-" + name }
  if (shift !== false && event.shiftKey) { name = "Shift-" + name }
  return name
}

// :: (Object) → Plugin
// Create a keymap plugin for the given set of bindings.
//
// Bindings should map key names to [command](#commands)-style
// functions, which will be called with `(EditorState, onAction,
// EditorView)` arguments, and should return true when they've handled
// the key. Note that the view argument isn't part of the command
// protocol, but can be used as an escape hatch if a binding needs to
// directly interact with the UI.
//
// Key names may be strings like `"Shift-Ctrl-Enter"`, a key
// identifier prefixed with zero or more modifiers. Key identifiers
// are based on the strings that can appear in
// [`KeyEvent.key`](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key).
// Use lowercase letters to refer to letter keys (or uppercase letters
// if you want shift to be held). You may use `"Space"` as an alias
// for the `" "` name.
//
// Modifiers can be given in any order. `Shift-` (or `s-`), `Alt-` (or
// `a-`), `Ctrl-` (or `c-` or `Control-`) and `Cmd-` (or `m-` or
// `Meta-`) are recognized. For characters that are created by holding
// shift, the `Shift-` prefix is implied, and should not be added
// explicitly.
//
// You can use `Mod-` as a shorthand for `Cmd-` on Mac and `Ctrl-` on
// other platforms.
//
// You can add multiple keymap plugins to an editor. The order in
// which they appear determines their precedence (the ones early in
// the array get to dispatch first).
function keymap(bindings) {
  var map = normalize(bindings)

  return new Plugin({
    props: {
      handleKeyDown: function handleKeyDown(view, event) {
        var name = keyName(event), isChar = name.length == 1 && name != " ", baseName
        var direct = map[modifiers(name, event, !isChar)]
        if (direct && direct(view.state, view.props.onAction, view)) { return true }
        if (event.shiftKey && isChar && (baseName = keyName.base[event.keyCode])) {
          var withShift = map[modifiers(baseName, event, true)]
          if (withShift && withShift(view.state, view.props.onAction, view)) { return true }
        }
        return false
      }
    }
  })
}
exports.keymap = keymap

},{"prosemirror-state":37,"w3c-keyname":61}],17:[function(require,module,exports){
var SVG = "http://www.w3.org/2000/svg"
var XLINK = "http://www.w3.org/1999/xlink"

var prefix = "ProseMirror-icon"

function hashPath(path) {
  var hash = 0
  for (var i = 0; i < path.length; i++)
    { hash = (((hash << 5) - hash) + path.charCodeAt(i)) | 0 }
  return hash
}

function getIcon(icon) {
  var node = document.createElement("div")
  node.className = prefix
  if (icon.path) {
    var name = "pm-icon-" + hashPath(icon.path).toString(16)
    if (!document.getElementById(name)) { buildSVG(name, icon) }
    var svg = node.appendChild(document.createElementNS(SVG, "svg"))
    svg.style.width = (icon.width / icon.height) + "em"
    var use = svg.appendChild(document.createElementNS(SVG, "use"))
    use.setAttributeNS(XLINK, "href", /([^#]*)/.exec(document.location)[1] + "#" + name)
  } else if (icon.dom) {
    node.appendChild(icon.dom.cloneNode(true))
  } else {
    node.appendChild(document.createElement("span")).textContent = icon.text || ''
    if (icon.css) { node.firstChild.style.cssText = icon.css }
  }
  return node
}
exports.getIcon = getIcon

function buildSVG(name, data) {
  var collection = document.getElementById(prefix + "-collection")
  if (!collection) {
    collection = document.createElementNS(SVG, "svg")
    collection.id = prefix + "-collection"
    collection.style.display = "none"
    document.body.insertBefore(collection, document.body.firstChild)
  }
  var sym = document.createElementNS(SVG, "symbol")
  sym.id = name
  sym.setAttribute("viewBox", "0 0 " + data.width + " " + data.height)
  var path = sym.appendChild(document.createElementNS(SVG, "path"))
  path.setAttribute("d", data.path)
  collection.appendChild(sym)
}

},{}],18:[function(require,module,exports){
;var assign;
((assign = require("./menu"), exports.MenuItem = assign.MenuItem, exports.Dropdown = assign.Dropdown, exports.DropdownSubmenu = assign.DropdownSubmenu, exports.renderGrouped = assign.renderGrouped, exports.icons = assign.icons, exports.joinUpItem = assign.joinUpItem, exports.liftItem = assign.liftItem, exports.selectParentNodeItem = assign.selectParentNodeItem, exports.undoItem = assign.undoItem, exports.redoItem = assign.redoItem, exports.wrapItem = assign.wrapItem, exports.blockTypeItem = assign.blockTypeItem))
exports.MenuBarEditorView = require("./menubar").MenuBarEditorView

// !! This module defines a number of building blocks for ProseMirror
// menus, along with a [menu bar](#menu.MenuBarEditorView) implementation.

// MenuElement:: interface
// The types defined in this module aren't the only thing you can
// display in your menu. Anything that conforms to this interface can
// be put into a menu structure.
//
//   render:: (pm: ProseMirror) → ?dom.Node
//   Render the element for display in the menu. Returning `null` can be
//   used to signal that this element shouldn't be displayed for the
//   given editor state.

},{"./menu":19,"./menubar":20}],19:[function(require,module,exports){
var crel = require("crel")
var ref = require("prosemirror-commands");
var lift = ref.lift;
var joinUp = ref.joinUp;
var selectParentNode = ref.selectParentNode;
var wrapIn = ref.wrapIn;
var setBlockType = ref.setBlockType;
var ref$1 = require("prosemirror-history");
var undo = ref$1.undo;
var redo = ref$1.redo;

var ref$2 = require("./icons");
var getIcon = ref$2.getIcon;

var prefix = "ProseMirror-menu"

// ::- An icon or label that, when clicked, executes a command.
var MenuItem = function MenuItem(spec) {
  // :: MenuItemSpec
  // The spec used to create the menu item.
  this.spec = spec
};

// :: (EditorView) → dom.Node
// Renders the icon according to its [display
// spec](#menu.MenuItemSpec.display), and adds an event handler which
// executes the command when the representation is clicked.
MenuItem.prototype.render = function render (view) {
  var disabled = false, spec = this.spec
  if (spec.select && !spec.select(view.state)) {
    if (spec.onDeselected == "disable") { disabled = true }
    else { return null }
  }
  var active = spec.active && !disabled && spec.active(view.state)

  var dom
  if (spec.render) {
    dom = spec.render(view)
  } else if (spec.icon) {
    dom = getIcon(spec.icon)
    if (active) { dom.classList.add(prefix + "-active") }
  } else if (spec.label) {
    dom = crel("div", null, translate(view, spec.label))
  } else {
    throw new RangeError("MenuItem without render, icon, or label property")
  }

  if (spec.title) { dom.setAttribute("title", translate(view, spec.title)) }
  if (spec.class) { dom.classList.add(spec.class) }
  if (disabled) { dom.classList.add(prefix + "-disabled") }
  if (spec.css) { dom.style.cssText += spec.css }
  if (!disabled) { dom.addEventListener(spec.execEvent || "mousedown", function (e) {
    e.preventDefault()
    spec.run(view.state, view.props.onAction, view)
  }) }
  return dom
};
exports.MenuItem = MenuItem

function translate(view, text) {
  return view.props.translate ? view.props.translate(text) : text
}

// MenuItemSpec:: interface
// The configuration object passed to the `MenuItem` constructor.
//
//   run:: (EditorState, (Action), EditorView)
//   The function to execute when the menu item is activated.
//
//   select:: ?(EditorState) → bool
//   Optional function that is used to determine whether the item is
//   appropriate at the moment.
//
//   onDeselect:: ?string
//   Determines what happens when [`select`](#menu.MenuItemSpec.select)
//   returns false. The default is to hide the item, you can set this to
//   `"disable"` to instead render the item with a disabled style.
//
//   active:: ?(EditorState) → bool
//   A predicate function to determine whether the item is 'active' (for
//   example, the item for toggling the strong mark might be active then
//   the cursor is in strong text).
//
//   render:: ?(EditorView) → dom.Node
//   A function that renders the item. You must provide either this,
//   [`icon`](#menu.MenuItemSpec.icon), or [`label`](#MenuItemSpec.label).
//
//   icon:: ?Object
//   Describes an icon to show for this item. The object may specify
//   an SVG icon, in which case its `path` property should be an [SVG
//   path
//   spec](https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/d),
//   and `width` and `height` should provide the viewbox in which that
//   path exists. Alternatively, it may have a `text` property
//   specifying a string of text that makes up the icon, with an
//   optional `css` property giving additional CSS styling for the
//   text. _Or_ it may contain `dom` property containing a DOM node.
//
//   label:: ?string
//   Makes the item show up as a text label. Mostly useful for items
//   wrapped in a [drop-down](#menu.Dropdown) or similar menu. The object
//   should have a `label` property providing the text to display.
//
//   title:: ?string
//   Defines DOM title (mouseover) text for the item.
//
//   class:: string
//   Optionally adds a CSS class to the item's DOM representation.
//
//   css:: string
//   Optionally adds a string of inline CSS to the item's DOM
//   representation.
//
//   execEvent:: string
//   Defines which event on the command's DOM representation should
//   trigger the execution of the command. Defaults to mousedown.

var lastMenuEvent = {time: 0, node: null}
function markMenuEvent(e) {
  lastMenuEvent.time = Date.now()
  lastMenuEvent.node = e.target
}
function isMenuEvent(wrapper) {
  return Date.now() - 100 < lastMenuEvent.time &&
    lastMenuEvent.node && wrapper.contains(lastMenuEvent.node)
}

// ::- A drop-down menu, displayed as a label with a downwards-pointing
// triangle to the right of it.
var Dropdown = function Dropdown(content, options) {
  this.options = options || {}
  this.content = Array.isArray(content) ? content : [content]
};

// :: (EditorView) → dom.Node
// Returns a node showing the collapsed menu, which expands when clicked.
Dropdown.prototype.render = function render (view) {
    var this$1 = this;

  var items = renderDropdownItems(this.content, view)
  if (!items.length) { return null }

  var label = crel("div", {class: prefix + "-dropdown " + (this.options.class || ""),
                           style: this.options.css,
                           title: this.options.title && translate(view, this.options.title)},
                  translate(view, this.options.label))
  var wrap = crel("div", {class: prefix + "-dropdown-wrap"}, label)
  var open = null, listeningOnClose = null
  var close = function () {
    if (open && open.close()) {
      open = null
      window.removeEventListener("mousedown", listeningOnClose)
    }
  }
  label.addEventListener("mousedown", function (e) {
    e.preventDefault()
    markMenuEvent(e)
    if (open) {
      close()
    } else {
      open = this$1.expand(wrap, items)
      window.addEventListener("mousedown", listeningOnClose = function () {
        if (!isMenuEvent(wrap)) { close() }
      })
    }
  })
  return wrap
};

Dropdown.prototype.expand = function expand (dom, items) {
  var menuDOM = crel("div", {class: prefix + "-dropdown-menu " + (this.options.class || "")}, items)

  var done = false
  function close() {
    if (done) { return }
    done = true
    dom.removeChild(menuDOM)
    return true
  }
  dom.appendChild(menuDOM)
  return {close: close, node: menuDOM}
};
exports.Dropdown = Dropdown

function renderDropdownItems(items, view) {
  var rendered = []
  for (var i = 0; i < items.length; i++) {
    var inner = items[i].render(view)
    if (inner) { rendered.push(crel("div", {class: prefix + "-dropdown-item"}, inner)) }
  }
  return rendered
}

// ::- Represents a submenu wrapping a group of elements that start
// hidden and expand to the right when hovered over or tapped.
var DropdownSubmenu = function DropdownSubmenu(content, options) {
  this.options = options || {}
  this.content = Array.isArray(content) ? content : [content]
};

// :: (EditorView) → dom.Node
// Renders the submenu.
DropdownSubmenu.prototype.render = function render (view) {
  var items = renderDropdownItems(this.content, view)
  if (!items.length) { return null }

  var label = crel("div", {class: prefix + "-submenu-label"}, translate(view, this.options.label))
  var wrap = crel("div", {class: prefix + "-submenu-wrap"}, label,
                 crel("div", {class: prefix + "-submenu"}, items))
  var listeningOnClose = null
  label.addEventListener("mousedown", function (e) {
    e.preventDefault()
    markMenuEvent(e)
    wrap.classList.toggle(prefix + "-submenu-wrap-active")
    if (!listeningOnClose)
      { window.addEventListener("mousedown", listeningOnClose = function () {
        if (!isMenuEvent(wrap)) {
          wrap.classList.remove(prefix + "-submenu-wrap-active")
          window.removeEventListener("mousedown", listeningOnClose)
          listeningOnClose = null
        }
      }) }
  })
  return wrap
};
exports.DropdownSubmenu = DropdownSubmenu

// :: (EditorView, [union<MenuElement, [MenuElement]>]) → ?dom.DocumentFragment
// Render the given, possibly nested, array of menu elements into a
// document fragment, placing separators between them (and ensuring no
// superfluous separators appear when some of the groups turn out to
// be empty).
function renderGrouped(view, content) {
  var result = document.createDocumentFragment(), needSep = false
  for (var i = 0; i < content.length; i++) {
    var items = content[i], added = false
    for (var j = 0; j < items.length; j++) {
      var rendered = items[j].render(view)
      if (rendered) {
        if (!added && needSep) { result.appendChild(separator()) }
        result.appendChild(crel("span", {class: prefix + "item"}, rendered))
        added = true
      }
    }
    if (added) { needSep = true }
  }
  return result
}
exports.renderGrouped = renderGrouped

function separator() {
  return crel("span", {class: prefix + "separator"})
}

// :: Object
// A set of basic editor-related icons. Contains the properties
// `join`, `lift`, `selectParentNode`, `undo`, `redo`, `strong`, `em`,
// `code`, `link`, `bulletList`, `orderedList`, and `blockquote`, each
// holding an object that can be used as the `icon` option to
// `MenuItem`.
var icons = {
  join: {
    width: 800, height: 900,
    path: "M0 75h800v125h-800z M0 825h800v-125h-800z M250 400h100v-100h100v100h100v100h-100v100h-100v-100h-100z"
  },
  lift: {
    width: 1024, height: 1024,
    path: "M219 310v329q0 7-5 12t-12 5q-8 0-13-5l-164-164q-5-5-5-13t5-13l164-164q5-5 13-5 7 0 12 5t5 12zM1024 749v109q0 7-5 12t-12 5h-987q-7 0-12-5t-5-12v-109q0-7 5-12t12-5h987q7 0 12 5t5 12zM1024 530v109q0 7-5 12t-12 5h-621q-7 0-12-5t-5-12v-109q0-7 5-12t12-5h621q7 0 12 5t5 12zM1024 310v109q0 7-5 12t-12 5h-621q-7 0-12-5t-5-12v-109q0-7 5-12t12-5h621q7 0 12 5t5 12zM1024 91v109q0 7-5 12t-12 5h-987q-7 0-12-5t-5-12v-109q0-7 5-12t12-5h987q7 0 12 5t5 12z"
  },
  selectParentNode: {text: "\u2b1a", css: "font-weight: bold"},
  undo: {
    width: 1024, height: 1024,
    path: "M761 1024c113-206 132-520-313-509v253l-384-384 384-384v248c534-13 594 472 313 775z"
  },
  redo: {
    width: 1024, height: 1024,
    path: "M576 248v-248l384 384-384 384v-253c-446-10-427 303-313 509-280-303-221-789 313-775z"
  },
  strong: {
    width: 805, height: 1024,
    path: "M317 869q42 18 80 18 214 0 214-191 0-65-23-102-15-25-35-42t-38-26-46-14-48-6-54-1q-41 0-57 5 0 30-0 90t-0 90q0 4-0 38t-0 55 2 47 6 38zM309 442q24 4 62 4 46 0 81-7t62-25 42-51 14-81q0-40-16-70t-45-46-61-24-70-8q-28 0-74 7 0 28 2 86t2 86q0 15-0 45t-0 45q0 26 0 39zM0 950l1-53q8-2 48-9t60-15q4-6 7-15t4-19 3-18 1-21 0-19v-37q0-561-12-585-2-4-12-8t-25-6-28-4-27-2-17-1l-2-47q56-1 194-6t213-5q13 0 39 0t38 0q40 0 78 7t73 24 61 40 42 59 16 78q0 29-9 54t-22 41-36 32-41 25-48 22q88 20 146 76t58 141q0 57-20 102t-53 74-78 48-93 27-100 8q-25 0-75-1t-75-1q-60 0-175 6t-132 6z"
  },
  em: {
    width: 585, height: 1024,
    path: "M0 949l9-48q3-1 46-12t63-21q16-20 23-57 0-4 35-165t65-310 29-169v-14q-13-7-31-10t-39-4-33-3l10-58q18 1 68 3t85 4 68 1q27 0 56-1t69-4 56-3q-2 22-10 50-17 5-58 16t-62 19q-4 10-8 24t-5 22-4 26-3 24q-15 84-50 239t-44 203q-1 5-7 33t-11 51-9 47-3 32l0 10q9 2 105 17-1 25-9 56-6 0-18 0t-18 0q-16 0-49-5t-49-5q-78-1-117-1-29 0-81 5t-69 6z"
  },
  code: {
    width: 896, height: 1024,
    path: "M608 192l-96 96 224 224-224 224 96 96 288-320-288-320zM288 192l-288 320 288 320 96-96-224-224 224-224-96-96z"
  },
  link: {
    width: 951, height: 1024,
    path: "M832 694q0-22-16-38l-118-118q-16-16-38-16-24 0-41 18 1 1 10 10t12 12 8 10 7 14 2 15q0 22-16 38t-38 16q-8 0-15-2t-14-7-10-8-12-12-10-10q-18 17-18 41 0 22 16 38l117 118q15 15 38 15 22 0 38-14l84-83q16-16 16-38zM430 292q0-22-16-38l-117-118q-16-16-38-16-22 0-38 15l-84 83q-16 16-16 38 0 22 16 38l118 118q15 15 38 15 24 0 41-17-1-1-10-10t-12-12-8-10-7-14-2-15q0-22 16-38t38-16q8 0 15 2t14 7 10 8 12 12 10 10q18-17 18-41zM941 694q0 68-48 116l-84 83q-47 47-116 47-69 0-116-48l-117-118q-47-47-47-116 0-70 50-119l-50-50q-49 50-118 50-68 0-116-48l-118-118q-48-48-48-116t48-116l84-83q47-47 116-47 69 0 116 48l117 118q47 47 47 116 0 70-50 119l50 50q49-50 118-50 68 0 116 48l118 118q48 48 48 116z"
  },
  bulletList: {
    width: 768, height: 896,
    path: "M0 512h128v-128h-128v128zM0 256h128v-128h-128v128zM0 768h128v-128h-128v128zM256 512h512v-128h-512v128zM256 256h512v-128h-512v128zM256 768h512v-128h-512v128z"
  },
  orderedList: {
    width: 768, height: 896,
    path: "M320 512h448v-128h-448v128zM320 768h448v-128h-448v128zM320 128v128h448v-128h-448zM79 384h78v-256h-36l-85 23v50l43-2v185zM189 590c0-36-12-78-96-78-33 0-64 6-83 16l1 66c21-10 42-15 67-15s32 11 32 28c0 26-30 58-110 112v50h192v-67l-91 2c49-30 87-66 87-113l1-1z"
  },
  blockquote: {
    width: 640, height: 896,
    path: "M0 448v256h256v-256h-128c0 0 0-128 128-128v-128c0 0-256 0-256 256zM640 320v-128c0 0-256 0-256 256v256h256v-256h-128c0 0 0-128 128-128z"
  }
}
exports.icons = icons

// :: MenuItem
// Menu item for the `joinUp` command.
var joinUpItem = new MenuItem({
  title: "Join with above block",
  run: joinUp,
  select: function (state) { return joinUp(state); },
  icon: icons.join
})
exports.joinUpItem = joinUpItem

// :: MenuItem
// Menu item for the `lift` command.
var liftItem = new MenuItem({
  title: "Lift out of enclosing block",
  run: lift,
  select: function (state) { return lift(state); },
  icon: icons.lift
})
exports.liftItem = liftItem

// :: MenuItem
// Menu item for the `selectParentNode` command.
var selectParentNodeItem = new MenuItem({
  title: "Select parent node",
  run: selectParentNode,
  select: function (state) { return selectParentNode(state); },
  icon: icons.selectParentNode
})
exports.selectParentNodeItem = selectParentNodeItem

// :: (Object) → MenuItem
// Menu item for the `undo` command.
var undoItem = new MenuItem({
  title: "Undo last change",
  run: undo,
  select: function (state) { return undo(state); },
  icon: icons.undo
})
exports.undoItem = undoItem

// :: (Object) → MenuItem
// Menu item for the `redo` command.
var redoItem = new MenuItem({
  title: "Redo last undone change",
  run: redo,
  select: function (state) { return redo(state); },
  icon: icons.redo
})
exports.redoItem = redoItem

// :: (NodeType, Object) → MenuItem
// Build a menu item for wrapping the selection in a given node type.
// Adds `run` and `select` properties to the ones present in
// `options`. `options.attrs` may be an object or a function, as in
// `toggleMarkItem`.
function wrapItem(nodeType, options) {
  var passedOptions = {
    run: function run(state, onAction) {
      // FIXME if (options.attrs instanceof Function) options.attrs(state, attrs => wrapIn(nodeType, attrs)(state))
      return wrapIn(nodeType, options.attrs)(state, onAction)
    },
    select: function select(state) {
      return wrapIn(nodeType, options.attrs instanceof Function ? null : options.attrs)(state)
    }
  }
  for (var prop in options) { passedOptions[prop] = options[prop] }
  return new MenuItem(passedOptions)
}
exports.wrapItem = wrapItem

// :: (NodeType, Object) → MenuItem
// Build a menu item for changing the type of the textblock around the
// selection to the given type. Provides `run`, `active`, and `select`
// properties. Others must be given in `options`. `options.attrs` may
// be an object to provide the attributes for the textblock node.
function blockTypeItem(nodeType, options) {
  var command = setBlockType(nodeType, options.attrs)
  var passedOptions = {
    run: command,
    select: function select(state) { return command(state) },
    active: function active(state) {
      var ref = state.selection;
      var $from = ref.$from;
      var to = ref.to;
      var node = ref.node;
      if (node) { return node.hasMarkup(nodeType, options.attrs) }
      return to <= $from.end() && $from.parent.hasMarkup(nodeType, options.attrs)
    }
  }
  for (var prop in options) { passedOptions[prop] = options[prop] }
  return new MenuItem(passedOptions)
}
exports.blockTypeItem = blockTypeItem

},{"./icons":17,"crel":2,"prosemirror-commands":5,"prosemirror-history":11}],20:[function(require,module,exports){
var crel = require("crel")
var ref = require("prosemirror-view");
var EditorView = ref.EditorView;

var ref$1 = require("./menu");
var renderGrouped = ref$1.renderGrouped;

var prefix = "ProseMirror-menubar"

// ::- A wrapper around
// [`EditorView`](http://prosemirror.net/ref.html#view.EditorView)
// that adds a menubar above the editor.
//
// Supports the following additional props:
//
// - **`floatingMenu`**`: ?bool` determines whether the menu floats,
//   i.e. whether it sticks to the top of the viewport when the editor
//   is partially scrolled out of view.
//
// - **`menuContent`**`: [[MenuElement]]` provides the content of the
//   menu, as a nested array to be passed to `renderGrouped`.
var MenuBarEditorView = function MenuBarEditorView(place, props) {
  var this$1 = this;

  // :: dom.Node The wrapping DOM element around the editor and the
  // menu. Will get the CSS class `ProseMirror-menubar-wrapper`.
  this.wrapper = crel("div", {class: prefix + "-wrapper"})
  if (place.appendChild) { place.appendChild(this.wrapper) }
  else if (place) { place(this.wrapper) }
  // :: EditorView The wrapped editor view. _Don't_ directly call
  // `update` or `updateState` on this, always go through the
  // wrapping view.
  this.editor = new EditorView(this.wrapper, props)

  this.menu = crel("div", {class: prefix})
  this.menu.className = prefix
  this.spacer = null

  this.wrapper.insertBefore(this.menu, this.wrapper.firstChild)

  this.maxHeight = 0
  this.widthForMaxHeight = 0
  this.floating = false

  // :: EditorProps The current props of this view.
  this.props = props
  this.updateMenu()

  if (this.editor.someProp("floatingMenu")) {
    this.updateFloat()
    this.scrollFunc = function () {
      if (!this$1.editor.root.contains(this$1.wrapper))
        { window.removeEventListener("scroll", this$1.scrollFunc) }
      else
        { this$1.updateFloat() }
    }
    window.addEventListener("scroll", this.scrollFunc)
  }
};

// :: (EditorProps) Update the view's props.
MenuBarEditorView.prototype.update = function update (props) {
  this.props = props
  this.editor.update(props)
  this.updateMenu()
};

// :: (EditorState) Update only the state of the editor.
MenuBarEditorView.prototype.updateState = function updateState (state) {
  this.editor.updateState(state)
  this.updateMenu()
};

MenuBarEditorView.prototype.updateMenu = function updateMenu () {
  this.menu.textContent = ""
  this.menu.appendChild(renderGrouped(this.editor, this.editor.someProp("menuContent")))

  if (this.floating) {
    this.updateScrollCursor()
  } else {
    if (this.menu.offsetWidth != this.widthForMaxHeight) {
      this.widthForMaxHeight = this.menu.offsetWidth
      this.maxHeight = 0
    }
    if (this.menu.offsetHeight > this.maxHeight) {
      this.maxHeight = this.menu.offsetHeight
      this.menu.style.minHeight = this.maxHeight + "px"
    }
  }
};


MenuBarEditorView.prototype.updateScrollCursor = function updateScrollCursor () {
  var selection = this.editor.root.getSelection()
  if (!selection.focusNode) { return }
  var rects = selection.getRangeAt(0).getClientRects()
  var selRect = rects[selectionIsInverted(selection) ? 0 : rects.length - 1]
  if (!selRect) { return }
  var menuRect = this.menu.getBoundingClientRect()
  if (selRect.top < menuRect.bottom && selRect.bottom > menuRect.top) {
    var scrollable = findWrappingScrollable(this.wrapper)
    if (scrollable) { scrollable.scrollTop -= (menuRect.bottom - selRect.top) }
  }
};

MenuBarEditorView.prototype.updateFloat = function updateFloat () {
  var parent = this.wrapper, editorRect = parent.getBoundingClientRect()
  if (this.floating) {
    if (editorRect.top >= 0 || editorRect.bottom < this.menu.offsetHeight + 10) {
      this.floating = false
      this.menu.style.position = this.menu.style.left = this.menu.style.width = ""
      this.menu.style.display = ""
      this.spacer.parentNode.removeChild(this.spacer)
      this.spacer = null
    } else {
      var border = (parent.offsetWidth - parent.clientWidth) / 2
      this.menu.style.left = (editorRect.left + border) + "px"
      this.menu.style.display = (editorRect.top > window.innerHeight ? "none" : "")
    }
  } else {
    if (editorRect.top < 0 && editorRect.bottom >= this.menu.offsetHeight + 10) {
      this.floating = true
      var menuRect = this.menu.getBoundingClientRect()
      this.menu.style.left = menuRect.left + "px"
      this.menu.style.width = menuRect.width + "px"
      this.menu.style.position = "fixed"
      this.spacer = crel("div", {class: prefix + "-spacer", style: ("height: " + (menuRect.height) + "px")})
      parent.insertBefore(this.spacer, this.menu)
    }
  }
};
exports.MenuBarEditorView = MenuBarEditorView

// Not precise, but close enough
function selectionIsInverted(selection) {
  if (selection.anchorNode == selection.focusNode) { return selection.anchorOffset > selection.focusOffset }
  return selection.anchorNode.compareDocumentPosition(selection.focusNode) == Node.DOCUMENT_POSITION_FOLLOWING
}

function findWrappingScrollable(node) {
  for (var cur = node.parentNode; cur; cur = cur.parentNode)
    { if (cur.scrollHeight > cur.clientHeight) { return cur } }
}

},{"./menu":19,"crel":2,"prosemirror-view":57}],21:[function(require,module,exports){
function compareDeep(a, b) {
  if (a === b) { return true }
  if (!(a && typeof a == "object") ||
      !(b && typeof b == "object")) { return false }
  var array = Array.isArray(a)
  if (Array.isArray(b) != array) { return false }
  if (array) {
    if (a.length != b.length) { return false }
    for (var i = 0; i < a.length; i++) { if (!compareDeep(a[i], b[i])) { return false } }
  } else {
    for (var p in a) { if (!(p in b) || !compareDeep(a[p], b[p])) { return false } }
    for (var p$1 in b) { if (!(p$1 in a)) { return false } }
  }
  return true
}
exports.compareDeep = compareDeep

},{}],22:[function(require,module,exports){
var ref = require("./fragment");
var Fragment = ref.Fragment;
var ref$1 = require("./mark");
var Mark = ref$1.Mark;

var ContentExpr = function ContentExpr(nodeType, elements, inlineContent) {
  this.nodeType = nodeType
  this.elements = elements
  this.inlineContent = inlineContent
};

var prototypeAccessors = { isLeaf: {} };

prototypeAccessors.isLeaf.get = function () {
  return this.elements.length == 0
};

ContentExpr.prototype.start = function start (attrs) {
  return new ContentMatch(this, attrs, 0, 0)
};

ContentExpr.prototype.matches = function matches (attrs, fragment, from, to) {
  return this.start(attrs).matchToEnd(fragment, from, to)
};

// Get a position in a known-valid fragment. If this is a simple
// (single-element) expression, we don't have to do any matching,
// and can simply skip to the position with count `index`.
ContentExpr.prototype.getMatchAt = function getMatchAt (attrs, fragment, index) {
    if ( index === void 0 ) index = fragment.childCount;

  if (this.elements.length == 1)
    { return new ContentMatch(this, attrs, 0, index) }
  else
    { return this.start(attrs).matchFragment(fragment, 0, index) }
};

ContentExpr.prototype.checkReplace = function checkReplace (attrs, content, from, to, replacement, start, end) {
    var this$1 = this;
    if ( replacement === void 0 ) replacement = Fragment.empty;
    if ( start === void 0 ) start = 0;
    if ( end === void 0 ) end = replacement.childCount;

  // Check for simple case, where the expression only has a single element
  // (Optimization to avoid matching more than we need)
  if (this.elements.length == 1) {
    var elt = this.elements[0]
    if (!checkCount(elt, content.childCount - (to - from) + (end - start), attrs, this)) { return false }
    for (var i = start; i < end; i++) { if (!elt.matches(replacement.child(i), attrs, this$1)) { return false } }
    return true
  }

  var match = this.getMatchAt(attrs, content, from).matchFragment(replacement, start, end)
  return match ? match.matchToEnd(content, to) : false
};

ContentExpr.prototype.checkReplaceWith = function checkReplaceWith (attrs, content, from, to, type, typeAttrs, marks) {
  if (this.elements.length == 1) {
    var elt = this.elements[0]
    if (!checkCount(elt, content.childCount - (to - from) + 1, attrs, this)) { return false }
    return elt.matchesType(type, typeAttrs, marks, attrs, this)
  }

  var match = this.getMatchAt(attrs, content, from).matchType(type, typeAttrs, marks)
  return match ? match.matchToEnd(content, to) : false
};

ContentExpr.prototype.compatible = function compatible (other) {
    var this$1 = this;

  for (var i = 0; i < this.elements.length; i++) {
    var elt = this$1.elements[i]
    for (var j = 0; j < other.elements.length; j++)
      { if (other.elements[j].compatible(elt)) { return true } }
  }
  return false
};

ContentExpr.prototype.generateContent = function generateContent (attrs) {
  return this.start(attrs).fillBefore(Fragment.empty, true)
};

ContentExpr.parse = function parse (nodeType, expr, specs) {
  var elements = [], pos = 0, inline = null
  for (;;) {
    pos += /^\s*/.exec(expr.slice(pos))[0].length
    if (pos == expr.length) { break }

    var types = /^(?:(\w+)|\(\s*(\w+(?:\s*\|\s*\w+)*)\s*\))/.exec(expr.slice(pos))
    if (!types) { throw new SyntaxError("Invalid content expression '" + expr + "' at " + pos) }
    pos += types[0].length
    var attrs = /^\[([^\]]+)\]/.exec(expr.slice(pos))
    if (attrs) { pos += attrs[0].length }
    var marks = /^<(?:(_)|\s*(\w+(?:\s+\w+)*)\s*)>/.exec(expr.slice(pos))
    if (marks) { pos += marks[0].length }
    var repeat = /^(?:([+*?])|\{\s*(\d+|\.\w+)\s*(,\s*(\d+|\.\w+)?)?\s*\})/.exec(expr.slice(pos))
    if (repeat) { pos += repeat[0].length }

    var nodeTypes = expandTypes(nodeType.schema, specs, types[1] ? [types[1]] : types[2].split(/\s*\|\s*/))
    for (var i = 0; i < nodeTypes.length; i++) {
      if (inline == null) { inline = nodeTypes[i].isInline }
      else if (inline != nodeTypes[i].isInline) { throw new SyntaxError("Mixing inline and block content in a single node") }
    }
    var attrSet = !attrs ? null : parseAttrs(nodeType, attrs[1])
    var markSet = !marks ? false : marks[1] ? true : checkMarks(nodeType.schema, marks[2].split(/\s+/))
    var ref = parseRepeat(nodeType, repeat);
      var min = ref.min;
      var max = ref.max;
    if (min != 0 && nodeTypes[0].hasRequiredAttrs(attrSet))
      { throw new SyntaxError("Node type " + types[0] + " in type " + nodeType.name +
                            " is required, but has non-optional attributes") }
    var newElt = new ContentElement(nodeTypes, attrSet, markSet, min, max)
    for (var i$1 = elements.length - 1; i$1 >= 0; i$1--) {
      var prev = elements[i$1]
      if (prev.min != prev.max && prev.overlaps(newElt))
        { throw new SyntaxError("Possibly ambiguous overlapping adjacent content expressions in '" + expr + "'") }
      if (prev.min != 0) { break }
    }
    elements.push(newElt)
  }

  return new ContentExpr(nodeType, elements, !!inline)
};

Object.defineProperties( ContentExpr.prototype, prototypeAccessors );
exports.ContentExpr = ContentExpr

var ContentElement = function ContentElement(nodeTypes, attrs, marks, min, max) {
  this.nodeTypes = nodeTypes
  this.attrs = attrs
  this.marks = marks
  this.min = min
  this.max = max
};

ContentElement.prototype.matchesType = function matchesType (type, attrs, marks, parentAttrs, parentExpr) {
    var this$1 = this;

  if (this.nodeTypes.indexOf(type) == -1) { return false }
  if (this.attrs) {
    if (!attrs) { return false }
    for (var prop in this.attrs)
      { if (attrs[prop] != resolveValue(this$1.attrs[prop], parentAttrs, parentExpr)) { return false } }
  }
  if (this.marks === true) { return true }
  if (this.marks === false) { return marks.length == 0 }
  for (var i = 0; i < marks.length; i++)
    { if (this$1.marks.indexOf(marks[i].type) == -1) { return false } }
  return true
};

ContentElement.prototype.matches = function matches (node, parentAttrs, parentExpr) {
  return this.matchesType(node.type, node.attrs, node.marks, parentAttrs, parentExpr)
};

ContentElement.prototype.compatible = function compatible (other) {
    var this$1 = this;

  for (var i = 0; i < this.nodeTypes.length; i++)
    { if (other.nodeTypes.indexOf(this$1.nodeTypes[i]) != -1) { return true } }
  return false
};

ContentElement.prototype.constrainedAttrs = function constrainedAttrs (parentAttrs, expr) {
    var this$1 = this;

  if (!this.attrs) { return null }
  var attrs = Object.create(null)
  for (var prop in this.attrs)
    { attrs[prop] = resolveValue(this$1.attrs[prop], parentAttrs, expr) }
  return attrs
};

ContentElement.prototype.createFiller = function createFiller (parentAttrs, expr) {
  var type = this.nodeTypes[0], attrs = type.computeAttrs(this.constrainedAttrs(parentAttrs, expr))
  return type.create(attrs, type.contentExpr.generateContent(attrs))
};

ContentElement.prototype.defaultType = function defaultType () {
  return this.nodeTypes[0].defaultAttrs && this.nodeTypes[0]
};

ContentElement.prototype.overlaps = function overlaps (other) {
  return this.nodeTypes.some(function (t) { return other.nodeTypes.indexOf(t) > -1; })
};

ContentElement.prototype.allowsMark = function allowsMark (markType) {
  return this.marks === true || this.marks && this.marks.indexOf(markType) > -1
};

// ::- Represents a partial match of a node type's [content
// expression](#model.NodeSpec), and can be used to find out whether further
// content matches here, and whether a given position is a valid end
// of the parent node.
var ContentMatch = function ContentMatch(expr, attrs, index, count) {
  this.expr = expr
  this.attrs = attrs
  this.index = index
  this.count = count
};

var prototypeAccessors$1 = { element: {},nextElement: {} };

prototypeAccessors$1.element.get = function () { return this.expr.elements[this.index] };

prototypeAccessors$1.nextElement.get = function () {
    var this$1 = this;

  for (var i = this.index, count = this.count; i < this.expr.elements.length; i++) {
    var element = this$1.expr.elements[i]
    if (this$1.resolveValue(element.max) > count) { return element }
    count = 0
  }
};

ContentMatch.prototype.move = function move (index, count) {
  return new ContentMatch(this.expr, this.attrs, index, count)
};

ContentMatch.prototype.resolveValue = function resolveValue$1 (value) {
  return value instanceof AttrValue ? resolveValue(value, this.attrs, this.expr) : value
};

// :: (Node) → ?ContentMatch
// Match a node, returning a new match after the node if successful.
ContentMatch.prototype.matchNode = function matchNode (node) {
  return this.matchType(node.type, node.attrs, node.marks)
};

// :: (NodeType, ?Object, [Mark]) → ?ContentMatch
// Match a node type and marks, returning an match after that node
// if successful.
ContentMatch.prototype.matchType = function matchType (type, attrs, marks) {
    var this$1 = this;
    if ( marks === void 0 ) marks = Mark.none;

  // FIXME `var` to work around Babel bug T7293
  for (var ref = this, index = ref.index, count = ref.count; index < this.expr.elements.length; index++, count = 0) {
    var elt = this$1.expr.elements[index], max = this$1.resolveValue(elt.max)
    if (count < max && elt.matchesType(type, attrs, marks, this$1.attrs, this$1.expr)) {
      count++
      return this$1.move(index, count)
    }
    if (count < this$1.resolveValue(elt.min)) { return null }
  }
};

// :: (Fragment, ?number, ?number) → ?union<ContentMatch, bool>
// Try to match a fragment. Returns a new match when successful,
// `null` when it ran into a required element it couldn't fit, and
// `false` if it reached the end of the expression without
// matching all nodes.
ContentMatch.prototype.matchFragment = function matchFragment (fragment, from, to) {
    var this$1 = this;
    if ( from === void 0 ) from = 0;
    if ( to === void 0 ) to = fragment.childCount;

  if (from == to) { return this }
  var fragPos = from, end = this.expr.elements.length
  for (var ref = this, index = ref.index, count = ref.count; index < end; index++, count = 0) {
    var elt = this$1.expr.elements[index], max = this$1.resolveValue(elt.max)

    while (count < max) {
      if (elt.matches(fragment.child(fragPos), this$1.attrs, this$1.expr)) {
        count++
        if (++fragPos == to) { return this$1.move(index, count) }
      } else {
        break
      }
    }
    if (count < this$1.resolveValue(elt.min)) { return null }
  }
  return false
};

// :: (Fragment, ?number, ?number) → bool
// Returns true only if the fragment matches here, and reaches all
// the way to the end of the content expression.
ContentMatch.prototype.matchToEnd = function matchToEnd (fragment, start, end) {
  var matched = this.matchFragment(fragment, start, end)
  return matched && matched.validEnd() || false
};

// :: () → bool
// Returns true if this position represents a valid end of the
// expression (no required content follows after it).
ContentMatch.prototype.validEnd = function validEnd () {
    var this$1 = this;

  for (var i = this.index, count = this.count; i < this.expr.elements.length; i++, count = 0)
    { if (count < this$1.resolveValue(this$1.expr.elements[i].min)) { return false } }
  return true
};

// :: (Fragment, bool, ?number) → ?Fragment
// Try to match the given fragment, and if that fails, see if it can
// be made to match by inserting nodes in front of it. When
// successful, return a fragment of inserted nodes (which may be
// empty if nothing had to be inserted). When `toEnd` is true, only
// return a fragment if the resulting match goes to the end of the
// content expression.
ContentMatch.prototype.fillBefore = function fillBefore (after, toEnd, startIndex) {
    var this$1 = this;

  var added = [], match = this, index = startIndex || 0, end = this.expr.elements.length
  for (;;) {
    var fits = match.matchFragment(after, index)
    if (fits && (!toEnd || fits.validEnd())) { return Fragment.from(added) }
    if (fits === false) { return null } // Matched to end with content remaining

    var elt = match.element
    if (match.count < this$1.resolveValue(elt.min)) {
      added.push(elt.createFiller(this$1.attrs, this$1.expr))
      match = match.move(match.index, match.count + 1)
    } else if (match.index < end) {
      match = match.move(match.index + 1, 0)
    } else if (after.childCount > index) {
      return null
    } else {
      return Fragment.from(added)
    }
  }
};

ContentMatch.prototype.possibleContent = function possibleContent () {
    var this$1 = this;

  var found = []
  for (var i = this.index, count = this.count; i < this.expr.elements.length; i++, count = 0) {
    var elt = this$1.expr.elements[i], attrs = elt.constrainedAttrs(this$1.attrs, this$1.expr)
    if (count < this$1.resolveValue(elt.max)) { for (var j = 0; j < elt.nodeTypes.length; j++) {
      var type = elt.nodeTypes[j]
      if (!type.hasRequiredAttrs(attrs)) { found.push({type: type, attrs: attrs}) }
    } }
    if (this$1.resolveValue(elt.min) > count) { break }
  }
  return found
};

// :: (MarkType) → bool
// Check whether a node with the given mark type is allowed after
// this position.
ContentMatch.prototype.allowsMark = function allowsMark (markType) {
  return this.element.allowsMark(markType)
};

// :: (NodeType, ?Object) → ?[{type: NodeType, attrs: Object}]
// Find a set of wrapping node types that would allow a node of type
// `target` with attributes `targetAttrs` to appear at this
// position. The result may be empty (when it fits directly) and
// will be null when no such wrapping exists.
ContentMatch.prototype.findWrapping = function findWrapping (target, targetAttrs) {
  // FIXME find out how expensive this is. Try to reintroduce caching?
  var seen = Object.create(null), first = {match: this, via: null}, active = [first]
  while (active.length) {
    var current = active.shift(), match = current.match
    if (match.matchType(target, targetAttrs)) {
      var result = []
      for (var obj = current; obj != first; obj = obj.via)
        { result.push({type: obj.match.expr.nodeType, attrs: obj.match.attrs}) }
      return result.reverse()
    }
    var possible = match.possibleContent()
    for (var i = 0; i < possible.length; i++) {
      var ref = possible[i];
        var type = ref.type;
        var attrs = ref.attrs;
        var fullAttrs = type.computeAttrs(attrs)
      if (!type.isLeaf && !(type.name in seen) &&
          (current == first || match.matchType(type, fullAttrs).validEnd())) {
        active.push({match: type.contentExpr.start(fullAttrs), via: current})
        seen[type.name] = true
      }
    }
  }
};

Object.defineProperties( ContentMatch.prototype, prototypeAccessors$1 );
exports.ContentMatch = ContentMatch

var AttrValue = function AttrValue(attr) { this.attr = attr };

function parseValue(nodeType, value) {
  if (value.charAt(0) == ".") {
    var attr = value.slice(1)
    if (!nodeType.attrs[attr]) { throw new SyntaxError("Node type " + nodeType.name + " has no attribute " + attr) }
    return new AttrValue(attr)
  } else {
    return JSON.parse(value)
  }
}

function checkMarks(schema, marks) {
  var found = []
  for (var i = 0; i < marks.length; i++) {
    var mark = schema.marks[marks[i]]
    if (mark) { found.push(mark) }
    else { throw new SyntaxError("Unknown mark type: '" + marks[i] + "'") }
  }
  return found
}

function resolveValue(value, attrs, expr) {
  if (!(value instanceof AttrValue)) { return value }
  var attrVal = attrs && attrs[value.attr]
  return attrVal !== undefined ? attrVal : expr.nodeType.defaultAttrs[value.attr]
}

function checkCount(elt, count, attrs, expr) {
  return count >= resolveValue(elt.min, attrs, expr) &&
    count <= resolveValue(elt.max, attrs, expr)
}

function expandTypes(schema, specs, types) {
  var result = []
  types.forEach(function (type) {
    var found = schema.nodes[type]
    if (found) {
      if (result.indexOf(found) == -1) { result.push(found) }
    } else {
      specs.forEach(function (name, spec) {
        if (spec.group && spec.group.split(" ").indexOf(type) > -1) {
          found = schema.nodes[name]
          if (result.indexOf(found) == -1) { result.push(found) }
        }
      })
    }
    if (!found)
      { throw new SyntaxError("Node type or group '" + type + "' does not exist") }
  })
  return result
}

var many = 2e9 // Big number representable as a 32-bit int

function parseRepeat(nodeType, match) {
  var min = 1, max = 1
  if (match) {
    if (match[1] == "+") {
      max = many
    } else if (match[1] == "*") {
      min = 0
      max = many
    } else if (match[1] == "?") {
      min = 0
    } else if (match[2]) {
      min = parseValue(nodeType, match[2])
      if (match[3])
        { max = match[4] ? parseValue(nodeType, match[4]) : many }
      else
        { max = min }
    }
    if (max == 0 || min > max)
      { throw new SyntaxError("Invalid repeat count in '" + match[0] + "'") }
  }
  return {min: min, max: max}
}

function parseAttrs(nodeType, expr) {
  var parts = expr.split(/\s*,\s*/)
  var attrs = Object.create(null)
  for (var i = 0; i < parts.length; i++) {
    var match = /^(\w+)=(\w+|\"(?:\\.|[^\\])*\"|\.\w+)$/.exec(parts[i])
    if (!match) { throw new SyntaxError("Invalid attribute syntax: " + parts[i]) }
    attrs[match[1]] = parseValue(nodeType, match[2])
  }
  return attrs
}

},{"./fragment":24,"./mark":27}],23:[function(require,module,exports){
function findDiffStart(a, b, pos) {
  for (var i = 0;; i++) {
    if (i == a.childCount || i == b.childCount)
      { return a.childCount == b.childCount ? null : pos }

    var childA = a.child(i), childB = b.child(i)
    if (childA == childB) { pos += childA.nodeSize; continue }

    if (!childA.sameMarkup(childB)) { return pos }

    if (childA.isText && childA.text != childB.text) {
      for (var j = 0; childA.text[j] == childB.text[j]; j++)
        { pos++ }
      return pos
    }
    if (childA.content.size || childB.content.size) {
      var inner = findDiffStart(childA.content, childB.content, pos + 1)
      if (inner != null) { return inner }
    }
    pos += childA.nodeSize
  }
}
exports.findDiffStart = findDiffStart

function findDiffEnd(a, b, posA, posB) {
  for (var iA = a.childCount, iB = b.childCount;;) {
    if (iA == 0 || iB == 0)
      { return iA == iB ? null : {a: posA, b: posB} }

    var childA = a.child(--iA), childB = b.child(--iB), size = childA.nodeSize
    if (childA == childB) {
      posA -= size; posB -= size
      continue
    }

    if (!childA.sameMarkup(childB)) { return {a: posA, b: posB} }

    if (childA.isText && childA.text != childB.text) {
      var same = 0, minSize = Math.min(childA.text.length, childB.text.length)
      while (same < minSize && childA.text[childA.text.length - same - 1] == childB.text[childB.text.length - same - 1]) {
        same++; posA--; posB--
      }
      return {a: posA, b: posB}
    }
    if (childA.content.size || childB.content.size) {
      var inner = findDiffEnd(childA.content, childB.content, posA - 1, posB - 1)
      if (inner) { return inner }
    }
    posA -= size; posB -= size
  }
}
exports.findDiffEnd = findDiffEnd

},{}],24:[function(require,module,exports){
var ref = require("./diff");
var findDiffStart = ref.findDiffStart;
var findDiffEnd = ref.findDiffEnd;

// ::- Fragment is the type used to represent a node's collection of
// child nodes.
//
// Fragments are persistent data structures. That means you should
// _not_ mutate them or their content, but create new instances
// whenever needed. The API tries to make this easy.
var Fragment = function Fragment(content, size) {
  var this$1 = this;

  this.content = content
  this.size = size || 0
  if (size == null) { for (var i = 0; i < content.length; i++)
    { this$1.size += content[i].nodeSize } }
};

var prototypeAccessors = { firstChild: {},lastChild: {},childCount: {} };

Fragment.prototype.nodesBetween = function nodesBetween (from, to, f, nodeStart, parent) {
    var this$1 = this;

  for (var i = 0, pos = 0; pos < to; i++) {
    var child = this$1.content[i], end = pos + child.nodeSize
    if (end > from && f(child, nodeStart + pos, parent, i) !== false && child.content.size) {
      var start = pos + 1
      child.nodesBetween(Math.max(0, from - start),
                         Math.min(child.content.size, to - start),
                         f, nodeStart + start)
    }
    pos = end
  }
};

// : (number, number, ?string, ?string) → string
Fragment.prototype.textBetween = function textBetween (from, to, blockSeparator, leafText) {
  var text = "", separated = true
  this.nodesBetween(from, to, function (node, pos) {
    if (node.isText) {
      text += node.text.slice(Math.max(from, pos) - pos, to - pos)
      separated = !blockSeparator
    } else if (node.isLeaf && leafText) {
      text += leafText
      separated = !blockSeparator
    } else if (!separated && node.isBlock) {
      text += blockSeparator
      separated = true
    }
  }, 0)
  return text
};

// :: (Fragment) → Fragment
// Create a new fragment containing the content of this fragment and
// `other`.
Fragment.prototype.append = function append (other) {
  if (!other.size) { return this }
  if (!this.size) { return other }
  var last = this.lastChild, first = other.firstChild, content = this.content.slice(), i = 0
  if (last.isText && last.sameMarkup(first)) {
    content[content.length - 1] = last.withText(last.text + first.text)
    i = 1
  }
  for (; i < other.content.length; i++) { content.push(other.content[i]) }
  return new Fragment(content, this.size + other.size)
};

// :: (number, ?number) → Fragment
// Cut out the sub-fragment between the two given positions.
Fragment.prototype.cut = function cut (from, to) {
    var this$1 = this;

  if (to == null) { to = this.size }
  if (from == 0 && to == this.size) { return this }
  var result = [], size = 0
  if (to > from) { for (var i = 0, pos = 0; pos < to; i++) {
    var child = this$1.content[i], end = pos + child.nodeSize
    if (end > from) {
      if (pos < from || end > to) {
        if (child.isText)
          { child = child.cut(Math.max(0, from - pos), Math.min(child.text.length, to - pos)) }
        else
          { child = child.cut(Math.max(0, from - pos - 1), Math.min(child.content.size, to - pos - 1)) }
      }
      result.push(child)
      size += child.nodeSize
    }
    pos = end
  } }
  return new Fragment(result, size)
};

Fragment.prototype.cutByIndex = function cutByIndex (from, to) {
  if (from == to) { return Fragment.empty }
  if (from == 0 && to == this.content.length) { return this }
  return new Fragment(this.content.slice(from, to))
};

// :: (number, Node) → Fragment
// Create a new fragment in which the node at the given index is
// replaced by the given node.
Fragment.prototype.replaceChild = function replaceChild (index, node) {
  var current = this.content[index]
  if (current == node) { return this }
  var copy = this.content.slice()
  var size = this.size + node.nodeSize - current.nodeSize
  copy[index] = node
  return new Fragment(copy, size)
};

// (Node) → Fragment
// Create a new fragment by prepending the given node to this
// fragment.
Fragment.prototype.addToStart = function addToStart (node) {
  return new Fragment([node].concat(this.content), this.size + node.nodeSize)
};

// (Node) → Fragment
// Create a new fragment by appending the given node to this
// fragment.
Fragment.prototype.addToEnd = function addToEnd (node) {
  return new Fragment(this.content.concat(node), this.size + node.nodeSize)
};

// :: (Fragment) → bool
// Compare this fragment to another one.
Fragment.prototype.eq = function eq (other) {
    var this$1 = this;

  if (this.content.length != other.content.length) { return false }
  for (var i = 0; i < this.content.length; i++)
    { if (!this$1.content[i].eq(other.content[i])) { return false } }
  return true
};

// :: ?Node
// The first child of the fragment, or `null` if it is empty.
prototypeAccessors.firstChild.get = function () { return this.content.length ? this.content[0] : null };

// :: ?Node
// The last child of the fragment, or `null` if it is empty.
prototypeAccessors.lastChild.get = function () { return this.content.length ? this.content[this.content.length - 1] : null };

// :: number
// The number of child nodes in this fragment.
prototypeAccessors.childCount.get = function () { return this.content.length };

// :: (number) → Node
// Get the child node at the given index. Raise an error when the
// index is out of range.
Fragment.prototype.child = function child (index) {
  var found = this.content[index]
  if (!found) { throw new RangeError("Index " + index + " out of range for " + this) }
  return found
};

// :: (number) → number
// Get the offset at (size of children before) the given index.
Fragment.prototype.offsetAt = function offsetAt (index) {
    var this$1 = this;

  var offset = 0
  for (var i = 0; i < index; i++) { offset += this$1.content[i].nodeSize }
  return offset
};

// :: (number) → ?Node
// Get the child node at the given index, if it exists.
Fragment.prototype.maybeChild = function maybeChild (index) {
  return this.content[index]
};

// :: ((node: Node, offset: number, index: number))
// Call `f` for every child node, passing the node, its offset
// into this parent node, and its index.
Fragment.prototype.forEach = function forEach (f) {
    var this$1 = this;

  for (var i = 0, p = 0; i < this.content.length; i++) {
    var child = this$1.content[i]
    f(child, p, i)
    p += child.nodeSize
  }
};

// :: (Fragment) → ?number
// Find the first position at which this fragment and another
// fragment differ, or `null` if they are the same.
Fragment.prototype.findDiffStart = function findDiffStart$1 (other, pos) {
    if ( pos === void 0 ) pos = 0;

  return findDiffStart(this, other, pos)
};

// :: (Node) → ?{a: number, b: number}
// Find the first position, searching from the end, at which this
// fragment and the given fragment differ, or `null` if they are the
// same. Since this position will not be the same in both nodes, an
// object with two separate positions is returned.
Fragment.prototype.findDiffEnd = function findDiffEnd$1 (other, pos, otherPos) {
    if ( pos === void 0 ) pos = this.size;
    if ( otherPos === void 0 ) otherPos = other.size;

  return findDiffEnd(this, other, pos, otherPos)
};

// : (number, ?number) → {index: number, offset: number}
// Find the index and inner offset corresponding to a given relative
// position in this fragment. The result object will be reused
// (overwritten) the next time the function is called. (Not public.)
Fragment.prototype.findIndex = function findIndex (pos, round) {
    var this$1 = this;
    if ( round === void 0 ) round = -1;

  if (pos == 0) { return retIndex(0, pos) }
  if (pos == this.size) { return retIndex(this.content.length, pos) }
  if (pos > this.size || pos < 0) { throw new RangeError(("Position " + pos + " outside of fragment (" + (this) + ")")) }
  for (var i = 0, curPos = 0;; i++) {
    var cur = this$1.child(i), end = curPos + cur.nodeSize
    if (end >= pos) {
      if (end == pos || round > 0) { return retIndex(i + 1, end) }
      return retIndex(i, curPos)
    }
    curPos = end
  }
};

// :: () → string
// Return a debugging string that describes this fragment.
Fragment.prototype.toString = function toString () { return "<" + this.toStringInner() + ">" };

Fragment.prototype.toStringInner = function toStringInner () { return this.content.join(", ") };

// :: () → ?Object
// Create a JSON-serializeable representation of this fragment.
Fragment.prototype.toJSON = function toJSON () {
  return this.content.length ? this.content.map(function (n) { return n.toJSON(); }) : null
};

// :: (Schema, ?Object) → Fragment
// Deserialize a fragment from its JSON representation.
Fragment.fromJSON = function fromJSON (schema, value) {
  return value ? new Fragment(value.map(schema.nodeFromJSON)) : Fragment.empty
};

// :: ([Node]) → Fragment
// Build a fragment from an array of nodes. Ensures that adjacent
// text nodes with the same style are joined together.
Fragment.fromArray = function fromArray (array) {
  if (!array.length) { return Fragment.empty }
  var joined, size = 0
  for (var i = 0; i < array.length; i++) {
    var node = array[i]
    size += node.nodeSize
    if (i && node.isText && array[i - 1].sameMarkup(node)) {
      if (!joined) { joined = array.slice(0, i) }
      joined[joined.length - 1] = node.withText(joined[joined.length - 1].text + node.text)
    } else if (joined) {
      joined.push(node)
    }
  }
  return new Fragment(joined || array, size)
};

// :: (?union<Fragment, Node, [Node]>) → Fragment
// Create a fragment from something that can be interpreted as a set
// of nodes. For `null`, it returns the empty fragment. For a
// fragment, the fragment itself. For a node or array of nodes, a
// fragment containing those nodes.
Fragment.from = function from (nodes) {
  if (!nodes) { return Fragment.empty }
  if (nodes instanceof Fragment) { return nodes }
  if (Array.isArray(nodes)) { return this.fromArray(nodes) }
  return new Fragment([nodes], nodes.nodeSize)
};

Object.defineProperties( Fragment.prototype, prototypeAccessors );
exports.Fragment = Fragment

var found = {index: 0, offset: 0}
function retIndex(index, offset) {
  found.index = index
  found.offset = offset
  return found
}

// :: Fragment
// An empty fragment. Intended to be reused whenever a node doesn't
// contain anything (rather than allocating a new empty fragment for
// each leaf node).
Fragment.empty = new Fragment([], 0)

},{"./diff":23}],25:[function(require,module,exports){
var ref = require("./fragment");
var Fragment = ref.Fragment;
var ref$1 = require("./mark");
var Mark = ref$1.Mark;

// ParseRule:: interface
// A value that describes how to parse a given DOM node or inline
// style as a ProseMirror node or mark.
//
//   tag:: ?string
//   A CSS selector describing the kind of DOM elements to match. A
//   single rule should have _either_ a `tag` or a `style` property.
//
//   style:: ?string
//   A CSS property name to match. When given, this rule matches
//   inline styles that list that property.
//
//   node:: ?string
//   The name of the node type to create when this rule matches. Only
//   valid for rules with a `tag` property, not for style rules. Each
//   rule should have one of a `node`, `mark`, or `ignore` property
//   (except when it appears in a [node](#model.NodeSpec.parseDOM) or
//   [mark spec](#model.MarkSpec.parseDOM), in which case the `node`
//   or `mark` property will be derived from its position).
//
//   mark:: ?string
//   The name of the mark type to wrap the matched content in.
//
//   ignore:: ?bool
//   When true, ignore content that matches this rule.
//
//   attrs:: ?Object
//   Attributes for the node or mark created by this rule. When
//   `getAttrs` is provided, it takes precedence.
//
//   getAttrs:: ?(union<dom.Node, string>) → ?union<bool, Object>
//   A function used to compute the attributes for the node or mark
//   created by this rule. Can also be used to describe further
//   conditions the DOM element or style must match. When it returns
//   `false`, the rule won't match. When it returns null or undefined,
//   that is interpreted as an empty/default set of attributes.
//
//   Called with a DOM Element for `tag` rules, and with a string (the
//   style's value) for `style` rules.
//
//   contentElement:: ?string
//   For `tag` rules that produce non-leaf nodes or marks, by default
//   the content of the DOM element is parsed as content of the mark
//   or node. If the child nodes are in a descendent node, this may be
//   a CSS selector string that the parser must use to find the actual
//   content element.
//
//   preserveWhitespace:: ?bool
//   Controls whether whitespace should be preserved when parsing the
//   content inside the matched element.

// ::- A DOM parser represents a strategy for parsing DOM content into
// a ProseMirror document conforming to a given schema. Its behavior
// is defined by an array of [rules](#model.ParseRule).
var DOMParser = function DOMParser(schema, rules) {
  var this$1 = this;

  // :: Schema
  this.schema = schema
  // :: [ParseRule]
  this.rules = rules
  this.tags = []
  this.styles = []

  rules.forEach(function (rule) {
    if (rule.tag) { this$1.tags.push(rule) }
    else if (rule.style) { this$1.styles.push(rule) }
  })
};

// :: (dom.Node, ?Object) → Node
// Parse a document from the content of a DOM node. To provide an
// explicit parent document (for example, when not in a browser
// window environment, where we simply use the global document),
// pass it as the `document` property of `options`.
DOMParser.prototype.parse = function parse (dom, options) {
    if ( options === void 0 ) options = {};

  var topNode = options.topNode
  var top = new NodeBuilder(topNode ? topNode.type : this.schema.nodes.doc,
                            topNode ? topNode.attrs : null, true, null, null, options.preserveWhitespace)
  var state = new DOMParseState(this, options, top)
  state.addAll(dom, null, options.from, options.to)
  return top.finish()
};

// : (ResolvedPos, dom.Node, ?Object) → Slice
// Parse a DOM fragment into a `Slice`, starting with the context at
// `$context`. If the DOM nodes are known to be 'open' (as in
// `Slice`), pass their left open depth as the `openLeft` option.
DOMParser.prototype.parseInContext = function parseInContext ($context, dom, options) {
    if ( options === void 0 ) options = {};

  var ref = builderFromContext($context, options.preserveWhitespace);
    var builder = ref.builder;
    var top = ref.top;
  var openLeft = options.openLeft, startPos = $context.depth

  new (function (DOMParseState) {
      function anonymous () {
        DOMParseState.apply(this, arguments);
      }

      if ( DOMParseState ) anonymous.__proto__ = DOMParseState;
      anonymous.prototype = Object.create( DOMParseState && DOMParseState.prototype );
      anonymous.prototype.constructor = anonymous;

      anonymous.prototype.enter = function enter (type, attrs, preserveWhitespace) {
      if (openLeft == null) { openLeft = type.isTextblock ? 1 : 0 }
      if (openLeft > 0 && this.top.match.matchType(type, attrs)) { openLeft = 0 }
      if (openLeft == 0) { return DOMParseState.prototype.enter.call(this, type, attrs, preserveWhitespace) }

      openLeft--
      return null
    };

      return anonymous;
    }(DOMParseState))(this, options, builder).addAll(dom)

  var openTo = top.openDepth, doc = top.finish(openTo), $startPos = doc.resolve(startPos)
  for (var d = $startPos.depth; d >= 0 && startPos == $startPos.end(d); d--) { ++startPos }
  return doc.slice(startPos, doc.content.size - openTo)
};

DOMParser.prototype.matchTag = function matchTag (dom) {
    var this$1 = this;

  for (var i = 0; i < this.tags.length; i++) {
    var rule = this$1.tags[i]
    if (matches(dom, rule.tag)) {
      if (rule.getAttrs) {
        var result = rule.getAttrs(dom)
        if (result === false) { continue }
        rule.attrs = result
      }
      return rule
    }
  }
};

DOMParser.prototype.matchStyle = function matchStyle (prop, value) {
    var this$1 = this;

  for (var i = 0; i < this.styles.length; i++) {
    var rule = this$1.styles[i]
    if (rule.style == prop) {
      if (rule.getAttrs) {
        var result = rule.getAttrs(value)
        if (result === false) { continue }
        rule.attrs = result
      }
      return rule
    }
  }
};

// :: (Schema) → [ParseRule]
// Extract the parse rules listed in a schema's [node
// specs](#model.NodeSpec.parseDOM).
DOMParser.schemaRules = function schemaRules (schema) {
  var result = []
  var loop = function ( name ) {
    var rules = schema.marks[name].spec.parseDOM
    if (rules) { rules.forEach(function (rule) {
      result.push(rule = copy(rule))
      rule.mark = name
    }) }
  };

    for (var name in schema.marks) loop( name );
  var loop$1 = function ( name ) {
    var rules$1 = schema.nodes[name$1].spec.parseDOM
    if (rules$1) { rules$1.forEach(function (rule) {
      result.push(rule = copy(rule))
      rule.node = name$1
    }) }
  };

    for (var name$1 in schema.nodes) loop$1( name );
  return result
};

// :: (Schema) → DOMParser
// Construct a DOM parser using the parsing rules listed in a
// schema's [node specs](#model.NodeSpec.parseDOM).
DOMParser.fromSchema = function fromSchema (schema) {
  return schema.cached.domParser ||
    (schema.cached.domParser = new DOMParser(schema, DOMParser.schemaRules(schema)))
};
exports.DOMParser = DOMParser

function builderFromContext($context, preserveWhitespace) {
  var top, builder
  for (var i = 0; i <= $context.depth; i++) {
    var node = $context.node(i), match = node.contentMatchAt($context.index(i))
    if (i == 0)
      { builder = top = new NodeBuilder(node.type, node.attrs, true, null, match, preserveWhitespace) }
    else
      { builder = builder.start(node.type, node.attrs, false, match, preserveWhitespace) }
  }
  return {builder: builder, top: top}
}

var NodeBuilder = function NodeBuilder(type, attrs, solid, prev, match, preserveWhitespace) {
  // : NodeType
  // The type of the node being built
  this.type = type
  // : ContentMatch
  // The content match at this point, used to determine whether
  // other nodes may be added here.
  this.match = match || type.contentExpr.start(attrs)
  // : bool
  // True when the node is found in the source, and thus should be
  // preserved until its end. False when it was made up to provide a
  // wrapper for another node.
  this.solid = solid
  // : [Node]
  // The nodes that have been added so far.
  this.content = []
  // : ?NodeBuilder
  // The builder for the parent node, if any.
  this.prev = prev
  // : ?NodeBuilder
  // The builder for the last child, if that is still open (see
  // `NodeBuilder.start`)
  this.openChild = null
  // : bool
  this.preserveWhitespace = preserveWhitespace
};

var prototypeAccessors = { depth: {},openDepth: {},posBeforeLastChild: {},currentPos: {} };

// : (Node) → ?Node
// Try to add a node. Strip it of marks if necessary. Returns null
// when the node doesn't fit here.
NodeBuilder.prototype.add = function add (node) {
    var this$1 = this;

  var matched = this.match.matchNode(node)
  if (!matched && node.marks.length) {
    node = node.mark(node.marks.filter(function (mark) { return this$1.match.allowsMark(mark.type); }))
    matched = this.match.matchNode(node)
  }
  if (!matched) { return null }
  this.closeChild()
  this.content.push(node)
  this.match = matched
  return node
};

// : (NodeType, ?Object, bool, ?ContentMatch) → ?NodeBuilder
// Try to start a new node at this point.
NodeBuilder.prototype.start = function start (type, attrs, solid, match, preserveWhitespace) {
  var matched = this.match.matchType(type, attrs)
  if (!matched) { return null }
  this.closeChild()
  this.match = matched
  return this.openChild = new NodeBuilder(type, attrs, solid, this, match, preserveWhitespace)
};

NodeBuilder.prototype.closeChild = function closeChild (openRight) {
  if (this.openChild) {
    this.content.push(this.openChild.finish(openRight && openRight - 1))
    this.openChild = null
  }
};

// : ()
// Strip any trailing space text from the builder's content.
NodeBuilder.prototype.stripTrailingSpace = function stripTrailingSpace () {
  if (this.openChild) { return }
  var last = this.content[this.content.length - 1], m
  if (last && last.isText && (m = /\s+$/.exec(last.text))) {
    if (last.text.length == m[0].length) { this.content.pop() }
    else { this.content[this.content.length - 1] = last.withText(last.text.slice(0, last.text.length - m[0].length)) }
  }
};

// : (?number) → Node
// Finish this node. If `openRight` is > 0, the node (and `openRight
// - 1` last children) is partial, and we don't need to 'close' it
// by filling in required content.
NodeBuilder.prototype.finish = function finish (openRight) {
  if (!openRight && !this.preserveWhitespace) { this.stripTrailingSpace() }
  this.closeChild(openRight)
  var content = Fragment.from(this.content)
  if (!openRight) { content = content.append(this.match.fillBefore(Fragment.empty, true)) }
  return this.type.create(this.match.attrs, content)
};

// : (NodeType, ?Object, ?Node) → ?NodeBuilder
// Try to find a valid place to add a node with the given type and
// attributes. When successful, if `node` was given, add it in its
// entirety and return the builder to which it was added. If not,
// start a node of the given type and return the builder for it.
NodeBuilder.prototype.findPlace = function findPlace (type, attrs, node, preserveWhitespace) {
  var route, builder
  for (var top = this;; top = top.prev) {
    var found = top.match.findWrapping(type, attrs)
    if (found && (!route || route.length > found.length)) {
      route = found
      builder = top
      if (!found.length) { break }
    }
    if (top.solid) { break }
  }

  if (!route) { return null }
  for (var i = 0; i < route.length; i++)
    { builder = builder.start(route[i].type, route[i].attrs, false) }
  return node ? builder.add(node) && builder : builder.start(type, attrs, true, null, preserveWhitespace)
};

prototypeAccessors.depth.get = function () {
  var d = 0
  for (var b = this.prev; b; b = b.prev) { d++ }
  return d
};

prototypeAccessors.openDepth.get = function () {
  var d = 0
  for (var c = this.openChild; c; c = c.openChild) { d++ }
  return d
};

prototypeAccessors.posBeforeLastChild.get = function () {
    var this$1 = this;

  var pos = this.prev ? this.prev.posBeforeLastChild + 1 : 0
  for (var i = 0; i < this.content.length; i++)
    { pos += this$1.content[i].nodeSize }
  return pos
};

prototypeAccessors.currentPos.get = function () {
  this.closeChild()
  return this.posBeforeLastChild
};

Object.defineProperties( NodeBuilder.prototype, prototypeAccessors );

// : Object<bool> The block-level tags in HTML5
var blockTags = {
  address: true, article: true, aside: true, blockquote: true, canvas: true,
  dd: true, div: true, dl: true, fieldset: true, figcaption: true, figure: true,
  footer: true, form: true, h1: true, h2: true, h3: true, h4: true, h5: true,
  h6: true, header: true, hgroup: true, hr: true, li: true, noscript: true, ol: true,
  output: true, p: true, pre: true, section: true, table: true, tfoot: true, ul: true
}

// : Object<bool> The tags that we normally ignore.
var ignoreTags = {
  head: true, noscript: true, object: true, script: true, style: true, title: true
}

// : Object<bool> List tags.
var listTags = {ol: true, ul: true}

// A state object used to track context during a parse.
var DOMParseState = function DOMParseState(parser, options, top) {
  // : Object The options passed to this parse.
  this.options = options || {}
  // : DOMParser The parser we are using.
  this.parser = parser
  this.top = top
  // : [Mark] The current set of marks
  this.marks = Mark.none
  this.find = options.findPositions
};

// : (Mark) → [Mark]
// Add a mark to the current set of marks, return the old set.
DOMParseState.prototype.addMark = function addMark (mark) {
  var old = this.marks
  this.marks = mark.addToSet(this.marks)
  return old
};

// : (dom.Node)
// Add a DOM node to the content. Text is inserted as text node,
// otherwise, the node is passed to `addElement` or, if it has a
// `style` attribute, `addElementWithStyles`.
DOMParseState.prototype.addDOM = function addDOM (dom) {
    var this$1 = this;

  if (dom.nodeType == 3) {
    var value = dom.nodeValue
    var top = this.top
    if (/\S/.test(value) || top.type.isTextblock) {
      if (!this.top.preserveWhitespace) {
        value = value.replace(/\s+/g, " ")
        // If this starts with whitespace, and there is either no node
        // before it or a node that ends with whitespace, strip the
        // leading space.
        if (/^\s/.test(value)) { top.stripTrailingSpace() }
      }
      if (value)
        { this.insertNode(this.parser.schema.text(value, this.marks)) }
      this.findInText(dom)
    } else {
      this.findInside(dom)
    }
  } else if (dom.nodeType == 1 && !dom.hasAttribute("pm-ignore")) {
    if (dom.hasAttribute("pm-decoration")) {
      for (var child = dom.firstChild; child; child = child.nextSibling)
        { this$1.addDOM(child) }
    } else {
      var style = dom.getAttribute("style")
      if (style) { this.addElementWithStyles(parseStyles(style), dom) }
      else { this.addElement(dom) }
    }
  }
};

// : (dom.Node)
// Try to find a handler for the given tag and use that to parse. If
// none is found, the element's content nodes are added directly.
DOMParseState.prototype.addElement = function addElement (dom) {
  var name = dom.nodeName.toLowerCase()
  if (listTags.hasOwnProperty(name)) { this.normalizeList(dom) }
  // Ignore trailing BR nodes, which browsers create during editing
  if (this.options.editableContent && name == "br" && !dom.nextSibling) { return }
  if (!this.parseNodeType(dom, name)) {
    if (ignoreTags.hasOwnProperty(name)) {
      this.findInside(dom)
    } else {
      var sync = blockTags.hasOwnProperty(name) && this.top
      this.addAll(dom)
      if (sync) { this.sync(sync) }
    }
  }
};

// Run any style parser associated with the node's styles. After
// that, if no style parser suppressed the node's content, pass it
// through to `addElement`.
DOMParseState.prototype.addElementWithStyles = function addElementWithStyles (styles, dom) {
    var this$1 = this;

  var oldMarks = this.marks, marks = this.marks
  for (var i = 0; i < styles.length; i += 2) {
    var rule = this$1.parser.matchStyle(styles[i], styles[i + 1])
    if (!rule) { continue }
    if (rule.ignore) { return }
    marks = this$1.parser.schema.marks[rule.mark].create(rule.attrs).addToSet(marks)
  }
  this.marks = marks
  this.addElement(dom)
  this.marks = oldMarks
};

// (dom.Node, string) → bool
// Look up a handler for the given node. If none are found, return
// false. Otherwise, apply it, use its return value to drive the way
// the node's content is wrapped, and return true.
DOMParseState.prototype.parseNodeType = function parseNodeType (dom) {
  var rule = this.parser.matchTag(dom)
  if (!rule) { return false }
  if (rule.ignore) { return true }

  var sync, before, nodeType, markType
  var contentDOM = (rule.contentElement && dom.querySelector(rule.contentElement)) || dom

  if (rule.node) {
    nodeType = this.parser.schema.nodes[rule.node]
    if (nodeType.isLeaf) { this.insertNode(nodeType.create(rule.attrs)) }
    else { sync = this.enter(nodeType, rule.attrs, rule.preserveWhitespace) }
  } else {
    markType = this.parser.schema.marks[rule.mark]
    before = this.addMark(markType.create(rule.attrs))
  }

  if (markType || !nodeType.isLeaf) {
    this.findAround(dom, contentDOM, true)
    this.addAll(contentDOM, sync)
    if (sync) { this.sync(sync.prev) }
    else if (before) { this.marks = before }
    this.findAround(dom, contentDOM, true)
  } else {
    this.findInside(dom)
  }
  return true
};

// : (dom.Node, ?NodeBuilder, ?number, ?number)
// Add all child nodes between `startIndex` and `endIndex` (or the
// whole node, if not given). If `sync` is passed, use it to
// synchronize after every block element.
DOMParseState.prototype.addAll = function addAll (parent, sync, startIndex, endIndex) {
    var this$1 = this;

  var index = startIndex || 0
  for (var dom = startIndex ? parent.childNodes[startIndex] : parent.firstChild,
           end = endIndex == null ? null : parent.childNodes[endIndex];
       dom != end; dom = dom.nextSibling, ++index) {
    this$1.findAtPoint(parent, index)
    this$1.addDOM(dom)
    if (sync && blockTags.hasOwnProperty(dom.nodeName.toLowerCase()))
      { this$1.sync(sync) }
  }
  this.findAtPoint(parent, index)
};

// : (Node) → ?Node
// Try to insert the given node, adjusting the context when needed.
DOMParseState.prototype.insertNode = function insertNode (node) {
  var ok = this.top.findPlace(node.type, node.attrs, node)
  if (ok) {
    this.sync(ok)
    return true
  }
};

// : (NodeType, ?Object) → ?NodeBuilder
// Try to start a node of the given type, adjusting the context when
// necessary.
DOMParseState.prototype.enter = function enter (type, attrs, preserveWhitespace) {
  var ok = this.top.findPlace(type, attrs, null, preserveWhitespace)
  if (ok) {
    this.sync(ok)
    return ok
  }
};

// : ()
// Leave the node currently at the top.
DOMParseState.prototype.leave = function leave () {
  this.top = this.top.prev
};

DOMParseState.prototype.sync = function sync (to) {
    var this$1 = this;

  for (;;) {
    for (var cur = to; cur; cur = cur.prev) { if (cur == this$1.top) {
      this$1.top = to
      return
    } }
    this$1.leave()
  }
};

// Kludge to work around directly nested list nodes produced by some
// tools and allowed by browsers to mean that the nested list is
// actually part of the list item above it.
DOMParseState.prototype.normalizeList = function normalizeList (dom) {
  for (var child = dom.firstChild, prevItem = null; child; child = child.nextSibling) {
    var name = child.nodeType == 1 ? child.nodeName.toLowerCase() : null
    if (name && listTags.hasOwnProperty(name) && prevItem) {
      prevItem.appendChild(child)
      child = prevItem
    } else if (name == "li") {
      prevItem = child
    } else if (name) {
      prevItem = null
    }
  }
};

DOMParseState.prototype.findAtPoint = function findAtPoint (parent, offset) {
    var this$1 = this;

  if (this.find) { for (var i = 0; i < this.find.length; i++) {
    if (this$1.find[i].node == parent && this$1.find[i].offset == offset)
      { this$1.find[i].pos = this$1.top.currentPos }
  } }
};

DOMParseState.prototype.findInside = function findInside (parent) {
    var this$1 = this;

  if (this.find) { for (var i = 0; i < this.find.length; i++) {
    if (this$1.find[i].pos == null && parent.contains(this$1.find[i].node))
      { this$1.find[i].pos = this$1.top.currentPos }
  } }
};

DOMParseState.prototype.findAround = function findAround (parent, content, before) {
    var this$1 = this;

  if (parent != content && this.find) { for (var i = 0; i < this.find.length; i++) {
    if (this$1.find[i].pos == null && parent.contains(this$1.find[i].node)) {
      var pos = content.compareDocumentPosition(this$1.find[i].node)
      if (pos & (before ? 2 : 4))
        { this$1.find[i].pos = this$1.top.currentPos }
    }
  } }
};

DOMParseState.prototype.findInText = function findInText (textNode) {
    var this$1 = this;

  if (this.find) { for (var i = 0; i < this.find.length; i++) {
    if (this$1.find[i].node == textNode)
      { this$1.find[i].pos = this$1.top.currentPos - (textNode.nodeValue.length - this$1.find[i].offset) }
  } }
};

// Apply a CSS selector.
function matches(dom, selector) {
  return (dom.matches || dom.msMatchesSelector || dom.webkitMatchesSelector || dom.mozMatchesSelector).call(dom, selector)
}

// : (string) → [string]
// Tokenize a style attribute into property/value pairs.
function parseStyles(style) {
  var re = /\s*([\w-]+)\s*:\s*([^;]+)/g, m, result = []
  while (m = re.exec(style)) { result.push(m[1], m[2].trim()) }
  return result
}

function copy(obj) {
  var copy = {}
  for (var prop in obj) { copy[prop] = obj[prop] }
  return copy
}

},{"./fragment":24,"./mark":27}],26:[function(require,module,exports){
exports.Node = require("./node").Node
;var assign;
((assign = require("./resolvedpos"), exports.ResolvedPos = assign.ResolvedPos, exports.NodeRange = assign.NodeRange))
exports.Fragment = require("./fragment").Fragment
;var assign$1;
((assign$1 = require("./replace"), exports.Slice = assign$1.Slice, exports.ReplaceError = assign$1.ReplaceError))
exports.Mark = require("./mark").Mark

;var assign$2;
((assign$2 = require("./schema"), exports.Schema = assign$2.Schema, exports.NodeType = assign$2.NodeType, exports.MarkType = assign$2.MarkType))
;var assign$3;
((assign$3 = require("./content"), exports.ContentMatch = assign$3.ContentMatch))

exports.DOMParser = require("./from_dom").DOMParser
exports.DOMSerializer =  require("./to_dom").DOMSerializer

},{"./content":22,"./fragment":24,"./from_dom":25,"./mark":27,"./node":28,"./replace":30,"./resolvedpos":31,"./schema":32,"./to_dom":33}],27:[function(require,module,exports){
var ref = require("./comparedeep");
var compareDeep = ref.compareDeep;

// ::- A mark is a piece of information that can be attached to a node,
// such as it being emphasized, in code font, or a link. It has a type
// and optionally a set of attributes that provide further information
// (such as the target of the link). Marks are created through a
// `Schema`, which controls which types exist and which
// attributes they have.
var Mark = function Mark(type, attrs) {
  // :: MarkType
  // The type of this mark.
  this.type = type
  // :: Object
  // The attributes associated with this mark.
  this.attrs = attrs
};

// :: ([Mark]) → [Mark]
// Given a set of marks, create a new set which contains this one as
// well, in the right position. If this mark is already in the set,
// the set itself is returned. If a mark of this type with different
// attributes is already in the set, a set in which it is replaced
// by this one is returned.
Mark.prototype.addToSet = function addToSet (set) {
    var this$1 = this;

  for (var i = 0; i < set.length; i++) {
    var other = set[i]
    if (other.type == this$1.type) {
      if (this$1.eq(other)) { return set }
      var copy = set.slice()
      copy[i] = this$1
      return copy
    }
    if (other.type.rank > this$1.type.rank)
      { return set.slice(0, i).concat(this$1).concat(set.slice(i)) }
  }
  return set.concat(this)
};

// :: ([Mark]) → [Mark]
// Remove this mark from the given set, returning a new set. If this
// mark is not in the set, the set itself is returned.
Mark.prototype.removeFromSet = function removeFromSet (set) {
    var this$1 = this;

  for (var i = 0; i < set.length; i++)
    { if (this$1.eq(set[i]))
      { return set.slice(0, i).concat(set.slice(i + 1)) } }
  return set
};

// :: ([Mark]) → bool
// Test whether this mark is in the given set of marks.
Mark.prototype.isInSet = function isInSet (set) {
    var this$1 = this;

  for (var i = 0; i < set.length; i++)
    { if (this$1.eq(set[i])) { return true } }
  return false
};

// :: (Mark) → bool
// Test whether this mark has the same type and attributes as
// another mark.
Mark.prototype.eq = function eq (other) {
  if (this == other) { return true }
  if (this.type != other.type) { return false }
  if (!compareDeep(other.attrs, this.attrs)) { return false }
  return true
};

// :: () → Object
// Convert this mark to a JSON-serializeable representation.
Mark.prototype.toJSON = function toJSON () {
    var this$1 = this;

  var obj = {type: this.type.name}
  for (var _ in this.attrs) {
    obj.attrs = this$1.attrs
    break
  }
  return obj
};

// :: (Schema, Object) → Mark
Mark.fromJSON = function fromJSON (schema, json) {
  return schema.marks[json.type].create(json.attrs)
};

// :: ([Mark], [Mark]) → bool
// Test whether two sets of marks are identical.
Mark.sameSet = function sameSet (a, b) {
  if (a == b) { return true }
  if (a.length != b.length) { return false }
  for (var i = 0; i < a.length; i++)
    { if (!a[i].eq(b[i])) { return false } }
  return true
};

// :: (?union<Mark, [Mark]>) → [Mark]
// Create a properly sorted mark set from null, a single mark, or an
// unsorted array of marks.
Mark.setFrom = function setFrom (marks) {
  if (!marks || marks.length == 0) { return Mark.none }
  if (marks instanceof Mark) { return [marks] }
  var copy = marks.slice()
  copy.sort(function (a, b) { return a.type.rank - b.type.rank; })
  return copy
};
exports.Mark = Mark

// :: [Mark] The empty set of marks.
Mark.none = []

},{"./comparedeep":21}],28:[function(require,module,exports){
var ref = require("./fragment");
var Fragment = ref.Fragment;
var ref$1 = require("./mark");
var Mark = ref$1.Mark;
var ref$2 = require("./replace");
var Slice = ref$2.Slice;
var replace = ref$2.replace;
var ref$3 = require("./resolvedpos");
var ResolvedPos = ref$3.ResolvedPos;
var ref$4 = require("./comparedeep");
var compareDeep = ref$4.compareDeep;

var emptyAttrs = Object.create(null)

// ::- This class represents a node in the tree that makes up a
// ProseMirror document. So a document is an instance of `Node`, with
// children that are also instances of `Node`.
//
// Nodes are persistent data structures. Instead of changing them, you
// create new ones with the content you want. Old ones keep pointing
// at the old document shape. This is made cheaper by sharing
// structure between the old and new data as much as possible, which a
// tree shape like this (without back pointers) makes easy.
//
// **Never** directly mutate the properties of a `Node` object. See
// [this guide](guide/doc.html) for more information.
var Node = function Node(type, attrs, content, marks) {
  // :: NodeType
  // The type of node that this is.
  this.type = type

  // :: Object
  // An object mapping attribute names to values. The kind of
  // attributes allowed and required are determined by the node
  // type.
  this.attrs = attrs

  // :: Fragment
  // A container holding the node's children.
  this.content = content || Fragment.empty

  // :: [Mark]
  // The marks (things like whether it is emphasized or part of a
  // link) associated with this node.
  this.marks = marks || Mark.none
};

var prototypeAccessors = { nodeSize: {},childCount: {},textContent: {},firstChild: {},lastChild: {},isBlock: {},isTextblock: {},isInline: {},isText: {},isLeaf: {} };

// text:: ?string
// For text nodes, this contains the node's text content.

// :: number
// The size of this node, as defined by the integer-based [indexing
// scheme](guide/doc.html#indexing). For text nodes, this is the
// amount of characters. For other leaf nodes, it is one. And for
// non-leaf nodes, it is the size of the content plus two (the start
// and end token).
prototypeAccessors.nodeSize.get = function () { return this.isLeaf ? 1 : 2 + this.content.size };

// :: number
// The number of children that the node has.
prototypeAccessors.childCount.get = function () { return this.content.childCount };

// :: (number) → Node
// Get the child node at the given index. Raises an error when the
// index is out of range.
Node.prototype.child = function child (index) { return this.content.child(index) };

// :: (number) → ?Node
// Get the child node at the given index, if it exists.
Node.prototype.maybeChild = function maybeChild (index) { return this.content.maybeChild(index) };

// :: ((node: Node, offset: number, index: number))
// Call `f` for every child node, passing the node, its offset
// into this parent node, and its index.
Node.prototype.forEach = function forEach (f) { this.content.forEach(f) };

// :: (?number, ?number, (node: Node, pos: number, parent: Node, index: number))
// Iterate over all nodes between the given two positions, calling
// the callback with the node, its position, its parent
// node, and its index in that node.
Node.prototype.nodesBetween = function nodesBetween (from, to, f, pos) {
    if ( pos === void 0 ) pos = 0;

  this.content.nodesBetween(from, to, f, pos, this)
};

// :: ((node: Node, pos: number, parent: Node))
// Call the given callback for every descendant node.
Node.prototype.descendants = function descendants (f) {
  this.nodesBetween(0, this.content.size, f)
};

// :: string
// Concatenates all the text nodes found in this fragment and its
// children.
prototypeAccessors.textContent.get = function () { return this.textBetween(0, this.content.size, "") };

// :: (number, number, ?string, ?string) → string
// Get all text between positions `from` and `to`. When
// `blockSeparator` is given, it will be inserted whenever a new
// block node is started. When `leafText` is given, it'll be
// inserted for every non-text leaf node encountered.
Node.prototype.textBetween = function textBetween (from, to, blockSeparator, leafText) {
  return this.content.textBetween(from, to, blockSeparator, leafText)
};

// :: ?Node
// Returns this node's first child, or `null` if there are no
// children.
prototypeAccessors.firstChild.get = function () { return this.content.firstChild };

// :: ?Node
// Returns this node's last child, or `null` if there are no
// children.
prototypeAccessors.lastChild.get = function () { return this.content.lastChild };

// :: (Node) → bool
// Test whether two nodes represent the same content.
Node.prototype.eq = function eq (other) {
  return this == other || (this.sameMarkup(other) && this.content.eq(other.content))
};

// :: (Node) → bool
// Compare the markup (type, attributes, and marks) of this node to
// those of another. Returns `true` if both have the same markup.
Node.prototype.sameMarkup = function sameMarkup (other) {
  return this.hasMarkup(other.type, other.attrs, other.marks)
};

// :: (NodeType, ?Object, ?[Mark]) → bool
// Check whether this node's markup correspond to the given type,
// attributes, and marks.
Node.prototype.hasMarkup = function hasMarkup (type, attrs, marks) {
  return this.type == type &&
    compareDeep(this.attrs, attrs || type.defaultAttrs || emptyAttrs) &&
    Mark.sameSet(this.marks, marks || Mark.none)
};

// :: (?Fragment) → Node
// Create a new node with the same markup as this node, containing
// the given content (or empty, if no content is given).
Node.prototype.copy = function copy (content) {
    if ( content === void 0 ) content = null;

  if (content == this.content) { return this }
  return new this.constructor(this.type, this.attrs, content, this.marks)
};

// :: ([Mark]) → Node
// Create a copy of this node, with the given set of marks instead
// of the node's own marks.
Node.prototype.mark = function mark (marks) {
  return marks == this.marks ? this : new this.constructor(this.type, this.attrs, this.content, marks)
};

// :: (number, ?number) → Node
// Create a copy of this node with only the content between the
// given offsets. If `to` is not given, it defaults to the end of
// the node.
Node.prototype.cut = function cut (from, to) {
  if (from == 0 && to == this.content.size) { return this }
  return this.copy(this.content.cut(from, to))
};

// :: (number, ?number) → Slice
// Cut out the part of the document between the given positions, and
// return it as a `Slice` object.
Node.prototype.slice = function slice (from, to) {
    if ( to === void 0 ) to = this.content.size;

  if (from == to) { return Slice.empty }

  var $from = this.resolve(from), $to = this.resolve(to)
  var depth = $from.sameDepth($to), start = $from.start(depth), node = $from.node(depth)
  var content = node.content.cut($from.pos - start, $to.pos - start)
  return new Slice(content, $from.depth - depth, $to.depth - depth, node)
};

// :: (number, number, Slice) → Node
// Replace the part of the document between the given positions with
// the given slice. The slice must 'fit', meaning its open sides
// must be able to connect to the surrounding content, and its
// content nodes must be valid children for the node they are placed
// into. If any of this is violated, an error of type
// [`ReplaceError`](#model.ReplaceError) is thrown.
Node.prototype.replace = function replace$1 (from, to, slice) {
  return replace(this.resolve(from), this.resolve(to), slice)
};

// :: (number) → ?Node
// Find the node after the given position.
Node.prototype.nodeAt = function nodeAt (pos) {
  for (var node = this;;) {
    var ref = node.content.findIndex(pos);
      var index = ref.index;
      var offset = ref.offset;
    node = node.maybeChild(index)
    if (!node) { return null }
    if (offset == pos || node.isText) { return node }
    pos -= offset + 1
  }
};

// :: (number) → {node: ?Node, index: number, offset: number}
// Find the (direct) child node after the given offset, if any,
// and return it along with its index and offset relative to this
// node.
Node.prototype.childAfter = function childAfter (pos) {
  var ref = this.content.findIndex(pos);
    var index = ref.index;
    var offset = ref.offset;
  return {node: this.content.maybeChild(index), index: index, offset: offset}
};

// :: (number) → {node: ?Node, index: number, offset: number}
// Find the (direct) child node before the given offset, if any,
// and return it along with its index and offset relative to this
// node.
Node.prototype.childBefore = function childBefore (pos) {
  if (pos == 0) { return {node: null, index: 0, offset: 0} }
  var ref = this.content.findIndex(pos);
    var index = ref.index;
    var offset = ref.offset;
  if (offset < pos) { return {node: this.content.child(index), index: index, offset: offset} }
  var node = this.content.child(index - 1)
  return {node: node, index: index - 1, offset: offset - node.nodeSize}
};

// :: (number) → ResolvedPos
// Resolve the given position in the document, returning an object
// describing its path through the document.
Node.prototype.resolve = function resolve (pos) { return ResolvedPos.resolveCached(this, pos) };

Node.prototype.resolveNoCache = function resolveNoCache (pos) { return ResolvedPos.resolve(this, pos) };

// :: (number, ?bool) → [Mark]
// Get the marks at the given position factoring in the surrounding
// marks' inclusiveRight property. If the position is at the start
// of a non-empty node, or `useAfter` is true, the marks of the node
// after it are returned.
Node.prototype.marksAt = function marksAt (pos, useAfter) {
  var $pos = this.resolve(pos), parent = $pos.parent, index = $pos.index()

  // In an empty parent, return the empty array
  if (parent.content.size == 0) { return Mark.none }
  // When inside a text node or at the start of the parent node, return the node's marks
  if (useAfter || index == 0 || !$pos.atNodeBoundary) { return parent.child(index).marks }

  var marks = parent.child(index - 1).marks
  for (var i = 0; i < marks.length; i++) { if (marks[i].type.inclusiveRight === false)
    { marks = marks[i--].removeFromSet(marks) } }
  return marks
};

// :: (?number, ?number, MarkType) → bool
// Test whether a mark of the given type occurs in this document
// between the two given positions.
Node.prototype.rangeHasMark = function rangeHasMark (from, to, type) {
  var found = false
  this.nodesBetween(from, to, function (node) {
    if (type.isInSet(node.marks)) { found = true }
    return !found
  })
  return found
};

// :: bool
// True when this is a block (non-inline node)
prototypeAccessors.isBlock.get = function () { return this.type.isBlock };

// :: bool
// True when this is a textblock node, a block node with inline
// content.
prototypeAccessors.isTextblock.get = function () { return this.type.isTextblock };

// :: bool
// True when this is an inline node (a text node or a node that can
// appear among text).
prototypeAccessors.isInline.get = function () { return this.type.isInline };

// :: bool
// True when this is a text node.
prototypeAccessors.isText.get = function () { return this.type.isText };

// :: bool
// True when this is a leaf node.
prototypeAccessors.isLeaf.get = function () { return this.type.isLeaf };

// :: () → string
// Return a string representation of this node for debugging
// purposes.
Node.prototype.toString = function toString () {
  var name = this.type.name
  if (this.content.size)
    { name += "(" + this.content.toStringInner() + ")" }
  return wrapMarks(this.marks, name)
};

// :: (number) → ContentMatch
// Get the content match in this node at the given index.
Node.prototype.contentMatchAt = function contentMatchAt (index) {
  return this.type.contentExpr.getMatchAt(this.attrs, this.content, index)
};

// :: (number, number, ?Fragment, ?number, ?number) → bool
// Test whether replacing the range `from` to `to` (by index) with
// the given replacement fragment (which defaults to the empty
// fragment) would leave the node's content valid. You can
// optionally pass `start` and `end` indices into the replacement
// fragment.
Node.prototype.canReplace = function canReplace (from, to, replacement, start, end) {
  return this.type.contentExpr.checkReplace(this.attrs, this.content, from, to, replacement, start, end)
};

// :: (number, number, NodeType, ?[Mark]) → bool
// Test whether replacing the range `from` to `to` (by index) with a
// node of the given type with the given attributes and marks would
// be valid.
Node.prototype.canReplaceWith = function canReplaceWith (from, to, type, attrs, marks) {
  return this.type.contentExpr.checkReplaceWith(this.attrs, this.content, from, to, type, attrs, marks || Mark.none)
};

// :: (Node) → bool
// Test whether the given node's content could be appended to this
// node. If that node is empty, this will only return true if there
// is at least one node type that can appear in both nodes (to avoid
// merging completely incompatible nodes).
Node.prototype.canAppend = function canAppend (other) {
  if (other.content.size) { return this.canReplace(this.childCount, this.childCount, other.content) }
  else { return this.type.compatibleContent(other.type) }
};

Node.prototype.defaultContentType = function defaultContentType (at) {
  var elt = this.contentMatchAt(at).nextElement
  return elt && elt.defaultType()
};

// :: () → Object
// Return a JSON-serializeable representation of this node.
Node.prototype.toJSON = function toJSON () {
    var this$1 = this;

  var obj = {type: this.type.name}
  for (var _ in this.attrs) {
    obj.attrs = this$1.attrs
    break
  }
  if (this.content.size)
    { obj.content = this.content.toJSON() }
  if (this.marks.length)
    { obj.marks = this.marks.map(function (n) { return n.toJSON(); }) }
  return obj
};

// :: (Schema, Object) → Node
// Deserialize a node from its JSON representation.
Node.fromJSON = function fromJSON (schema, json) {
  var marks = json.marks && json.marks.map(schema.markFromJSON)
  if (json.type == "text") { return schema.text(json.text, marks) }
  return schema.nodeType(json.type).create(json.attrs, Fragment.fromJSON(schema, json.content), marks)
};

Object.defineProperties( Node.prototype, prototypeAccessors );
exports.Node = Node

var TextNode = (function (Node) {
  function TextNode(type, attrs, content, marks) {
    Node.call(this, type, attrs, null, marks)

    if (!content) { throw new RangeError("Empty text nodes are not allowed") }

    this.text = content
  }

  if ( Node ) TextNode.__proto__ = Node;
  TextNode.prototype = Object.create( Node && Node.prototype );
  TextNode.prototype.constructor = TextNode;

  var prototypeAccessors$1 = { textContent: {},nodeSize: {} };

  TextNode.prototype.toString = function toString () { return wrapMarks(this.marks, JSON.stringify(this.text)) };

  prototypeAccessors$1.textContent.get = function () { return this.text };

  TextNode.prototype.textBetween = function textBetween (from, to) { return this.text.slice(from, to) };

  prototypeAccessors$1.nodeSize.get = function () { return this.text.length };

  TextNode.prototype.mark = function mark (marks) {
    return new TextNode(this.type, this.attrs, this.text, marks)
  };

  TextNode.prototype.withText = function withText (text) {
    if (text == this.text) { return this }
    return new TextNode(this.type, this.attrs, text, this.marks)
  };

  TextNode.prototype.cut = function cut (from, to) {
    if ( from === void 0 ) from = 0;
    if ( to === void 0 ) to = this.text.length;

    if (from == 0 && to == this.text.length) { return this }
    return this.withText(this.text.slice(from, to))
  };

  TextNode.prototype.eq = function eq (other) {
    return this.sameMarkup(other) && this.text == other.text
  };

  TextNode.prototype.toJSON = function toJSON () {
    var base = Node.prototype.toJSON.call(this)
    base.text = this.text
    return base
  };

  Object.defineProperties( TextNode.prototype, prototypeAccessors$1 );

  return TextNode;
}(Node));
exports.TextNode = TextNode

function wrapMarks(marks, str) {
  for (var i = marks.length - 1; i >= 0; i--)
    { str = marks[i].type.name + "(" + str + ")" }
  return str
}

},{"./comparedeep":21,"./fragment":24,"./mark":27,"./replace":30,"./resolvedpos":31}],29:[function(require,module,exports){
// ::- Persistent data structure representing an ordered mapping from
// strings to values, with some convenient update methods.
var OrderedMap = function OrderedMap(content) {
  this.content = content
};

var prototypeAccessors = { size: {} };

OrderedMap.prototype.find = function find (key) {
    var this$1 = this;

  for (var i = 0; i < this.content.length; i += 2)
    { if (this$1.content[i] == key) { return i } }
  return -1
};

// :: (string) → ?any
// Retrieve the value stored under `key`, or return undefined when
// no such key exists.
OrderedMap.prototype.get = function get (key) {
  var found = this.find(key)
  return found == -1 ? undefined : this.content[found + 1]
};

// :: (string, any, ?string) → OrderedMap
// Create a new map by replacing the value of `key` with a new
// value, or adding a binding to the end of the map. If `newKey` is
// given, the key of the binding will be replaced with that key.
OrderedMap.prototype.update = function update (key, value, newKey) {
  var self = newKey && newKey != key ? this.remove(newKey) : this
  var found = self.find(key), content = self.content.slice()
  if (found == -1) {
    content.push(newKey || key, value)
  } else {
    content[found + 1] = value
    if (newKey) { content[found] = newKey }
  }
  return new OrderedMap(content)
};

// :: (string) → OrderedMap
// Return a map with the given key removed, if it existed.
OrderedMap.prototype.remove = function remove (key) {
  var found = this.find(key)
  if (found == -1) { return this }
  var content = this.content.slice()
  content.splice(found, 2)
  return new OrderedMap(content)
};

// :: (string, any) → OrderedMap
// Add a new key to the start of the map.
OrderedMap.prototype.addToStart = function addToStart (key, value) {
  return new OrderedMap([key, value].concat(this.remove(key).content))
};

// :: (string, any) → OrderedMap
// Add a new key to the end of the map.
OrderedMap.prototype.addToEnd = function addToEnd (key, value) {
  var content = this.remove(key).content.slice()
  content.push(key, value)
  return new OrderedMap(content)
};

// :: (string, string, any) → OrderedMap
// Add a key after the given key. If `place` is not found, the new
// key is added to the end.
OrderedMap.prototype.addBefore = function addBefore (place, key, value) {
  var without = this.remove(key), content = without.content.slice()
  var found = without.find(place)
  content.splice(found == -1 ? content.length : found, 0, key, value)
  return new OrderedMap(content)
};

// :: ((key: string, value: any))
// Call the given function for each key/value pair in the map, in
// order.
OrderedMap.prototype.forEach = function forEach (f) {
    var this$1 = this;

  for (var i = 0; i < this.content.length; i += 2)
    { f(this$1.content[i], this$1.content[i + 1]) }
};

// :: (union<Object, OrderedMap>) → OrderedMap
// Create a new map by prepending the keys in this map that don't
// appear in `map` before the keys in `map`.
OrderedMap.prototype.prepend = function prepend (map) {
  map = OrderedMap.from(map)
  if (!map.size) { return this }
  return new OrderedMap(map.content.concat(this.subtract(map).content))
};

// :: (union<Object, OrderedMap>) → OrderedMap
// Create a new map by appending the keys in this map that don't
// appear in `map` after the keys in `map`.
OrderedMap.prototype.append = function append (map) {
  map = OrderedMap.from(map)
  if (!map.size) { return this }
  return new OrderedMap(this.subtract(map).content.concat(map.content))
};

// :: (union<Object, OrderedMap>) → OrderedMap
// Create a map containing all the keys in this map that don't
// appear in `map`.
OrderedMap.prototype.subtract = function subtract (map) {
  var result = this
  OrderedMap.from(map).forEach(function (key) { return result = result.remove(key); })
  return result
};

// :: number
// The amount of keys in this map.
prototypeAccessors.size.get = function () {
  return this.content.length >> 1
};

// :: (?union<Object, OrderedMap>) → OrderedMap
// Return a map with the given content. If null, create an empty
// map. If given an ordered map, return that map itself. If given an
// object, create a map from the object's properties.
OrderedMap.from = function from (value) {
  if (value instanceof OrderedMap) { return value }
  var content = []
  if (value) { for (var prop in value) { content.push(prop, value[prop]) } }
  return new OrderedMap(content)
};

Object.defineProperties( OrderedMap.prototype, prototypeAccessors );
exports.OrderedMap = OrderedMap

},{}],30:[function(require,module,exports){
var ref = require("./fragment");
var Fragment = ref.Fragment;

// ::- Error type raised by [`Node.replace`](#model.Node.replace) when
// given an invalid replacement.
var ReplaceError = (function (Error) {
  function ReplaceError(message) {
    Error.call(this, message)
    this.message = message
  }

  if ( Error ) ReplaceError.__proto__ = Error;
  ReplaceError.prototype = Object.create( Error && Error.prototype );
  ReplaceError.prototype.constructor = ReplaceError;

  return ReplaceError;
}(Error));
exports.ReplaceError = ReplaceError

// ::- A slice represents a piece cut out of a larger document. It
// stores not only a fragment, but also the depth up to which nodes on
// both side are 'open' / cut through.
var Slice = function Slice(content, openLeft, openRight, possibleParent) {
  // :: Fragment The slice's content nodes.
  this.content = content
  // :: number The open depth at the start.
  this.openLeft = openLeft
  // :: number The open depth at the end.
  this.openRight = openRight
  this.possibleParent = possibleParent
};

var prototypeAccessors = { size: {} };

// :: number
// The size this slice would add when inserted into a document.
prototypeAccessors.size.get = function () {
  return this.content.size - this.openLeft - this.openRight
};

Slice.prototype.insertAt = function insertAt (pos, fragment) {
  function insertInto(content, dist, insert, parent) {
    var ref = content.findIndex(dist);
      var index = ref.index;
      var offset = ref.offset;
      var child = content.maybeChild(index)
    if (offset == dist || child.isText) {
      if (parent && !parent.canReplace(index, index, insert)) { return null }
      return content.cut(0, dist).append(insert).append(content.cut(dist))
    }
    var inner = insertInto(child.content, dist - offset - 1, insert)
    return inner && content.replaceChild(index, child.copy(inner))
  }
  var content = insertInto(this.content, pos + this.openLeft, fragment, null)
  return content && new Slice(content, this.openLeft, this.openRight)
};

Slice.prototype.removeBetween = function removeBetween (from, to) {
  function removeRange(content, from, to) {
    var ref = content.findIndex(from);
      var index = ref.index;
      var offset = ref.offset;
      var child = content.maybeChild(index)
    var ref$1 = content.findIndex(to);
      var indexTo = ref$1.index;
      var offsetTo = ref$1.offset;
    if (offset == from || child.isText) {
      if (offsetTo != to && !content.child(indexTo).isText) { throw new RangeError("Removing non-flat range") }
      return content.cut(0, from).append(content.cut(to))
    }
    if (index != indexTo) { throw new RangeError("Removing non-flat range") }
    return content.replaceChild(index, child.copy(removeRange(child.content, from - offset - 1, to - offset - 1)))
  }
  return new Slice(removeRange(this.content, from + this.openLeft, to + this.openLeft), this.openLeft, this.openRight)
};

Slice.prototype.eq = function eq (other) {
  return this.content.eq(other.content) && this.openLeft == other.openLeft && this.openRight == other.openRight
};

Slice.prototype.toString = function toString () {
  return this.content + "(" + this.openLeft + "," + this.openRight + ")"
};

// :: () → ?Object
// Convert a slice to a JSON-serializable representation.
Slice.prototype.toJSON = function toJSON () {
  if (!this.content.size) { return null }
  return {content: this.content.toJSON(),
          openLeft: this.openLeft,
          openRight: this.openRight}
};

// :: (Schema, ?Object) → Slice
// Deserialize a slice from its JSON representation.
Slice.fromJSON = function fromJSON (schema, json) {
  if (!json) { return Slice.empty }
  return new Slice(Fragment.fromJSON(schema, json.content), json.openLeft, json.openRight)
};

Object.defineProperties( Slice.prototype, prototypeAccessors );
exports.Slice = Slice

// :: Slice
// The empty slice.
Slice.empty = new Slice(Fragment.empty, 0, 0)

function replace($from, $to, slice) {
  if (slice.openLeft > $from.depth)
    { throw new ReplaceError("Inserted content deeper than insertion position") }
  if ($from.depth - slice.openLeft != $to.depth - slice.openRight)
    { throw new ReplaceError("Inconsistent open depths") }
  return replaceOuter($from, $to, slice, 0)
}
exports.replace = replace

function replaceOuter($from, $to, slice, depth) {
  var index = $from.index(depth), node = $from.node(depth)
  if (index == $to.index(depth) && depth < $from.depth - slice.openLeft) {
    var inner = replaceOuter($from, $to, slice, depth + 1)
    return node.copy(node.content.replaceChild(index, inner))
  } else if (slice.content.size) {
    var ref = prepareSliceForReplace(slice, $from);
    var start = ref.start;
    var end = ref.end;
    return close(node, replaceThreeWay($from, start, end, $to, depth))
  } else {
    return close(node, replaceTwoWay($from, $to, depth))
  }
}

function checkJoin(main, sub) {
  if (!sub.type.compatibleContent(main.type))
    { throw new ReplaceError("Cannot join " + sub.type.name + " onto " + main.type.name) }
}

function joinable($before, $after, depth) {
  var node = $before.node(depth)
  checkJoin(node, $after.node(depth))
  return node
}

function addNode(child, target) {
  var last = target.length - 1
  if (last >= 0 && child.isText && child.sameMarkup(target[last]))
    { target[last] = child.withText(target[last].text + child.text) }
  else
    { target.push(child) }
}

function addRange($start, $end, depth, target) {
  var node = ($end || $start).node(depth)
  var startIndex = 0, endIndex = $end ? $end.index(depth) : node.childCount
  if ($start) {
    startIndex = $start.index(depth)
    if ($start.depth > depth) {
      startIndex++
    } else if (!$start.atNodeBoundary) {
      addNode($start.nodeAfter, target)
      startIndex++
    }
  }
  for (var i = startIndex; i < endIndex; i++) { addNode(node.child(i), target) }
  if ($end && $end.depth == depth && !$end.atNodeBoundary)
    { addNode($end.nodeBefore, target) }
}

function close(node, content) {
  if (!node.type.validContent(content, node.attrs))
    { throw new ReplaceError("Invalid content for node " + node.type.name) }
  return node.copy(content)
}

function replaceThreeWay($from, $start, $end, $to, depth) {
  var openLeft = $from.depth > depth && joinable($from, $start, depth + 1)
  var openRight = $to.depth > depth && joinable($end, $to, depth + 1)

  var content = []
  addRange(null, $from, depth, content)
  if (openLeft && openRight && $start.index(depth) == $end.index(depth)) {
    checkJoin(openLeft, openRight)
    addNode(close(openLeft, replaceThreeWay($from, $start, $end, $to, depth + 1)), content)
  } else {
    if (openLeft)
      { addNode(close(openLeft, replaceTwoWay($from, $start, depth + 1)), content) }
    addRange($start, $end, depth, content)
    if (openRight)
      { addNode(close(openRight, replaceTwoWay($end, $to, depth + 1)), content) }
  }
  addRange($to, null, depth, content)
  return new Fragment(content)
}

function replaceTwoWay($from, $to, depth) {
  var content = []
  addRange(null, $from, depth, content)
  if ($from.depth > depth) {
    var type = joinable($from, $to, depth + 1)
    addNode(close(type, replaceTwoWay($from, $to, depth + 1)), content)
  }
  addRange($to, null, depth, content)
  return new Fragment(content)
}

function prepareSliceForReplace(slice, $along) {
  var extra = $along.depth - slice.openLeft, parent = $along.node(extra)
  var node = parent.copy(slice.content)
  for (var i = extra - 1; i >= 0; i--)
    { node = $along.node(i).copy(Fragment.from(node)) }
  return {start: node.resolveNoCache(slice.openLeft + extra),
          end: node.resolveNoCache(node.content.size - slice.openRight - extra)}
}

},{"./fragment":24}],31:[function(require,module,exports){
// ::- You'll often have to '[resolve](#model.Node.resolve)' a
// position to get the context you need. Objects of this class
// represent such a resolved position, providing various pieces of
// context information and helper methods.
//
// Throughout this interface, methods that take an optional `depth`
// parameter will interpret undefined as `this.depth` and negative
// numbers as `this.depth + value`.
var ResolvedPos = function ResolvedPos(pos, path, parentOffset) {
  // :: number The position that was resolved.
  this.pos = pos
  this.path = path
  // :: number
  // The number of levels the parent node is from the root. If this
  // position points directly into the root, it is 0. If it points
  // into a top-level paragraph, 1, and so on.
  this.depth = path.length / 3 - 1
  // :: number The offset this position has into its parent node.
  this.parentOffset = parentOffset
};

var prototypeAccessors = { parent: {},atNodeBoundary: {},nodeAfter: {},nodeBefore: {} };

ResolvedPos.prototype.resolveDepth = function resolveDepth (val) {
  if (val == null) { return this.depth }
  if (val < 0) { return this.depth + val }
  return val
};

// :: Node
// The parent node that the position points into. Note that even if
// a position points into a text node, that node is not considered
// the parent—text nodes are 'flat' in this model.
prototypeAccessors.parent.get = function () { return this.node(this.depth) };

// :: (?number) → Node
// The ancestor node at the given level. `p.node(p.depth)` is the
// same as `p.parent`.
ResolvedPos.prototype.node = function node (depth) { return this.path[this.resolveDepth(depth) * 3] };

// :: (?number) → number
// The index into the ancestor at the given level. If this points at
// the 3rd node in the 2nd paragraph on the top level, for example,
// `p.index(0)` is 2 and `p.index(1)` is 3.
ResolvedPos.prototype.index = function index (depth) { return this.path[this.resolveDepth(depth) * 3 + 1] };

// :: (?number) → number
// The index pointing after this position into the ancestor at the
// given level.
ResolvedPos.prototype.indexAfter = function indexAfter (depth) {
  depth = this.resolveDepth(depth)
  return this.index(depth) + (depth == this.depth && this.atNodeBoundary ? 0 : 1)
};

// :: (?number) → number
// The (absolute) position at the start of the node at the given
// level.
ResolvedPos.prototype.start = function start (depth) {
  depth = this.resolveDepth(depth)
  return depth == 0 ? 0 : this.path[depth * 3 - 1] + 1
};

// :: (?number) → number
// The (absolute) position at the end of the node at the given
// level.
ResolvedPos.prototype.end = function end (depth) {
  depth = this.resolveDepth(depth)
  return this.start(depth) + this.node(depth).content.size
};

// :: (?number) → number
// The (absolute) position directly before the node at the given
// level, or, when `level` is `this.level + 1`, the original
// position.
ResolvedPos.prototype.before = function before (depth) {
  depth = this.resolveDepth(depth)
  if (!depth) { throw new RangeError("There is no position before the top-level node") }
  return depth == this.depth + 1 ? this.pos : this.path[depth * 3 - 1]
};

// :: (?number) → number
// The (absolute) position directly after the node at the given
// level, or, when `level` is `this.level + 1`, the original
// position.
ResolvedPos.prototype.after = function after (depth) {
  depth = this.resolveDepth(depth)
  if (!depth) { throw new RangeError("There is no position after the top-level node") }
  return depth == this.depth + 1 ? this.pos : this.path[depth * 3 - 1] + this.path[depth * 3].nodeSize
};

// :: bool
// True if this position points at a node boundary, false if it
// points into a text node.
prototypeAccessors.atNodeBoundary.get = function () { return this.path[this.path.length - 1] == this.pos };

// :: ?Node
// Get the node directly after the position, if any. If the position
// points into a text node, only the part of that node after the
// position is returned.
prototypeAccessors.nodeAfter.get = function () {
  var parent = this.parent, index = this.index(this.depth)
  if (index == parent.childCount) { return null }
  var dOff = this.pos - this.path[this.path.length - 1], child = parent.child(index)
  return dOff ? parent.child(index).cut(dOff) : child
};

// :: ?Node
// Get the node directly before the position, if any. If the
// position points into a text node, only the part of that node
// before the position is returned.
prototypeAccessors.nodeBefore.get = function () {
  var index = this.index(this.depth)
  var dOff = this.pos - this.path[this.path.length - 1]
  if (dOff) { return this.parent.child(index).cut(0, dOff) }
  return index == 0 ? null : this.parent.child(index - 1)
};

// :: (ResolvedPos) → number
// The depth up to which this position and the other share the same
// parent nodes.
ResolvedPos.prototype.sameDepth = function sameDepth (other) {
  var depth = 0, max = Math.min(this.depth, other.depth)
  while (depth < max && this.index(depth) == other.index(depth)) { ++depth }
  return depth
};

// :: (?ResolvedPos, ?(Node) → bool) → ?NodeRange
// Returns a range based on the place where this position and the
// given position diverge around block content. If both point into
// the same textblock, for example, a range around that textblock
// will be returned. If they point into different blocks, the range
// around those blocks or their ancestors in their common ancestor
// is returned. You can pass in an optional predicate that will be
// called with a parent node to see if a range into that parent is
// acceptable.
ResolvedPos.prototype.blockRange = function blockRange (other, pred) {
    var this$1 = this;
    if ( other === void 0 ) other = this;

  if (other.pos < this.pos) { return other.blockRange(this) }
  for (var d = this.depth - (this.parent.isTextblock || this.pos == other.pos ? 1 : 0); d >= 0; d--)
    { if (other.pos <= this$1.end(d) && (!pred || pred(this$1.node(d))))
      { return new NodeRange(this$1, other, d) } }
};

// :: (ResolvedPos) → bool
// Query whether the given position shares the same parent node.
ResolvedPos.prototype.sameParent = function sameParent (other) {
  return this.pos - this.parentOffset == other.pos - other.parentOffset
};

ResolvedPos.prototype.toString = function toString () {
    var this$1 = this;

  var str = ""
  for (var i = 1; i <= this.depth; i++)
    { str += (str ? "/" : "") + this$1.node(i).type.name + "_" + this$1.index(i - 1) }
  return str + ":" + this.parentOffset
};

ResolvedPos.prototype.plusOne = function plusOne () {
  var copy = this.path.slice(), skip = this.nodeAfter.nodeSize
  copy[copy.length - 2] += 1
  var pos = copy[copy.length - 1] = this.pos + skip
  return new ResolvedPos(pos, copy, this.parentOffset + skip)
};

ResolvedPos.resolve = function resolve (doc, pos) {
  if (!(pos >= 0 && pos <= doc.content.size)) { throw new RangeError("Position " + pos + " out of range") }
  var path = []
  var start = 0, parentOffset = pos
  for (var node = doc;;) {
    var ref = node.content.findIndex(parentOffset);
      var index = ref.index;
      var offset = ref.offset;
    var rem = parentOffset - offset
    path.push(node, index, start + offset)
    if (!rem) { break }
    node = node.child(index)
    if (node.isText) { break }
    parentOffset = rem - 1
    start += offset + 1
  }
  return new ResolvedPos(pos, path, parentOffset)
};

ResolvedPos.resolveCached = function resolveCached (doc, pos) {
  for (var i = 0; i < resolveCache.length; i++) {
    var cached = resolveCache[i]
    if (cached.pos == pos && cached.node(0) == doc) { return cached }
  }
  var result = resolveCache[resolveCachePos] = ResolvedPos.resolve(doc, pos)
  resolveCachePos = (resolveCachePos + 1) % resolveCacheSize
  return result
};

Object.defineProperties( ResolvedPos.prototype, prototypeAccessors );
exports.ResolvedPos = ResolvedPos

var resolveCache = [], resolveCachePos = 0, resolveCacheSize = 6

// ::- Represents a flat range of content.
var NodeRange = function NodeRange($from, $to, depth) {
  // :: ResolvedPos A resolved position along the start of the
  // content. May have a `depth` greater than this object's `depth`
  // property, since these are the positions that were used to
  // compute the range, not re-resolved positions directly at its
  // boundaries.
  this.$from = $from
  // :: ResolvedPos A position along the end of the content. See
  // caveat for [`$from`](#model.NodeRange.$from).
  this.$to = $to
  // :: number The depth of the node that this range points into.
  this.depth = depth
};

var prototypeAccessors$1 = { start: {},end: {},parent: {},startIndex: {},endIndex: {} };

// :: number The position at the start of the range.
prototypeAccessors$1.start.get = function () { return this.$from.before(this.depth + 1) };
// :: number The position at the end of the range.
prototypeAccessors$1.end.get = function () { return this.$to.after(this.depth + 1) };

// :: Node The parent node that the range points into.
prototypeAccessors$1.parent.get = function () { return this.$from.node(this.depth) };
// :: number The start index of the range in the parent node.
prototypeAccessors$1.startIndex.get = function () { return this.$from.index(this.depth) };
// :: number The end index of the range in the parent node.
prototypeAccessors$1.endIndex.get = function () { return this.$to.indexAfter(this.depth) };

Object.defineProperties( NodeRange.prototype, prototypeAccessors$1 );
exports.NodeRange = NodeRange

},{}],32:[function(require,module,exports){
var ref = require("./node");
var Node = ref.Node;
var TextNode = ref.TextNode;
var ref$1 = require("./fragment");
var Fragment = ref$1.Fragment;
var ref$2 = require("./mark");
var Mark = ref$2.Mark;
var ref$3 = require("./content");
var ContentExpr = ref$3.ContentExpr;
var ref$4 = require("./orderedmap");
var OrderedMap = ref$4.OrderedMap;

// For node types where all attrs have a default value (or which don't
// have any attributes), build up a single reusable default attribute
// object, and use it for all nodes that don't specify specific
// attributes.
function defaultAttrs(attrs) {
  var defaults = Object.create(null)
  for (var attrName in attrs) {
    var attr = attrs[attrName]
    if (attr.default === undefined) { return null }
    defaults[attrName] = attr.default
  }
  return defaults
}

function computeAttrs(attrs, value) {
  var built = Object.create(null)
  for (var name in attrs) {
    var given = value && value[name]
    if (given == null) {
      var attr = attrs[name]
      if (attr.default !== undefined)
        { given = attr.default }
      else if (attr.compute)
        { given = attr.compute() }
      else
        { throw new RangeError("No value supplied for attribute " + name) }
    }
    built[name] = given
  }
  return built
}

function initAttrs(attrs) {
  var result = Object.create(null)
  if (attrs) { for (var name in attrs) { result[name] = new Attribute(attrs[name]) } }
  return result
}

// ::- Node types are objects allocated once per `Schema` and used to
// tag `Node` instances with a type. They contain information about
// the node type, such as its name and what kind of node it
// represents.
var NodeType = function NodeType(name, schema, spec) {
  // :: string
  // The name the node type has in this schema.
  this.name = name

  // :: Schema
  // A link back to the `Schema` the node type belongs to.
  this.schema = schema

  // :: NodeSpec
  // The spec that this type is based on
  this.spec = spec

  this.attrs = initAttrs(spec.attrs)

  this.defaultAttrs = defaultAttrs(this.attrs)
  this.contentExpr = null

  // :: bool
  // True if this is a block type
  this.isBlock = !(spec.inline || name == "text")

  // :: bool
  // True if this is the text node type.
  this.isText = name == "text"
};

var prototypeAccessors = { isInline: {},isTextblock: {},isLeaf: {} };

// :: bool
// True if this is an inline type.
prototypeAccessors.isInline.get = function () { return !this.isBlock };

// :: bool
// True if this is a textblock type, a block that contains inline
// content.
prototypeAccessors.isTextblock.get = function () { return this.isBlock && this.contentExpr.inlineContent };

// :: bool
// True for node types that allow no content.
prototypeAccessors.isLeaf.get = function () { return this.contentExpr.isLeaf };

NodeType.prototype.hasRequiredAttrs = function hasRequiredAttrs (ignore) {
    var this$1 = this;

  for (var n in this.attrs)
    { if (this$1.attrs[n].isRequired && (!ignore || !(n in ignore))) { return true } }
  return false
};

NodeType.prototype.compatibleContent = function compatibleContent (other) {
  return this == other || this.contentExpr.compatible(other.contentExpr)
};

NodeType.prototype.computeAttrs = function computeAttrs$1 (attrs) {
  if (!attrs && this.defaultAttrs) { return this.defaultAttrs }
  else { return computeAttrs(this.attrs, attrs) }
};

// :: (?Object, ?union<Fragment, Node, [Node]>, ?[Mark]) → Node
// Create a `Node` of this type. The given attributes are
// checked and defaulted (you can pass `null` to use the type's
// defaults entirely, if no required attributes exist). `content`
// may be a `Fragment`, a node, an array of nodes, or
// `null`. Similarly `marks` may be `null` to default to the empty
// set of marks.
NodeType.prototype.create = function create (attrs, content, marks) {
  if (typeof content == "string") { throw new Error("Calling create with string") }
  return new Node(this, this.computeAttrs(attrs), Fragment.from(content), Mark.setFrom(marks))
};

// :: (?Object, ?union<Fragment, Node, [Node]>, ?[Mark]) → Node
// Like [`create`](#model.NodeType.create), but check the given content
// against the node type's content restrictions, and throw an error
// if it doesn't match.
NodeType.prototype.createChecked = function createChecked (attrs, content, marks) {
  attrs = this.computeAttrs(attrs)
  content = Fragment.from(content)
  if (!this.validContent(content, attrs))
    { throw new RangeError("Invalid content for node " + this.name) }
  return new Node(this, attrs, content, Mark.setFrom(marks))
};

// :: (?Object, ?union<Fragment, Node, [Node]>, ?[Mark]) → ?Node
// Like [`create`](#model.NodeType.create), but see if it is necessary to
// add nodes to the start or end of the given fragment to make it
// fit the node. If no fitting wrapping can be found, return null.
// Note that, due to the fact that required nodes can always be
// created, this will always succeed if you pass null or
// `Fragment.empty` as content.
NodeType.prototype.createAndFill = function createAndFill (attrs, content, marks) {
  attrs = this.computeAttrs(attrs)
  content = Fragment.from(content)
  if (content.size) {
    var before = this.contentExpr.start(attrs).fillBefore(content)
    if (!before) { return null }
    content = before.append(content)
  }
  var after = this.contentExpr.getMatchAt(attrs, content).fillBefore(Fragment.empty, true)
  if (!after) { return null }
  return new Node(this, attrs, content.append(after), Mark.setFrom(marks))
};

// :: (Fragment, ?Object) → bool
// Returns true if the given fragment is valid content for this node
// type with the given attributes.
NodeType.prototype.validContent = function validContent (content, attrs) {
  return this.contentExpr.matches(attrs, content)
};

NodeType.compile = function compile (nodes, schema) {
  var result = Object.create(null)
  nodes.forEach(function (name, spec) { return result[name] = new NodeType(name, schema, spec); })

  if (!result.doc) { throw new RangeError("Every schema needs a 'doc' type") }
  if (!result.text) { throw new RangeError("Every schema needs a 'text' type") }

  return result
};

Object.defineProperties( NodeType.prototype, prototypeAccessors );
exports.NodeType = NodeType

// Attribute descriptors

var Attribute = function Attribute(options) {
  this.default = options.default
  this.compute = options.compute
};

var prototypeAccessors$1 = { isRequired: {} };

prototypeAccessors$1.isRequired.get = function () {
  return this.default === undefined && !this.compute
};

Object.defineProperties( Attribute.prototype, prototypeAccessors$1 );

// Marks

// ::- Like nodes, marks (which are associated with nodes to signify
// things like emphasis or being part of a link) are tagged with type
// objects, which are instantiated once per `Schema`.
var MarkType = function MarkType(name, rank, schema, spec) {
  // :: string
  // The name of the mark type.
  this.name = name

  // :: Schema
  // The schema that this mark type instance is part of.
  this.schema = schema

  // :: MarkSpec
  // The spec on which the type is based.
  this.spec = spec

  this.attrs = initAttrs(spec.attrs)

  this.rank = rank
  var defaults = defaultAttrs(this.attrs)
  this.instance = defaults && new Mark(this, defaults)
};

// :: (?Object) → Mark
// Create a mark of this type. `attrs` may be `null` or an object
// containing only some of the mark's attributes. The others, if
// they have defaults, will be added.
MarkType.prototype.create = function create (attrs) {
  if (!attrs && this.instance) { return this.instance }
  return new Mark(this, computeAttrs(this.attrs, attrs))
};

MarkType.compile = function compile (marks, schema) {
  var result = Object.create(null), rank = 0
  marks.forEach(function (name, spec) { return result[name] = new MarkType(name, rank++, schema, spec); })
  return result
};

// :: ([Mark]) → [Mark]
// When there is a mark of this type in the given set, a new set
// without it is returned. Otherwise, the input set is returned.
MarkType.prototype.removeFromSet = function removeFromSet (set) {
    var this$1 = this;

  for (var i = 0; i < set.length; i++)
    { if (set[i].type == this$1)
      { return set.slice(0, i).concat(set.slice(i + 1)) } }
  return set
};

// :: ([Mark]) → ?Mark
// Tests whether there is a mark of this type in the given set.
MarkType.prototype.isInSet = function isInSet (set) {
    var this$1 = this;

  for (var i = 0; i < set.length; i++)
    { if (set[i].type == this$1) { return set[i] } }
};
exports.MarkType = MarkType

// SchemaSpec:: interface
// An object describing a schema, as passed to the `Schema`
// constructor.
//
//   nodes:: union<Object<NodeSpec>, OrderedMap<NodeSpec>>
//   The node types in this schema. Maps names to `NodeSpec` objects
//   describing the node to be associated with that name. Their order
//   is significant
//
//   marks:: ?union<Object<MarkSpec>, OrderedMap<MarkSpec>>
//   The mark types that exist in this schema.

// NodeSpec:: interface
//
//   content:: ?string
//   The content expression for this node, as described in the [schema
//   guide](guide/schema.html). When not given, the node does not allow
//   any content.
//
//   group:: ?string
//   The group or space-separated groups to which this node belongs, as
//   referred to in the content expressions for the schema.
//
//   inline:: ?bool
//   Should be set to a truthy value for inline nodes. (Implied for
//   text nodes.)
//
//   attrs:: ?Object<AttributeSpec>
//   The attributes that nodes of this type get.
//
//   selectable:: ?bool
//   Controls whether nodes of this type can be selected (as a [node
//   selection](#state.NodeSelection)). Defaults to true for non-text
//   nodes.
//
//   draggable:: ?bool
//   Determines whether nodes of this type can be dragged. Enabling it
//   causes ProseMirror to set a `draggable` attribute on its DOM
//   representation, and to put its HTML serialization into the drag
//   event's [data
//   transfer](https://developer.mozilla.org/en-US/docs/Web/API/DataTransfer)
//   when dragged. Defaults to false.
//
//   code:: ?bool
//   Can be used to indicate that this node contains code, which
//   causes some commands to behave differently.
//
//   toDOM:: ?(Node) → DOMOutputSpec
//   Defines the default way a node of this type should be serialized
//   to DOM/HTML (as used by
//   [`DOMSerializer.fromSchema`](#model.DOMSerializer^fromSchema).
//   Should return an [array structure](#model.DOMOutputSpec) that
//   describes the resulting DOM structure, with an optional number
//   zero (“hole”) in it to indicate where the node's content should
//   be inserted.
//
//   parseDOM:: ?[ParseRule]
//   Associates DOM parser information with this node, which an be
//   used by [`DOMParser.fromSchema`](#model.DOMParser^fromSchema) to
//   automatically derive a parser. The `node` field in the rules is
//   implied (the name of this node will be filled in automatically).
//   If you supply your own parser, you do not need to also specify
//   parsing rules in your schema.

// MarkSpec:: interface
//
//   attrs:: ?Object<AttributeSpec>
//   The attributes that marks of this type get.
//
//   inclusiveRight:: ?bool
//   Whether this mark should be active when the cursor is positioned
//   at the end of the mark. Defaults to true.
//
//   toDOM:: ?(mark: Mark) → DOMOutputSpec
//   Defines the default way marks of this type should be serialized
//   to DOM/HTML.
//
//   parseDOM:: ?[ParseRule]
//   Associates DOM parser information with this mark (see the
//   corresponding [node spec field](#model.NodeSpec.parseDOM). The
//   `mark` field in the rules is implied.

// AttributeSpec:: interface
//
// Used to define attributes. Attributes that have no default or
// compute property must be provided whenever a node or mark of a type
// that has them is created.
//
// The following fields are supported:
//
//   default:: ?any
//   The default value for this attribute, to choose when no
//   explicit value is provided.
//
//   compute:: ?() → any
//   A function that computes a default value for the attribute.

// ::- A document schema.
var Schema = function Schema(spec) {
  var this$1 = this;

  // :: OrderedMap<NodeSpec> The node specs that the schema is based on.
  this.nodeSpec = OrderedMap.from(spec.nodes)
  // :: OrderedMap<constructor<MarkType>> The mark spec that the schema is based on.
  this.markSpec = OrderedMap.from(spec.marks)

  // :: Object<NodeType>
  // An object mapping the schema's node names to node type objects.
  this.nodes = NodeType.compile(this.nodeSpec, this)

  // :: Object<MarkType>
  // A map from mark names to mark type objects.
  this.marks = MarkType.compile(this.markSpec, this)

  for (var prop in this.nodes) {
    if (prop in this$1.marks)
      { throw new RangeError(prop + " can not be both a node and a mark") }
    var type = this$1.nodes[prop]
    type.contentExpr = ContentExpr.parse(type, this$1.nodeSpec.get(prop).content || "", this$1.nodeSpec)
  }

  // :: Object
  // An object for storing whatever values modules may want to
  // compute and cache per schema. (If you want to store something
  // in it, try to use property names unlikely to clash.)
  this.cached = Object.create(null)
  this.cached.wrappings = Object.create(null)

  this.nodeFromJSON = this.nodeFromJSON.bind(this)
  this.markFromJSON = this.markFromJSON.bind(this)
};

// :: (union<string, NodeType>, ?Object, ?union<Fragment, Node, [Node]>, ?[Mark]) → Node
// Create a node in this schema. The `type` may be a string or a
// `NodeType` instance. Attributes will be extended
// with defaults, `content` may be a `Fragment`,
// `null`, a `Node`, or an array of nodes.
Schema.prototype.node = function node (type, attrs, content, marks) {
  if (typeof type == "string")
    { type = this.nodeType(type) }
  else if (!(type instanceof NodeType))
    { throw new RangeError("Invalid node type: " + type) }
  else if (type.schema != this)
    { throw new RangeError("Node type from different schema used (" + type.name + ")") }

  return type.createChecked(attrs, content, marks)
};

// :: (string, ?[Mark]) → Node
// Create a text node in the schema. Empty text nodes are not
// allowed.
Schema.prototype.text = function text (text, marks) {
  var type = this.nodes.text
  return new TextNode(type, type.defaultAttrs, text, Mark.setFrom(marks))
};

// :: (union<string, MarkType>, ?Object) → Mark
// Create a mark with the given type and attributes.
Schema.prototype.mark = function mark (type, attrs) {
  if (typeof type == "string") { type = this.marks[type] }
  return type.create(attrs)
};

// :: (Object) → Node
// Deserialize a node from its JSON representation. This method is
// bound.
Schema.prototype.nodeFromJSON = function nodeFromJSON (json) {
  return Node.fromJSON(this, json)
};

// :: (Object) → Mark
// Deserialize a mark from its JSON representation. This method is
// bound.
Schema.prototype.markFromJSON = function markFromJSON (json) {
  return Mark.fromJSON(this, json)
};

Schema.prototype.nodeType = function nodeType (name) {
  var found = this.nodes[name]
  if (!found) { throw new RangeError("Unknown node type: " + name) }
  return found
};
exports.Schema = Schema

},{"./content":22,"./fragment":24,"./mark":27,"./node":28,"./orderedmap":29}],33:[function(require,module,exports){
// DOMOutputSpec:: interface
// A description of a DOM structure. Can be either a string, which is
// interpreted as a text node, a DOM node, which is interpreted as
// itself, or an array.
//
// An array describes a DOM element. The first element in the array
// should be a string, and is the name of the DOM element. If the
// second element is a non-Array, non-DOM node object, it is
// interpreted as an object providing the DOM element's attributes.
// Any elements after that (including the 2nd if it's not an attribute
// object) are interpreted as children of the DOM elements, and must
// either be valid `DOMOutputSpec` values, or the number zero.
//
// The number zero (pronounced “hole”) is used to indicate the place
// where a ProseMirror node's content should be inserted.

// ::- A DOM serializer knows how to convert ProseMirror nodes and
// marks of various types to DOM nodes.
var DOMSerializer = function DOMSerializer(nodes, marks) {
  this.nodes = nodes || {}
  this.marks = marks || {}
};

// :: (Fragment, ?Object) → dom.DocumentFragment
// Serialize the content of this fragment to a DOM fragment. When
// not in the browser, the `document` option, containing a DOM
// document, should be passed so that the serialize can create
// nodes.
DOMSerializer.prototype.serializeFragment = function serializeFragment (fragment, options, target) {
    var this$1 = this;
    if ( options === void 0 ) options = {};

  if (!target) { target = doc(options).createDocumentFragment() }

  var top = target, active = null
  fragment.forEach(function (node) {
    if (active || node.marks.length) {
      if (!active) { active = [] }
      var keep = 0
      for (; keep < Math.min(active.length, node.marks.length); ++keep)
        { if (!node.marks[keep].eq(active[keep])) { break } }
      while (keep < active.length) {
        active.pop()
        top = top.parentNode
      }
      while (active.length < node.marks.length) {
        var add = node.marks[active.length]
        active.push(add)
        top = top.appendChild(this$1.serializeMark(add, options))
      }
    }
    top.appendChild(this$1.serializeNode(node, options))
  })

  return target
};

// :: (Node, ?Object) → dom.Node
// Serialize this node to a DOM node. This can be useful when you
// need to serialize a part of a document, as opposed to the whole
// document. To serialize a whole document, use
// [`serializeFragment`](#model.DOMSerializer.serializeFragment) on
// its [`content`](#model.Node.content).
DOMSerializer.prototype.serializeNode = function serializeNode (node, options) {
    if ( options === void 0 ) options = {};

  return this.renderStructure(this.nodes[node.type.name](node), node, options)
};

DOMSerializer.prototype.serializeNodeAndMarks = function serializeNodeAndMarks (node, options) {
    var this$1 = this;
    if ( options === void 0 ) options = {};

  var dom = this.serializeNode(node, options)
  for (var i = node.marks.length - 1; i >= 0; i--) {
    var wrap = this$1.serializeMark(node.marks[i], options)
    wrap.appendChild(dom)
    dom = wrap
  }
  return dom
};

DOMSerializer.prototype.serializeMark = function serializeMark (mark, options) {
    if ( options === void 0 ) options = {};

  return this.renderStructure(this.marks[mark.type.name](mark), null, options)
};

DOMSerializer.prototype.renderStructure = function renderStructure (structure, node, options) {
    var this$1 = this;

  if (typeof structure == "string")
    { return doc(options).createTextNode(structure) }
  if (structure.nodeType != null)
    { return structure }
  var dom = doc(options).createElement(structure[0]), attrs = structure[1], start = 1
  if (attrs && typeof attrs == "object" && attrs.nodeType == null && !Array.isArray(attrs)) {
    start = 2
    for (var name in attrs) {
      if (name == "style") { dom.style.cssText = attrs[name] }
      else if (attrs[name] != null) { dom.setAttribute(name, attrs[name]) }
    }
  }
  for (var i = start; i < structure.length; i++) {
    var child = structure[i]
    if (child === 0) {
      if (!node || node.isLeaf)
        { throw new RangeError("Content hole not allowed in a mark or leaf node spec (must produce a single node)") }
      if (i < structure.length - 1 || i > start)
        { throw new RangeError("Content hole must be the only child of its parent node") }
      if (options.onContent)
        { options.onContent(node, dom, options) }
      else
        { this$1.serializeFragment(node.content, options, dom) }
    } else {
      dom.appendChild(this$1.renderStructure(child, node, options))
    }
  }
  return dom
};

// :: (Schema) → DOMSerializer
// Build a serializer using the [`toDOM`](#model.NodeSpec.toDOM)
// properties in a schema's node and mark specs.
DOMSerializer.fromSchema = function fromSchema (schema) {
  return schema.cached.domSerializer ||
    (schema.cached.domSerializer = new DOMSerializer(this.nodesFromSchema(schema), this.marksFromSchema(schema)))
};

// :: (Schema) → Object<(node: Node) → DOMOutputSpec>
// Gather the serializers in a schema's node specs into an object.
// This can be useful as a base to build a custom serializer from.
DOMSerializer.nodesFromSchema = function nodesFromSchema (schema) {
  return gatherToDOM(schema.nodes)
};

// :: (Schema) → Object<(mark: Mark) → DOMOutputSpec>
// Gather the serializers in a schema's mark specs into an object.
DOMSerializer.marksFromSchema = function marksFromSchema (schema) {
  return gatherToDOM(schema.marks)
};
exports.DOMSerializer = DOMSerializer

function gatherToDOM(obj) {
  var result = {}
  for (var name in obj) {
    var toDOM = obj[name].spec.toDOM
    if (toDOM) { result[name] = toDOM }
  }
  return result
}

function doc(options) {
  // declare global: window
  return options.document || window.document
}

},{}],34:[function(require,module,exports){
var ref = require("prosemirror-model");
var Schema = ref.Schema;

// :: Object
//
//   doc:: NodeSpec The top level document node.
//
//   paragraph:: NodeSpec A plain paragraph textblock.
//
//   blockquote:: NodeSpec A blockquote wrapping one or more blocks.
//
//   horizontal_rule:: NodeSpec A horizontal rule.
//
//   heading:: NodeSpec A heading textblock, with a `level`
//   attribute that should hold the number 1 to 6.
//
//   code_block:: NodeSpec A code listing. Disallows marks or
//   non-text inline nodes by default.
//
//   text:: NodeSpec The text node.
//
//   image:: NodeSpec An inline image node. Supports `src`, `alt`, and
//   `href` attributes. The latter two default to the empty string.
//
//   hard_break:: NodeSpec A hard line break.
var nodes = {
  doc: {
    content: "block+"
  },

  paragraph: {
    content: "inline<_>*",
    group: "block",
    parseDOM: [{tag: "p"}],
    toDOM: function toDOM() { return ["p", 0] }
  },

  blockquote: {
    content: "block+",
    group: "block",
    parseDOM: [{tag: "blockquote"}],
    toDOM: function toDOM$1() { return ["blockquote", 0] }
  },

  horizontal_rule: {
    group: "block",
    parseDOM: [{tag: "hr"}],
    toDOM: function toDOM$2() { return ["div", ["hr"]] }
  },

  heading: {
    attrs: {level: {default: 1}},
    content: "inline<_>*",
    group: "block",
    parseDOM: [{tag: "h1", attrs: {level: 1}},
               {tag: "h2", attrs: {level: 2}},
               {tag: "h3", attrs: {level: 3}},
               {tag: "h4", attrs: {level: 4}},
               {tag: "h5", attrs: {level: 5}},
               {tag: "h6", attrs: {level: 6}}],
    toDOM: function toDOM$3(node) { return ["h" + node.attrs.level, 0] }
  },

  code_block: {
    content: "text*",
    group: "block",
    code: true,
    parseDOM: [{tag: "pre", preserveWhitespace: true}],
    toDOM: function toDOM$4() { return ["pre", ["code", 0]] }
  },

  text: {
    group: "inline",
    toDOM: function toDOM$5(node) { return node.text }
  },

  image: {
    inline: true,
    attrs: {
      src: {},
      alt: {default: null},
      title: {default: null}
    },
    group: "inline",
    draggable: true,
    parseDOM: [{tag: "img[src]", getAttrs: function getAttrs(dom) {
      return {
        src: dom.getAttribute("src"),
        title: dom.getAttribute("title"),
        alt: dom.getAttribute("alt")
      }
    }}],
    toDOM: function toDOM$6(node) { return ["img", node.attrs] }
  },

  hard_break: {
    inline: true,
    group: "inline",
    selectable: false,
    parseDOM: [{tag: "br"}],
    toDOM: function toDOM$7() { return ["br"] }
  }
}
exports.nodes = nodes

// :: Object
//
//  em:: MarkSpec An emphasis mark.
//
//  strong:: MarkSpec A strong mark.
//
//  link:: MarkSpec A link. Has `href` and `title` attributes.
//  `title` defaults to the empty string.
//
//  code:: MarkSpec Code font mark.
var marks = {
  em: {
    parseDOM: [{tag: "i"}, {tag: "em"},
               {style: "font-style", getAttrs: function (value) { return value == "italic" && null; }}],
    toDOM: function toDOM$8() { return ["em"] }
  },

  strong: {
    parseDOM: [{tag: "strong"},
               // This works around a Google Docs misbehavior where
               // pasted content will be inexplicably wrapped in `<b>`
               // tags with a font-weight normal.
               {tag: "b", getAttrs: function (node) { return node.style.fontWeight != "normal" && null; }},
               {style: "font-weight", getAttrs: function (value) { return /^(bold(er)?|[5-9]\d{2,})$/.test(value) && null; }}],
    toDOM: function toDOM$9() { return ["strong"] }
  },

  link: {
    attrs: {
      href: {},
      title: {default: null}
    },
    parseDOM: [{tag: "a[href]", getAttrs: function getAttrs$1(dom) {
      return {href: dom.getAttribute("href"), title: dom.getAttribute("title")}
    }}],
    toDOM: function toDOM$10(node) { return ["a", node.attrs] }
  },

  code: {
    parseDOM: [{tag: "code"}],
    toDOM: function toDOM$11() { return ["code"] }
  }
}
exports.marks = marks

// :: Schema
// This schema rougly corresponds to the document schema used by
// CommonMark, minus the list elements, which are defined in the
// [schema-list](#schema-list) module.
//
// To reuse elements from this schema, extend or read from its
// [`nodeSpec`](#model.Schema.nodeSpec) and
// [`markSpec`](#model.Schema.markSpec) properties.
var schema = new Schema({nodes: nodes, marks: marks})
exports.schema = schema

},{"prosemirror-model":26}],35:[function(require,module,exports){
var ref = require("prosemirror-transform");
var findWrapping = ref.findWrapping;
var liftTarget = ref.liftTarget;
var canSplit = ref.canSplit;
var ReplaceAroundStep = ref.ReplaceAroundStep;
var ref$1 = require("prosemirror-model");
var Slice = ref$1.Slice;
var Fragment = ref$1.Fragment;
var NodeRange = ref$1.NodeRange;

// :: NodeSpec
// An ordered list node type spec. Has a single attribute, `order`,
// which determines the number at which the list starts counting, and
// defaults to 1.
var orderedList = {
  attrs: {order: {default: 1}},
  parseDOM: [{tag: "ol", getAttrs: function getAttrs(dom) {
    return {order: dom.hasAttribute("start") ? +dom.getAttribute("start") : 1}
  }}],
  toDOM: function toDOM(node) {
    return ["ol", {start: node.attrs.order == 1 ? null : node.attrs.order}, 0]
  }
}
exports.orderedList = orderedList

// :: NodeSpec
// A bullet list node spec.
var bulletList = {
  parseDOM: [{tag: "ul"}],
  toDOM: function toDOM$1() { return ["ul", 0] }
}
exports.bulletList = bulletList

// :: NodeSpec
// A list item node spec.
var listItem = {
  parseDOM: [{tag: "li"}],
  toDOM: function toDOM$2() { return ["li", 0] }
}
exports.listItem = listItem

function add(obj, props) {
  var copy = {}
  for (var prop in obj) { copy[prop] = obj[prop] }
  for (var prop$1 in props) { copy[prop$1] = props[prop$1] }
  return copy
}

// :: (OrderedMap, string, ?string) → OrderedMap
// Convenience function for adding list-related node types to a map
// describing the nodes in a schema. Adds `OrderedList` as
// `"ordered_list"`, `BulletList` as `"bullet_list"`, and `ListItem`
// as `"list_item"`. `itemContent` determines the content expression
// for the list items. If you want the commands defined in this module
// to apply to your list structure, it should have a shape like
// `"paragraph block*"`, a plain textblock type followed by zero or
// more arbitrary nodes. `listGroup` can be given to assign a group
// name to the list node types, for example `"block"`.
function addListNodes(nodes, itemContent, listGroup) {
  return nodes.append({
    ordered_list: add(orderedList, {content: "list_item+", group: listGroup}),
    bullet_list: add(bulletList, {content: "list_item+", group: listGroup}),
    list_item: add(listItem, {content: itemContent})
  })
}
exports.addListNodes = addListNodes

// :: (NodeType, ?Object) → (state: EditorState, onAction: ?(action: Action)) → bool
// Returns a command function that wraps the selection in a list with
// the given type an attributes. If `apply` is `false`, only return a
// value to indicate whether this is possible, but don't actually
// perform the change.
function wrapInList(nodeType, attrs) {
  return function(state, onAction) {
    var ref = state.selection;
    var $from = ref.$from;
    var $to = ref.$to;
    var range = $from.blockRange($to), doJoin = false, outerRange = range
    // This is at the top of an existing list item
    if (range.depth >= 2 && $from.node(range.depth - 1).type.compatibleContent(nodeType) && range.startIndex == 0) {
      // Don't do anything if this is the top of the list
      if ($from.index(range.depth - 1) == 0) { return false }
      var $insert = state.doc.resolve(range.start - 2)
      outerRange = new NodeRange($insert, $insert, range.depth)
      if (range.endIndex < range.parent.childCount)
        { range = new NodeRange($from, state.doc.resolve($to.end(range.depth)), range.depth) }
      doJoin = true
    }
    var wrap = findWrapping(outerRange, nodeType, attrs, range)
    if (!wrap) { return false }
    if (onAction) { onAction(doWrapInList(state.tr, range, wrap, doJoin, nodeType).scrollAction()) }
    return true
  }
}
exports.wrapInList = wrapInList

function doWrapInList(tr, range, wrappers, joinBefore, nodeType) {
  var content = Fragment.empty
  for (var i = wrappers.length - 1; i >= 0; i--)
    { content = Fragment.from(wrappers[i].type.create(wrappers[i].attrs, content)) }

  tr.step(new ReplaceAroundStep(range.start - (joinBefore ? 2 : 0), range.end, range.start, range.end,
                                new Slice(content, 0, 0), wrappers.length, true))

  var found = 0
  for (var i$1 = 0; i$1 < wrappers.length; i$1++) { if (wrappers[i$1].type == nodeType) { found = i$1 + 1 } }
  var splitDepth = wrappers.length - found

  var splitPos = range.start + wrappers.length - (joinBefore ? 2 : 0), parent = range.parent
  for (var i$2 = range.startIndex, e = range.endIndex, first = true; i$2 < e; i$2++, first = false) {
    if (!first && canSplit(tr.doc, splitPos, splitDepth)) { tr.split(splitPos, splitDepth) }
    splitPos += parent.child(i$2).nodeSize + (first ? 0 : 2 * splitDepth)
  }
  return tr
}

// :: (NodeType) → (state: EditorState) → bool
// Build a command that splits a non-empty textblock at the top level
// of a list item by also splitting that list item.
function splitListItem(nodeType) {
  return function(state, onAction) {
    var ref = state.selection;
    var $from = ref.$from;
    var $to = ref.$to;
    var node = ref.node;
    if ((node && node.isBlock) || !$from.parent.content.size ||
        $from.depth < 2 || !$from.sameParent($to)) { return false }
    var grandParent = $from.node(-1)
    if (grandParent.type != nodeType) { return false }
    var nextType = $to.pos == $from.end() ? grandParent.defaultContentType($from.indexAfter(-1)) : null
    var tr = state.tr.delete($from.pos, $to.pos)
    var types = nextType && [null, {type: nextType}]
    if (!canSplit(tr.doc, $from.pos, 2, types)) { return false }
    if (onAction) { onAction(tr.split($from.pos, 2, types).scrollAction()) }
    return true
  }
}
exports.splitListItem = splitListItem

// :: (NodeType) → (state: EditorState, onAction: ?(action: Action)) → bool
// Create a command to lift the list item around the selection up into
// a wrapping list.
function liftListItem(nodeType) {
  return function(state, onAction) {
    var ref = state.selection;
    var $from = ref.$from;
    var $to = ref.$to;
    var range = $from.blockRange($to, function (node) { return node.childCount && node.firstChild.type == nodeType; })
    if (!range || range.depth < 2 || $from.node(range.depth - 1).type != nodeType) { return false }
    if (onAction) {
      var tr = state.tr, end = range.end, endOfList = $to.end(range.depth)
      if (end < endOfList) {
        // There are siblings after the lifted items, which must become
        // children of the last item
        tr.step(new ReplaceAroundStep(end - 1, endOfList, end, endOfList,
                                      new Slice(Fragment.from(nodeType.create(null, range.parent.copy())), 1, 0), 1, true))
        range = new NodeRange(tr.doc.resolveNoCache($from.pos), tr.doc.resolveNoCache(endOfList), range.depth)
      }
      onAction(tr.lift(range, liftTarget(range)).scrollAction())
    }
    return true
  }
}
exports.liftListItem = liftListItem

// :: (NodeType) → (state: EditorState, onAction: ?(action: Action)) → bool
// Create a command to sink the list item around the selection down
// into an inner list.
function sinkListItem(nodeType) {
  return function(state, onAction) {
    var ref = state.selection;
    var $from = ref.$from;
    var $to = ref.$to;
    var range = $from.blockRange($to, function (node) { return node.childCount && node.firstChild.type == nodeType; })
    if (!range) { return false }
    var startIndex = range.startIndex
    if (startIndex == 0) { return false }
    var parent = range.parent, nodeBefore = parent.child(startIndex - 1)
    if (nodeBefore.type != nodeType) { return false }

    if (onAction) {
      var nestedBefore = nodeBefore.lastChild && nodeBefore.lastChild.type == parent.type
      var inner = Fragment.from(nestedBefore ? nodeType.create() : null)
      var slice = new Slice(Fragment.from(nodeType.create(null, Fragment.from(parent.copy(inner)))),
                            nestedBefore ? 3 : 1, 0)
      var before = range.start, after = range.end
      onAction(state.tr.step(new ReplaceAroundStep(before - (nestedBefore ? 3 : 1), after,
                                                   before, after, slice, 1, true))
               .scrollAction())
    }
    return true
  }
}
exports.sinkListItem = sinkListItem

},{"prosemirror-model":26,"prosemirror-transform":42}],36:[function(require,module,exports){
var ref = require("prosemirror-model");
var Fragment = ref.Fragment;
var Slice = ref.Slice;
var ref$1 = require("prosemirror-transform");
var Step = ref$1.Step;
var StepResult = ref$1.StepResult;
var StepMap = ref$1.StepMap;
var ReplaceStep = ref$1.ReplaceStep;
var ref$2 = require("prosemirror-state");
var Selection = ref$2.Selection;

// :: NodeSpec
// A table node spec. Has one attribute, **`columns`**, which holds
// a number indicating the amount of columns in the table.
var table = {
  attrs: {columns: {default: 1}},
  parseDOM: [{tag: "table", getAttrs: function getAttrs(dom) {
    var row = dom.querySelector("tr")
    if (!row || !row.children.length) { return false }
    // FIXME using the child count as column width is problematic
    // when parsing document fragments
    return {columns: row.children.length}
  }}],
  toDOM: function toDOM() { return ["table", ["tbody", 0]] }
}
exports.table = table

// :: NodeSpec
// A table row node spec. Has one attribute, **`columns`**, which
// holds a number indicating the amount of columns in the table.
var tableRow = {
  attrs: {columns: {default: 1}},
  parseDOM: [{tag: "tr", getAttrs: function (dom) { return dom.children.length ? {columns: dom.children.length} : false; }}],
  toDOM: function toDOM$1() { return ["tr", 0] },
  tableRow: true
}
exports.tableRow = tableRow

// :: NodeSpec
// A table cell node spec.
var tableCell = {
  parseDOM: [{tag: "td"}],
  toDOM: function toDOM$2() { return ["td", 0] }
}
exports.tableCell = tableCell

function add(obj, props) {
  var copy = {}
  for (var prop in obj) { copy[prop] = obj[prop] }
  for (var prop$1 in props) { copy[prop$1] = props[prop$1] }
  return copy
}

// :: (OrderedMap, string, ?string) → OrderedMap
// Convenience function for adding table-related node types to a map
// describing the nodes in a schema. Adds `Table` as `"table"`,
// `TableRow` as `"table_row"`, and `TableCell` as `"table_cell"`.
// `cellContent` should be a content expression describing what may
// occur inside cells.
function addTableNodes(nodes, cellContent, tableGroup) {
  return nodes.append({
    table: add(table, {content: "table_row[columns=.columns]+", group: tableGroup}),
    table_row: add(tableRow, {content: "table_cell{.columns}"}),
    table_cell: add(tableCell, {content: cellContent})
  })
}
exports.addTableNodes = addTableNodes

// :: (NodeType, number, number, ?Object) → Node
// Create a table node with the given number of rows and columns.
function createTable(nodeType, rows, columns, attrs) {
  attrs = setColumns(attrs, columns)
  var rowType = nodeType.contentExpr.elements[0].nodeTypes[0]
  var cellType = rowType.contentExpr.elements[0].nodeTypes[0]
  var cell = cellType.createAndFill(), cells = []
  for (var i = 0; i < columns; i++) { cells.push(cell) }
  var row = rowType.create({columns: columns}, Fragment.from(cells)), rowNodes = []
  for (var i$1 = 0; i$1 < rows; i$1++) { rowNodes.push(row) }
  return nodeType.create(attrs, Fragment.from(rowNodes))
}
exports.createTable = createTable

// Steps to add and remove a column

function setColumns(attrs, columns) {
  var result = Object.create(null)
  if (attrs) { for (var prop in attrs) { result[prop] = attrs[prop] } }
  result.columns = columns
  return result
}

function adjustColumns(attrs, diff) {
  return setColumns(attrs, attrs.columns + diff)
}

// ::- A `Step` subclass for adding a column to a table in a single
// atomic step.
var AddColumnStep = (function (Step) {
  function AddColumnStep(positions, cells) {
    Step.call(this)
    this.positions = positions
    this.cells = cells
  }

  if ( Step ) AddColumnStep.__proto__ = Step;
  AddColumnStep.prototype = Object.create( Step && Step.prototype );
  AddColumnStep.prototype.constructor = AddColumnStep;

  // :: (Node, number, number, NodeType, ?Object) → AddColumnStep
  // Create a step that inserts a column into the table after
  // `tablePos`, at the index given by `columnIndex`, using cells with
  // the given type and attributes.
  AddColumnStep.create = function create (doc, tablePos, columnIndex, cellType, cellAttrs) {
    var cell = cellType.createAndFill(cellAttrs)
    var positions = [], cells = []
    var table = doc.nodeAt(tablePos)
    table.forEach(function (row, rowOff) {
      var cellPos = tablePos + 2 + rowOff
      for (var i = 0; i < columnIndex; i++) { cellPos += row.child(i).nodeSize }
      positions.push(cellPos)
      cells.push(cell)
    })
    return new AddColumnStep(positions, cells)
  };

  AddColumnStep.prototype.apply = function apply (doc) {
    var this$1 = this;

    var index = null, table = null, tablePos = null
    for (var i = 0; i < this.positions.length; i++) {
      var $pos = doc.resolve(this$1.positions[i])
      if ($pos.depth < 2 || $pos.index(-1) != i)
        { return StepResult.fail("Invalid cell insert position") }
      if (table == null) {
        table = $pos.node(-1)
        if (table.childCount != this$1.positions.length)
          { return StepResult.fail("Mismatch in number of rows") }
        tablePos = $pos.before(-1)
        index = $pos.index()
      } else if ($pos.before(-1) != tablePos || $pos.index() != index) {
        return StepResult.fail("Column insert positions not consistent")
      }
    }

    var updatedRows = []
    for (var i$1 = 0; i$1 < table.childCount; i$1++) {
      var row = table.child(i$1), rowCells = index ? [] : [this$1.cells[i$1]]
      for (var j = 0; j < row.childCount; j++) {
        rowCells.push(row.child(j))
        if (j + 1 == index) { rowCells.push(this$1.cells[i$1]) }
      }
      updatedRows.push(row.type.create(adjustColumns(row.attrs, 1), Fragment.from(rowCells)))
    }
    var updatedTable = table.type.create(adjustColumns(table.attrs, 1),  Fragment.from(updatedRows))
    return StepResult.fromReplace(doc, tablePos, tablePos + table.nodeSize,
                                  new Slice(Fragment.from(updatedTable), 0, 0))
  };

  AddColumnStep.prototype.getMap = function getMap () {
    var this$1 = this;

    var ranges = []
    for (var i = 0; i < this.positions.length; i++)
      { ranges.push(this$1.positions[i], 0, this$1.cells[i].nodeSize) }
    return new StepMap(ranges)
  };

  AddColumnStep.prototype.invert = function invert (doc) {
    var this$1 = this;

    var $first = doc.resolve(this.positions[0])
    var table = $first.node(-1)
    var from = [], to = [], dPos = 0
    for (var i = 0; i < table.childCount; i++) {
      var pos = this$1.positions[i] + dPos, size = this$1.cells[i].nodeSize
      from.push(pos)
      to.push(pos + size)
      dPos += size
    }
    return new RemoveColumnStep(from, to)
  };

  AddColumnStep.prototype.map = function map (mapping) {
    return new AddColumnStep(this.positions.map(function (p) { return mapping.map(p); }), this.cells)
  };

  AddColumnStep.prototype.toJSON = function toJSON () {
    return {stepType: this.jsonID,
            positions: this.positions,
            cells: this.cells.map(function (c) { return c.toJSON(); })}
  };

  AddColumnStep.fromJSON = function fromJSON (schema, json) {
    return new AddColumnStep(json.positions, json.cells.map(schema.nodeFromJSON))
  };

  return AddColumnStep;
}(Step));
exports.AddColumnStep = AddColumnStep

Step.jsonID("addTableColumn", AddColumnStep)

// ::- A subclass of `Step` that removes a column from a table.
var RemoveColumnStep = (function (Step) {
  function RemoveColumnStep(from, to) {
    Step.call(this)
    this.from = from
    this.to = to
  }

  if ( Step ) RemoveColumnStep.__proto__ = Step;
  RemoveColumnStep.prototype = Object.create( Step && Step.prototype );
  RemoveColumnStep.prototype.constructor = RemoveColumnStep;

  // :: (Node, number, number) → RemoveColumnStep
  // Create a step that deletes the column at `columnIndex` in the
  // table after `tablePos`.
  RemoveColumnStep.create = function create (doc, tablePos, columnIndex) {
    var from = [], to = []
    var table = doc.nodeAt(tablePos)
    table.forEach(function (row, rowOff) {
      var cellPos = tablePos + 2 + rowOff
      for (var i = 0; i < columnIndex; i++) { cellPos += row.child(i).nodeSize }
      from.push(cellPos)
      to.push(cellPos + row.child(columnIndex).nodeSize)
    })
    return new RemoveColumnStep(from, to)
  };

  RemoveColumnStep.prototype.apply = function apply (doc) {
    var this$1 = this;

    var index = null, table = null, tablePos = null
    for (var i = 0; i < this.from.length; i++) {
      var $from = doc.resolve(this$1.from[i]), after = $from.nodeAfter
      if ($from.depth < 2 || $from.index(-1) != i || !after || this$1.from[i] + after.nodeSize != this$1.to[i])
        { return StepResult.fail("Invalid cell delete positions") }
      if (table == null) {
        table = $from.node(-1)
        if (table.childCount != this$1.from.length)
          { return StepResult.fail("Mismatch in number of rows") }
        tablePos = $from.before(-1)
        index = $from.index()
      } else if ($from.before(-1) != tablePos || $from.index() != index) {
        return StepResult.fail("Column delete positions not consistent")
      }
    }

    var updatedRows = []
    for (var i$1 = 0; i$1 < table.childCount; i$1++) {
      var row = table.child(i$1), rowCells = []
      for (var j = 0; j < row.childCount; j++)
        { if (j != index) { rowCells.push(row.child(j)) } }
      updatedRows.push(row.type.create(adjustColumns(row.attrs, -1), Fragment.from(rowCells)))
    }
    var updatedTable = table.type.create(adjustColumns(table.attrs, -1),  Fragment.from(updatedRows))
    return StepResult.fromReplace(doc, tablePos, tablePos + table.nodeSize,
                                  new Slice(Fragment.from(updatedTable), 0, 0))
  };

  RemoveColumnStep.prototype.getMap = function getMap () {
    var this$1 = this;

    var ranges = []
    for (var i = 0; i < this.from.length; i++)
      { ranges.push(this$1.from[i], this$1.to[i] - this$1.from[i], 0) }
    return new StepMap(ranges)
  };

  RemoveColumnStep.prototype.invert = function invert (doc) {
    var this$1 = this;

    var $first = doc.resolve(this.from[0])
    var table = $first.node(-1), index = $first.index()
    var positions = [], cells = [], dPos = 0
    for (var i = 0; i < table.childCount; i++) {
      positions.push(this$1.from[i] - dPos)
      var cell = table.child(i).child(index)
      dPos += cell.nodeSize
      cells.push(cell)
    }
    return new AddColumnStep(positions, cells)
  };

  RemoveColumnStep.prototype.map = function map (mapping) {
    var this$1 = this;

    var from = [], to = []
    for (var i = 0; i < this.from.length; i++) {
      var start = mapping.map(this$1.from[i], 1), end = mapping.map(this$1.to[i], -1)
      if (end <= start) { return null }
      from.push(start)
      to.push(end)
    }
    return new RemoveColumnStep(from, to)
  };

  RemoveColumnStep.fromJSON = function fromJSON (_schema, json) {
    return new RemoveColumnStep(json.from, json.to)
  };

  return RemoveColumnStep;
}(Step));
exports.RemoveColumnStep = RemoveColumnStep

Step.jsonID("removeTableColumn", RemoveColumnStep)

// Table-related command functions

function findRow($pos, pred) {
  for (var d = $pos.depth; d > 0; d--)
    { if ($pos.node(d).type.spec.tableRow && (!pred || pred(d))) { return d } }
  return -1
}

// :: (EditorState, onAction: ?(action: Action)) → bool
// Command function that adds a column before the column with the
// selection.
function addColumnBefore(state, onAction) {
  var $from = state.selection.$from, cellFrom
  var rowDepth = findRow($from, function (d) { return cellFrom = d == $from.depth ? $from.nodeBefore : $from.node(d + 1); })
  if (rowDepth == -1) { return false }
  if (onAction)
    { onAction(state.tr.step(AddColumnStep.create(state.doc, $from.before(rowDepth - 1), $from.index(rowDepth),
                                                cellFrom.type, cellFrom.attrs)).action()) }
  return true
}
exports.addColumnBefore = addColumnBefore

// :: (EditorState, onAction: ?(action: Action)) → bool
// Command function that adds a column after the column with the
// selection.
function addColumnAfter(state, onAction) {
  var $from = state.selection.$from, cellFrom
  var rowDepth = findRow($from, function (d) { return cellFrom = d == $from.depth ? $from.nodeAfter : $from.node(d + 1); })
  if (rowDepth == -1) { return false }
  if (onAction)
    { onAction(state.tr.step(AddColumnStep.create(state.doc, $from.before(rowDepth - 1),
                                                $from.indexAfter(rowDepth) + (rowDepth == $from.depth ? 1 : 0),
                                                cellFrom.type, cellFrom.attrs)).action()) }
  return true
}
exports.addColumnAfter = addColumnAfter

// :: (EditorState, onAction: ?(action: Action)) → bool
// Command function that removes the column with the selection.
function removeColumn(state, onAction) {
  var $from = state.selection.$from
  var rowDepth = findRow($from, function (d) { return $from.node(d).childCount > 1; })
  if (rowDepth == -1) { return false }
  if (onAction)
    { onAction(state.tr.step(RemoveColumnStep.create(state.doc, $from.before(rowDepth - 1), $from.index(rowDepth))).action()) }
  return true
}
exports.removeColumn = removeColumn

function addRow(state, onAction, side) {
  var $from = state.selection.$from
  var rowDepth = findRow($from)
  if (rowDepth == -1) { return false }
  if (onAction) {
    var exampleRow = $from.node(rowDepth)
    var cells = [], pos = side < 0 ? $from.before(rowDepth) : $from.after(rowDepth)
    exampleRow.forEach(function (cell) { return cells.push(cell.type.createAndFill(cell.attrs)); })
    var row = exampleRow.copy(Fragment.from(cells))
    onAction(state.tr.step(new ReplaceStep(pos, pos, new Slice(Fragment.from(row), 0, 0))).action())
  }
  return true
}

// :: (EditorState, onAction: ?(action: Action)) → bool
// Command function that adds a row after the row with the
// selection.
function addRowBefore(state, onAction) {
  return addRow(state, onAction, -1)
}
exports.addRowBefore = addRowBefore

// :: (EditorState, onAction: ?(action: Action)) → bool
// Command function that adds a row before the row with the
// selection.
function addRowAfter(state, onAction) {
  return addRow(state, onAction, 1)
}
exports.addRowAfter = addRowAfter

// :: (EditorState, onAction: ?(action: Action)) → bool
// Command function that removes the row with the selection.
function removeRow(state, onAction) {
  var $from = state.selection.$from
  var rowDepth = findRow($from, function (d) { return $from.node(d - 1).childCount > 1; })
  if (rowDepth == -1) { return false }
  if (onAction)
    { onAction(state.tr.step(new ReplaceStep($from.before(rowDepth), $from.after(rowDepth), Slice.empty)).action()) }
  return true
}
exports.removeRow = removeRow

function moveCell(state, dir, onAction) {
  var ref = state.selection;
  var $from = ref.$from;
  var rowDepth = findRow($from)
  if (rowDepth == -1) { return false }
  var row = $from.node(rowDepth), newIndex = $from.index(rowDepth) + dir
  if (newIndex >= 0 && newIndex < row.childCount) {
    var $cellStart = state.doc.resolve(row.content.offsetAt(newIndex) + $from.start(rowDepth))
    var sel = Selection.findFrom($cellStart, 1)
    if (!sel || sel.from >= $cellStart.end()) { return false }
    if (onAction) { onAction(sel.action()) }
    return true
  } else {
    var rowIndex = $from.index(rowDepth - 1) + dir, table = $from.node(rowDepth - 1)
    if (rowIndex < 0 || rowIndex >= table.childCount) { return false }
    var cellStart = dir > 0 ? $from.after(rowDepth) + 2 : $from.before(rowDepth) - 2 - table.child(rowIndex).lastChild.content.size
    var $cellStart$1 = state.doc.resolve(cellStart), sel$1 = Selection.findFrom($cellStart$1, 1)
    if (!sel$1 || sel$1.from >= $cellStart$1.end()) { return false }
    if (onAction) { onAction(sel$1.action()) }
    return true
  }
}

// :: (EditorState, onAction: ?(action: Action)) → bool
// Move to the next cell in the current table, if there is one.
function selectNextCell(state, onAction) { return moveCell(state, 1, onAction) }
exports.selectNextCell = selectNextCell

// :: (EditorState, onAction: ?(action: Action)) → bool
// Move to the previous cell in the current table, if there is one.
function selectPreviousCell(state, onAction) { return moveCell(state, -1, onAction) }
exports.selectPreviousCell = selectPreviousCell

},{"prosemirror-model":26,"prosemirror-state":37,"prosemirror-transform":42}],37:[function(require,module,exports){
;var assign;
((assign = require("./selection"), exports.Selection = assign.Selection, exports.TextSelection = assign.TextSelection, exports.NodeSelection = assign.NodeSelection, exports.isSelectable = assign.isSelectable))

exports.EditorTransform = require("./transform").EditorTransform

exports.EditorState = require("./state").EditorState

;var assign$1;
((assign$1 = require("./plugin"), exports.Plugin = assign$1.Plugin, exports.PluginKey = assign$1.PluginKey))

},{"./plugin":38,"./selection":39,"./state":40,"./transform":41}],38:[function(require,module,exports){
// ::- Plugins wrap extra functionality that can be added to an
// editor. They can define new [state fields](#state.StateField), and
// add [view props](#view.EditorProps).
var Plugin = function Plugin(options) {
  var this$1 = this;

  // :: EditorProps
  // The props exported by this plugin.
  this.props = {}
  if (options.props) { for (var prop in options.props) {
    var val = options.props[prop]
    if (val instanceof Function) { val = val.bind(this$1) }
    this$1.props[prop] = val
  } }
  // :: Object
  // The plugin's configuration object.
  this.options = options
  this.key = options.key ? options.key.key : createKey("plugin")
};

// :: (EditorState) → any
// Get the state field for this plugin.
Plugin.prototype.getState = function getState (state) { return state[this.key] };
exports.Plugin = Plugin

// StateField:: interface<T>
// A plugin may provide a state field (under its `state` property) of
// this type, which describes the state it wants to keep. Functions
// provided here are always called with the plugin instance as their
// `this` binding.
//
//   init:: (config: Object, instance: EditorState) → T
//   Initialize the value of this field. `config` will be the object
//   passed to [`EditorState.create`](#state.EditorState^create). Note
//   that `instance` is a half-initialized state instance, and will
//   not have values for any fields initialzed after this one.
//
//   applyAction:: (state: EditorState, action: Action) → T
//   Apply the given action to this state field, producing a new field
//   value. Note that the `state` argument is the _old_ state, before
//   the action was applied.
//
//   toJSON:: ?(value: T) → *
//   Convert this field to JSON. Optional, can be left off to disable
//   JSON serialization for the field.
//
//   fromJSON:: ?(config: Object, value: *, state: EditorState) → T
//   Deserialize the JSON representation of this field. Note that the
//   `state` argument is again a half-initialized state.

var keys = Object.create(null)

function createKey(name) {
  if (name in keys) { return name + "$" + ++keys[name] }
  keys[name] = 0
  return name + "$"
}

// ::- A key is used to [tag](#state.Plugin.constructor^options.key)
// plugins in a way that makes it possible to find them, given an
// editor state. Assigning a key does mean only one plugin of that
// type can be active in a state.
var PluginKey = function PluginKey(name) {
if ( name === void 0 ) name = "key";
 this.key = createKey(name) };

// :: (EditorState) → ?Plugin
// Get the active plugin with this key, if any, from an editor
// state.
PluginKey.prototype.get = function get (state) { return state.config.pluginsByKey[this.key] };

// :: (EditorState) → ?any
// Get the plugin's state from an editor state.
PluginKey.prototype.getState = function getState (state) { return state[this.key] };
exports.PluginKey = PluginKey

},{}],39:[function(require,module,exports){
// ::- Superclass for editor selections.
var Selection = function Selection($from, $to) {
  // :: ResolvedPos
  // The resolved lower bound of the selection
  this.$from = $from
  // :: ResolvedPos
  // The resolved upper bound of the selection
  this.$to = $to
};

var prototypeAccessors = { from: {},to: {},empty: {} };

// :: bool
// True if the selection is an empty text selection (head an anchor
// are the same).
prototypeAccessors.from.get = function () { return this.$from.pos };

// :: number
// The upper bound of the selection.
prototypeAccessors.to.get = function () { return this.$to.pos };

prototypeAccessors.empty.get = function () {
  return this.from == this.to
};

// :: (?Object) → SelectionAction
// Create an [action](#state.Action) that updates the selection to
// this one.
Selection.prototype.action = function action (options) {
  var action = {type: "selection", selection: this}
  if (options) { for (var prop in options) { action[prop] = options[prop] } }
  return action
};

// eq:: (other: Selection) → bool
// Test whether the selection is the same as another selection.

// map:: (doc: Node, mapping: Mappable) → Selection
// Map this selection through a [mappable](#transform.Mappable) thing. `doc`
// should be the new document, to which we are mapping.

// toJSON:: () → Object
// Convert the selection to a JSON representation.

// :: (ResolvedPos, number, ?bool) → ?Selection
// Find a valid cursor or leaf node selection starting at the given
// position and searching back if `dir` is negative, and forward if
// negative. When `textOnly` is true, only consider cursor
// selections.
Selection.findFrom = function findFrom ($pos, dir, textOnly) {
  var inner = $pos.parent.isTextblock ? new TextSelection($pos)
      : findSelectionIn($pos.node(0), $pos.parent, $pos.pos, $pos.index(), dir, textOnly)
  if (inner) { return inner }

  for (var depth = $pos.depth - 1; depth >= 0; depth--) {
    var found = dir < 0
        ? findSelectionIn($pos.node(0), $pos.node(depth), $pos.before(depth + 1), $pos.index(depth), dir, textOnly)
        : findSelectionIn($pos.node(0), $pos.node(depth), $pos.after(depth + 1), $pos.index(depth) + 1, dir, textOnly)
    if (found) { return found }
  }
};

// :: (ResolvedPos, ?number, ?bool) → Selection
// Find a valid cursor or leaf node selection near the given
// position. Searches forward first by default, but if `bias` is
// negative, it will search backwards first.
Selection.near = function near ($pos, bias) {
    if ( bias === void 0 ) bias = 1;

  var result = this.findFrom($pos, bias) || this.findFrom($pos, -bias)
  if (!result) { throw new RangeError("Searching for selection in invalid document " + $pos.node(0)) }
  return result
};

// :: (Node, ?bool) → ?Selection
// Find the cursor or leaf node selection closest to the start of
// the given document. When `textOnly` is true, only consider cursor
// selections.
Selection.atStart = function atStart (doc, textOnly) {
  return findSelectionIn(doc, doc, 0, 0, 1, textOnly)
};

// :: (Node, ?bool) → ?Selection
// Find the cursor or leaf node selection closest to the end of
// the given document. When `textOnly` is true, only consider cursor
// selections.
Selection.atEnd = function atEnd (doc, textOnly) {
  return findSelectionIn(doc, doc, doc.content.size, doc.childCount, -1, textOnly)
};

// :: (ResolvedPos, ResolvedPos, ?number) → Selection
// Find a selection that spans the given positions, if both are text
// positions. If not, return some other selection nearby, where
// `bias` determines whether the method searches forward (default)
// or backwards (negative number) first.
Selection.between = function between ($anchor, $head, bias) {
  var found = Selection.near($head, bias)
  if (found instanceof TextSelection) {
    var nearAnchor = Selection.findFrom($anchor, $anchor.pos > found.to ? -1 : 1, true)
    found = new TextSelection(nearAnchor.$anchor, found.$head)
  } else if ($anchor.pos < found.from || $anchor.pos > found.to) {
    // If head falls on a node, but anchor falls outside of it, create
    // a text selection between them
    var inv = $anchor.pos > found.to
    var foundAnchor = Selection.findFrom($anchor, inv ? -1 : 1, true)
    var foundHead = Selection.findFrom(inv ? found.$from : found.$to, inv ? 1 : -1, true)
    if (foundAnchor && foundHead)
      { found = new TextSelection(foundAnchor.$anchor, foundHead.$head) }
  }
  return found
};

Selection.mapJSON = function mapJSON (json, mapping) {
  if (json.anchor != null)
    { return {head: mapping.map(json.head), anchor: mapping.map(json.anchor)} }
  else
    { return {node: mapping.map(json.node), after: mapping.map(json.after, -1)} }
};

// :: (Node, Object) → Selection
// Deserialize a JSON representation of a selection.
Selection.fromJSON = function fromJSON (doc, json) {
  // This is cautious, because the history will blindly map
  // selections and then try to deserialize them, and the endpoints
  // might not point at appropriate positions anymore (though they
  // are guaranteed to be inside of the document's range).
  if (json.head != null) {
    var $anchor = doc.resolve(json.anchor), $head = doc.resolve(json.head)
    if ($anchor.parent.isTextblock && $head.parent.isTextblock) { return new TextSelection($anchor, $head) }
    else { return Selection.between($anchor, $head) }
  } else {
    var $pos = doc.resolve(json.node), after = $pos.nodeAfter
    if (after && json.after == json.pos + after.nodeSize && isSelectable(after)) { return new NodeSelection($pos) }
    else { return Selection.near($pos) }
  }
};

Object.defineProperties( Selection.prototype, prototypeAccessors );
exports.Selection = Selection

// ::- A text selection represents a classical editor
// selection, with a head (the moving side) and anchor (immobile
// side), both of which point into textblock nodes. It can be empty (a
// regular cursor position).
var TextSelection = (function (Selection) {
  function TextSelection($anchor, $head) {
    if ( $head === void 0 ) $head = $anchor;

    var inv = $anchor.pos > $head.pos
    Selection.call(this, inv ? $head : $anchor, inv ? $anchor : $head)
    // :: ResolvedPos The resolved anchor of the selection.
    this.$anchor = $anchor
    // :: ResolvedPos The resolved head of the selection.
    this.$head = $head
  }

  if ( Selection ) TextSelection.__proto__ = Selection;
  TextSelection.prototype = Object.create( Selection && Selection.prototype );
  TextSelection.prototype.constructor = TextSelection;

  var prototypeAccessors$1 = { anchor: {},head: {},inverted: {} };

  prototypeAccessors$1.anchor.get = function () { return this.$anchor.pos };
  // :: number
  // The selection's mobile side (the side that moves when pressing
  // shift-arrow).
  prototypeAccessors$1.head.get = function () { return this.$head.pos };

  prototypeAccessors$1.inverted.get = function () { return this.anchor > this.head };

  TextSelection.prototype.eq = function eq (other) {
    return other instanceof TextSelection && other.head == this.head && other.anchor == this.anchor
  };

  TextSelection.prototype.map = function map (doc, mapping) {
    var $head = doc.resolve(mapping.map(this.head))
    if (!$head.parent.isTextblock) { return Selection.near($head) }
    var $anchor = doc.resolve(mapping.map(this.anchor))
    return new TextSelection($anchor.parent.isTextblock ? $anchor : $head, $head)
  };

  TextSelection.prototype.toJSON = function toJSON () {
    return {head: this.head, anchor: this.anchor}
  };

  Object.defineProperties( TextSelection.prototype, prototypeAccessors$1 );

  return TextSelection;
}(Selection));
exports.TextSelection = TextSelection

// ::- A node selection is a selection that points at a
// single node. All nodes marked [selectable](#model.NodeSpec.selectable)
// can be the target of a node selection. In such an object, `from`
// and `to` point directly before and after the selected node.
var NodeSelection = (function (Selection) {
  function NodeSelection($from) {
    var $to = $from.plusOne()
    Selection.call(this, $from, $to)
    // :: Node The selected node.
    this.node = $from.nodeAfter
  }

  if ( Selection ) NodeSelection.__proto__ = Selection;
  NodeSelection.prototype = Object.create( Selection && Selection.prototype );
  NodeSelection.prototype.constructor = NodeSelection;

  NodeSelection.prototype.eq = function eq (other) {
    return other instanceof NodeSelection && this.from == other.from
  };

  NodeSelection.prototype.map = function map (doc, mapping) {
    var from = mapping.mapResult(this.from, 1), to = mapping.mapResult(this.to, -1)
    var $from = doc.resolve(from.pos), node = $from.nodeAfter
    if (!from.deleted && !to.deleted && node && to.pos == from.pos + node.nodeSize && isSelectable(node))
      { return new NodeSelection($from) }
    return Selection.near($from)
  };

  NodeSelection.prototype.toJSON = function toJSON () {
    return {node: this.from, after: this.to}
  };

  return NodeSelection;
}(Selection));
exports.NodeSelection = NodeSelection

// FIXME we'll need some awareness of text direction when scanning for selections

// Try to find a selection inside the given node. `pos` points at the
// position where the search starts. When `text` is true, only return
// text selections.
function findSelectionIn(doc, node, pos, index, dir, text) {
  if (node.isTextblock) { return new TextSelection(doc.resolve(pos)) }
  for (var i = index - (dir > 0 ? 0 : 1); dir > 0 ? i < node.childCount : i >= 0; i += dir) {
    var child = node.child(i)
    if (!child.isLeaf) {
      var inner = findSelectionIn(doc, child, pos + dir, dir < 0 ? child.childCount : 0, dir, text)
      if (inner) { return inner }
    } else if (!text && isSelectable(child)) {
      return new NodeSelection(doc.resolve(pos - (dir < 0 ? child.nodeSize : 0)))
    }
    pos += child.nodeSize * dir
  }
}

function isSelectable(node) {
  return !node.isText && node.type.spec.selectable !== false
}
exports.isSelectable = isSelectable

},{}],40:[function(require,module,exports){
var ref = require("prosemirror-model");
var Mark = ref.Mark;
var Node = ref.Node;
var ref$1 = require("prosemirror-transform");
var Mapping = ref$1.Mapping;

var ref$2 = require("./selection");
var Selection = ref$2.Selection;
var ref$3 = require("./transform");
var EditorTransform = ref$3.EditorTransform;

var ViewState = function ViewState(inDOMChange, domChangeMapping, scrollToSelection) {
  this.inDOMChange = inDOMChange
  this.domChangeMapping = domChangeMapping
  this.scrollToSelection = scrollToSelection
};
ViewState.initial = new ViewState(null, null, false)
exports.ViewState = ViewState

function bind(f, self) {
  return !self || !f ? f : f.bind(self)
}

var FieldDesc = function FieldDesc(name, desc, self) {
  this.name = name
  this.init = bind(desc.init, self)
  this.applyAction = bind(desc.applyAction, self)
};

var baseFields = [
  new FieldDesc("doc", {
    init: function init(config) { return config.doc || config.schema.nodes.doc.createAndFill() },
    applyAction: function applyAction(action, doc) {
      return action.type == "transform" ? action.transform.doc : doc
    }
  }),

  new FieldDesc("selection", {
    init: function init$1(config, instance) { return config.selection || Selection.atStart(instance.doc) },
    applyAction: function applyAction$1(action, selection) {
      if (action.type == "transform")
        { return action.selection || selection.map(action.transform.doc, action.transform.mapping) }
      if (action.type == "selection")
        { return action.selection }
      return selection
    }
  }),

  new FieldDesc("storedMarks", {
    init: function init$2() { return null },
    applyAction: function applyAction$2(action, storedMarks, state) {
      if (action.type == "transform") { return action.selection ? null : storedMarks }
      if (action.type == "selection") { return null }
      if (action.type == "addStoredMark" && state.selection.empty)
        { return action.mark.addToSet(storedMarks || currentMarks(state.doc, state.selection)) }
      if (action.type == "removeStoredMark" && state.selection.empty)
        { return action.markType.removeFromSet(storedMarks || currentMarks(state.doc, state.selection)) }
      return storedMarks
    }
  }),

  new FieldDesc("view", {
    init: function init$3() { return ViewState.initial },
    applyAction: function applyAction$3(action, view) {
      if (action.type == "transform")
        { return new ViewState(view.inDOMChange,
                             view.domChangeMapping && view.domChangeMapping.copy().appendMapping(action.transform.mapping),
                             action.scrollIntoView ? true : action.selection ? false : view.scrollToSelection) }
      if (action.type == "selection")
        { return new ViewState(view.inDOMChange, view.domChangeMapping, action.scrollIntoView) }
      if (action.type == "startDOMChange")
        { return new ViewState(action.id, new Mapping, view.scrollToSelection) }
      if (action.type == "endDOMChange")
        { return new ViewState(null, null, view.scrollToSelection) }
      return view
    }
  })
]

function currentMarks(doc, selection) {
  return selection.head == null ? Mark.none : doc.marksAt(selection.head)
}

// Object wrapping the part of a state object that stays the same
// across actions. Stored in the state's `config` property.
var Configuration = function Configuration(schema, plugins) {
  var this$1 = this;

  this.schema = schema
  this.fields = baseFields.slice()
  this.plugins = []
  this.pluginsByKey = Object.create(null)
  if (plugins) { plugins.forEach(function (plugin) {
    if (this$1.pluginsByKey[plugin.key])
      { throw new RangeError("Adding different instances of a keyed plugin (" + plugin.key + ")") }
    this$1.plugins.push(plugin)
    this$1.pluginsByKey[plugin.key] = plugin
    if (plugin.options.state)
      { this$1.fields.push(new FieldDesc(plugin.key, plugin.options.state, plugin)) }
  }) }
};

// ::- The state of a ProseMirror editor is represented by an object
// of this type. This is a persistent data structure—it isn't updated,
// but rather a new state value is computed from an old one with the
// [`applyAction`](state.EditorState.applyAction) method.
//
// In addition to the built-in state fields, plugins can define
// additional pieces of state.
var EditorState = function EditorState(config) {
  this.config = config
};

var prototypeAccessors = { schema: {},plugins: {},tr: {} };

// doc:: Node
// The current document.

// selection:: Selection
// The selection.

// storedMarks:: ?[Mark]
// A set of marks to apply to the next character that's typed. Will
// be null whenever no explicit marks have been set.

// :: Schema
// The schema of the state's document.
prototypeAccessors.schema.get = function () {
  return this.config.schema
};

// :: [Plugin]
// The plugins that are active in this state.
prototypeAccessors.plugins.get = function () {
  return this.config.plugins
};

// :: (Action) → EditorState
// Apply the given action to produce a new state.
EditorState.prototype.applyAction = function applyAction (action) {
    var this$1 = this;

  var newInstance = new EditorState(this.config), fields = this.config.fields
  for (var i = 0; i < fields.length; i++) {
    var field = fields[i]
    newInstance[field.name] = field.applyAction(action, this$1[field.name], this$1)
  }
  return newInstance
};

// :: EditorTransform
// Create a selection-aware [`Transform` object](#state.EditorTransform).
prototypeAccessors.tr.get = function () { return new EditorTransform(this) };

// :: (Object) → EditorState
// Create a state. `config` must be an object containing at least a
// `schema` (the schema to use) or `doc` (the starting document)
// property. When it has a `selection` property, that should be a
// valid [selection](#state.Selection) in the given document, to use
// as starting selection. Plugins, which are specified as an array
// in the `plugins` property, may read additional fields from the
// config object.
EditorState.create = function create (config) {
  var $config = new Configuration(config.schema || config.doc.type.schema, config.plugins)
  var instance = new EditorState($config)
  for (var i = 0; i < $config.fields.length; i++)
    { instance[$config.fields[i].name] = $config.fields[i].init(config, instance) }
  return instance
};

// :: (Object) → EditorState
// Create a new state based on this one, but with an adjusted set of
// active plugins. State fields that exist in both sets of plugins
// are kept unchanged. Those that no longer exist are dropped, and
// those that are new are initialized using their
// [`init`](#state.StateField.init) method, passing in the new
// configuration object..
EditorState.prototype.reconfigure = function reconfigure (config) {
    var this$1 = this;

  var $config = new Configuration(config.schema || this.schema, config.plugins)
  var fields = $config.fields, instance = new EditorState($config)
  for (var i = 0; i < fields.length; i++) {
    var name = fields[i].name
    instance[name] = this$1.hasOwnProperty(name) ? this$1[name] : fields[i].init(config, instance)
  }
  return instance
};

// :: (?Object<Plugin>) → Object
// Serialize this state to JSON. If you want to serialize the state
// of plugins, pass an object mapping property names to use in the
// resulting JSON object to plugin objects.
EditorState.prototype.toJSON = function toJSON (pluginFields) {
    var this$1 = this;

  var result = {doc: this.doc.toJSON(), selection: this.selection.toJSON()}
  if (pluginFields) { for (var prop in pluginFields) {
    if (prop == "doc" || prop == "selection")
      { throw new RangeError("The JSON fields `doc` and `selection` are reserved") }
    var plugin = pluginFields[prop], state = plugin.options.state
    if (state && state.toJSON) { result[prop] = state.toJSON.call(plugin, this$1[plugin.key]) }
  } }
  return result
};

// :: (Object, Object, ?Object<Plugin>) → EditorState
// Deserialize a JSON representation of a state. `config` should
// have at least a `schema` field, and should contain array of
// plugins to initialize the state with. `pluginFields` can be used
// to deserialize the state of plugins, by associating plugin
// instances with the property names they use in the JSON object.
EditorState.fromJSON = function fromJSON (config, json, pluginFields) {
  if (!config.schema) { throw new RangeError("Required config field 'schema' missing") }
  var $config = new Configuration(config.schema, config.plugins)
  var instance = new EditorState($config)
  $config.fields.forEach(function (field) {
    if (field.name == "doc") {
      instance.doc = Node.fromJSON(config.schema, json.doc)
    } else if (field.name == "selection") {
      instance.selection = Selection.fromJSON(instance.doc, json.selection)
    } else {
      if (pluginFields) { for (var prop in pluginFields) {
        var plugin = pluginFields[prop], state = plugin.options.state
        if (plugin.key == field.name && state && state.fromJSON &&
            Object.prototype.hasOwnProperty.call(json, prop)) {
          // This field belongs to a plugin mapped to a JSON field, read it from there.
          instance[field.name] = state.fromJSON.call(plugin, config, json[prop], instance)
          return
        }
      } }
      instance[field.name] = field.init(config, instance)
    }
  })
  return instance
};

Object.defineProperties( EditorState.prototype, prototypeAccessors );
exports.EditorState = EditorState

// Action:: interface
// State updates are performed through actions, which are objects that
// describe the update.
//
//  type:: string
//  The type of this action. This determines the way the action is
//  interpreted, and which other fields it should have.

// TransformAction:: interface
// An action type that transforms the state's document. Applying this
// will create a state in which the document is the result of this
// transformation.
//
//   type:: "transform"
//
//   transform:: Transform
//
//   selection:: ?Selection
//   If given, this selection will be used as the new selection. If
//   not, the old selection is mapped through the transform.
//
//   time:: number
//   The timestamp at which the change was made.
//
//   scrollIntoView:: ?bool
//   When true, the next display update will scroll the cursor into
//   view.

// SelectionAction:: interface
// An action that updates the selection.
//
//   type:: "selection"
//
//   selection:: Selection
//   The new selection.
//
//   scrollIntoView:: ?bool
//   When true, the next display update will scroll the cursor into
//   view.

// AddStoredMarkAction:: interface
// An action type that adds a stored mark to the state.
//
//   type:: "addStoredMark"
//
//   mark:: Mark

// RemoveStoredMarkAction:: interface
// An action type that removes a stored mark from the state.
//
//   type:: "removeStoredMark"
//
//   markType:: MarkType

},{"./selection":39,"./transform":41,"prosemirror-model":26,"prosemirror-transform":42}],41:[function(require,module,exports){
var ref = require("prosemirror-model");
var Fragment = ref.Fragment;
var ref$1 = require("prosemirror-transform");
var Transform = ref$1.Transform;
var insertPoint = ref$1.insertPoint;
var ref$2 = require("./selection");
var Selection = ref$2.Selection;

// ::- A selection-aware extension of `Transform`. Use
// [`EditorState.tr`](#state.EditorState.tr) to create an instance.
var EditorTransform = (function (Transform) {
  function EditorTransform(state) {
    Transform.call(this, state.doc)
    this.state = state
    this.curSelection = state.selection
    this.curSelectionAt = 0
    this.selectionSet = false
  }

  if ( Transform ) EditorTransform.__proto__ = Transform;
  EditorTransform.prototype = Object.create( Transform && Transform.prototype );
  EditorTransform.prototype.constructor = EditorTransform;

  var prototypeAccessors = { selection: {} };

  // :: Selection
  // The transform's current selection. This defaults to the
  // editor selection [mapped](#state.Selection.map) through the steps in
  // this transform, but can be overwritten with
  // [`setSelection`](#state.EditorTransform.setSelection).
  prototypeAccessors.selection.get = function () {
    if (this.curSelectionAt < this.steps.length) {
      this.curSelection = this.curSelection.map(this.doc, this.mapping.slice(this.curSelectionAt))
      this.curSelectionAt = this.steps.length
    }
    return this.curSelection
  };

  // :: (Selection) → EditorTransform
  // Update the transform's current selection. This will determine the
  // selection that the editor gets when the transform is applied.
  EditorTransform.prototype.setSelection = function setSelection (selection) {
    this.curSelection = selection
    this.curSelectionAt = this.steps.length
    this.selectionSet = true
    return this
  };

  // :: (?Node, ?bool) → EditorTransform
  // Replace the selection with the given node, or delete it if `node`
  // is null. When `inheritMarks` is true and the node is an inline
  // node, it inherits the marks from the place where it is inserted.
  EditorTransform.prototype.replaceSelection = function replaceSelection (node, inheritMarks) {
    var ref = this.selection;
    var $from = ref.$from;
    var $to = ref.$to;
    var from = ref.from;
    var to = ref.to;
    var selNode = ref.node;

    if (node && node.isInline && inheritMarks !== false)
      { node = node.mark(this.state.storedMarks || this.doc.marksAt(from, to > from)) }
    var fragment = Fragment.from(node)

    if (selNode && selNode.isTextblock && node && node.isInline) {
      // Putting inline stuff onto a selected textblock puts it
      // inside, so cut off the sides
      from++
      to--
    } else if (selNode) {
      var depth = $from.depth
      // This node can not simply be removed/replaced. Remove its parent as well
      while (depth && $from.node(depth).childCount == 1 &&
             !$from.node(depth).canReplace($from.index(depth), $to.indexAfter(depth), fragment)) {
        depth--
      }
      if (depth < $from.depth) {
        from = $from.before(depth + 1)
        to = $from.after(depth + 1)
      }
    } else if (node && from == to) {
      var point = insertPoint(this.doc, from, node.type, node.attrs)
      if (point != null) { from = to = point }
    }

    this.replaceWith(from, to, fragment)
    var map = this.mapping.maps[this.mapping.maps.length - 1]
    this.setSelection(Selection.near(this.doc.resolve(map.map(to)), node && node.isInline ? -1 : 1))
    return this
  };

  // :: () → EditorTransform
  // Delete the selection.
  EditorTransform.prototype.deleteSelection = function deleteSelection () {
    return this.replaceSelection()
  };

  // :: (string, from: ?number, to: ?number) → EditorTransform
  // Replace the given range, or the selection if no range is given,
  // with a text node containing the given string.
  EditorTransform.prototype.insertText = function insertText (text, from, to) {
    if ( to === void 0 ) to = from;

    var useSel = from == null
    if (useSel) {
      ;var assign;
      ((assign = this.selection, from = assign.from, to = assign.to))
    }

    var node = text ? this.state.schema.text(text, this.state.storedMarks || this.doc.marksAt(from, to > from)) : null
    if (useSel)
      { this.replaceSelection(node, false) }
    else
      { this.replaceWith(from, to, node) }

    if (text && useSel) {
      var map = this.mapping.maps[this.mapping.maps.length - 1]
      this.setSelection(Selection.findFrom(this.doc.resolve(map.map(to)), -1))
    }
    return this
  };

  // :: (?Object) → TransformAction
  // Create a transform action. `options` can be given to add extra
  // properties to the action object.
  EditorTransform.prototype.action = function action (options) {
    var action = {type: "transform",
                  transform: this,
                  selection: this.selectionSet ? this.selection : null,
                  time: Date.now()}
    if (options) { for (var prop in options) { action[prop] = options[prop] } }
    return action
  };

  // :: () → TransformAction
  // Create a transform action with the `scrollIntoView` property set
  // to true (this is common enough to warrant a shortcut method).
  EditorTransform.prototype.scrollAction = function scrollAction () {
    return this.action({scrollIntoView: true})
  };

  Object.defineProperties( EditorTransform.prototype, prototypeAccessors );

  return EditorTransform;
}(Transform));
exports.EditorTransform = EditorTransform

},{"./selection":39,"prosemirror-model":26,"prosemirror-transform":42}],42:[function(require,module,exports){
;var assign;
((assign = require("./transform"), exports.Transform = assign.Transform, exports.TransformError = assign.TransformError))
;var assign$1;
((assign$1 = require("./step"), exports.Step = assign$1.Step, exports.StepResult = assign$1.StepResult))
;var assign$2;
((assign$2 = require("./structure"), exports.joinPoint = assign$2.joinPoint, exports.canJoin = assign$2.canJoin, exports.canSplit = assign$2.canSplit, exports.insertPoint = assign$2.insertPoint, exports.liftTarget = assign$2.liftTarget, exports.findWrapping = assign$2.findWrapping))
;var assign$3;
((assign$3 = require("./map"), exports.StepMap = assign$3.StepMap, exports.MapResult = assign$3.MapResult, exports.Mapping = assign$3.Mapping))
;var assign$4;
((assign$4 = require("./mark_step"), exports.AddMarkStep = assign$4.AddMarkStep, exports.RemoveMarkStep = assign$4.RemoveMarkStep))
;var assign$5;
((assign$5 = require("./replace_step"), exports.ReplaceStep = assign$5.ReplaceStep, exports.ReplaceAroundStep = assign$5.ReplaceAroundStep))
require("./mark")
;var assign$6;
((assign$6 = require("./replace"), exports.replaceStep = assign$6.replaceStep))

},{"./map":43,"./mark":44,"./mark_step":45,"./replace":46,"./replace_step":47,"./step":48,"./structure":49,"./transform":50}],43:[function(require,module,exports){
// Mappable:: interface
// There are several things that positions can be mapped through.
// We'll denote those as 'mappable'.
//
//   map:: (pos: number, assoc: ?number) → number
//   Map a position through this object. When given, `assoc` (should
//   be -1 or 1, defaults to 1) determines with which side the
//   position is associated, which determines in which direction to
//   move when a chunk of content is inserted at the mapped position,
//   and when to consider the position to be deleted.
//
//   mapResult:: (pos: number, assoc: ?number) → MapResult
//   Map a position, and return an object containing additional
//   information about the mapping. The result's `deleted` field tells
//   you whether the position was deleted (completely enclosed in a
//   replaced range) during the mapping.

// Recovery values encode a range index and an offset. They are
// represented as numbers, because tons of them will be created when
// mapping, for example, a large number of marked ranges. The number's
// lower 16 bits provide the index, the remaining bits the offset.
//
// Note: We intentionally don't use bit shift operators to en- and
// decode these, since those clip to 32 bits, which we might in rare
// cases want to overflow. A 64-bit float can represent 48-bit
// integers precisely.

var lower16 = 0xffff
var factor16 = Math.pow(2, 16)

function makeRecover(index, offset) { return index + offset * factor16 }
function recoverIndex(value) { return value & lower16 }
function recoverOffset(value) { return (value - (value & lower16)) / factor16 }

// ::- An object representing a mapped position with extra
// information.
var MapResult = function MapResult(pos, deleted, recover) {
  if ( deleted === void 0 ) deleted = false;
  if ( recover === void 0 ) recover = null;

  // :: number The mapped version of the position.
  this.pos = pos
  // :: bool Tells you whether the position was deleted, that is,
  // whether the step removed its surroundings from the document.
  this.deleted = deleted
  this.recover = recover
};
exports.MapResult = MapResult

// ::- A map describing the deletions and insertions made by a step,
// which can be used to find the correspondence between positions in
// the pre-step version of a document and the same position in the
// post-step version. This class implements [`Mappable`](#transform.Mappable).
var StepMap = function StepMap(ranges, inverted) {
  if ( inverted === void 0 ) inverted = false;

  this.ranges = ranges
  this.inverted = inverted
};

StepMap.prototype.recover = function recover (value) {
    var this$1 = this;

  var diff = 0, index = recoverIndex(value)
  if (!this.inverted) { for (var i = 0; i < index; i++)
    { diff += this$1.ranges[i * 3 + 2] - this$1.ranges[i * 3 + 1] } }
  return this.ranges[index * 3] + diff + recoverOffset(value)
};

// :: (number, ?number) → MapResult
// Map the given position through this map. The `assoc` parameter can
// be used to control what happens when the transform inserted
// content at (or around) this position—if `assoc` is negative, the a
// position before the inserted content will be returned, if it is
// positive, a position after the insertion is returned.
StepMap.prototype.mapResult = function mapResult (pos, assoc) { return this._map(pos, assoc, false) };

// :: (number, ?number) → number
// Map the given position through this map, returning only the
// mapped position.
StepMap.prototype.map = function map (pos, assoc) { return this._map(pos, assoc, true) };

StepMap.prototype._map = function _map (pos, assoc, simple) {
    var this$1 = this;

  var diff = 0, oldIndex = this.inverted ? 2 : 1, newIndex = this.inverted ? 1 : 2
  for (var i = 0; i < this.ranges.length; i += 3) {
    var start = this$1.ranges[i] - (this$1.inverted ? diff : 0)
    if (start > pos) { break }
    var oldSize = this$1.ranges[i + oldIndex], newSize = this$1.ranges[i + newIndex], end = start + oldSize
    if (pos <= end) {
      var side = !oldSize ? assoc : pos == start ? -1 : pos == end ? 1 : assoc
      var result = start + diff + (side < 0 ? 0 : newSize)
      if (simple) { return result }
      var recover = makeRecover(i / 3, pos - start)
      return new MapResult(result, assoc < 0 ? pos != start : pos != end, recover)
    }
    diff += newSize - oldSize
  }
  return simple ? pos + diff : new MapResult(pos + diff)
};

StepMap.prototype.touches = function touches (pos, recover) {
    var this$1 = this;

  var diff = 0, index = recoverIndex(recover)
  var oldIndex = this.inverted ? 2 : 1, newIndex = this.inverted ? 1 : 2
  for (var i = 0; i < this.ranges.length; i += 3) {
    var start = this$1.ranges[i] - (this$1.inverted ? diff : 0)
    if (start > pos) { break }
    var oldSize = this$1.ranges[i + oldIndex], end = start + oldSize
    if (pos <= end && i == index * 3) { return true }
    diff += this$1.ranges[i + newIndex] - oldSize
  }
  return false
};

// :: ((oldStart: number, oldEnd: number, newStart: number, newEnd: number))
// Calls the given function on each of the changed ranges denoted by
// this map.
StepMap.prototype.forEach = function forEach (f) {
    var this$1 = this;

  var oldIndex = this.inverted ? 2 : 1, newIndex = this.inverted ? 1 : 2
  for (var i = 0, diff = 0; i < this.ranges.length; i += 3) {
    var start = this$1.ranges[i], oldStart = start - (this$1.inverted ? diff : 0), newStart = start + (this$1.inverted ? 0 : diff)
    var oldSize = this$1.ranges[i + oldIndex], newSize = this$1.ranges[i + newIndex]
    f(oldStart, oldStart + oldSize, newStart, newStart + newSize)
    diff += newSize - oldSize
  }
};

// :: () → StepMap
// Create an inverted version of this map. The result can be used to
// map positions in the post-step document to the pre-step document.
StepMap.prototype.invert = function invert () {
  return new StepMap(this.ranges, !this.inverted)
};

StepMap.prototype.toString = function toString () {
  return (this.inverted ? "-" : "") + JSON.stringify(this.ranges)
};
exports.StepMap = StepMap

StepMap.empty = new StepMap([])

// ::- A mapping represents a pipeline of zero or more [step
// maps](#transform.StepMap). It has special provisions for losslessly
// handling mapping positions through a series of steps in which some
// steps are inverted versions of earlier steps. (This comes up when
// ‘rebasing’ steps for collaboration or history management.) This
// class implements [`Mappable`](#transform.Mappable).
var Mapping = function Mapping(maps, mirror, from, to) {
  // :: [StepMap]
  // The step maps in this mapping.
  this.maps = maps || []
  // :: number
  // The starting position in the `maps` array, used when `map` or
  // `mapResult` is called.
  this.from = from || 0
  // :: number
  // The end positions in the `maps` array.
  this.to = to == null ? this.maps.length : to
  this.mirror = mirror
};

// :: (?number, ?number) → Mapping
// Create a remapping that maps only through a part of this one.
Mapping.prototype.slice = function slice (from, to) {
    if ( from === void 0 ) from = 0;
    if ( to === void 0 ) to = this.maps.length;

  return new Mapping(this.maps, this.mirror, from, to)
};

Mapping.prototype.copy = function copy () {
  return new Mapping(this.maps.slice(), this.mirror && this.mirror.slice(), this.from, this.to)
};

Mapping.prototype.getMirror = function getMirror (n) {
    var this$1 = this;

  if (this.mirror) { for (var i = 0; i < this.mirror.length; i++)
    { if (this$1.mirror[i] == n) { return this$1.mirror[i + (i % 2 ? -1 : 1)] } } }
};

Mapping.prototype.setMirror = function setMirror (n, m) {
  if (!this.mirror) { this.mirror = [] }
  this.mirror.push(n, m)
};

// :: (StepMap, ?number)
// Add a step map to the end of this remapping. If `mirrors` is
// given, it should be the index of the step map that is the mirror
// image of this one.
Mapping.prototype.appendMap = function appendMap (map, mirrors) {
  this.to = this.maps.push(map)
  if (mirrors != null) { this.setMirror(this.maps.length - 1, mirrors) }
};

// :: (Mapping)
// Add all the step maps in a given mapping to this one (preserving
// mirroring information).
Mapping.prototype.appendMapping = function appendMapping (mapping) {
    var this$1 = this;

  for (var i = 0, startSize = this.maps.length; i < mapping.maps.length; i++) {
    var mirr = mapping.getMirror(i)
    this$1.appendMap(mapping.maps[i], mirr != null && mirr < i ? startSize + mirr : null)
  }
};

// :: (number, ?number) → number
// Map a position through this remapping.
Mapping.prototype.map = function map (pos, assoc) {
    var this$1 = this;

  if (this.mirror) { return this._map(pos, assoc, true) }
  for (var i = this.from; i < this.to; i++)
    { pos = this$1.maps[i].map(pos, assoc) }
  return pos
};

// :: (number, ?number) → MapResult
// Map a position through this remapping, returning a mapping
// result.
Mapping.prototype.mapResult = function mapResult (pos, assoc) { return this._map(pos, assoc, false) };

Mapping.prototype._map = function _map (pos, assoc, simple) {
    var this$1 = this;

  var deleted = false, recoverables = null

  for (var i = this.from; i < this.to; i++) {
    var map = this$1.maps[i], rec = recoverables && recoverables[i]
    if (rec != null && map.touches(pos, rec)) {
      pos = map.recover(rec)
      continue
    }

    var result = map.mapResult(pos, assoc)
    if (result.recover != null) {
      var corr = this$1.getMirror(i)
      if (corr != null && corr > i && corr < this$1.to) {
        if (result.deleted) {
          i = corr
          pos = this$1.maps[corr].recover(result.recover)
          continue
        } else {
          ;(recoverables || (recoverables = Object.create(null)))[corr] = result.recover
        }
      }
    }

    if (result.deleted) { deleted = true }
    pos = result.pos
  }

  return simple ? pos : new MapResult(pos, deleted)
};
exports.Mapping = Mapping

},{}],44:[function(require,module,exports){
var ref = require("prosemirror-model");
var MarkType = ref.MarkType;
var Slice = ref.Slice;

var ref$1 = require("./transform");
var Transform = ref$1.Transform;
var ref$2 = require("./mark_step");
var AddMarkStep = ref$2.AddMarkStep;
var RemoveMarkStep = ref$2.RemoveMarkStep;
var ref$3 = require("./replace_step");
var ReplaceStep = ref$3.ReplaceStep;

// :: (number, number, Mark) → Transform
// Add the given mark to the inline content between `from` and `to`.
Transform.prototype.addMark = function(from, to, mark) {
  var this$1 = this;

  var removed = [], added = [], removing = null, adding = null
  this.doc.nodesBetween(from, to, function (node, pos, parent, index) {
    if (!node.isInline) { return }
    var marks = node.marks
    if (mark.isInSet(marks) || !parent.contentMatchAt(index + 1).allowsMark(mark.type)) {
      adding = removing = null
    } else {
      var start = Math.max(pos, from), end = Math.min(pos + node.nodeSize, to)
      var rm = mark.type.isInSet(marks)

      if (!rm)
        { removing = null }
      else if (removing && removing.mark.eq(rm))
        { removing.to = end }
      else
        { removed.push(removing = new RemoveMarkStep(start, end, rm)) }

      if (adding)
        { adding.to = end }
      else
        { added.push(adding = new AddMarkStep(start, end, mark)) }
    }
  })

  removed.forEach(function (s) { return this$1.step(s); })
  added.forEach(function (s) { return this$1.step(s); })
  return this
}

// :: (number, number, ?union<Mark, MarkType>) → Transform
// Remove the given mark, or all marks of the given type, from inline
// nodes between `from` and `to`.
Transform.prototype.removeMark = function(from, to, mark) {
  var this$1 = this;
  if ( mark === void 0 ) mark = null;

  var matched = [], step = 0
  this.doc.nodesBetween(from, to, function (node, pos) {
    if (!node.isInline) { return }
    step++
    var toRemove = null
    if (mark instanceof MarkType) {
      var found = mark.isInSet(node.marks)
      if (found) { toRemove = [found] }
    } else if (mark) {
      if (mark.isInSet(node.marks)) { toRemove = [mark] }
    } else {
      toRemove = node.marks
    }
    if (toRemove && toRemove.length) {
      var end = Math.min(pos + node.nodeSize, to)
      for (var i = 0; i < toRemove.length; i++) {
        var style = toRemove[i], found$1 = void 0
        for (var j = 0; j < matched.length; j++) {
          var m = matched[j]
          if (m.step == step - 1 && style.eq(matched[j].style)) { found$1 = m }
        }
        if (found$1) {
          found$1.to = end
          found$1.step = step
        } else {
          matched.push({style: style, from: Math.max(pos, from), to: end, step: step})
        }
      }
    }
  })
  matched.forEach(function (m) { return this$1.step(new RemoveMarkStep(m.from, m.to, m.style)); })
  return this
}

// :: (number, number) → Transform
// Remove all marks and non-text inline nodes from the given range.
Transform.prototype.clearMarkup = function(from, to) {
  var this$1 = this;

  var delSteps = [] // Must be accumulated and applied in inverse order
  this.doc.nodesBetween(from, to, function (node, pos) {
    if (!node.isInline) { return }
    if (!node.type.isText) {
      delSteps.push(new ReplaceStep(pos, pos + node.nodeSize, Slice.empty))
      return
    }
    for (var i = 0; i < node.marks.length; i++)
      { this$1.step(new RemoveMarkStep(Math.max(pos, from), Math.min(pos + node.nodeSize, to), node.marks[i])) }
  })
  for (var i = delSteps.length - 1; i >= 0; i--) { this$1.step(delSteps[i]) }
  return this
}

Transform.prototype.clearMarkupFor = function(pos, newType, newAttrs) {
  var this$1 = this;

  var node = this.doc.nodeAt(pos), match = newType.contentExpr.start(newAttrs)
  var delSteps = []
  for (var i = 0, cur = pos + 1; i < node.childCount; i++) {
    var child = node.child(i), end = cur + child.nodeSize
    var allowed = match.matchType(child.type, child.attrs)
    if (!allowed) {
      delSteps.push(new ReplaceStep(cur, end, Slice.empty))
    } else {
      match = allowed
      for (var j = 0; j < child.marks.length; j++) { if (!match.allowsMark(child.marks[j]))
        { this$1.step(new RemoveMarkStep(cur, end, child.marks[j])) } }
    }
    cur = end
  }
  for (var i$1 = delSteps.length - 1; i$1 >= 0; i$1--) { this$1.step(delSteps[i$1]) }
  return this
}

},{"./mark_step":45,"./replace_step":47,"./transform":50,"prosemirror-model":26}],45:[function(require,module,exports){
var ref = require("prosemirror-model");
var Fragment = ref.Fragment;
var Slice = ref.Slice;
var ref$1 = require("./step");
var Step = ref$1.Step;
var StepResult = ref$1.StepResult;

function mapFragment(fragment, f, parent) {
  var mapped = []
  for (var i = 0; i < fragment.childCount; i++) {
    var child = fragment.child(i)
    if (child.content.size) { child = child.copy(mapFragment(child.content, f, child)) }
    if (child.isInline) { child = f(child, parent, i) }
    mapped.push(child)
  }
  return Fragment.fromArray(mapped)
}

// ::- Add a mark to all inline content between two positions.
var AddMarkStep = (function (Step) {
  function AddMarkStep(from, to, mark) {
    Step.call(this)
    this.from = from
    this.to = to
    this.mark = mark
  }

  if ( Step ) AddMarkStep.__proto__ = Step;
  AddMarkStep.prototype = Object.create( Step && Step.prototype );
  AddMarkStep.prototype.constructor = AddMarkStep;

  AddMarkStep.prototype.apply = function apply (doc) {
    var this$1 = this;

    var oldSlice = doc.slice(this.from, this.to)
    var slice = new Slice(mapFragment(oldSlice.content, function (node, parent, index) {
      if (!parent.contentMatchAt(index + 1).allowsMark(this$1.mark.type)) { return node }
      return node.mark(this$1.mark.addToSet(node.marks))
    }, oldSlice.possibleParent), oldSlice.openLeft, oldSlice.openRight)
    return StepResult.fromReplace(doc, this.from, this.to, slice)
  };

  AddMarkStep.prototype.invert = function invert () {
    return new RemoveMarkStep(this.from, this.to, this.mark)
  };

  AddMarkStep.prototype.map = function map (mapping) {
    var from = mapping.mapResult(this.from, 1), to = mapping.mapResult(this.to, -1)
    if (from.deleted && to.deleted || from.pos >= to.pos) { return null }
    return new AddMarkStep(from.pos, to.pos, this.mark)
  };

  AddMarkStep.prototype.merge = function merge (other) {
    if (other instanceof AddMarkStep &&
        other.mark.eq(this.mark) &&
        this.from <= other.to && this.to >= other.from)
      { return new AddMarkStep(Math.min(this.from, other.from),
                             Math.max(this.to, other.to), this.mark) }
  };

  AddMarkStep.fromJSON = function fromJSON (schema, json) {
    return new AddMarkStep(json.from, json.to, schema.markFromJSON(json.mark))
  };

  return AddMarkStep;
}(Step));
exports.AddMarkStep = AddMarkStep

Step.jsonID("addMark", AddMarkStep)

// ::- Remove a mark from all inline content between two positions.
var RemoveMarkStep = (function (Step) {
  function RemoveMarkStep(from, to, mark) {
    Step.call(this)
    this.from = from
    this.to = to
    this.mark = mark
  }

  if ( Step ) RemoveMarkStep.__proto__ = Step;
  RemoveMarkStep.prototype = Object.create( Step && Step.prototype );
  RemoveMarkStep.prototype.constructor = RemoveMarkStep;

  RemoveMarkStep.prototype.apply = function apply (doc) {
    var this$1 = this;

    var oldSlice = doc.slice(this.from, this.to)
    var slice = new Slice(mapFragment(oldSlice.content, function (node) {
      return node.mark(this$1.mark.removeFromSet(node.marks))
    }), oldSlice.openLeft, oldSlice.openRight)
    return StepResult.fromReplace(doc, this.from, this.to, slice)
  };

  RemoveMarkStep.prototype.invert = function invert () {
    return new AddMarkStep(this.from, this.to, this.mark)
  };

  RemoveMarkStep.prototype.map = function map (mapping) {
    var from = mapping.mapResult(this.from, 1), to = mapping.mapResult(this.to, -1)
    if (from.deleted && to.deleted || from.pos >= to.pos) { return null }
    return new RemoveMarkStep(from.pos, to.pos, this.mark)
  };

  RemoveMarkStep.prototype.merge = function merge (other) {
    if (other instanceof RemoveMarkStep &&
        other.mark.eq(this.mark) &&
        this.from <= other.to && this.to >= other.from)
      { return new RemoveMarkStep(Math.min(this.from, other.from),
                                Math.max(this.to, other.to), this.mark) }
  };

  RemoveMarkStep.fromJSON = function fromJSON (schema, json) {
    return new RemoveMarkStep(json.from, json.to, schema.markFromJSON(json.mark))
  };

  return RemoveMarkStep;
}(Step));
exports.RemoveMarkStep = RemoveMarkStep

Step.jsonID("removeMark", RemoveMarkStep)

},{"./step":48,"prosemirror-model":26}],46:[function(require,module,exports){
var ref = require("prosemirror-model");
var Fragment = ref.Fragment;
var Slice = ref.Slice;

var ref$1 = require("./replace_step");
var ReplaceStep = ref$1.ReplaceStep;
var ReplaceAroundStep = ref$1.ReplaceAroundStep;
var ref$2 = require("./transform");
var Transform = ref$2.Transform;

// :: (number, number) → Transform
// Delete the content between the given positions.
Transform.prototype.delete = function(from, to) {
  return this.replace(from, to, Slice.empty)
}

// :: (Node, number, ?number, ?Slice) → ?Step
// "Fit" a slice into a given position in the document, producing a
// [step](#transform.Step) that inserts it.
function replaceStep(doc, from, to, slice) {
  if ( to === void 0 ) to = from;
  if ( slice === void 0 ) slice = Slice.empty;

  if (from == to && !slice.size) { return null }

  var $from = doc.resolve(from), $to = doc.resolve(to)
  var placed = placeSlice($from, slice)

  var fittedLeft = fitLeft($from, placed)
  var fitted = fitRight($from, $to, fittedLeft)
  if (!fitted) { return null }
  if (fittedLeft.size != fitted.size && canMoveText($from, $to, fittedLeft)) {
    var d = $to.depth, after = $to.after(d)
    while (d > 1 && after == $to.end(--d)) { ++after }
    var fittedAfter = fitRight($from, doc.resolve(after), fittedLeft)
    if (fittedAfter)
      { return new ReplaceAroundStep(from, after, to, $to.end(), fittedAfter, fittedLeft.size) }
  }
  return new ReplaceStep(from, to, fitted)
}
exports.replaceStep = replaceStep

// :: (number, ?number, ?Slice) → Transform
// Replace the part of the document between `from` and `to` with the
// part of the `source` between `start` and `end`.
Transform.prototype.replace = function(from, to, slice) {
  if ( to === void 0 ) to = from;
  if ( slice === void 0 ) slice = Slice.empty;

  var step = replaceStep(this.doc, from, to, slice)
  if (step) { this.step(step) }
  return this
}

// :: (number, number, union<Fragment, Node, [Node]>) → Transform
// Replace the given range with the given content, which may be a
// fragment, node, or array of nodes.
Transform.prototype.replaceWith = function(from, to, content) {
  return this.replace(from, to, new Slice(Fragment.from(content), 0, 0))
}

// :: (number, union<Fragment, Node, [Node]>) → Transform
// Insert the given content at the given position.
Transform.prototype.insert = function(pos, content) {
  return this.replaceWith(pos, pos, content)
}



function fitLeftInner($from, depth, placed, placedBelow) {
  var content = Fragment.empty, openRight = 0, placedHere = placed[depth]
  if ($from.depth > depth) {
    var inner = fitLeftInner($from, depth + 1, placed, placedBelow || placedHere)
    openRight = inner.openRight + 1
    content = Fragment.from($from.node(depth + 1).copy(inner.content))
  }

  if (placedHere) {
    content = content.append(placedHere.content)
    openRight = placedHere.openRight
  }
  if (placedBelow) {
    content = content.append($from.node(depth).contentMatchAt($from.indexAfter(depth)).fillBefore(Fragment.empty, true))
    openRight = 0
  }

  return {content: content, openRight: openRight}
}

function fitLeft($from, placed) {
  var ref = fitLeftInner($from, 0, placed, false);
  var content = ref.content;
  var openRight = ref.openRight;
  return new Slice(content, $from.depth, openRight || 0)
}

function fitRightJoin(content, parent, $from, $to, depth, openLeft, openRight) {
  var match, count = content.childCount, matchCount = count - (openRight > 0 ? 1 : 0)
  if (openLeft < 0)
    { match = parent.contentMatchAt(matchCount) }
  else if (count == 1 && openRight > 0)
    { match = $from.node(depth).contentMatchAt(openLeft ? $from.index(depth) : $from.indexAfter(depth)) }
  else
    { match = $from.node(depth).contentMatchAt($from.indexAfter(depth))
      .matchFragment(content, count > 0 && openLeft ? 1 : 0, matchCount) }

  var toNode = $to.node(depth)
  if (openRight > 0 && depth < $to.depth) {
    // FIXME find a less allocaty approach
    var after = toNode.content.cutByIndex($to.indexAfter(depth)).addToStart(content.lastChild)
    var joinable$1 = match.fillBefore(after, true)
    // Can't insert content if there's a single node stretched across this gap
    if (joinable$1 && joinable$1.size && openLeft > 0 && count == 1) { joinable$1 = null }

    if (joinable$1) {
      var inner = fitRightJoin(content.lastChild.content, content.lastChild, $from, $to,
                               depth + 1, count == 1 ? openLeft - 1 : -1, openRight - 1)
      if (inner) {
        var last = content.lastChild.copy(inner)
        if (joinable$1.size)
          { return content.cutByIndex(0, count - 1).append(joinable$1).addToEnd(last) }
        else
          { return content.replaceChild(count - 1, last) }
      }
    }
  }
  if (openRight > 0)
    { match = match.matchNode(count == 1 && openLeft > 0 ? $from.node(depth + 1) : content.lastChild) }

  // If we're here, the next level can't be joined, so we see what
  // happens if we leave it open.
  var toIndex = $to.index(depth)
  if (toIndex == toNode.childCount && !toNode.type.compatibleContent(parent.type)) { return null }
  var joinable = match.fillBefore(toNode.content, true, toIndex)
  if (!joinable) { return null }

  if (openRight > 0) {
    var closed = fitRightClosed(content.lastChild, openRight - 1, $from, depth + 1,
                                count == 1 ? openLeft - 1 : -1)
    content = content.replaceChild(count - 1, closed)
  }
  content = content.append(joinable)
  if ($to.depth > depth)
    { content = content.addToEnd(fitRightSeparate($to, depth + 1)) }
  return content
}

function fitRightClosed(node, openRight, $from, depth, openLeft) {
  var match, content = node.content, count = content.childCount
  if (openLeft >= 0)
    { match = $from.node(depth).contentMatchAt($from.indexAfter(depth))
      .matchFragment(content, openLeft > 0 ? 1 : 0, count) }
  else
    { match = node.contentMatchAt(count) }

  if (openRight > 0) {
    var closed = fitRightClosed(content.lastChild, openRight - 1, $from, depth + 1,
                                count == 1 ? openLeft - 1 : -1)
    content = content.replaceChild(count - 1, closed)
  }

  return node.copy(content.append(match.fillBefore(Fragment.empty, true)))
}

function fitRightSeparate($to, depth) {
  var node = $to.node(depth)
  var fill = node.contentMatchAt(0).fillBefore(node.content, true, $to.index(depth))
  if ($to.depth > depth) { fill = fill.addToEnd(fitRightSeparate($to, depth + 1)) }
  return node.copy(fill)
}

function normalizeSlice(content, openLeft, openRight) {
  while (openLeft > 0 && openRight > 0 && content.childCount == 1) {
    content = content.firstChild.content
    openLeft--
    openRight--
  }
  return new Slice(content, openLeft, openRight)
}

// : (ResolvedPos, ResolvedPos, number, Slice) → Slice
function fitRight($from, $to, slice) {
  var fitted = fitRightJoin(slice.content, $from.node(0), $from, $to, 0, slice.openLeft, slice.openRight)
  // FIXME we might want to be clever about selectively dropping nodes here?
  if (!fitted) { return null }
  return normalizeSlice(fitted, slice.openLeft, $to.depth)
}

function canMoveText($from, $to, slice) {
  if (!$to.parent.isTextblock) { return false }

  var match
  if (!slice.openRight) {
    var parent = $from.node($from.depth - (slice.openLeft - slice.openRight))
    if (!parent.isTextblock) { return false }
    match = parent.contentMatchAt(parent.childCount)
    if (slice.size)
      { match = match.matchFragment(slice.content, slice.openLeft ? 1 : 0) }
  } else {
    var parent$1 = nodeRight(slice.content, slice.openRight)
    if (!parent$1.isTextblock) { return false }
    match = parent$1.contentMatchAt(parent$1.childCount)
  }
  match = match.matchFragment($to.parent.content, $to.index())
  return match && match.validEnd()
}

// Algorithm for 'placing' the elements of a slice into a gap:
//
// We consider the content of each node that is open to the left to be
// independently placeable. I.e. in <p("foo"), p("bar")>, when the
// paragraph on the left is open, "foo" can be placed (somewhere on
// the left side of the replacement gap) independently from p("bar").
//
// So placeSlice splits up a slice into a number of sub-slices,
// along with information on where they can be placed on the given
// left-side edge. It works by walking the open side of the slice,
// from the inside out, and trying to find a landing spot for each
// element, by simultaneously scanning over the gap side. When no
// place is found for an open node's content, it is left in that node.
//
// If the outer content can't be placed, a set of wrapper nodes is
// made up for it (by rooting it in the document node type using
// findWrapping), and the algorithm continues to iterate over those.
// This is guaranteed to find a fit, since both stacks now start with
// the same node type (doc).

function nodeLeft(content, depth) {
  for (var i = 1; i < depth; i++) { content = content.firstChild.content }
  return content.firstChild
}

function nodeRight(content, depth) {
  for (var i = 1; i < depth; i++) { content = content.lastChild.content }
  return content.lastChild
}

function placeSlice($from, slice) {
  var dFrom = $from.depth, unplaced = null
  var placed = [], parents = null

  for (var dSlice = slice.openLeft;; --dSlice) {
    var curType = void 0, curAttrs = void 0, curFragment = void 0
    if (dSlice >= 0) {
      if (dSlice > 0) { // Inside slice
        ;var assign;
        ((assign = nodeLeft(slice.content, dSlice), curType = assign.type, curAttrs = assign.attrs, curFragment = assign.content))
      } else if (dSlice == 0) { // Top of slice
        curFragment = slice.content
      }
      if (dSlice < slice.openLeft) { curFragment = curFragment.cut(curFragment.firstChild.nodeSize) }
    } else { // Outside slice
      curFragment = Fragment.empty
      var parent = parents[parents.length + dSlice - 1]
      curType = parent.type
      curAttrs = parent.attrs
    }
    if (unplaced)
      { curFragment = curFragment.addToStart(unplaced) }

    if (curFragment.size == 0 && dSlice <= 0) { break }

    // FIXME cut/remove marks when it helps find a placement
    var found = findPlacement(curFragment, $from, dFrom)
    if (found) {
      if (found.fragment.size > 0) { placed[found.depth] = {
        content: found.fill.append(found.fragment),
        openRight: dSlice > 0 ? 0 : slice.openRight - dSlice,
        depth: found.depth
      } }
      if (dSlice <= 0) { break }
      unplaced = null
      dFrom = Math.max(0, found.depth - 1)
    } else {
      if (dSlice == 0) {
        var top = $from.node(0)
        parents = top.contentMatchAt($from.index(0)).findWrapping(curFragment.firstChild.type, curFragment.firstChild.attrs)
        if (!parents) { break }
        var last = parents[parents.length - 1]
        if (last ? !last.type.contentExpr.matches(last.attrs, curFragment)
                 : !top.canReplace($from.indexAfter(0), $from.depth ? $from.index(0) : $from.indexAfter(0), curFragment)) { break }
        parents = [{type: top.type, attrs: top.attrs}].concat(parents)
        curType = parents[parents.length - 1].type
        curAttrs = parents[parents.length - 1].type
      }
      curFragment = curType.contentExpr.start(curAttrs).fillBefore(curFragment, true).append(curFragment)
      unplaced = curType.create(curAttrs, curFragment)
    }
  }

  return placed
}

function findPlacement(fragment, $from, start) {
  var hasMarks = false
  for (var i = 0; i < fragment.childCount; i++)
    { if (fragment.child(i).marks.length) { hasMarks = true } }
  for (var d = start; d >= 0; d--) {
    var startMatch = $from.node(d).contentMatchAt($from.indexAfter(d))
    var match = startMatch.fillBefore(fragment)
    if (match) { return {depth: d, fill: match, fragment: fragment} }
    if (hasMarks) {
      var stripped = matchStrippingMarks(startMatch, fragment)
      if (stripped) { return {depth: d, fill: Fragment.empty, fragment: stripped} }
    }
  }
}

function matchStrippingMarks(match, fragment) {
  var newNodes = []
  for (var i = 0; i < fragment.childCount; i++) {
    var node = fragment.child(i), stripped = node.mark(node.marks.filter(function (m) { return match.allowsMark(m.type); }))
    match = match.matchNode(stripped)
    if (!match) { return null }
    newNodes.push(stripped)
  }
  return Fragment.from(newNodes)
}

},{"./replace_step":47,"./transform":50,"prosemirror-model":26}],47:[function(require,module,exports){
var ref = require("prosemirror-model");
var Slice = ref.Slice;

var ref$1 = require("./step");
var Step = ref$1.Step;
var StepResult = ref$1.StepResult;
var ref$2 = require("./map");
var StepMap = ref$2.StepMap;

// ::- Replace a part of the document with a slice of new content.
var ReplaceStep = (function (Step) {
  function ReplaceStep(from, to, slice, structure) {
    Step.call(this)
    this.from = from
    this.to = to
    this.slice = slice
    this.structure = !!structure
  }

  if ( Step ) ReplaceStep.__proto__ = Step;
  ReplaceStep.prototype = Object.create( Step && Step.prototype );
  ReplaceStep.prototype.constructor = ReplaceStep;

  ReplaceStep.prototype.apply = function apply (doc) {
    if (this.structure && contentBetween(doc, this.from, this.to))
      { return StepResult.fail("Structure replace would overwrite content") }
    return StepResult.fromReplace(doc, this.from, this.to, this.slice)
  };

  ReplaceStep.prototype.getMap = function getMap () {
    return new StepMap([this.from, this.to - this.from, this.slice.size])
  };

  ReplaceStep.prototype.invert = function invert (doc) {
    return new ReplaceStep(this.from, this.from + this.slice.size, doc.slice(this.from, this.to))
  };

  ReplaceStep.prototype.map = function map (mapping) {
    var from = mapping.mapResult(this.from, 1), to = mapping.mapResult(this.to, -1)
    if (from.deleted && to.deleted) { return null }
    return new ReplaceStep(from.pos, Math.max(from.pos, to.pos), this.slice)
  };

  ReplaceStep.prototype.merge = function merge (other) {
    if (!(other instanceof ReplaceStep) || other.structure != this.structure) { return null }

    if (this.from + this.slice.size == other.from && !this.slice.openRight && !other.slice.openLeft) {
      var slice = this.slice.size + other.slice.size == 0 ? Slice.empty
          : new Slice(this.slice.content.append(other.slice.content), this.slice.openLeft, other.slice.openRight)
      return new ReplaceStep(this.from, this.to + (other.to - other.from), slice, this.structure)
    } else if (other.to == this.from && !this.slice.openLeft && !other.slice.openRight) {
      var slice$1 = this.slice.size + other.slice.size == 0 ? Slice.empty
          : new Slice(other.slice.content.append(this.slice.content), other.slice.openLeft, this.slice.openRight)
      return new ReplaceStep(other.from, this.to, slice$1, this.structure)
    } else {
      return null
    }
  };

  ReplaceStep.prototype.toJSON = function toJSON () {
    var json = {stepType: "replace", from: this.from, to: this.to}
    if (this.slice.size) { json.slice = this.slice.toJSON() }
    if (this.structure) { json.structure = true }
    return json
  };

  ReplaceStep.fromJSON = function fromJSON (schema, json) {
    return new ReplaceStep(json.from, json.to, Slice.fromJSON(schema, json.slice), !!json.structure)
  };

  return ReplaceStep;
}(Step));
exports.ReplaceStep = ReplaceStep

Step.jsonID("replace", ReplaceStep)

// ::- Replace a part of the document with a slice of content, but
// preserve a range of the replaced content by moving it into the
// slice.
var ReplaceAroundStep = (function (Step) {
  function ReplaceAroundStep(from, to, gapFrom, gapTo, slice, insert, structure) {
    Step.call(this)
    this.from = from
    this.to = to
    this.gapFrom = gapFrom
    this.gapTo = gapTo
    this.slice = slice
    this.insert = insert
    this.structure = !!structure
  }

  if ( Step ) ReplaceAroundStep.__proto__ = Step;
  ReplaceAroundStep.prototype = Object.create( Step && Step.prototype );
  ReplaceAroundStep.prototype.constructor = ReplaceAroundStep;

  ReplaceAroundStep.prototype.apply = function apply (doc) {
    if (this.structure && (contentBetween(doc, this.from, this.gapFrom) ||
                           contentBetween(doc, this.gapTo, this.to)))
      { return StepResult.fail("Structure gap-replace would overwrite content") }

    var gap = doc.slice(this.gapFrom, this.gapTo)
    if (gap.openLeft || gap.openRight)
      { return StepResult.fail("Gap is not a flat range") }
    var inserted = this.slice.insertAt(this.insert, gap.content)
    if (!inserted) { return StepResult.fail("Content does not fit in gap") }
    return StepResult.fromReplace(doc, this.from, this.to, inserted)
  };

  ReplaceAroundStep.prototype.getMap = function getMap () {
    return new StepMap([this.from, this.gapFrom - this.from, this.insert,
                        this.gapTo, this.to - this.gapTo, this.slice.size - this.insert])
  };

  ReplaceAroundStep.prototype.invert = function invert (doc) {
    var gap = this.gapTo - this.gapFrom
    return new ReplaceAroundStep(this.from, this.from + this.slice.size + gap,
                                 this.from + this.insert, this.from + this.insert + gap,
                                 doc.slice(this.from, this.to).removeBetween(this.gapFrom - this.from, this.gapTo - this.from),
                                 this.gapFrom - this.from, this.structure)
  };

  ReplaceAroundStep.prototype.map = function map (mapping) {
    var from = mapping.mapResult(this.from, 1), to = mapping.mapResult(this.to, -1)
    var gapFrom = mapping.map(this.gapFrom, -1), gapTo = mapping.map(this.gapTo, 1)
    if ((from.deleted && to.deleted) || gapFrom < from.pos || gapTo > to.pos) { return null }
    return new ReplaceAroundStep(from.pos, to.pos, gapFrom, gapTo, this.slice, this.insert, this.structure)
  };

  ReplaceAroundStep.toJSON = function toJSON () {
    var json = {stepType: "replaceAround", from: this.from, to: this.to,
                gapFrom: this.gapFrom, gapTo: this.gapTo, slice: this.slice.toJSON()}
    if (this.structure) { json.structure = true }
    return true
  };

  ReplaceAroundStep.fromJSON = function fromJSON (schema, json) {
    return new ReplaceAroundStep(json.from, json.to, json.gapFrom, json.gapTo,
                                 Slice.fromJSON(schema, json.slice), json.insert, !!json.structure)
  };

  return ReplaceAroundStep;
}(Step));
exports.ReplaceAroundStep = ReplaceAroundStep

Step.jsonID("replaceAround", ReplaceAroundStep)

function contentBetween(doc, from, to) {
  var $from = doc.resolve(from), dist = to - from, depth = $from.depth
  while (dist > 0 && depth > 0 && $from.indexAfter(depth) == $from.node(depth).childCount) {
    depth--
    dist--
  }
  if (dist > 0) {
    var next = $from.node(depth).maybeChild($from.indexAfter(depth))
    while (dist > 0) {
      if (!next || next.isLeaf) { return true }
      next = next.firstChild
      dist--
    }
  }
  return false
}

},{"./map":43,"./step":48,"prosemirror-model":26}],48:[function(require,module,exports){
var ref = require("prosemirror-model");
var ReplaceError = ref.ReplaceError;

var ref$1 = require("./map");
var StepMap = ref$1.StepMap;

function mustOverride() { throw new Error("Override me") }

var stepsByID = Object.create(null)

// ::- A step object wraps an atomic operation. It generally applies
// only to the document it was created for, since the positions
// associated with it will only make sense for that document.
//
// New steps are defined by creating classes that extend `Step`,
// overriding the `apply`, `invert`, `map`, `getMap` and `fromJSON`
// methods, and registering your class with a unique
// JSON-serialization identifier using
// [`Step.jsonID`](#transform.Step^jsonID).
var Step = function Step () {};

Step.prototype.apply = function apply (_doc) { return mustOverride() };

// :: () → StepMap
// Get the step map that represents the changes made by this
// step.
Step.prototype.getMap = function getMap () { return StepMap.empty };

// :: (doc: Node) → Step
// Create an inverted version of this step. Needs the document as it
// was before the step as argument.
Step.prototype.invert = function invert (_doc) { return mustOverride() };

// :: (mapping: Mappable) → ?Step
// Map this step through a mappable thing, returning either a
// version of that step with its positions adjusted, or `null` if
// the step was entirely deleted by the mapping.
Step.prototype.map = function map (_mapping) { return mustOverride() };

// :: (other: Step) → ?Step
// Try to merge this step with another one, to be applied directly
// after it. Returns the merged step when possible, null if the
// steps can't be merged.
Step.prototype.merge = function merge (_other) { return null };

// :: () → Object
// Create a JSON-serializeable representation of this step. By
// default, it'll create an object with the step's [JSON
// id](#transform.Step^jsonID), and each of the steps's own properties,
// automatically calling `toJSON` on the property values that have
// such a method.
Step.prototype.toJSON = function toJSON () {
    var this$1 = this;

  var obj = {stepType: this.jsonID}
  for (var prop in this) { if (this$1.hasOwnProperty(prop)) {
    var val = this$1[prop]
    obj[prop] = val && val.toJSON ? val.toJSON() : val
  } }
  return obj
};

// :: (Schema, Object) → Step
// Deserialize a step from its JSON representation. Will call
// through to the step class' own implementation of this method.
Step.fromJSON = function fromJSON (schema, json) {
  return stepsByID[json.stepType].fromJSON(schema, json)
};

// :: (string, constructor<Step>)
// To be able to serialize steps to JSON, each step needs a string
// ID to attach to its JSON representation. Use this method to
// register an ID for your step classes. Try to pick something
// that's unlikely to clash with steps from other modules.
Step.jsonID = function jsonID (id, stepClass) {
  if (id in stepsByID) { throw new RangeError("Duplicate use of step JSON ID " + id) }
  stepsByID[id] = stepClass
  stepClass.prototype.jsonID = id
  return stepClass
};
exports.Step = Step

// ::- The result of [applying](#transform.Step.apply) a step. Contains either a
// new document or a failure value.
var StepResult = function StepResult(doc, failed) {
  // :: ?Node The transformed document.
  this.doc = doc
  // :: ?string Text providing information about a failed step.
  this.failed = failed
};

// :: (Node) → StepResult
// Create a successful step result.
StepResult.ok = function ok (doc) { return new StepResult(doc, null) };

// :: (string) → StepResult
// Create a failed step result.
StepResult.fail = function fail (message) { return new StepResult(null, message) };

// :: (Node, number, number, Slice) → StepResult
// Call [`Node.replace`](#model.Node.replace) with the given
// arguments. Create a successful result if it succeeds, and a
// failed one if it throws a `ReplaceError`.
StepResult.fromReplace = function fromReplace (doc, from, to, slice) {
  try {
    return StepResult.ok(doc.replace(from, to, slice))
  } catch (e) {
    if (e instanceof ReplaceError) { return StepResult.fail(e.message) }
    throw e
  }
};
exports.StepResult = StepResult

},{"./map":43,"prosemirror-model":26}],49:[function(require,module,exports){
var ref = require("prosemirror-model");
var Slice = ref.Slice;
var Fragment = ref.Fragment;

var ref$1 = require("./transform");
var Transform = ref$1.Transform;
var ref$2 = require("./replace_step");
var ReplaceStep = ref$2.ReplaceStep;
var ReplaceAroundStep = ref$2.ReplaceAroundStep;

function canCut(node, start, end) {
  return (start == 0 || node.canReplace(start, node.childCount)) &&
    (end == node.childCount || node.canReplace(0, end))
}

// :: (NodeRange) → ?number
// Try to find a target depth to which the content in the given range
// can be lifted.
function liftTarget(range) {
  var parent = range.parent
  var content = parent.content.cutByIndex(range.startIndex, range.endIndex)
  for (var depth = range.depth;; --depth) {
    var node = range.$from.node(depth), index = range.$from.index(depth), endIndex = range.$to.indexAfter(depth)
    if (depth < range.depth && node.canReplace(index, endIndex, content))
      { return depth }
    if (depth == 0 || !canCut(node, index, endIndex)) { break }
  }
}
exports.liftTarget = liftTarget

// :: (NodeRange, number) → Transform
// Split the content in the given range off from its parent, if there
// is sibling content before or after it, and move it up the tree to
// the depth specified by `target`. You'll probably want to use
// `liftTarget` to compute `target`, in order to be sure the lift is
// valid.
Transform.prototype.lift = function(range, target) {
  var $from = range.$from;
  var $to = range.$to;
  var depth = range.depth;

  var gapStart = $from.before(depth + 1), gapEnd = $to.after(depth + 1)
  var start = gapStart, end = gapEnd

  var before = Fragment.empty, openLeft = 0
  for (var d = depth, splitting = false; d > target; d--)
    { if (splitting || $from.index(d) > 0) {
      splitting = true
      before = Fragment.from($from.node(d).copy(before))
      openLeft++
    } else {
      start--
    } }
  var after = Fragment.empty, openRight = 0
  for (var d$1 = depth, splitting$1 = false; d$1 > target; d$1--)
    { if (splitting$1 || $to.after(d$1 + 1) < $to.end(d$1)) {
      splitting$1 = true
      after = Fragment.from($to.node(d$1).copy(after))
      openRight++
    } else {
      end++
    } }

  return this.step(new ReplaceAroundStep(start, end, gapStart, gapEnd,
                                         new Slice(before.append(after), openLeft, openRight),
                                         before.size - openLeft, true))
}

// :: (NodeRange, NodeType, ?Object) → ?[{type: NodeType, attrs: ?Object}]
// Try to find a valid way to wrap the content in the given range in a
// node of the given type. May introduce extra nodes around and inside
// the wrapper node, if necessary. Returns null if no valid wrapping
// could be found.
function findWrapping(range, nodeType, attrs, innerRange) {
  if ( innerRange === void 0 ) innerRange = range;

  var wrap = {type: nodeType, attrs: attrs}
  var around = findWrappingOutside(range, wrap)
  var inner = around && findWrappingInside(innerRange, wrap)
  if (!inner) { return null }
  return around.concat(wrap).concat(inner)
}
exports.findWrapping = findWrapping

function findWrappingOutside(range, wrap) {
  var parent = range.parent;
  var startIndex = range.startIndex;
  var endIndex = range.endIndex;
  var around = parent.contentMatchAt(startIndex).findWrapping(wrap.type, wrap.attrs)
  if (!around) { return null }
  var outer = around.length ? around[0] : wrap
  if (!parent.canReplaceWith(startIndex, endIndex, outer.type, outer.attrs))
    { return null }
  return around
}

function findWrappingInside(range, wrap) {
  var parent = range.parent;
  var startIndex = range.startIndex;
  var endIndex = range.endIndex;
  var inner = parent.child(startIndex)
  var inside = wrap.type.contentExpr.start(wrap.attrs).findWrapping(inner.type, inner.attrs)
  if (!inside) { return null }
  var last = inside.length ? inside[inside.length - 1] : wrap
  var innerMatch = last.type.contentExpr.start(last.attrs)
  for (var i = startIndex; i < endIndex; i++)
    { innerMatch = innerMatch && innerMatch.matchNode(parent.child(i)) }
  if (!innerMatch || !innerMatch.validEnd()) { return null }
  return inside
}

// :: (NodeRange, [{type: NodeType, attrs: ?Object}]) → Transform
// Wrap the given [range](#model.NodeRange) in the given set of wrappers.
// The wrappers are assumed to be valid in this position, and should
// probably be computed with `findWrapping`.
Transform.prototype.wrap = function(range, wrappers) {
  var content = Fragment.empty
  for (var i = wrappers.length - 1; i >= 0; i--)
    { content = Fragment.from(wrappers[i].type.create(wrappers[i].attrs, content)) }

  var start = range.start, end = range.end
  return this.step(new ReplaceAroundStep(start, end, start, end, new Slice(content, 0, 0), wrappers.length, true))
}

// :: (number, ?number, NodeType, ?Object) → Transform
// Set the type of all textblocks (partly) between `from` and `to` to
// the given node type with the given attributes.
Transform.prototype.setBlockType = function(from, to, type, attrs) {
  var this$1 = this;
  if ( to === void 0 ) to = from;

  if (!type.isTextblock) { throw new RangeError("Type given to setBlockType should be a textblock") }
  var mapFrom = this.steps.length
  this.doc.nodesBetween(from, to, function (node, pos) {
    if (node.isTextblock && !node.hasMarkup(type, attrs)) {
      // Ensure all markup that isn't allowed in the new node type is cleared
      this$1.clearMarkupFor(this$1.mapping.map(pos, 1, mapFrom), type, attrs)
      var startM = this$1.mapping.map(pos, 1, mapFrom), endM = this$1.mapping.map(pos + node.nodeSize, 1, mapFrom)
      this$1.step(new ReplaceAroundStep(startM, endM, startM + 1, endM - 1,
                                      new Slice(Fragment.from(type.create(attrs)), 0, 0), 1, true))
      return false
    }
  })
  return this
}

// :: (number, ?NodeType, ?Object) → Transform
// Change the type and attributes of the node after `pos`.
Transform.prototype.setNodeType = function(pos, type, attrs) {
  var node = this.doc.nodeAt(pos)
  if (!node) { throw new RangeError("No node at given position") }
  if (!type) { type = node.type }
  if (node.isLeaf)
    { return this.replaceWith(pos, pos + node.nodeSize, type.create(attrs, null, node.marks)) }

  if (!type.validContent(node.content, attrs))
    { throw new RangeError("Invalid content for node type " + type.name) }

  return this.step(new ReplaceAroundStep(pos, pos + node.nodeSize, pos + 1, pos + node.nodeSize - 1,
                                         new Slice(Fragment.from(type.create(attrs)), 0, 0), 1, true))
}

// :: (Node, number, ?[?{type: NodeType, attrs: ?Object}]) → bool
// Check whether splitting at the given position is allowed.
function canSplit(doc, pos, depth, typesAfter) {
  if ( depth === void 0 ) depth = 1;

  var $pos = doc.resolve(pos), base = $pos.depth - depth
  if (base < 0 ||
      !$pos.parent.canReplace($pos.index(), $pos.parent.childCount) ||
      !$pos.parent.canReplace(0, $pos.indexAfter()))
    { return false }
  for (var d = $pos.depth - 1, i = depth - 1; d > base; d--, i--) {
    var node = $pos.node(d), index$1 = $pos.index(d)
    var typeAfter = typesAfter && typesAfter[i]
    if (!node.canReplace(0, index$1) ||
        !node.canReplaceWith(index$1, node.childCount, typeAfter ? typeAfter.type : $pos.node(d + 1).type,
                             typeAfter ? typeAfter.attrs : $pos.node(d + 1).attrs))
      { return false }
  }
  var index = $pos.indexAfter(base)
  var baseType = typesAfter && typesAfter[0]
  return $pos.node(base).canReplaceWith(index, index, baseType ? baseType.type : $pos.node(base + 1).type,
                                        baseType ? baseType.attrs : $pos.node(base + 1).attrs)
}
exports.canSplit = canSplit

// :: (number, ?number, ?[?{type: NodeType, attrs: ?Object}]) → Transform
// Split the node at the given position, and optionally, if `depth` is
// greater than one, any number of nodes above that. By default, the
// parts split off will inherit the node type of the original node.
// This can be changed by passing an array of types and attributes to
// use after the split.
Transform.prototype.split = function(pos, depth, typesAfter) {
  if ( depth === void 0 ) depth = 1;

  var $pos = this.doc.resolve(pos), before = Fragment.empty, after = Fragment.empty
  for (var d = $pos.depth, e = $pos.depth - depth, i = depth - 1; d > e; d--, i--) {
    before = Fragment.from($pos.node(d).copy(before))
    var typeAfter = typesAfter && typesAfter[i]
    after = Fragment.from(typeAfter ? typeAfter.type.create(typeAfter.attrs, after) : $pos.node(d).copy(after))
  }
  return this.step(new ReplaceStep(pos, pos, new Slice(before.append(after), depth, depth, true)))
}

// :: (Node, number) → bool
// Test whether the blocks before and after a given position can be
// joined.
function canJoin(doc, pos) {
  var $pos = doc.resolve(pos), index = $pos.index()
  return joinable($pos.nodeBefore, $pos.nodeAfter) &&
    $pos.parent.canReplace(index, index + 1)
}
exports.canJoin = canJoin

function joinable(a, b) {
  return a && b && !a.isLeaf && a.canAppend(b)
}

// :: (Node, number, ?number) → ?number
// Find an ancestor of the given position that can be joined to the
// block before (or after if `dir` is positive). Returns the joinable
// point, if any.
function joinPoint(doc, pos, dir) {
  if ( dir === void 0 ) dir = -1;

  var $pos = doc.resolve(pos)
  for (var d = $pos.depth;; d--) {
    var before = void 0, after = void 0
    if (d == $pos.depth) {
      before = $pos.nodeBefore
      after = $pos.nodeAfter
    } else if (dir > 0) {
      before = $pos.node(d + 1)
      after = $pos.node(d).maybeChild($pos.index(d) + 1)
    } else {
      before = $pos.node(d).maybeChild($pos.index(d) - 1)
      after = $pos.node(d + 1)
    }
    if (before && !before.isTextblock && joinable(before, after)) { return pos }
    if (d == 0) { break }
    pos = dir < 0 ? $pos.before(d) : $pos.after(d)
  }
}
exports.joinPoint = joinPoint

// :: (number, ?number, ?bool) → Transform
// Join the blocks around the given position. If depth is 2, their
// last and first siblings are also joined, and so on.
Transform.prototype.join = function(pos, depth) {
  if ( depth === void 0 ) depth = 1;

  var step = new ReplaceStep(pos - depth, pos + depth, Slice.empty, true)
  return this.step(step)
}

// :: (Node, number, NodeType, ?Object) → ?number
// Try to find a point where a node of the given type can be inserted
// near `pos`, by searching up the node hierarchy when `pos` itself
// isn't a valid place but is at the start or end of a node. Return
// null if no position was found.
function insertPoint(doc, pos, nodeType, attrs) {
  var $pos = doc.resolve(pos)
  if ($pos.parent.canReplaceWith($pos.index(), $pos.index(), nodeType, attrs)) { return pos }

  if ($pos.parentOffset == 0)
    { for (var d = $pos.depth - 1; d >= 0; d--) {
      var index = $pos.index(d)
      if ($pos.node(d).canReplaceWith(index, index, nodeType, attrs)) { return $pos.before(d + 1) }
      if (index > 0) { return null }
    } }
  if ($pos.parentOffset == $pos.parent.content.size)
    { for (var d$1 = $pos.depth - 1; d$1 >= 0; d$1--) {
      var index$1 = $pos.indexAfter(d$1)
      if ($pos.node(d$1).canReplaceWith(index$1, index$1, nodeType, attrs)) { return $pos.after(d$1 + 1) }
      if (index$1 < $pos.node(d$1).childCount) { return null }
    } }
}
exports.insertPoint = insertPoint

},{"./replace_step":47,"./transform":50,"prosemirror-model":26}],50:[function(require,module,exports){
var ref = require("./map");
var Mapping = ref.Mapping;

var TransformError = (function (Error) {
  function TransformError () {
    Error.apply(this, arguments);
  }if ( Error ) TransformError.__proto__ = Error;
  TransformError.prototype = Object.create( Error && Error.prototype );
  TransformError.prototype.constructor = TransformError;

  

  return TransformError;
}(Error));
exports.TransformError = TransformError

// ::- Abstraction to build up and track such an array of
// [steps](#transform.Step).
//
// The high-level transforming methods return the `Transform` object
// itself, so that they can be chained.
var Transform = function Transform(doc) {
  // :: Node
  // The current document (the result of applying the steps in the
  // transform).
  this.doc = doc
  // :: [Step]
  // The steps in this transform.
  this.steps = []
  // :: [Node]
  // The documents before each of the steps.
  this.docs = []
  // :: Mapping
  // A mapping with the maps for each of the steps in this transform.
  this.mapping = new Mapping
};

var prototypeAccessors = { before: {} };

// :: Node The document at the start of the transformation.
prototypeAccessors.before.get = function () { return this.docs.length ? this.docs[0] : this.doc };

// :: (Step) → Transform
// Apply a new step in this transformation, saving the result.
// Throws an error when the step fails.
Transform.prototype.step = function step (step) {
  var result = this.maybeStep(step)
  if (result.failed) { throw new TransformError(result.failed) }
  return this
};

// :: (Step) → StepResult
// Try to apply a step in this transformation, ignoring it if it
// fails. Returns the step result.
Transform.prototype.maybeStep = function maybeStep (step) {
  var result = step.apply(this.doc)
  if (!result.failed) {
    this.docs.push(this.doc)
    this.steps.push(step)
    this.mapping.appendMap(step.getMap())
    this.doc = result.doc
  }
  return result
};

Object.defineProperties( Transform.prototype, prototypeAccessors );
exports.Transform = Transform

},{"./map":43}],51:[function(require,module,exports){
var ie_upto10 = /MSIE \d/.test(navigator.userAgent)
var ie_11up = /Trident\/(?:[7-9]|\d{2,})\..*rv:(\d+)/.exec(navigator.userAgent)

module.exports = {
  mac: /Mac/.test(navigator.platform),
  ie: ie_upto10 || !!ie_11up,
  ie_version: ie_upto10 ? document.documentMode || 6 : ie_11up && +ie_11up[1],
  gecko: /gecko\/\d/i.test(navigator.userAgent),
  ios: /AppleWebKit/.test(navigator.userAgent) && /Mobile\/\w+/.test(navigator.userAgent)
}

},{}],52:[function(require,module,exports){
var ref = require("prosemirror-state");
var Selection = ref.Selection;
var NodeSelection = ref.NodeSelection;
var TextSelection = ref.TextSelection;
var isSelectable = ref.isSelectable;
var browser = require("./browser")

var ref$1 = require("./selection");
var verticalMotionLeavesTextblock = ref$1.verticalMotionLeavesTextblock;

function moveSelectionBlock(state, dir) {
  var ref = state.selection;
  var $from = ref.$from;
  var $to = ref.$to;
  var node = ref.node;
  var $side = dir > 0 ? $to : $from
  var $start = node && node.isBlock ? $side : $side.depth ? state.doc.resolve(dir > 0 ? $side.after() : $side.before()) : null
  return $start && Selection.findFrom($start, dir)
}

function apply(view, sel) {
  view.props.onAction(sel.action({scrollIntoView: true}))
  return true
}

function selectHorizontally(view, dir) {
  var ref = view.state.selection;
  var empty = ref.empty;
  var node = ref.node;
  var $from = ref.$from;
  var $to = ref.$to;
  if (!empty && !node) { return false }

  if (node && node.isInline)
    { return apply(view, new TextSelection(dir > 0 ? $to : $from)) }

  if (!node) {
    var ref$1 = dir > 0
        ? $from.parent.childAfter($from.parentOffset)
        : $from.parent.childBefore($from.parentOffset);
    var nextNode = ref$1.node;
    var offset = ref$1.offset;
    if (nextNode) {
      if (isSelectable(nextNode) && offset == $from.parentOffset - (dir > 0 ? 0 : nextNode.nodeSize))
        { return apply(view, new NodeSelection(dir < 0 ? view.state.doc.resolve($from.pos - nextNode.nodeSize) : $from))
      ; }(dir < 0 ? skipIgnoredNodesLeft : skipIgnoredNodesRight)(view)
      return null
    }
  }

  var next = moveSelectionBlock(view.state, dir)
  if (next && (next instanceof NodeSelection || node))
    { return apply(view, next) }

  return false
}

function nodeLen(node) {
  return node.nodeType == 3 ? node.nodeValue.length : node.childNodes.length
}

// Make sure the cursor isn't directly after one or more ignored
// nodes, which will confuse the browser's cursor motion logic.
function skipIgnoredNodesLeft(view) {
  var sel = view.root.getSelection(), moved = false
  var node = sel.anchorNode, offset = sel.anchorOffset
  for (;;) {
    if (offset > 0) {
      if (node.nodeType != 1) { break }
      var before = node.childNodes[offset - 1]
      if (before.nodeType == 1 && before.hasAttribute("pm-ignore")) { moved = true; offset-- }
      else { break }
    } else {
      var prev = node.previousSibling
      while (prev && prev.nodeType == 1 && prev.hasAttribute("pm-ignore")) { moved = prev = prev.previousSibling }
      if (!prev) {
        node = node.parentNode
        if (node == view.content) { break }
        offset = 0
      } else {
        node = prev
        offset = nodeLen(node)
      }
    }
  }
  if (moved) { setSel(sel, node, offset) }
}

// Make sure the cursor isn't directly before one or more ignored
// nodes.
function skipIgnoredNodesRight(view) {
  var sel = view.root.getSelection(), moved = false
  var node = sel.anchorNode, offset = sel.anchorOffset, len = nodeLen(node)
  for (;;) {
    if (offset < len) {
      if (node.nodeType != 1) { break }
      var after = node.childNodes[offset]
      if (after.nodeType == 1 && after.hasAttribute("pm-ignore")) { moved = true; offset++ }
      else { break }
    } else {
      var next = node.nextSibling
      while (next && next.nodeType == 1 && next.hasAttribute("pm-ignore")) { moved = next = next.previousSibling }
      if (!next) {
        node = node.parentNode
        if (node == view.content) { break }
        offset = len = 0
      } else {
        node = next
        offset = 0
        len = nodeLen(node)
      }
    }
  }
  if (moved) { setSel(sel, node, offset) }
}

function setSel(sel, node, offset) {
  var range = document.createRange()
  range.setEnd(node, offset)
  range.setStart(node, offset)
  sel.removeAllRanges()
  sel.addRange(range)
}

// : (EditorState, number)
// Check whether vertical selection motion would involve node
// selections. If so, apply it (if not, the result is left to the
// browser)
function selectVertically(view, dir) {
  var ref = view.state.selection;
  var empty = ref.empty;
  var node = ref.node;
  var $from = ref.$from;
  var $to = ref.$to;
  if (!empty && !node) { return false }

  var leavingTextblock = true, $start = dir < 0 ? $from : $to
  if (!node || node.isInline)
    { leavingTextblock = verticalMotionLeavesTextblock(view, dir) } // FIXME need access to the view

  if (leavingTextblock) {
    var next = moveSelectionBlock(view.state, dir)
    if (next && (next instanceof NodeSelection))
      { return apply(view, next) }
  }

  if (!node || node.isInline) { return false }

  var beyond = Selection.findFrom($start, dir)
  return beyond ? apply(view, beyond) : true
}

// A backdrop keymap used to make sure we always suppress keys that
// have a dangerous default effect, even if the commands they are
// bound to return false, and to make sure that cursor-motion keys
// find a cursor (as opposed to a node selection) when pressed. For
// cursor-motion keys, the code in the handlers also takes care of
// block selections.

function captureKeyDown(view, event) {
  var code = event.keyCode, mod = browser.mac ? event.metaKey : event.ctrlKey
  if (code == 8) { // Backspace
    return browser.ios ? false : true
  } else if (code == 13 || code == 27 || code == 46) { // Enter, Esc, Delete
    return true
  } else if (mod && !event.altKey && !event.shiftKey &&
             (code == 66 || code == 73 || code == 89 || code == 90 || code == 68 || code == 72)) { // Mod-[BIYZDH]
    return true
  } else if (code == 68 && event.altKey && !mod && !event.shiftKey) { // Alt-D
    return true
  } else if (code == 37) { // Left arrow
    return selectHorizontally(view, -1)
  } else if (code == 39) { // Right arrow
    return selectHorizontally(view, 1)
  } else if (code == 38) { // Up arrow
    return selectVertically(view, -1)
  } else if (code == 40) { // Down arrow
    return selectVertically(view, 1)
  }
}
exports.captureKeyDown = captureKeyDown

},{"./browser":51,"./selection":59,"prosemirror-state":37}],53:[function(require,module,exports){
var WidgetType = function WidgetType(widget, options) {
  if (widget.nodeType != 1) {
    var wrap = document.createElement("span")
    wrap.appendChild(widget)
    widget = wrap
  }
  widget.setAttribute("pm-ignore", "true")
  widget.contentEditable = false
  this.widget = widget
  this.options = options || noOptions
};

WidgetType.prototype.map = function map (mapping, span, offset, oldOffset) {
  var ref = mapping.mapResult(span.from + oldOffset);
    var pos = ref.pos;
    var deleted = ref.deleted;
  return deleted ? null : new Decoration(pos - offset, pos - offset, this)
};

WidgetType.prototype.valid = function valid () { return true };

WidgetType.prototype.apply = function apply (domParent, domNode) {
  domParent.insertBefore(this.widget, domNode)
  return domNode
};

var InlineType = function InlineType(attrs, options) {
  this.options = options || noOptions
  this.attrs = attrs
};

InlineType.prototype.map = function map (mapping, span, offset, oldOffset) {
  var from = mapping.map(span.from + oldOffset, this.options.inclusiveLeft ? -1 : 1) - offset
  var to = mapping.map(span.to + oldOffset, this.options.inclusiveRight ? 1 : -1) - offset
  return from >= to ? null : new Decoration(from, to, this)
};

InlineType.prototype.valid = function valid () { return true };

InlineType.prototype.apply = function apply (_domParent, domNode) {
  return applyContentDecoration(this.attrs, domNode)
};

InlineType.is = function is (span) { return span.type instanceof InlineType };

var NodeType = function NodeType(attrs, options) {
  this.attrs = attrs
  this.options = options || noOptions
};

NodeType.prototype.map = function map (mapping, span, offset, oldOffset) {
  var from = mapping.mapResult(span.from + oldOffset, 1)
  if (from.deleted) { return null }
  var to = mapping.mapResult(span.to + oldOffset, -1)
  if (to.deleted || to.pos <= from.pos) { return null }
  return new Decoration(from.pos - offset, to.pos - offset, this)
};

NodeType.prototype.valid = function valid (node, span) {
  var ref = node.content.findIndex(span.from);
    var index = ref.index;
    var offset = ref.offset;
  return offset == span.from && offset + node.child(index).nodeSize == span.to
};

NodeType.prototype.apply = function apply (_domParent, domNode) {
  return applyContentDecoration(this.attrs, domNode)
};

var DummyType = function DummyType () {};

DummyType.prototype.valid = function valid () { return true };
DummyType.prototype.apply = function apply () {};

function applyContentDecoration(attrs, domNode) {
  for (var name in attrs) {
    var val = attrs[name]
    if (name == "class") { domNode.classList.add(val) }
    else if (name == "style") { domNode.style.cssText += ";" + val }
    else if (name != "wrapper") { domNode.setAttribute(name, val) }
  }
  var wrap = attrs.wrapper
  return wrap ? wrapNode(domNode, wrap) : domNode
}

function wrapNode(domNode, wrapper) {
  domNode.parentNode.replaceChild(wrapper, domNode)
  var position = wrapper.querySelector("[pm-placeholder]")
  if (position && position != wrapper) {
    position.parentNode.replaceChild(domNode, position)
    domNode.setAttribute("pm-placeholder", "true")
  } else {
    wrapper.appendChild(domNode)
  }
  wrapper.setAttribute("pm-size", domNode.getAttribute("pm-size"))
  domNode.removeAttribute("pm-size")
  wrapper.setAttribute("pm-offset", domNode.getAttribute("pm-offset"))
  domNode.removeAttribute("pm-offset")
  wrapper.setAttribute("pm-decoration", "true")
  return wrapper
}

// ::- Decorations can be provided to the view (through the
// [`decorations` prop](#view.EditorProps.decorations)) to adjust the
// way the document is drawn. They come in several variants. See the
// static members of this class for details.
var Decoration = function Decoration(from, to, type) {
  this.from = from
  this.to = to
  this.type = type
};

var prototypeAccessors = { options: {} };

Decoration.prototype.copy = function copy (from, to) {
  return new Decoration(from, to, this.type)
};

Decoration.prototype.sameOutput = function sameOutput (other) {
  return this.type == other.type && this.from == other.from && this.to == other.to
};

Decoration.prototype.map = function map (mapping, offset, oldOffset) {
  return this.type.map(mapping, this, offset, oldOffset)
};

// :: (number, dom.Node, ?Object) → Decoration
// Creates a widget decoration, which is a DOM node that's shown in
// the document at the given position.
Decoration.widget = function widget (pos, widget, options) {
  return new Decoration(pos, pos, new WidgetType(widget, options))
};

// :: (number, number, DecorationAttrs, ?Object) → Decoration
// Creates an inline decoration, which adds the given attributes to
// each inline node between `from` and `to`.
//
// options::- These options are recognized:
//
//   inclusiveLeft:: ?bool
//   Determines how the left side of the decoration is
//   [mapped](#transform.Position_Mapping) when content is
//   inserted directly at that positon. By default, the decoration
//   won't include the new content, but you can set this to `true`
//   to make it inclusive.
//
//   inclusiveRight:: ?bool
//   Determines how the right side of the decoration is mapped.
//   See
//   [`inclusiveLeft`](#view.Decoration^inline^options.inclusiveLeft).
Decoration.inline = function inline (from, to, attrs, options) {
  return new Decoration(from, to, new InlineType(attrs, options))
};

// :: (number, number, DecorationAttrs, ?Object) → Decoration
// Creates a node decoration. `from` and `to` should point precisely
// before and after a node in the document. That node, and only that
// node, will receive the given attributes.
Decoration.node = function node (from, to, attrs, options) {
  return new Decoration(from, to, new NodeType(attrs, options))
};

// :: Object
// The options provided when creating this decoration. Can be useful
// if you've stored extra information in that object.
prototypeAccessors.options.get = function () { return this.type.options };

Object.defineProperties( Decoration.prototype, prototypeAccessors );
exports.Decoration = Decoration

// DecorationAttrs:: interface
// A set of attributes to add to a decorated node. Most properties
// simply directly correspond to DOM attributes of the same name,
// which will be set to the property's value. These are exceptions:
//
//   class:: ?string
//   A CSS class name to be _added_ to the classes that the node
//   already had.
//
//   style:: ?string
//   A string of CSS to be _added_ to the node's existing `style` property.
//
//   wrapper:: ?dom.Node
//   A DOM node to use as a wrapper around the original node. Will be
//   applied after the other properties. By default, the original node
//   will be appended to this. If you put a child node with a
//   `pm-placeholder` attribute in the node, that node will be
//   replaced with the original node.

var none = [], noOptions = {}

// ::- A collection of [decorations](#view.Decoration), organized in
// such a way that the drawing algorithm can efficiently use and
// compare them. This is a persistent data structure—it is not
// modified, updates create a new value.
var DecorationSet = function DecorationSet(local, children) {
  this.local = local && local.length ? local : none
  this.children = children && children.length ? children : none
};

// :: (Node, [Decoration]) → DecorationSet
// Create a set of decorations, using the structure of the given
// document.
DecorationSet.create = function create (doc, decorations) {
  return decorations.length ? buildTree(decorations, doc, 0) : empty
};

// :: (?number, ?number) → [Decoration]
// Find all decorations in this set which touch the given range
// (including decorations that start or end directly at the
// boundaries). When the arguments are omitted, all decorations in
// the set are collected.
DecorationSet.prototype.find = function find (start, end) {
  var result = []
  this.findInner(start == null ? 0 : start, end == null ? 1e9 : end, result, 0)
  return result
};

DecorationSet.prototype.findInner = function findInner (start, end, result, offset) {
    var this$1 = this;

  for (var i = 0; i < this.local.length; i++) {
    var span = this$1.local[i]
    if (span.from <= end && span.to >= start)
      { result.push(span.copy(span.from + offset, span.to + offset)) }
  }
  for (var i$1 = 0; i$1 < this.children.length; i$1 += 3) {
    if (this$1.children[i$1] < end && this$1.children[i$1 + 1] > start) {
      var childOff = this$1.children[i$1] + 1
      this$1.children[i$1 + 2].findInner(start - childOff, end - childOff, result, offset + childOff)
    }
  }
};

// :: (Mapping, Node) → DecorationSet
// Map the set of decorations in response to a change in the
// document.
DecorationSet.prototype.map = function map (mapping, doc) {
  if (this == empty) { return this }
  return this.mapInner(mapping, doc, 0, 0)
};

DecorationSet.prototype.mapInner = function mapInner (mapping, node, offset, oldOffset) {
    var this$1 = this;

  var newLocal
  for (var i = 0; i < this.local.length; i++) {
    var mapped = this$1.local[i].map(mapping, offset, oldOffset)
    if (mapped && mapped.type.valid(node, mapped)) { (newLocal || (newLocal = [])).push(mapped) }
  }

  if (this.children.length)
    { return mapChildren(this.children, newLocal, mapping, node, offset, oldOffset) }
  else
    { return newLocal ? new DecorationSet(newLocal.sort(byPos)) : empty }
};

// :: (Node, [Decoration]) → DecorationSet
// Add the given array of decorations to the ones in the set,
// producing a new set. Needs access to the current document to
// create the appropriate tree structure.
DecorationSet.prototype.add = function add (doc, decorations) {
  if (!decorations.length) { return this }
  if (this == empty) { return DecorationSet.create(doc, decorations) }
  return this.addInner(doc, decorations, 0)
};

DecorationSet.prototype.addInner = function addInner (doc, decorations, offset) {
    var this$1 = this;

  var children, childIndex = 0
  doc.forEach(function (childNode, childOffset) {
    var baseOffset = childOffset + offset, found
    if (!(found = takeSpansForNode(decorations, childNode, baseOffset))) { return }

    if (!children) { children = this$1.children.slice() }
    while (childIndex < children.length && children[childIndex] < childOffset) { childIndex += 3 }
    if (children[childIndex] == childOffset)
      { children[childIndex + 2] = children[childIndex + 2].addInner(childNode, found, baseOffset + 1) }
    else
      { children.splice(childIndex, 0, childOffset, childOffset + childNode.nodeSize, buildTree(found, childNode, baseOffset + 1)) }
    childIndex += 3
  })

  var local = moveSpans(childIndex ? withoutNulls(decorations) : decorations, -offset)
  return new DecorationSet(local.length ? this.local.concat(local).sort(byPos) : this.local,
                           children || this.children)
};

// :: ([Decoration]) → DecorationSet
// Create a new set that contains the decorations in this set, minus
// the ones in the given array.
DecorationSet.prototype.remove = function remove (decorations) {
  if (decorations.length == 0 || this == empty) { return this }
  return this.removeInner(decorations, 0)
};

DecorationSet.prototype.removeInner = function removeInner (decorations, offset) {
    var this$1 = this;

  var children = this.children, local = this.local
  for (var i = 0; i < children.length; i += 3) {
    var found = void 0, from = children[i] + offset, to = children[i + 1] + offset
    for (var j = 0, span = void 0; j < decorations.length; j++) { if (span = decorations[j]) {
      if (span.from > from && span.to < to) {
        decorations[j] = null
        ;(found || (found = [])).push(span)
      }
    } }
    if (!found) { continue }
    if (children == this$1.children) { children = this$1.children.slice() }
    var removed = children[i + 2].removeInner(found, from + 1)
    if (removed != empty) {
      children[i + 2] = removed
    } else {
      children.splice(i, 3)
      i -= 3
    }
  }
  if (local.length) { for (var i$1 = 0, span$1 = void 0; i$1 < decorations.length; i$1++) { if (span$1 = decorations[i$1]) {
    for (var j$1 = 0; j$1 < local.length; j$1++) { if (local[j$1].type == span$1.type) {
      if (local == this$1.local) { local = this$1.local.slice() }
      local.splice(j$1--, 1)
    } }
  } } }
  if (children == this.children && local == this.local) { return this }
  return local.length || children.length ? new DecorationSet(local, children) : empty
};

DecorationSet.prototype.forChild = function forChild (offset, node) {
    var this$1 = this;

  if (this == empty) { return this }
  if (node.isLeaf) { return DecorationSet.empty }

  var child, local
  for (var i = 0; i < this.children.length; i += 3) { if (this$1.children[i] >= offset) {
    if (this$1.children[i] == offset) { child = this$1.children[i + 2] }
    break
  } }
  var start = offset + 1, end = start + node.content.size
  for (var i$1 = 0; i$1 < this.local.length; i$1++) {
    var dec = this$1.local[i$1]
    if (dec.from < end && dec.to > start)
      { (local || (local = [])).push(dec.copy(Math.max(start, dec.from) - start,
                                            Math.min(end, dec.to) - start)) }
  }
  if (local && local.some(InlineType.is)) {
    var localSet = new DecorationSet(local.filter(InlineType.is))
    return child ? new DecorationGroup([localSet, child]) : localSet
  }
  return child || empty
};

DecorationSet.prototype.sameOutput = function sameOutput (other) {
    var this$1 = this;

  if (this == other) { return true }
  if (!(other instanceof DecorationSet) ||
      this.local.length != other.local.length ||
      this.children.length != other.children.length) { return false }
  for (var i = 0; i < this.local.length; i++)
    { if (!this$1.local[i].sameOutput(other.local[i])) { return false } }
  for (var i$1 = 0; i$1 < this.children.length; i$1 += 3)
    { if (this$1.children[i$1] != other.children[i$1] ||
        this$1.children[i$1 + 1] != other.children[i$1 + 1] ||
        !this$1.children[i$1 + 2].sameOutput(other.children[i$1 + 2])) { return false } }
  return false
};

DecorationSet.prototype.locals = function locals (node) {
  return removeOverlap(this.localsInner(node))
};

DecorationSet.prototype.localsInner = function localsInner (node) {
    var this$1 = this;

  if (this == empty) { return none }
  if (node.isTextblock || !this.local.some(InlineType.is)) { return this.local }
  var result = []
  for (var i = 0; i < this.local.length; i++) {
    if (!(this$1.local[i].type instanceof InlineType))
      { result.push(this$1.local[i]) }
  }
  return result
};
exports.DecorationSet = DecorationSet

var empty = new DecorationSet()

// :: DecorationSet
// The empty set of decorations.
DecorationSet.empty = empty

var DecorationGroup = function DecorationGroup(members) {
  this.members = members
};

DecorationGroup.prototype.forChild = function forChild (offset, child) {
    var this$1 = this;

  if (child.isLeaf) { return DecorationSet.empty }
  var found = []
  for (var i = 0; i < this.members.length; i++) {
    var result = this$1.members[i].forChild(offset, child)
    if (result == empty) { continue }
    if (result instanceof DecorationGroup) { found = found.concat(result.members) }
    else { found.push(result) }
  }
  return DecorationGroup.from(found)
};

DecorationGroup.prototype.sameOutput = function sameOutput (other) {
    var this$1 = this;

  if (!(other instanceof DecorationGroup) ||
      other.members.length != this.members.length) { return false }
  for (var i = 0; i < this.members.length; i++)
    { if (!this$1.members[i].sameOutput(other.members[i])) { return false } }
  return true
};

DecorationGroup.prototype.locals = function locals (node) {
    var this$1 = this;

  var result, sorted = true
  for (var i = 0; i < this.members.length; i++) {
    var locals = this$1.members[i].localsInner(node)
    if (!locals.length) { continue }
    if (!result) {
      result = locals
    } else {
      if (sorted) {
        result = result.slice()
        sorted = false
      }
      for (var j = 0; j < locals.length; j++) { result.push(locals[j]) }
    }
  }
  return result ? removeOverlap(sorted ? result : result.sort(byPos)) : none
};

DecorationGroup.from = function from (members) {
  switch (members.length) {
    case 0: return empty
    case 1: return members[0]
    default: return new DecorationGroup(members)
  }
};
exports.DecorationGroup = DecorationGroup

function mapChildren(oldChildren, newLocal, mapping, node, offset, oldOffset) {
  var children = oldChildren.slice()

  // Mark the children that are directly touched by changes, and
  // move those after changes.
  var shift = function (oldStart, oldEnd, newStart, newEnd) {
    for (var i = 0; i < children.length; i += 3) {
      var end = children[i + 1], dSize = void 0
      if (end == -1 || oldStart > end + oldOffset) { continue }
      if (oldEnd >= children[i] + oldOffset) {
        children[i + 1] = -1
      } else if (dSize = (newEnd - newStart) - (oldEnd - oldStart)) {
        children[i] += dSize
        children[i + 1] += dSize
      }
    }
  }
  for (var i = 0; i < mapping.maps.length; i++) { mapping.maps[i].forEach(shift) }

  // Find the child nodes that still correspond to a single node,
  // recursively call mapInner on them and update their positions.
  var mustRebuild = false
  for (var i$1 = 0; i$1 < children.length; i$1 += 3) { if (children[i$1 + 1] == -1) { // Untouched nodes
    var from = mapping.map(children[i$1] + oldOffset), fromLocal = from - offset
    // Must read oldChildren because children was tagged with -1
    var to = mapping.map(oldChildren[i$1 + 1] + oldOffset, -1), toLocal = to - offset
    var ref = node.content.findIndex(fromLocal);
    var index = ref.index;
    var childOffset = ref.offset;
    var childNode = node.maybeChild(index)
    if (childNode && childOffset == fromLocal && childOffset + childNode.nodeSize == toLocal) {
      var mapped = children[i$1 + 2].mapInner(mapping, childNode, from + 1, children[i$1] + oldOffset + 1)
      if (mapped != empty) {
        children[i$1] = fromLocal
        children[i$1 + 1] = toLocal
        children[i$1 + 2] = mapped
      } else {
        children.splice(i$1, 3)
        i$1 -= 3
      }
    } else {
      mustRebuild = true
    }
  } }

  // Remaining children must be collected and rebuilt into the appropriate structure
  if (mustRebuild) {
    var decorations = mapAndGatherRemainingDecorations(children, newLocal ? moveSpans(newLocal, offset) : [], mapping, oldOffset)
    var built = buildTree(decorations, node, 0)
    newLocal = built.local
    for (var i$2 = 0; i$2 < children.length; i$2 += 3) { if (children[i$2 + 1] == -1) {
      children.splice(i$2, 3)
      i$2 -= 3
    } }
    for (var i$3 = 0, j = 0; i$3 < built.children.length; i$3 += 3) {
      var from$1 = built.children[i$3]
      while (j < children.length && children[j] < from$1) { j += 3 }
      children.splice(j, 0, built.children[i$3], built.children[i$3 + 1], built.children[i$3 + 2])
    }
  }

  return new DecorationSet(newLocal && newLocal.sort(byPos), children)
}

function moveSpans(spans, offset) {
  if (!offset || !spans.length) { return spans }
  var result = []
  for (var i = 0; i < spans.length; i++) {
    var span = spans[i]
    result.push(new Decoration(span.from + offset, span.to + offset, span.type))
  }
  return result
}

function mapAndGatherRemainingDecorations(children, decorations, mapping, oldOffset) {
  // Gather all decorations from the remaining marked children
  function gather(set, oldOffset) {
    for (var i = 0; i < set.local.length; i++) {
      var mapped = set.local[i].map(mapping, 0, oldOffset)
      if (mapped) { decorations.push(mapped) }
    }
    for (var i$1 = 0; i$1 < set.children.length; i$1 += 3)
      { gather(set.children[i$1 + 2], set.children[i$1] + oldOffset + 1) }
  }
  for (var i = 0; i < children.length; i += 3) { if (children[i + 1] == -1)
    { gather(children[i + 2], children[i] + oldOffset + 1) } }

  return decorations
}

function takeSpansForNode(spans, node, offset) {
  if (node.isLeaf) { return null }
  var end = offset + node.nodeSize, found = null
  for (var i = 0, span = void 0; i < spans.length; i++) {
    if ((span = spans[i]) && span.from > offset && span.to < end) {
      ;(found || (found = [])).push(span)
      spans[i] = null
    }
  }
  return found
}

function withoutNulls(array) {
  var result = []
  for (var i = 0; i < array.length; i++)
    { if (array[i] != null) { result.push(array[i]) } }
  return result
}

// : ([Decoration], Node, number) → DecorationSet
// Build up a tree that corresponds to a set of decorations. `offset`
// is a base offset that should be subtractet from the `from` and `to`
// positions in the spans (so that we don't have to allocate new spans
// for recursive calls).
function buildTree(spans, node, offset) {
  var children = []
  node.forEach(function (childNode, localStart) {
    var found = takeSpansForNode(spans, childNode, localStart + offset)
    if (found)
      { children.push(localStart, localStart + childNode.nodeSize, buildTree(found, childNode, offset + localStart + 1)) }
  })
  var locals = moveSpans(children.length ? withoutNulls(spans) : spans, -offset).sort(byPos)
  for (var i = 0; i < locals.length; i++)
    { if (!locals[i].type.valid(node, locals[i])) { locals.splice(i--, 1) } }
  return new DecorationSet(locals, children)
}

// : (Decoration, Decoration) → number
// Used to sort decorations so that ones with a low start position
// come first, and within a set with the same start position, those
// with an smaller end position come first.
function byPos(a, b) {
  return a.from - b.from || a.to - b.to
}

// : ([Decoration]) → [Decorations]
// Scan a sorted array of decorations for partially overlapping spans,
// and split those so that only fully overlapping spans are left (to
// make subsequent rendering easier). Will return the input array if
// no partially overlapping spans are found (the common case).
function removeOverlap(spans) {
  var working = spans
  for (var i = 0; i < working.length - 1; i++) {
    var span = working[i]
    if (span.from != span.to) { for (var j = i + 1; j < working.length; j++) {
      var next = working[j]
      if (next.from == span.from) {
        if (next.to != span.to) {
          if (working == spans) { working = spans.slice() }
          // Followed by a partially overlapping larger span. Split that
          // span.
          working[j] = next.copy(next.from, span.to)
          insertAhead(working, j + 1, next.copy(span.to, next.to))
        }
        continue
      } else {
        if (next.from < span.to) {
          if (working == spans) { working = spans.slice() }
          // The end of this one overlaps with a subsequent span. Split
          // this one.
          working[i] = span.copy(span.from, next.from)
          insertAhead(working, j, span.copy(next.from, span.to))
        }
        break
      }
    } }
  }
  return working
}
exports.removeOverlap = removeOverlap

function insertAhead(array, i, deco) {
  while (i < array.length && byPos(deco, array[i]) > 0) { i++ }
  array.splice(i, 0, deco)
}

// : (EditorView) → union<DecorationSet, DecorationGroup>
// Get the decorations associated with the current props of a view.
function viewDecorations(view) {
  var found = []
  view.someProp("decorations", function (f) {
    var result = f(view.state)
    if (result && result != empty) { found.push(result) }
  })
  return DecorationGroup.from(found)
}
exports.viewDecorations = viewDecorations

function addDummy(decorations, doc, from, to) {
  var dummy = DecorationSet.create(doc, [new Decoration(from, to, new DummyType)])
  if (decorations == empty) { return dummy }
  var others = decorations instanceof DecorationGroup ? decorations.members : [decorations]
  return new DecorationGroup(others.concat(dummy))
}
exports.addDummy = addDummy

},{}],54:[function(require,module,exports){
var ref = require("prosemirror-model");
var Mark = ref.Mark;
var DOMParser = ref.DOMParser;
var ref$1 = require("prosemirror-state");
var Selection = ref$1.Selection;

var ref$2 = require("./dompos");
var DOMFromPos = ref$2.DOMFromPos;
var DOMFromPosFromEnd = ref$2.DOMFromPosFromEnd;

function readInputChange(view, oldState) {
  return readDOMChange(view, oldState, rangeAroundSelection(oldState.selection))
}
exports.readInputChange = readInputChange

function readCompositionChange(view, oldState, margin) {
  return readDOMChange(view, oldState, rangeAroundComposition(oldState.selection, margin))
}
exports.readCompositionChange = readCompositionChange

// Note that all referencing and parsing is done with the
// start-of-operation selection and document, since that's the one
// that the DOM represents. If any changes came in in the meantime,
// the modification is mapped over those before it is applied, in
// readDOMChange.

function parseBetween(view, oldState, from, to) {
  var ref = DOMFromPos(view, from, true);
  var parent = ref.node;
  var startOff = ref.offset;
  var ref$1 = DOMFromPosFromEnd(view, to);
  var parentRight = ref$1.node;
  var endOff = ref$1.offset;
  if (parent != parentRight) { return null }
  while (startOff) {
    var prev = parent.childNodes[startOff - 1]
    if (prev.nodeType != 1 || !prev.hasAttribute("pm-offset")) { --startOff }
    else { break }
  }
  while (endOff < parent.childNodes.length) {
    var next = parent.childNodes[endOff]
    if (next.nodeType != 1 || !next.hasAttribute("pm-offset")) { ++endOff }
    else { break }
  }
  var domSel = view.root.getSelection(), find = null
  if (domSel.anchorNode && view.content.contains(domSel.anchorNode)) {
    find = [{node: domSel.anchorNode, offset: domSel.anchorOffset}]
    if (!domSel.isCollapsed)
      { find.push({node: domSel.focusNode, offset: domSel.focusOffset}) }
  }
  var startDoc = oldState.doc
  var parser = view.someProp("domParser") || DOMParser.fromSchema(view.state.schema)
  var sel = null, doc = parser.parse(parent, {
    topNode: startDoc.resolve(from).parent.copy(),
    from: startOff,
    to: endOff,
    preserveWhitespace: true,
    editableContent: true,
    findPositions: find
  })
  if (find && find[0].pos != null) {
    var anchor = find[0].pos, head = find[1] && find[1].pos
    if (head == null) { head = anchor }
    sel = {anchor: anchor + from, head: head + from}
  }
  return {doc: doc, sel: sel}
}

function isAtEnd($pos, depth) {
  for (var i = depth || 0; i < $pos.depth; i++)
    { if ($pos.index(i) + 1 < $pos.node(i).childCount) { return false } }
  return $pos.parentOffset == $pos.parent.content.size
}
function isAtStart($pos, depth) {
  for (var i = depth || 0; i < $pos.depth; i++)
    { if ($pos.index(0) > 0) { return false } }
  return $pos.parentOffset == 0
}

function rangeAroundSelection(selection) {
  var $from = selection.$from;
  var $to = selection.$to;
  // When the selection is entirely inside a text block, use
  // rangeAroundComposition to get a narrow range.
  if ($from.sameParent($to) && $from.parent.isTextblock && $from.parentOffset && $to.parentOffset < $to.parent.content.size)
    { return rangeAroundComposition(selection, 0) }

  for (var depth = 0;; depth++) {
    var fromStart = isAtStart($from, depth + 1), toEnd = isAtEnd($to, depth + 1)
    if (fromStart || toEnd || $from.index(depth) != $to.index(depth) || $to.node(depth).isTextblock) {
      var from = $from.before(depth + 1), to = $to.after(depth + 1)
      if (fromStart && $from.index(depth) > 0)
        { from -= $from.node(depth).child($from.index(depth) - 1).nodeSize }
      if (toEnd && $to.index(depth) + 1 < $to.node(depth).childCount)
        { to += $to.node(depth).child($to.index(depth) + 1).nodeSize }
      return {from: from, to: to}
    }
  }
}

function rangeAroundComposition(selection, margin) {
  var $from = selection.$from;
  var $to = selection.$to;
  if (!$from.sameParent($to)) { return rangeAroundSelection(selection) }
  var startOff = Math.max(0, $from.parentOffset - margin)
  var size = $from.parent.content.size
  var endOff = Math.min(size, $to.parentOffset + margin)

  if (startOff > 0)
    { startOff = $from.parent.childBefore(startOff).offset }
  if (endOff < size) {
    var after = $from.parent.childAfter(endOff)
    endOff = after.offset + after.node.nodeSize
  }
  var nodeStart = $from.start()
  return {from: nodeStart + startOff, to: nodeStart + endOff}
}

function enterEvent() {
  var event = document.createEvent("Event")
  event.initEvent("keydown", true, true)
  event.keyCode = 13
  event.code = "Enter"
  return event
}

function readDOMChange(view, oldState, range) {
  var parseResult, doc = oldState.doc
  for (;;) {
    parseResult = parseBetween(view, oldState, range.from, range.to)
    if (parseResult) { break }
    var $from$1 = doc.resolve(range.from), $to$1 = doc.resolve(range.to)
    range = {from: $from$1.depth ? $from$1.before() : 0,
             to: $to$1.depth ? $to$1.after() : doc.content.size}
  }
  var parsed = parseResult.doc;
  var parsedSel = parseResult.sel;

  var compare = doc.slice(range.from, range.to)
  var change = findDiff(compare.content, parsed.content, range.from, oldState.selection.from)
  if (!change) { return false }

  // Mark nodes touched by this change as 'to be redrawn'
  view.dirty = {from: change.start, to: change.endA}

  var $from = parsed.resolveNoCache(change.start - range.from)
  var $to = parsed.resolveNoCache(change.endB - range.from)
  var nextSel, text, event
  // If this looks like the effect of pressing Enter, just dispatch an
  // Enter key instead.
  if (!$from.sameParent($to) && $from.pos < parsed.content.size &&
      (nextSel = Selection.findFrom(parsed.resolve($from.pos + 1), 1, true)) &&
      nextSel.head == $to.pos &&
      (event = enterEvent()) &&
      view.someProp("handleKeyDown", function (f) { return f(view, event); }))
    { return }

  var from = change.start, to = change.endA
  // If there have been changes since this DOM update started, we must
  // map our start and end positions, as well as the new selection
  // positions, through them.
  var mapping = view.state.view.domChangeMapping
  if (mapping) {
    from = mapping.map(from)
    to = mapping.map(to)
    if (parsedSel) { parsedSel = {anchor: mapping.map(parsedSel.anchor),
                                head: mapping.map(parsedSel.head)} }
  }

  var tr = view.state.tr
  if ($from.sameParent($to) && $from.parent.isTextblock &&
      (text = uniformTextBetween(parsed, $from.pos, $to.pos)) != null) {
    if (view.someProp("handleTextInput", function (f) { return f(view, from, to, text); })) { return }
    tr.insertText(text, from, to)
  } else {
    tr.replace(from, to, parsed.slice(change.start - range.from, change.endB - range.from))
  }

  if (parsedSel)
    { tr.setSelection(Selection.between(tr.doc.resolve(parsedSel.anchor),
                                      tr.doc.resolve(parsedSel.head))) }
  view.props.onAction(tr.scrollAction())
}

function uniformTextBetween(node, from, to) {
  var result = "", valid = true, marks = null
  node.nodesBetween(from, to, function (node, pos) {
    if (!node.isInline && pos < from) { return }
    if (!node.isText) { return valid = false }
    if (!marks) { marks = node.marks }
    else if (!Mark.sameSet(marks, node.marks)) { valid = false }
    result += node.text.slice(Math.max(0, from - pos), to - pos)
  })
  return valid ? result : null
}

function findDiff(a, b, pos, preferedStart) {
  var start = a.findDiffStart(b, pos)
  if (!start) { return null }
  var ref = a.findDiffEnd(b, pos + a.size, pos + b.size);
  var endA = ref.a;
  var endB = ref.b;
  if (endA < start) {
    var move = preferedStart <= start && preferedStart >= endA ? start - preferedStart : 0
    start -= move
    endB = start + (endB - endA)
    endA = start
  } else if (endB < start) {
    var move$1 = preferedStart <= start && preferedStart >= endB ? start - preferedStart : 0
    start -= move$1
    endA = start + (endA - endB)
    endB = start
  }
  return {start: start, endA: endA, endB: endB}
}

},{"./dompos":55,"prosemirror-model":26,"prosemirror-state":37}],55:[function(require,module,exports){
function isEditorContent(dom) {
  return dom.nodeType == 1 && dom.classList.contains("ProseMirror-content")
}

// : (dom.Node) → number
// Get the position before a given a DOM node in a document. Returns
// -1 when the first annotated parent node is the top-level document.
function posBeforeFromDOM(node) {
  var pos = -1
  for (var cur = node; !isEditorContent(cur); cur = cur.parentNode) {
    var attr = cur.nodeType == 1 && cur.getAttribute("pm-offset")
    if (attr) { pos += +attr + 1 }
  }
  return pos
}

// : (dom.Node, number) → number
function posFromDOM(dom, domOffset, bias) {
  if ( bias === void 0 ) bias = 0;

  if (domOffset == null) {
    domOffset = Array.prototype.indexOf.call(dom.parentNode.childNodes, dom)
    dom = dom.parentNode
  }

  // Move up to the wrapping container, counting local offset along
  // the way.
  var innerOffset = 0, tag
  for (;;) {
    var adjust = 0
    if (dom.nodeType == 3) {
      innerOffset += domOffset
    } else if (dom.nodeType != 1 || dom.hasAttribute("pm-ignore")) {
      innerOffset = 0
    } else if (tag = dom.getAttribute("pm-offset") && !childContainer(dom)) {
      var size = +dom.getAttribute("pm-size")
      if (dom.nodeType == 1 && !dom.firstChild) { innerOffset = bias > 0 ? size : 0 }
      else if (domOffset == dom.childNodes.length) { innerOffset = size }
      else { innerOffset = Math.min(innerOffset, size) }
      return posBeforeFromDOM(dom) + innerOffset
    } else if (dom.hasAttribute("pm-container")) {
      break
    } else if (domOffset == dom.childNodes.length) {
      if (domOffset) { adjust = 1 }
      else { adjust = bias > 0 ? 1 : 0 }
    }

    var parent = dom.parentNode
    domOffset = adjust < 0 ? 0 : Array.prototype.indexOf.call(parent.childNodes, dom) + adjust
    dom = parent
    bias = 0
  }

  var start = posBeforeFromDOM(dom) + 1, before = 0

  for (var child = dom.childNodes[domOffset - 1]; child; child = child.previousSibling) {
    if (child.nodeType == 1 && (tag = child.getAttribute("pm-offset"))) {
      before += +tag + +child.getAttribute("pm-size")
      break
    }
  }

  return start + before + innerOffset
}
exports.posFromDOM = posFromDOM

// : (dom.Node) → ?dom.Node
function childContainer(dom) {
  return dom.nodeType != 1 ? null : dom.hasAttribute("pm-container") ? dom : dom.querySelector("[pm-container]")
}
exports.childContainer = childContainer

// : (ProseMirror, number) → {node: dom.Node, offset: number}
// Find the DOM node and offset into that node that the given document
// position refers to.
function DOMFromPos(view, pos, loose) {
  var container = view.content, offset = pos
  for (;;) {
    for (var child = container.firstChild, i = 0;; child = child.nextSibling, i++) {
      if (!child) {
        if (offset && !loose) { throw new RangeError("Failed to find node at " + pos) }
        return {node: container, offset: i}
      }

      var size = child.nodeType == 1 && child.getAttribute("pm-size")
      if (size) {
        if (!offset) { return {node: container, offset: i} }
        size = +size
        if (offset < size) {
          container = childContainer(child)
          if (!container) {
            return leafAt(child, offset)
          } else {
            offset--
            break
          }
        } else {
          offset -= size
        }
      }
    }
  }
}
exports.DOMFromPos = DOMFromPos

// : (ProseMirror, number) → {node: dom.Node, offset: number}
// The same as DOMFromPos, but searching from the bottom instead of
// the top. This is needed in domchange.js, when there is an arbitrary
// DOM change somewhere in our document, and we can no longer rely on
// the DOM structure around the selection.
function DOMFromPosFromEnd(view, pos) {
  var container = view.content, dist = view.state.doc.content.size - pos
  for (;;) {
    for (var child = container.lastChild, i = container.childNodes.length;; child = child.previousSibling, i--) {
      if (!child) { return {node: container, offset: i} }

      var size = child.nodeType == 1 && child.getAttribute("pm-size")
      if (size) {
        if (!dist) { return {node: container, offset: i} }
        size = +size
        if (dist < size) {
          container = childContainer(child)
          if (!container) {
            return leafAt(child, size - dist)
          } else {
            dist--
            break
          }
        } else {
          dist -= size
        }
      }
    }
  }
}
exports.DOMFromPosFromEnd = DOMFromPosFromEnd

// : (ProseMirror, number) → dom.Node
function DOMAfterPos(view, pos) {
  var ref = DOMFromPos(view, pos);
  var node = ref.node;
  var offset = ref.offset;
  if (node.nodeType != 1 || offset == node.childNodes.length)
    { throw new RangeError("No node after pos " + pos) }
  return node.childNodes[offset]
}
exports.DOMAfterPos = DOMAfterPos

// : (dom.Node, number) → {node: dom.Node, offset: number}
function leafAt(node, offset) {
  for (;;) {
    var child = node.firstChild
    if (!child) { return {node: node, offset: offset} }
    if (child.nodeType != 1) { return {node: child, offset: offset} }
    node = child
  }
}

function windowRect() {
  return {left: 0, right: window.innerWidth,
          top: 0, bottom: window.innerHeight}
}

function parentNode(node) {
  var parent = node.parentNode
  return parent.nodeType == 11 ? parent.host : parent
}

function scrollPosIntoView(view, pos) {
  var coords = coordsAtPos(view, pos)
  for (var parent = view.content;; parent = parentNode(parent)) {
    var scrollThreshold = view.someProp("scrollThreshold") || 0, scrollMargin = view.someProp("scrollMargin")
    if (scrollMargin == null) { scrollMargin = 5 }
    var atBody = parent == document.body
    var rect = atBody ? windowRect() : parent.getBoundingClientRect()
    var moveX = 0, moveY = 0
    if (coords.top < rect.top + scrollThreshold)
      { moveY = -(rect.top - coords.top + scrollMargin) }
    else if (coords.bottom > rect.bottom - scrollThreshold)
      { moveY = coords.bottom - rect.bottom + scrollMargin }
    if (coords.left < rect.left + scrollThreshold)
      { moveX = -(rect.left - coords.left + scrollMargin) }
    else if (coords.right > rect.right - scrollThreshold)
      { moveX = coords.right - rect.right + scrollMargin }
    if (moveX || moveY) {
      if (atBody) {
        window.scrollBy(moveX, moveY)
      } else {
        if (moveY) { parent.scrollTop += moveY }
        if (moveX) { parent.scrollLeft += moveX }
      }
    }
    if (atBody) { break }
  }
}
exports.scrollPosIntoView = scrollPosIntoView

function findOffsetInNode(node, coords) {
  var closest, dxClosest = 2e8, coordsClosest, offset = 0
  for (var child = node.firstChild, childIndex = 0; child; child = child.nextSibling, childIndex++) {
    var rects = void 0
    if (child.nodeType == 1) { rects = child.getClientRects() }
    else if (child.nodeType == 3) { rects = textRange(child).getClientRects() }
    else { continue }

    for (var i = 0; i < rects.length; i++) {
      var rect = rects[i]
      if (rect.top <= coords.top && rect.bottom >= coords.top) {
        var dx = rect.left > coords.left ? rect.left - coords.left
            : rect.right < coords.left ? coords.left - rect.right : 0
        if (dx < dxClosest) {
          closest = child
          dxClosest = dx
          coordsClosest = dx && closest.nodeType == 3 ? {left: rect.right < coords.left ? rect.right : rect.left, top: coords.top} : coords
          if (child.nodeType == 1 && dx)
            { offset = childIndex + (coords.left >= (rect.left + rect.right) / 2 ? 1 : 0) }
          continue
        }
      }
      if (!closest && (coords.left >= rect.right || coords.left >= rect.left && coords.top >= rect.bottom))
        { offset = i + 1 }
    }
  }
  if (closest && closest.nodeType == 3) { return findOffsetInText(closest, coordsClosest) }
  if (!closest || (dxClosest && closest.nodeType == 1)) { return {node: node, offset: offset} }
  return findOffsetInNode(closest, coordsClosest)
}

function findOffsetInText(node, coords) {
  var len = node.nodeValue.length
  var range = document.createRange()
  for (var i = 0; i < len; i++) {
    range.setEnd(node, i + 1)
    range.setStart(node, i)
    var rect = singleRect(range, 1)
    if (rect.top == rect.bottom) { continue }
    if (rect.left - 1 <= coords.left && rect.right + 1 >= coords.left &&
        rect.top - 1 <= coords.top && rect.bottom + 1 >= coords.top)
      { return {node: node, offset: i + (coords.left >= (rect.left + rect.right) / 2 ? 1 : 0)} }
  }
  return {node: node, offset: 0}
}

function targetKludge(dom, coords) {
  if (/^[uo]l$/i.test(dom.nodeName)) {
    for (var child = dom.firstChild; child; child = child.nextSibling) {
      if (child.nodeType != 1 || !child.hasAttribute("pm-offset") || !/^li$/i.test(child.nodeName)) { continue }
      var childBox = child.getBoundingClientRect()
      if (coords.left > childBox.left - 2) { break }
      if (childBox.top <= coords.top && childBox.bottom >= coords.top) { return child }
    }
  }
  return dom
}

// Given an x,y position on the editor, get the position in the document.
function posAtCoords(view, coords) {
  var elt = targetKludge(view.root.elementFromPoint(coords.left, coords.top + 1), coords)
  if (!view.content.contains(elt.nodeType == 3 ? elt.parentNode : elt)) { return null }

  var ref = findOffsetInNode(elt, coords);
  var node = ref.node;
  var offset = ref.offset;
  var bias = -1
  if (node.nodeType == 1 && !node.firstChild) {
    var rect = node.getBoundingClientRect()
    bias = rect.left != rect.right && coords.left > (rect.left + rect.right) / 2 ? 1 : -1
  }
  return {pos: posFromDOM(node, offset, bias), inside: posBeforeFromDOM(node)}
}
exports.posAtCoords = posAtCoords

function textRange(node, from, to) {
  var range = document.createRange()
  range.setEnd(node, to == null ? node.nodeValue.length : to)
  range.setStart(node, from || 0)
  return range
}

function singleRect(object, bias) {
  var rects = object.getClientRects()
  return !rects.length ? object.getBoundingClientRect() : rects[bias < 0 ? 0 : rects.length - 1]
}

// : (ProseMirror, number) → {left: number, top: number, right: number, bottom: number}
// Given a position in the document model, get a bounding box of the
// character at that position, relative to the window.
function coordsAtPos(view, pos) {
  var ref = DOMFromPos(view, pos);
  var node = ref.node;
  var offset = ref.offset;
  var side, rect
  if (node.nodeType == 3) {
    if (offset < node.nodeValue.length) {
      rect = singleRect(textRange(node, offset, offset + 1), -1)
      side = "left"
    }
    if ((!rect || rect.left == rect.right) && offset) {
      rect = singleRect(textRange(node, offset - 1, offset), 1)
      side = "right"
    }
  } else if (node.firstChild) {
    if (offset < node.childNodes.length) {
      var child = node.childNodes[offset]
      rect = singleRect(child.nodeType == 3 ? textRange(child) : child, -1)
      side = "left"
    }
    if ((!rect || rect.top == rect.bottom) && offset) {
      var child$1 = node.childNodes[offset - 1]
      rect = singleRect(child$1.nodeType == 3 ? textRange(child$1) : child$1, 1)
      side = "right"
    }
  } else {
    rect = node.getBoundingClientRect()
    side = "left"
  }
  var x = rect[side]
  return {top: rect.top, bottom: rect.bottom, left: x, right: x}
}
exports.coordsAtPos = coordsAtPos

},{}],56:[function(require,module,exports){
var ref = require("prosemirror-model");
var DOMSerializer = ref.DOMSerializer;

var browser = require("./browser")
var ref$1 = require("./dompos");
var childContainer = ref$1.childContainer;

function getSerializer(view) {
  return view.someProp("domSerializer") || DOMSerializer.fromSchema(view.state.schema)
}

function draw(view, doc, decorations) {
  view.content.textContent = ""
  new Context(getSerializer(view), decorations).serializeContent(doc, view.content)
}
exports.draw = draw

function redraw(view, oldDoc, newDoc, oldDecorations, newDecorations) {
  var serializer = getSerializer(view)
  var onUnmountDOM = []
  view.someProp("onUnmountDOM", function (f) { onUnmountDOM.push(f) })

  function scan(dom, node, prev, oldDecorations, newDecorations) {
    var iPrev = 0, oPrev = 0, pChild = prev.firstChild
    var domPos = dom.firstChild
    while (domPos && (domPos.nodeType != 1 || domPos.hasAttribute("pm-ignore")))
      { domPos = movePast(domPos, view, onUnmountDOM) }

    var localDecorations = newDecorations.locals(node)
    var decoIndex = applyDecorations(localDecorations, 0, 0, 0, dom, domPos, false)

    function syncDOM() {
      while (domPos) {
        var curOff = domPos.nodeType == 1 && domPos.getAttribute("pm-offset")
        if (!curOff || +curOff < oPrev)
          { domPos = movePast(domPos, view, onUnmountDOM) }
        else
          { return +curOff == oPrev }
      }
      return false
    }
    var oldLocalDecorations, offset = 0, child
    function sameLocalDeco() {
      return compareDecorations(oldLocalDecorations || (oldLocalDecorations = oldDecorations.locals(prev)),
                                localDecorations, decoIndex,
                                oPrev, oPrev + pChild.nodeSize, offset, offset + child.nodeSize)
    }

    for (var iNode = 0; iNode < node.childCount; iNode++) {
      var matching = void 0, reuseDOM = void 0
      child = node.child(iNode)
      var found = pChild == child ? iPrev : findNodeIn(prev, iPrev + 1, child)
      if (found > -1) {
        matching = child
        while (iPrev != found) {
          oPrev += pChild.nodeSize
          pChild = prev.maybeChild(++iPrev)
        }
      }

      var childDeco = newDecorations.forChild(offset, child), prevChildDeco = void 0, matchedLocalDeco = void 0
      if (matching &&
          childDeco.sameOutput(prevChildDeco = oldDecorations.forChild(offset, child)) &&
          (matchedLocalDeco = sameLocalDeco()) != null &&
          syncDOM()) {
        reuseDOM = true
        decoIndex = matchedLocalDeco
      } else if (pChild && !child.isText && child.sameMarkup(pChild) &&
                 (matchedLocalDeco = sameLocalDeco()) != null && syncDOM()) {
        reuseDOM = true
        decoIndex = matchedLocalDeco
        if (!pChild.isLeaf)
          { scan(childContainer(domPos), child, pChild, prevChildDeco || oldDecorations.forChild(oPrev, pChild), childDeco) }
        domPos.setAttribute("pm-size", child.nodeSize)
      } else {
        var rendered = new Context(serializer, childDeco).serialize(child, offset)
        dom.insertBefore(rendered, domPos)
        reuseDOM = false
        decoIndex = applyDecorations(localDecorations, decoIndex, offset, offset + child.nodeSize, dom, rendered)
      }

      if (reuseDOM) {
        // Text nodes might be split into smaller segments
        if (child.isText) {
          for (var off = offset, end = off + child.nodeSize; off < end;) {
            if (offset != oPrev)
              { domPos.setAttribute("pm-offset", off) }
            off += +domPos.getAttribute("pm-size")
            domPos = domPos.nextSibling
          }
        } else {
          if (offset != oPrev)
            { domPos.setAttribute("pm-offset", offset) }
          domPos = domPos.nextSibling
        }
        oPrev += pChild.nodeSize
        pChild = prev.maybeChild(++iPrev)
        var end$1 = offset + child.nodeSize
        decoIndex = applyDecorations(localDecorations, decoIndex, end$1, end$1, dom, domPos)
      }
      offset += child.nodeSize
    }

    while (domPos) { domPos = movePast(domPos, view, onUnmountDOM) }

    if (node.isTextblock) { adjustTrailingHacks(serializer, dom, node) }

    if (browser.ios) { iosHacks(dom) }
  }
  scan(view.content, newDoc, oldDoc, oldDecorations, newDecorations)
}
exports.redraw = redraw

var Context = function Context(serializer, decorations) {
  this.serializer = serializer
  this.decorations = decorations
};

Context.prototype.onContent = function onContent (parent, target) {
  target.setAttribute("pm-container", true)
  this.serializeContent(parent, target, this.decorations)
};

Context.prototype.serialize = function serialize (node, offset) {
  var dom = this.serializer.serializeNodeAndMarks(node, this)
  if (dom.nodeType != 1 || dom.contentEditable == "false") {
    var wrap = document.createElement(node.isInline ? "span" : "div")
    wrap.appendChild(dom)
    dom = wrap
  }
  dom.setAttribute("pm-size", node.nodeSize)
  dom.setAttribute("pm-offset", offset)
  if (node.isTextblock) { adjustTrailingHacks(this.serializer, dom, node) }
  return dom
};

Context.prototype.serializeContent = function serializeContent (node, target) {
    var this$1 = this;

  var decorations = this.decorations
  var locals = decorations.locals(node)
  var i = applyDecorations(locals, 0, 0, 0, target, null, false)
  node.content.forEach(function (child, offset) {
    this$1.decorations = decorations.forChild(offset, child)
    var dom = target.appendChild(this$1.serialize(child, offset))
    i = applyDecorations(locals, i, offset, offset + child.nodeSize, target, dom)
  })
};

// : ([Decoration], number, number, number, dom.Node, ?dom.Node) → number
// Used to apply decorations, either at a given point in a node that's
// being updated, or those in and after a given child node. `i` is an
// index into the local set of (non-overlapping) decorations, which is
// used to avoid scanning through the array multiple times.
//
// When `from` == `to`, this should only draw inserted decorations at
// the given position. When `from` < `to`, this should also decorate a
// node. That node may be a text node, which may have different
// decorations at different points, in which case it has to be split.
//
// `domNode` should be _the node after `from`_. That means that it is
// the current node when `from` < `to`, and the node after the current
// position when they are equal. It may be null, when `from` == `to`
// and there are no nodes after the current point.
//
// Returns the updated index, which can be passed back to this
// function later.
function applyDecorations(locals, i, from, to, domParent, domNode) {
  var result = i
  for (; i < locals.length; i++) {
    var span = locals[i]
    if (span.from > to || (span.from == to && span.to > to)) { break }
    if (from < span.from) {
      domNode = span.from < to ? splitText(domNode, span.from - from) : domNode.nextSibling
      from = span.from
    }
    var curNode = domNode
    if (span.to < to && span.from < span.to) {
      domNode = splitText(domNode, span.to - from)
      from = span.to
    }

    for (;;) {
      curNode = span.type.apply(domParent, curNode)
      if (i < locals.length - 1 && locals[i + 1].to == span.to && locals[i + 1].from == span.from) { span = locals[++i] }
      else { break }
    }
    if (span.to <= to) { result = i + 1 }
  }
  return result
}

function compareDecorations(old, cur, i, oldFrom, oldTo, curFrom, curTo) {
  var j = 0, result = i
  while (j < old.length && old[j].to <= oldFrom) { j++ }
  for (;; i++, j++) {
    var oldEnd = j == old.length || old[j].from >= oldTo
    if (i == cur.length || cur[i].from >= curTo) { return oldEnd ? result : null }
    else if (oldEnd) { return null }
    var oldNext = old[j], curNext = cur[i]
    if (oldNext.type != curNext.type ||
        oldNext.from - oldFrom != curNext.from - curFrom ||
        oldNext.to - oldFrom != curNext.to - curFrom) { return null }
    if (curNext.to <= curTo) { result = i + 1 }
  }
}

function splitText(node, offset) {
  var inner = node
  while (inner.nodeType != 3) { inner = inner.firstChild }
  var newNode = document.createTextNode(inner.nodeValue.slice(offset))
  inner.nodeValue = inner.nodeValue.slice(0, offset)
  while (inner != node) {
    var parent = inner.parentNode, wrap = parent.cloneNode(false)
    wrap.appendChild(newNode)
    newNode = wrap
    inner = parent
  }
  node.parentNode.insertBefore(newNode, node.nextSibling)
  var size = +node.getAttribute("pm-size")
  newNode.setAttribute("pm-size", size - offset)
  node.setAttribute("pm-size", offset)
  newNode.setAttribute("pm-offset", +node.getAttribute("pm-offset") + offset)
  return newNode
}

function findNodeIn(parent, i, node) {
  for (; i < parent.childCount; i++) {
    var child = parent.child(i)
    if (child == node) { return i }
  }
  return -1
}

function movePast(dom, view, onUnmount) {
  var next = dom.nextSibling
  for (var i = 0; i < onUnmount.length; i++) { onUnmount[i](view, dom) }
  dom.parentNode.removeChild(dom)
  return next
}

function isBR(node, serializer) {
  if (!node.isLeaf || node.isText || !node.isInline) { return false }
  var ser = serializer.nodes[node.type.name](node)
  return Array.isArray(ser) ? ser[0] == "br" : ser && ser.nodeName == "BR"
}

function adjustTrailingHacks(serializer, dom, node) {
  var needs = node.content.size == 0 || isBR(node.lastChild, serializer) ||
      (node.type.spec.code && node.lastChild.isText && /\n$/.test(node.lastChild.text))
      ? "br" : !node.lastChild.isText && node.lastChild.isLeaf ? "text" : null
  var last = dom.lastChild
  var has = !last || last.nodeType != 1 || !last.hasAttribute("pm-ignore") ? null
      : last.nodeName == "BR" ? "br" : "text"
  if (needs != has) {
    if (has == "br") { dom.removeChild(last) }
    if (needs) {
      var add = document.createElement(needs == "br" ? "br" : "span")
      add.setAttribute("pm-ignore", needs == "br" ? "trailing-break" : "cursor-text")
      dom.appendChild(add)
    }
  }
}

function iosHacks(dom) {
  if (dom.nodeName == "UL" || dom.nodeName == "OL") {
    var oldCSS = dom.style.cssText
    dom.style.cssText = oldCSS + "; list-style: square !important"
    window.getComputedStyle(dom).listStyle
    dom.style.cssText = oldCSS
  }
}

},{"./browser":51,"./dompos":55,"prosemirror-model":26}],57:[function(require,module,exports){
var ref = require("./dompos");
var scrollPosIntoView = ref.scrollPosIntoView;
var posAtCoords = ref.posAtCoords;
var coordsAtPos = ref.coordsAtPos;
var ref$1 = require("./draw");
var draw = ref$1.draw;
var redraw = ref$1.redraw;
var ref$2 = require("./input");
var initInput = ref$2.initInput;
var finishUpdateFromDOM = ref$2.finishUpdateFromDOM;
var dispatchKeyDown = ref$2.dispatchKeyDown;
var dispatchKeyPress = ref$2.dispatchKeyPress;
var ref$3 = require("./selection");
var SelectionReader = ref$3.SelectionReader;
var selectionToDOM = ref$3.selectionToDOM;
var ref$4 = require("./decoration");
var viewDecorations = ref$4.viewDecorations;
var addDummy = ref$4.addDummy;var assign;
((assign = require("./decoration"), exports.Decoration = assign.Decoration, exports.DecorationSet = assign.DecorationSet))

// ::- An editor view manages the DOM structure that represents an
// editor. Its state and behavior are determined by its
// [props](#view.EditorProps).
var EditorView = function EditorView(place, props) {
  // :: EditorProps
  // The view's current [props](#view.EditorProps).
  this.props = props
  // :: EditorState
  // The view's current [state](#state.EditorState).
  this.state = this.drawnState = props.state
  this.dirty = null

  // :: dom.Node
  // The editable DOM node containing the document. (You probably
  // should not be directly interfering with its child nodes.)
  this.content = document.createElement("div")
  this.content.setAttribute("pm-container", "true")
  this.content.classList.add("ProseMirror-content")

  this.wrapper = document.createElement("div")
  this.wrapper.appendChild(this.content)

  this._root = null

  this.updateDOMForProps()

  if (place && place.appendChild) { place.appendChild(this.wrapper) }
  else if (place) { place(this.wrapper) }

  draw(this, this.state.doc, this.drawnDecorations = viewDecorations(this))
  this.content.contentEditable = true

  this.lastSelectedNode = null
  this.selectionReader = new SelectionReader(this)
  initInput(this)
};

var prototypeAccessors = { root: {} };

// :: (EditorProps)
// Update the view's props. Will immediately cause an update to
// the view's DOM.
EditorView.prototype.update = function update (props) {
  this.props = props
  this.updateState(props.state)
  this.updateDOMForProps()
};

// :: (EditorState)
// Update the editor's `state` prop, without touching any of the
// other props.
EditorView.prototype.updateState = function updateState (state) {
    var this$1 = this;

  this.state = state

  if (this.inDOMChange) {
    if (state.view.inDOMChange != this.inDOMChange.id)
      { setTimeout(function () { return finishUpdateFromDOM(this$1); }, 0) }
    return
  } else if (state.view.inDOMChange != null) {
    setTimeout(function () { return this$1.props.onAction({type: "endDOMChange"}); }, 0)
    return
  }

  var redrawn = false
  var docChange = !state.doc.eq(this.drawnState.doc)

  var decorations = viewDecorations(this)
  if (docChange || this.dirty || !decorations.sameOutput(this.drawnDecorations)) {
    this.redraw(state.doc, decorations)
    redrawn = true
  }

  if (redrawn || !state.selection.eq(this.drawnState.selection))
    { selectionToDOM(this, state.selection) }

  // FIXME somehow schedule this relative to ui/update so that it
  // doesn't cause extra layout
  if (state.view.scrollToSelection)
    { scrollPosIntoView(this, state.selection.head == null ? state.selection.from : state.selection.from) }

  // Make sure we don't use an outdated range on drop event
  if (this.dragging && docChange) { this.dragging.move = false }
  this.drawnState = state
};

EditorView.prototype.redraw = function redraw$1 (doc, decorations) {
  var oldDecorations = this.drawnDecorations, oldDoc = this.drawnState.doc
  this.drawnDecorations = decorations

  if (this.dirty) {
    var $start = oldDoc.resolve(this.dirty.from), $end = oldDoc.resolve(this.dirty.to), same = $start.sameDepth($end)
    this.dirty = null
    if (same == 0)
      { return draw(this, doc, decorations) }
    oldDecorations = addDummy(decorations, doc, $start.before(same), $start.after(same))
  }
  redraw(this, oldDoc, doc, oldDecorations, decorations)
};

EditorView.prototype.updateDOMForProps = function updateDOMForProps () {
    var this$1 = this;

  var spellcheck = !!this.someProp("spellcheck")
  if (spellcheck != this.content.spellcheck) { this.content.spellcheck = spellcheck }
  var label = this.someProp("label", function (f) { return f(this$1.state); }) || ""
  if (this.content.getAttribute("aria-label") != label) { this.content.setAttribute("aria-label", label) }
  var className = "ProseMirror"
  this.someProp("class", function (f) { var cls = f(this$1.state); if (cls) { className += " " + cls } })
  if (this.wrapper.className != className) { this.wrapper.className = className }
};

// :: () → bool
// Query whether the view has focus.
EditorView.prototype.hasFocus = function hasFocus () {
  if (this.content.ownerDocument.activeElement != this.content) { return false }
  var sel = this.root.getSelection()
  return sel.rangeCount && this.content.contains(sel.anchorNode.nodeType == 3 ? sel.anchorNode.parentNode : sel.anchorNode)
};

// :: (string, (prop: *) → *) → *
// Goes over the values of a prop, first those provided directly,
// then those from plugins (in order), and calls `f` every time a
// non-undefined value is found. When `f` returns a truthy value,
// that is immediately returned. When `f` isn't provided, it is
// treated as the identity function (the prop value is returned
// directly).
EditorView.prototype.someProp = function someProp (propName, f) {
  var prop = this.props && this.props[propName], value
  if (prop && (value = f ? f(prop) : prop)) { return value }
  var plugins = this.state.plugins
  if (plugins) { for (var i = 0; i < plugins.length; i++) {
    var prop$1 = plugins[i].props[propName]
    if (prop$1 != null && (value = f ? f(prop$1) : prop$1)) { return value }
  } }
};

// :: ()
// Focus the editor.
EditorView.prototype.focus = function focus () {
  selectionToDOM(this, this.state.selection, true)
  this.content.focus()
};

// :: union<dom.Document, dom.DocumentFragment>
// Get the document root in which the editor exists. This will
// usually be the top-level `document`, but might be a shadow DOM
// root if the editor is inside a shadow DOM.
prototypeAccessors.root.get = function () {
    var this$1 = this;

  var cached = this._root
  if (cached == null) { for (var search = this.wrapper.parentNode; search; search = search.parentNode) {
    if (search.nodeType == 9 || (search.nodeType == 11 && search.host))
      { return this$1._root = search }
  } }
  return cached || document
};

// :: ({left: number, top: number}) → ?{pos: number, inside: number}
// Given a pair of coordinates, return the document position that
// corresponds to them. May return null if the given coordinates
// aren't inside of the visible editor. When an object is returned,
// its `pos` property is the position nearest to the coordinates,
// and its `inside` property holds the position before the inner
// node that the click happened inside of, or -1 if the click was at
// the top level.
EditorView.prototype.posAtCoords = function posAtCoords$1 (coords) { return posAtCoords(this, coords) };

// :: (number) → {left: number, right: number, top: number, bottom: number}
// Returns the screen rectangle at a given document position. `left`
// and `right` will be the same number, as this returns a flat
// cursor-ish rectangle.
EditorView.prototype.coordsAtPos = function coordsAtPos$1 (pos) { return coordsAtPos(this, pos) };

EditorView.prototype.dispatchKeyDown = function dispatchKeyDown$1 (event) {
  return dispatchKeyDown(this, event)
};

EditorView.prototype.dispatchKeyPress = function dispatchKeyPress$1 (event) {
  return dispatchKeyPress(this, event)
};

Object.defineProperties( EditorView.prototype, prototypeAccessors );
exports.EditorView = EditorView

// EditorProps:: interface
//
// The configuration object that can be passed to an editor view. It
// supports the following properties (only `state` and `onAction` are
// required).
//
// The various event-handling functions may all return `true` to
// indicate that they handled the given event. The view will then take
// care to call `preventDefault` on the event, except with
// `handleDOMEvent, where the handler itself is responsible for that.
//
// Except for `state` and `onAction`, these may also be present on the
// `props` property of plugins. How a prop is resolved depends on the
// prop. Handler functions are called one at a time, starting with the
// plugins (in order of appearance), and finally looking at the base
// props, until one of them returns true. For some props, the first
// plugin that yields a value gets precedence. For `class`, all the
// classes returned are combined.
//
//   state:: EditorState
//   The state of the editor.
//
//   onAction:: (action: Action)
//   The callback over which to send actions (state updates) produced
//   by the view. You'll usually want to make sure this ends up
//   calling the view's [`update`](#view.EditorView.update) method
//   with a new state that has the action
//   [applied](#state.EditorState.applyAction).
//
//   handleDOMEvent:: ?(view: EditorView, event: dom.Event) → bool
//   Called before the view handles a DOM event. This is a kind of
//   catch-all override hook. Contrary to the other event handling
//   props, when returning true from this one, you are responsible for
//   calling `preventDefault` yourself (or not, if you want to allow
//   the default behavior).
//
//   handleKeyDown:: ?(view: EditorView, event: dom.KeyboardEvent) → bool
//   Called when the editor receives a `keydown` event.
//
//   handleKeyPress:: ?(view: EditorView, event: dom.KeyboardEvent) → bool
//   Handler for `keypress` events.
//
//   handleTextInput:: ?(view: EditorView, from: number, to: number, text: string) → bool
//   Whenever the user directly input text, this handler is called
//   before the input is applied. If it returns `true`, the default
//   effect of actually inserting the text is suppressed.
//
//   handleClickOn:: ?(view: EditorView, pos: number, node: Node, nodePos: number, event: dom.MouseEvent) → bool
//   Called for each node around a click, from the inside out.
//
//   handleClick:: ?(view: EditorView, pos: number, event: dom.MouseEvent) → bool
//   Called when the editor is clicked, after `handleClickOn` handlers
//   have been called.
//
//   handleDoubleClickOn:: ?(view: EditorView, pos: number, node: Node, nodePos: number, event: dom.MouseEvent) → bool
//   Called for each node around a double click.
//
//   handleDoubleClick:: ?(view: EditorView, pos: number, event: dom.MouseEvent) → bool
//   Called when the editor is double-clicked, after `handleDoubleClickOn`.
//
//   handleTripleClickOn:: ?(view: EditorView, pos: number, node: Node, nodePos: number, event: dom.MouseEvent) → bool
//   Called for each node around a triple click.
//
//   handleTripleClick:: ?(view: EditorView, pos: number, event: dom.MouseEvent) → bool
//   Called when the editor is triple-clicked, after `handleTripleClickOn`.
//
//   handleContextMenu:: ?(view: EditorView, pos: number, event: dom.MouseEvent) → bool
//   Called when a context menu event is fired in the editor.
//
//   onFocus:: ?(view: EditorView, event: dom.Event)
//   Called when the editor is focused.
//
//   onBlur:: ?(view: EditorView, event: dom.Event)
//   Called when the editor loses focus.
//
//   onUnmountDOM:: ?(view: EditorView, dom.Node)
//   Called when a display update throws away a DOM node that was part
//   of the previous document view. Can be useful when your node
//   representations need to be cleaned up somehow. Note that this is
//   only called with the top of the unmounted tree, not with every
//   node in it.
//
//   domParser:: ?DOMParser
//   The [parser](#model.DOMParser) to use when reading editor changes
//   from the DOM. Defaults to calling
//   [`DOMParser.fromSchema`](#model.DOMParser^fromSchema) on the
//   editor's schema.
//
//   clipboardParser:: ?DOMParser
//   The [parser](#model.DOMParser) to use when reading content from
//   the clipboard. When not given, the value of the
//   [`domParser`](#view.EditorProps.domParser) prop is used.
//
//   transformPasted:: ?(Slice) → Slice
//   Can be used to transform pasted content before it is applied to
//   the document.
//
//   domSerializer:: ?DOMSerializer
//   The [serializer](#model.DOMSerializer) to use when drawing the
//   document to the display. If not given, the result of
//   [`DOMSerializer.fromSchema`](#model.DOMSerializer^fromSchema)
//   will be used.
//
//   clipboardSerializer:: ?DOMSerializer
//   The DOM serializer to use when putting content onto the
//   clipboard. When not given, the value of the
//   [`domSerializer`](#view.EditorProps.domSerializer) prop is used.
//
//   decorations:: ?DecorationSet
//   A set of [document decorations](#view.Decoration) to add to the
//   view.
//
//   spellcheck:: ?bool
//   Controls whether the DOM spellcheck attribute is enabled on the
//   editable content. Defaults to false.
//
//   class:: ?(state: EditorState) → ?string
//   Controls the CSS class name of the editor DOM node. Any classes
//   returned from this will be added to the default `ProseMirror`
//   class.
//
//   label:: ?(state: EditorState) → ?string
//   Can be used to set an `aria-label` attribute on the editable
//   content node.
//
//   scrollThreshold:: ?number
//   Determines the distance (in pixels) between the cursor and the
//   end of the visible viewport at which point, when scrolling the
//   cursor into view, scrolling takes place. Defaults to 0.
//
//   scrollMargin:: ?number
//   Determines the extra space (in pixels) that is left above or
//   below the cursor when it is scrolled into view. Defaults to 5.

},{"./decoration":53,"./dompos":55,"./draw":56,"./input":58,"./selection":59}],58:[function(require,module,exports){
var ref = require("prosemirror-model");
var Slice = ref.Slice;
var Fragment = ref.Fragment;
var DOMParser = ref.DOMParser;
var DOMSerializer = ref.DOMSerializer;
var ref$1 = require("prosemirror-state");
var Selection = ref$1.Selection;
var NodeSelection = ref$1.NodeSelection;
var TextSelection = ref$1.TextSelection;
var isSelectable = ref$1.isSelectable;

var browser = require("./browser")
var ref$2 = require("./capturekeys");
var captureKeyDown = ref$2.captureKeyDown;
var ref$3 = require("./domchange");
var readInputChange = ref$3.readInputChange;
var readCompositionChange = ref$3.readCompositionChange;

// A collection of DOM events that occur within the editor, and callback functions
// to invoke when the event fires.
var handlers = {}

function initInput(view) {
  view.shiftKey = false
  view.mouseDown = null
  view.dragging = null
  view.dropTarget = null
  view.finishUpdateFromDOM = null
  view.inDOMChange = null

  var loop = function ( event ) {
    var handler = handlers[event]
    view.content.addEventListener(event, function (e) {
      if (!view.someProp("handleDOMEvent", function (f) { return f(view, e); }))
        { handler(view, e) }
    })
  };

  for (var event in handlers) loop( event );
}
exports.initInput = initInput

function dispatchKeyDown(view, event) {
  return view.someProp("handleKeyDown", function (f) { return f(view, event); }) || captureKeyDown(view, event)
}
exports.dispatchKeyDown = dispatchKeyDown

handlers.keydown = function (view, e) {
  if (e.keyCode == 16) { view.shiftKey = true }
  if (!view.hasFocus() || view.inDOMChange) { return }
  if (dispatchKeyDown(view, e))
    { e.preventDefault() }
  else
    { view.selectionReader.fastPoll() }
}

handlers.keyup = function (view, e) {
  if (e.keyCode == 16) { view.shiftKey = false }
}

function insertText(view, text) {
  var ref = view.state.selection;
  var from = ref.from;
  var to = ref.to;
  if (!view.someProp("handleTextInput", function (f) { return f(view, from, to, text); }))
    { view.props.onAction(view.state.tr.insertText(text).scrollAction()) }
}

function dispatchKeyPress(view, event) {
  return view.someProp("handleKeyPress", function (f) { return f(view, event); })
}
exports.dispatchKeyPress = dispatchKeyPress

handlers.keypress = function (view, e) {
  if (!view.hasFocus() || view.inDOMChange || !e.charCode ||
      e.ctrlKey && !e.altKey || browser.mac && e.metaKey) { return }
  if (dispatchKeyPress(view, e)) {
    e.preventDefault()
    return
  }

  // On iOS, let input through, because if we handle it the virtual
  // keyboard's default case doesn't update (it only does so when the
  // user types or taps, not on selection updates from JavaScript).
  if (!browser.ios) {
    insertText(view, String.fromCharCode(e.charCode))
    e.preventDefault()
  }
}

function eventCoords(event) { return {left: event.clientX, top: event.clientY} }

var lastClick = {time: 0, x: 0, y: 0}, oneButLastClick = lastClick

function isNear(event, click) {
  var dx = click.x - event.clientX, dy = click.y - event.clientY
  return dx * dx + dy * dy < 100
}

function runHandlerOnContext(view, propName, pos, inside, event) {
  if (inside == -1) { return false }
  var $pos = view.state.doc.resolve(inside)
  var loop = function ( i ) {
    var node = i > $pos.depth ? $pos.nodeAfter : $pos.node(i)
    if (view.someProp(propName, function (f) { return f(view, pos, node, $pos.before(i), event); }))
      { return { v: true } }
  };

  for (var i = $pos.depth + 1; i > 0; i--) {
    var returned = loop( i );

    if ( returned ) return returned.v;
  }
  return false
}

function updateSelection(view, selection) {
  view.focus()
  view.props.onAction(selection.action())
}

function selectClickedLeaf(view, inside) {
  if (inside == -1) { return false }
  var $pos = view.state.doc.resolve(inside), node = $pos.nodeAfter
  if (node && node.isLeaf && isSelectable(node)) {
    updateSelection(view, new NodeSelection($pos))
    return true
  }
  return false
}

function selectClickedNode(view, inside) {
  if (inside == -1) { return false }
  var ref = view.state.selection;
  var selectedNode = ref.node;
  var $from = ref.$from;
  var selectAt

  var $pos = view.state.doc.resolve(inside)
  for (var i = $pos.depth + 1; i > 0; i--) {
    var node = i > $pos.depth ? $pos.nodeAfter : $pos.node(i)
    if (isSelectable(node)) {
     if (selectedNode && $from.depth > 0 &&
          i >= $from.depth && $pos.before($from.depth + 1) == $from.pos)
        { selectAt = $pos.before($from.depth) }
      else
        { selectAt = $pos.before(i) }
      break
    }
  }

  if (selectAt != null) {
    updateSelection(view, new NodeSelection(view.state.doc.resolve(selectAt)))
    return true
  } else {
    return false
  }
}

function handleSingleClick(view, pos, inside, ctrl, event) {
  if (ctrl) { return selectClickedNode(view, inside) }

  return runHandlerOnContext(view, "handleClickOn", pos, inside, event) ||
    view.someProp("handleClick", function (f) { return f(view, pos, event); }) ||
    selectClickedLeaf(view, inside)
}

function handleDoubleClick(view, pos, inside, event) {
  return runHandlerOnContext(view, "handleDoubleClickOn", pos, inside, event) ||
    view.someProp("handleDoubleClick", function (f) { return f(view, pos, event); })
}

function handleTripleClick(view, pos, inside, event) {
  return runHandlerOnContext(view, "handleTripleClickOn", pos, inside, event) ||
    view.someProp("handleTripleClick", function (f) { return f(view, pos, event); }) ||
    defaultTripleClick(view, inside)
}

function defaultTripleClick(view, inside) {
  var doc = view.state.doc
  if (inside == -1) {
    if (doc.isTextblock) {
      updateSelection(view, new TextSelection(doc.resolve(0), doc.resolve(doc.content.size)))
      return true
    }
    return false
  }

  var $pos = doc.resolve(inside)
  for (var i = $pos.depth + 1; i > 0; i--) {
    var node = i > $pos.depth ? $pos.nodeAfter : $pos.node(i)
    var nodePos = $pos.before(i)
    if (node.isTextblock)
      { updateSelection(view, new TextSelection(doc.resolve(nodePos + 1),
                                              doc.resolve(nodePos + 1 + node.content.size))) }
    else if (isSelectable(node))
      { updateSelection(view, new NodeSelection(doc.resolve(nodePos))) }
    else
      { continue }
    return true
  }
}

function forceDOMFlush(view) {
  if (!view.inDOMChange) { return false }
  finishUpdateFromDOM(view)
  return true
}

handlers.mousedown = function (view, event) {
  var flushed = forceDOMFlush(view)
  var now = Date.now(), type
  if (now - lastClick.time >= 500 || !isNear(event, lastClick) || event.ctrlKey) { type = "singleClick" }
  else if (now - oneButLastClick.time >= 600 || !isNear(event, oneButLastClick)) { type = "doubleClick" }
  else { type = "tripleClick" }
  oneButLastClick = lastClick
  lastClick = {time: now, x: event.clientX, y: event.clientY}

  var pos = view.posAtCoords(eventCoords(event))
  if (!pos) { return }

  if (type == "singleClick")
    { view.mouseDown = new MouseDown(view, pos, event, flushed) }
  else if ((type == "doubleClick" ? handleDoubleClick : handleTripleClick)(view, pos.pos, pos.inside, event))
    { event.preventDefault() }
  else
    { view.selectionReader.fastPoll() }
}

var MouseDown = function MouseDown(view, pos, event, flushed) {
  this.view = view
  this.pos = pos
  this.flushed = flushed
  this.ctrlKey = event.ctrlKey
  this.allowDefault = event.shiftKey

  var targetNode, targetPos
  if (pos.inside > -1) {
    targetNode = view.state.doc.nodeAt(pos.inside)
    targetPos = pos.inside
  } else {
    var $pos = view.state.doc.resolve(pos.pos)
    targetNode = $pos.parent
    targetPos = $pos.depth ? $pos.before() : 0
  }

  this.mightDrag = (targetNode.type.spec.draggable || targetNode == view.state.selection.node) ? {node: targetNode, pos: targetPos} : null
  this.target = flushed ? null : event.target
  if (this.target && this.mightDrag) {
    this.target.draggable = true
    if (browser.gecko && (this.setContentEditable = !this.target.hasAttribute("contentEditable")))
      { this.target.setAttribute("contentEditable", "false") }
  }

  view.root.addEventListener("mouseup", this.up = this.up.bind(this))
  view.root.addEventListener("mousemove", this.move = this.move.bind(this))
  view.selectionReader.fastPoll()
};

MouseDown.prototype.done = function done () {
  this.view.root.removeEventListener("mouseup", this.up)
  this.view.root.removeEventListener("mousemove", this.move)
  if (this.mightDrag && this.target) {
    this.target.draggable = false
    if (browser.gecko && this.setContentEditable)
      { this.target.removeAttribute("contentEditable") }
  }
};

MouseDown.prototype.up = function up (event) {
  this.done()

  if (!this.view.content.contains(event.target.nodeType == 3 ? event.target.parentNode : event.target))
    { return }

  if (this.allowDefault) {
    this.view.selectionReader.fastPoll()
  } else if (handleSingleClick(this.view, this.pos.pos, this.pos.inside, this.ctrlKey, event)) {
    event.preventDefault()
  } else if (this.flushed) {
    this.view.focus()
    this.view.props.onAction(Selection.near(this.view.state.doc.resolve(this.pos.pos)).action())
    event.preventDefault()
  } else {
    this.view.selectionReader.fastPoll()
  }
};

MouseDown.prototype.move = function move (event) {
  if (!this.allowDefault && (Math.abs(this.x - event.clientX) > 4 ||
                             Math.abs(this.y - event.clientY) > 4))
    { this.allowDefault = true }
  this.view.selectionReader.fastPoll()
};

handlers.touchdown = function (view) {
  forceDOMFlush(view)
  view.selectionReader.fastPoll()
}

handlers.contextmenu = function (view, e) {
  forceDOMFlush(view)
  var pos = view.posAtCoords(eventCoords(e))
  if (pos && view.someProp("handleContextMenu", function (f) { return f(view, pos.pos, e); }))
    { e.preventDefault() }
}

// Input compositions are hard. Mostly because the events fired by
// browsers are A) very unpredictable and inconsistent, and B) not
// cancelable.
//
// ProseMirror has the problem that it must not update the DOM during
// a composition, or the browser will cancel it. What it does is keep
// long-running operations (delayed DOM updates) when a composition is
// active.
//
// We _do not_ trust the information in the composition events which,
// apart from being very uninformative to begin with, is often just
// plain wrong. Instead, when a composition ends, we parse the dom
// around the original selection, and derive an update from that.

function startComposition(view, dataLen) {
  view.inDOMChange = {id: domChangeID(), state: view.state,
                      composition: true, composeMargin: dataLen}
  clearTimeout(view.finishUpdateFromDOM)
  view.props.onAction({type: "startDOMChange", id: view.inDOMChange.id})
}

function domChangeID() {
  return Math.floor(Math.random() * 0xffffffff)
}

function scheduleUpdateFromDOM(view) {
  clearTimeout(view.finishUpdateFromDOM)
  // Give the browser a moment to fire input events or start a new
  // composition, and only apply the change from the DOM afterwards.
  view.finishUpdateFromDOM = window.setTimeout(function () { return finishUpdateFromDOM(view); }, 50)
}

handlers.compositionstart = function (view, e) {
  if (!view.inDOMChange && view.hasFocus())
    { startComposition(view, e.data ? e.data.length : 0) }
}

handlers.compositionupdate = function (view) {
  if (!view.inDOMChange && view.hasFocus())
    { startComposition(view, 0) }
}

handlers.compositionend = function (view, e) {
  if (!view.hasFocus()) { return }
  if (!view.inDOMChange) {
    // We received a compositionend without having seen any previous
    // events for the composition. If there's data in the event
    // object, we assume that it's a real change, and start a
    // composition. Otherwise, we just ignore it.
    if (e.data) { startComposition(view, e.data.length) }
    else { return }
  }

  scheduleUpdateFromDOM(view)
}

function finishUpdateFromDOM(view) {
  clearTimeout(view.finishUpdateFromDOM)
  var change = view.inDOMChange
  if (!change) { return }
  if (change.composition) { readCompositionChange(view, change.state, change.composeMargin) }
  else { readInputChange(view, change.state) }
  view.inDOMChange = null
  view.props.onAction({type: "endDOMChange"})
}
exports.finishUpdateFromDOM = finishUpdateFromDOM

handlers.input = function (view) {
  if (view.inDOMChange || !view.hasFocus()) { return }
  view.inDOMChange = {id: domChangeID(), state: view.state}
  view.props.onAction({type: "startDOMChange", id: view.inDOMChange.id})
  scheduleUpdateFromDOM(view)
}

function toClipboard(view, from, to, dataTransfer) {
  var doc = view.state.doc
  var $from = doc.resolve(from), start = from
  for (var d = $from.depth; d > 0 && $from.end(d) == start; d--) { start++ }
  var slice = doc.slice(start, to)
  if (slice.possibleParent.type != doc.type.schema.nodes.doc)
    { slice = new Slice(Fragment.from(slice.possibleParent.copy(slice.content)), slice.openLeft + 1, slice.openRight + 1) }
  var serializer = view.someProp("clipboardSerializer") || view.someProp("domSerializer") || DOMSerializer.fromSchema(view.state.schema)
  var dom = serializer.serializeFragment(slice.content), wrap = document.createElement("div")
  if (dom.firstChild && dom.firstChild.nodeType == 1)
    { dom.firstChild.setAttribute("pm-open-left", slice.openLeft) }
  wrap.appendChild(dom)
  dataTransfer.clearData()
  dataTransfer.setData("text/html", wrap.innerHTML)
  dataTransfer.setData("text/plain", slice.content.textBetween(0, slice.content.size, "\n\n"))
  return slice
}

var cachedCanUpdateClipboard = null

function canUpdateClipboard(dataTransfer) {
  if (cachedCanUpdateClipboard != null) { return cachedCanUpdateClipboard }
  dataTransfer.setData("text/html", "<hr>")
  return cachedCanUpdateClipboard = dataTransfer.getData("text/html") == "<hr>"
}

// : (DataTransfer, ?bool, ResolvedPos) → ?Slice
function fromClipboard(view, dataTransfer, plainText, $target) {
  var txt = dataTransfer.getData("text/plain")
  var html = dataTransfer.getData("text/html")
  if (!html && !txt) { return null }
  var dom
  if ((plainText || !html) && txt) {
    dom = document.createElement("div")
    txt.split(/(?:\r\n?|\n){2,}/).forEach(function (block) {
      var para = dom.appendChild(document.createElement("p"))
      block.split(/\r\n?|\n/).forEach(function (line, i) {
        if (i) { para.appendChild(document.createElement("br")) }
        para.appendChild(document.createTextNode(line))
      })
    })
  } else {
    dom = readHTML(html)
  }
  var openLeft = null, m
  var foundLeft = dom.querySelector("[pm-open-left]")
  if (foundLeft && (m = /^\d+$/.exec(foundLeft.getAttribute("pm-open-left"))))
    { openLeft = +m[0] }

  var parser = view.someProp("clipboardParser") || view.someProp("domParser") || DOMParser.fromSchema(view.state.schema)
  var slice = parser.parseInContext($target, dom, {openLeft: openLeft, preserveWhitespace: true})
  return slice
}

function insertRange($from, $to) {
  var from = $from.pos, to = $to.pos
  for (var d = $to.depth; d > 0 && $to.end(d) == to; d--) { to++ }
  for (var d$1 = $from.depth; d$1 > 0 && $from.start(d$1) == from && $from.end(d$1) <= to; d$1--) { from-- }
  return {from: from, to: to}
}

// Trick from jQuery -- some elements must be wrapped in other
// elements for innerHTML to work. I.e. if you do `div.innerHTML =
// "<td>..</td>"` the table cells are ignored.
var wrapMap = {thead: "table", colgroup: "table", col: "table colgroup",
                 tr: "table tbody", td: "table tbody tr", th: "table tbody tr"}
function readHTML(html) {
  var metas = /(\s*<meta [^>]*>)*/.exec(html)
  if (metas) { html = html.slice(metas[0].length) }
  var elt = document.createElement("div")
  var firstTag = /(?:<meta [^>]*>)*<([a-z][^>\s]+)/i.exec(html), wrap, depth = 0
  if (wrap = firstTag && wrapMap[firstTag[1].toLowerCase()]) {
    var nodes = wrap.split(" ")
    html = nodes.map(function (n) { return "<" + n + ">"; }).join("") + html + nodes.map(function (n) { return "</" + n + ">"; }).reverse().join("")
    depth = nodes.length
  }
  elt.innerHTML = html
  for (var i = 0; i < depth; i++) { elt = elt.firstChild }
  return elt
}

handlers.copy = handlers.cut = function (view, e) {
  var ref = view.state.selection;
  var from = ref.from;
  var to = ref.to;
  var empty = ref.empty;
  var cut = e.type == "cut"
  if (empty) { return }
  if (!e.clipboardData || !canUpdateClipboard(e.clipboardData)) {
    if (cut && browser.ie && browser.ie_version <= 11) { scheduleUpdateFromDOM(view) }
    return
  }
  toClipboard(view, from, to, e.clipboardData)
  e.preventDefault()
  if (cut) { view.props.onAction(view.state.tr.delete(from, to).scrollAction()) }
}

handlers.paste = function (view, e) {
  if (!view.hasFocus()) { return }
  if (!e.clipboardData) {
    if (browser.ie && browser.ie_version <= 11) { scheduleUpdateFromDOM(view) }
    return
  }
  var range = insertRange(view.state.selection.$from, view.state.selection.$to)
  var slice = fromClipboard(view, e.clipboardData, view.shiftKey, view.state.doc.resolve(range.from))
  if (slice) {
    e.preventDefault()
    view.someProp("transformPasted", function (f) { slice = f(slice) })
    view.props.onAction(view.state.tr.replace(range.from, range.to, slice).scrollAction())
  }
}

var Dragging = function Dragging(slice, from, to, move) {
  this.slice = slice
  this.from = from
  this.to = to
  this.move = move
};

function dropPos(slice, $pos) {
  if (!slice || !slice.content.size) { return $pos.pos }
  var content = slice.content
  for (var i = 0; i < slice.openLeft; i++) { content = content.firstChild.content }
  for (var d = $pos.depth; d >= 0; d--) {
    var bias = d == $pos.depth ? 0 : $pos.pos <= ($pos.start(d + 1) + $pos.end(d + 1)) / 2 ? -1 : 1
    var insertPos = $pos.index(d) + (bias > 0 ? 1 : 0)
    if ($pos.node(d).canReplace(insertPos, insertPos, content))
      { return bias == 0 ? $pos.pos : bias < 0 ? $pos.before(d + 1) : $pos.after(d + 1) }
  }
  return $pos.pos
}

function removeDropTarget(view) {
  if (view.dropTarget) {
    view.wrapper.removeChild(view.dropTarget)
    view.dropTarget = null
  }
}

handlers.dragstart = function (view, e) {
  var mouseDown = view.mouseDown
  if (mouseDown) { mouseDown.done() }
  if (!e.dataTransfer) { return }

  var ref = view.state.selection;
  var from = ref.from;
  var to = ref.to;
  var empty = ref.empty;
  var dragging
  var pos = empty ? null : view.posAtCoords(eventCoords(e))
  if (pos != null && pos.pos >= from && pos.pos <= to) {
    dragging = {from: from, to: to}
  } else if (mouseDown && mouseDown.mightDrag) {
    var pos$1 = mouseDown.mightDrag.pos
    dragging = {from: pos$1, to: pos$1 + mouseDown.mightDrag.node.nodeSize}
  }

  if (dragging) {
    var slice = toClipboard(view, dragging.from, dragging.to, e.dataTransfer)
    view.dragging = new Dragging(slice, dragging.from, dragging.to, !e.ctrlKey)
  }
}

handlers.dragend = function (view) {
  removeDropTarget(view)
  window.setTimeout(function () { return view.dragging = null; }, 50)
}

handlers.dragover = handlers.dragenter = function (view, e) {
  e.preventDefault()

  var target = view.dropTarget
  if (!target) {
    target = view.dropTarget = view.wrapper.appendChild(document.createElement("div"))
    target.className = "ProseMirror-drop-target"
  }

  var pos = dropPos(view.dragging && view.dragging.slice,
                    view.state.doc.resolve(view.posAtCoords(eventCoords(e)).pos))
  if (pos == null) { return }
  var coords = view.coordsAtPos(pos)
  var rect = view.wrapper.getBoundingClientRect()
  coords.top -= rect.top
  coords.right -= rect.left
  coords.bottom -= rect.top
  coords.left -= rect.left
  target.style.left = (coords.left - 1) + "px"
  target.style.top = coords.top + "px"
  target.style.height = (coords.bottom - coords.top) + "px"
}

handlers.dragleave = function (view, e) {
  if (e.target == view.content) { removeDropTarget(view) }
}

handlers.drop = function (view, e) {
  var dragging = view.dragging
  view.dragging = null
  removeDropTarget(view)

  if (!e.dataTransfer) { return }

  var $mouse = view.state.doc.resolve(view.posAtCoords(eventCoords(e)).pos)
  if (!$mouse) { return }
  var range = insertRange($mouse, $mouse)
  var slice = dragging && dragging.slice || fromClipboard(view, e.dataTransfer, false, $mouse)
  if (!slice) { return }
  var insertPos = dropPos(slice, view.state.doc.resolve(range.from))

  e.preventDefault()
  var tr = view.state.tr
  if (dragging && dragging.move)
    { tr.delete(dragging.from, dragging.to) }
  view.someProp("transformPasted", function (f) { slice = f(slice) })
  var pos = tr.mapping.map(insertPos)
  tr.replace(pos, pos, slice)
  tr.setSelection(Selection.between(tr.doc.resolve(pos), tr.doc.resolve(tr.mapping.map(insertPos))))
  view.focus()
  view.props.onAction(tr.action())
}

handlers.focus = function (view, event) {
  view.wrapper.classList.add("ProseMirror-focused")
  view.someProp("onFocus", function (f) { f(view, event) })
}

handlers.blur = function (view, event) {
  view.wrapper.classList.remove("ProseMirror-focused")
  view.someProp("onBlur", function (f) { f(view, event) })
}

},{"./browser":51,"./capturekeys":52,"./domchange":54,"prosemirror-model":26,"prosemirror-state":37}],59:[function(require,module,exports){
var ref = require("prosemirror-state");
var Selection = ref.Selection;
var NodeSelection = ref.NodeSelection;
var isSelectable = ref.isSelectable;

var browser = require("./browser")
var ref$1 = require("./dompos");
var posFromDOM = ref$1.posFromDOM;
var DOMAfterPos = ref$1.DOMAfterPos;
var DOMFromPos = ref$1.DOMFromPos;
var coordsAtPos = ref$1.coordsAtPos;
var childContainer = ref$1.childContainer;

// Track the state of the current editor selection. Keeps the editor
// selection in sync with the DOM selection by polling for changes,
// as there is no DOM event for DOM selection changes.
var SelectionReader = function SelectionReader(view) {
  var this$1 = this;

  this.view = view

  // Track the state of the DOM selection.
  this.lastAnchorNode = this.lastHeadNode = this.lastAnchorOffset = this.lastHeadOffset = null

  view.content.addEventListener("focus", function () { return this$1.receivedFocus(); })

  // The timeout ID for the poller when active.
  this.polling = null
  this.poller = this.poller.bind(this)
};

SelectionReader.prototype.poller = function poller () {
  if (this.view.hasFocus()) {
    this.readFromDOM()
    this.polling = setTimeout(this.poller, 100)
  } else {
    this.polling = null
  }
};

SelectionReader.prototype.startPolling = function startPolling () {
  clearTimeout(this.polling)
  this.polling = setTimeout(this.poller, 50)
};

SelectionReader.prototype.fastPoll = function fastPoll () {
  this.startPolling()
};

SelectionReader.prototype.stopPolling = function stopPolling () {
  clearTimeout(this.polling)
  this.polling = null
};

// : () → bool
// Whether the DOM selection has changed from the last known state.
SelectionReader.prototype.domChanged = function domChanged () {
  var sel = this.view.root.getSelection()
  return sel.anchorNode != this.lastAnchorNode || sel.anchorOffset != this.lastAnchorOffset ||
    sel.focusNode != this.lastHeadNode || sel.focusOffset != this.lastHeadOffset
};

// Store the current state of the DOM selection.
SelectionReader.prototype.storeDOMState = function storeDOMState () {
  var sel = this.view.root.getSelection()
  this.lastAnchorNode = sel.anchorNode; this.lastAnchorOffset = sel.anchorOffset
  this.lastHeadNode = sel.focusNode; this.lastHeadOffset = sel.focusOffset
};

// : () → bool
// When the DOM selection changes in a notable manner, modify the
// current selection state to match.
SelectionReader.prototype.readFromDOM = function readFromDOM () {
  if (!this.view.hasFocus() || !this.domChanged()) { return }

  var domSel = this.view.root.getSelection(), doc = this.view.state.doc
  var domNode = domSel.focusNode, head = posFromDOM(domNode, domSel.focusOffset)
  var $head = doc.resolve(head), $anchor, selection, nodeAfter
  if (domSel.isCollapsed) {
    $anchor = $head
    if (!childContainer(domNode) && (nodeAfter = $head.nodeAfter) && nodeAfter.isLeaf && isSelectable(nodeAfter))
      { selection = new NodeSelection($head) }
  } else {
    $anchor = doc.resolve(posFromDOM(domSel.anchorNode, domSel.anchorOffset))
  }

  if (!selection) {
    var bias = this.view.state.selection.head != null && this.view.state.selection.head < $head.pos ? 1 : -1
    selection = Selection.between($anchor, $head, bias)
  }
  if ($head.pos == selection.head && $anchor.pos == selection.anchor)
    { this.storeDOMState() }
  this.view.props.onAction(selection.action())
};

SelectionReader.prototype.receivedFocus = function receivedFocus () {
  if (this.polling == null) { this.startPolling() }
};
exports.SelectionReader = SelectionReader

function selectionToDOM(view, sel, takeFocus) {
  if (!view.hasFocus()) {
    if (!takeFocus) { return }
    // See https://bugzilla.mozilla.org/show_bug.cgi?id=921444
    else if (browser.gecko) { this.view.content.focus() }
  }

  if (sel instanceof NodeSelection)
    { nodeSelectionToDOM(view, sel) }
  else
    { textSelectionToDOM(view, sel) }
}
exports.selectionToDOM = selectionToDOM

// Make changes to the DOM for a node selection.
function nodeSelectionToDOM(view, sel) {
  var dom = DOMAfterPos(view, sel.from)
  if (dom != view.lastSelectedNode) {
    clearNodeSelection(view)
    dom.classList.add("ProseMirror-selectednode")
    view.content.classList.add("ProseMirror-nodeselection")
    view.lastSelectedNode = dom
  }
  var range = document.createRange(), domSel = view.root.getSelection()
  range.selectNode(dom)
  domSel.removeAllRanges()
  domSel.addRange(range)
  view.selectionReader.storeDOMState()
}

// Make changes to the DOM for a text selection.
function textSelectionToDOM(view, sel) {
  clearNodeSelection(view)

  var anchor = DOMFromPos(view, sel.anchor)
  var head = DOMFromPos(view, sel.head)

  var domSel = view.root.getSelection(), range = document.createRange()
  if (domSel.extend) {
    range.setEnd(anchor.node, anchor.offset)
    range.collapse(false)
  } else {
    if (sel.anchor > sel.head) { var tmp = anchor; anchor = head; head = tmp }
    range.setEnd(head.node, head.offset)
    range.setStart(anchor.node, anchor.offset)
  }
  domSel.removeAllRanges()
  domSel.addRange(range)
  if (domSel.extend)
    { domSel.extend(head.node, head.offset) }
  view.selectionReader.storeDOMState()
}

// Clear all DOM statefulness of the last node selection.
function clearNodeSelection(view) {
  if (view.lastSelectedNode) {
    view.lastSelectedNode.classList.remove("ProseMirror-selectednode")
    view.content.classList.remove("ProseMirror-nodeselection")
    view.lastSelectedNode = null
  }
}

// : (ProseMirror, number, number)
// Whether vertical position motion in a given direction
// from a position would leave a text block.
function verticalMotionLeavesTextblock(view, dir) {
  var $pos = dir < 0 ? view.state.selection.$from : view.state.selection.$to
  if (!$pos.depth) { return false }
  var dom = DOMAfterPos(view, $pos.before())
  var coords = coordsAtPos(view, $pos.pos)
  for (var child = dom.firstChild; child; child = child.nextSibling) {
    if (child.nodeType != 1) { continue }
    var boxes = child.getClientRects()
    for (var i = 0; i < boxes.length; i++) {
      var box = boxes[i]
      if (dir < 0 ? box.bottom < coords.top : box.top > coords.bottom)
        { return false }
    }
  }
  return true
}
exports.verticalMotionLeavesTextblock = verticalMotionLeavesTextblock

},{"./browser":51,"./dompos":55,"prosemirror-state":37}],60:[function(require,module,exports){
var GOOD_LEAF_SIZE = 50

// :: class<T> A rope sequence is a persistent sequence data structure
// that supports appending, prepending, and slicing without doing a
// full copy. It is represented as a mostly-balanced tree.
var RopeSequence = function RopeSequence () {};

RopeSequence.prototype.append = function append (other) {
  if (!other.length) return this
  other = RopeSequence.from(other)

  return (!this.length && other) ||
    (other.length < GOOD_LEAF_SIZE && this.leafAppend(other)) ||
    (this.length < GOOD_LEAF_SIZE && other.leafPrepend(this)) ||
    this.appendInner(other)
};

// :: (union<[T], RopeSequence<T>>) → RopeSequence<T>
// Prepend an array or other rope to this one, returning a new rope.
RopeSequence.prototype.prepend = function prepend (other) {
  if (!other.length) return this
  return RopeSequence.from(other).append(this)
};

RopeSequence.prototype.appendInner = function appendInner (other) {
  return new Append(this, other)
};

// :: (?number, ?number) → RopeSequence<T>
// Create a rope repesenting a sub-sequence of this rope.
RopeSequence.prototype.slice = function slice (from, to) {
    if ( from === void 0 ) from = 0;
    if ( to === void 0 ) to = this.length;

  if (from >= to) return RopeSequence.empty
  return this.sliceInner(Math.max(0, from), Math.min(this.length, to))
};

// :: (number) → T
// Retrieve the element at the given position from this rope.
RopeSequence.prototype.get = function get (i) {
  if (i < 0 || i >= this.length) return undefined
  return this.getInner(i)
};

// :: ((element: T, index: number) → ?bool, ?number, ?number)
// Call the given function for each element between the given
// indices. This tends to be more efficient than looping over the
// indices and calling `get`, because it doesn't have to descend the
// tree for every element.
RopeSequence.prototype.forEach = function forEach (f, from, to) {
    if ( from === void 0 ) from = 0;
    if ( to === void 0 ) to = this.length;

  if (from <= to)
    this.forEachInner(f, from, to, 0)
  else
    this.forEachInvertedInner(f, from, to, 0)
};

// :: ((element: T, index: number) → U, ?number, ?number) → [U]
// Map the given functions over the elements of the rope, producing
// a flat array.
RopeSequence.prototype.map = function map (f, from, to) {
    if ( from === void 0 ) from = 0;
    if ( to === void 0 ) to = this.length;

  var result = []
  this.forEach(function (elt, i) { return result.push(f(elt, i)); }, from, to)
  return result
};

// :: (?union<[T], RopeSequence<T>>) → RopeSequence<T>
// Create a rope representing the given array, or return the rope
// itself if a rope was given.
RopeSequence.from = function from (values) {
  if (values instanceof RopeSequence) return values
  return values && values.length ? new Leaf(values) : RopeSequence.empty
};

var Leaf = (function (RopeSequence) {
  function Leaf(values) {
    RopeSequence.call(this)
    this.values = values
  }

  if ( RopeSequence ) Leaf.__proto__ = RopeSequence;
  Leaf.prototype = Object.create( RopeSequence && RopeSequence.prototype );
  Leaf.prototype.constructor = Leaf;

  var prototypeAccessors = { length: {},depth: {} };

  Leaf.prototype.flatten = function flatten () {
    return this.values
  };

  Leaf.prototype.sliceInner = function sliceInner (from, to) {
    if (from == 0 && to == this.length) return this
    return new Leaf(this.values.slice(from, to))
  };

  Leaf.prototype.getInner = function getInner (i) {
    return this.values[i]
  };

  Leaf.prototype.forEachInner = function forEachInner (f, from, to, start) {
    var this$1 = this;

    for (var i = from; i < to; i++)
      if (f(this$1.values[i], start + i) === false) return false
  };

  Leaf.prototype.forEachInvertedInner = function forEachInvertedInner (f, from, to, start) {
    var this$1 = this;

    for (var i = from - 1; i >= to; i--)
      if (f(this$1.values[i], start + i) === false) return false
  };

  Leaf.prototype.leafAppend = function leafAppend (other) {
    if (this.length + other.length <= GOOD_LEAF_SIZE)
      return new Leaf(this.values.concat(other.flatten()))
  };

  Leaf.prototype.leafPrepend = function leafPrepend (other) {
    if (this.length + other.length <= GOOD_LEAF_SIZE)
      return new Leaf(other.flatten().concat(this.values))
  };

  prototypeAccessors.length.get = function () { return this.values.length };

  prototypeAccessors.depth.get = function () { return 0 };

  Object.defineProperties( Leaf.prototype, prototypeAccessors );

  return Leaf;
}(RopeSequence));

// :: RopeSequence
// The empty rope sequence.
RopeSequence.empty = new Leaf([])

var Append = (function (RopeSequence) {
  function Append(left, right) {
    RopeSequence.call(this)
    this.left = left
    this.right = right
    this.length = left.length + right.length
    this.depth = Math.max(left.depth, right.depth) + 1
  }

  if ( RopeSequence ) Append.__proto__ = RopeSequence;
  Append.prototype = Object.create( RopeSequence && RopeSequence.prototype );
  Append.prototype.constructor = Append;

  Append.prototype.flatten = function flatten () {
    return this.left.flatten().concat(this.right.flatten())
  };

  Append.prototype.getInner = function getInner (i) {
    return i < this.left.length ? this.left.get(i) : this.right.get(i - this.left.length)
  };

  Append.prototype.forEachInner = function forEachInner (f, from, to, start) {
    var leftLen = this.left.length
    if (from < leftLen &&
        this.left.forEachInner(f, from, Math.min(to, leftLen), start) === false)
      return false
    if (to > leftLen &&
        this.right.forEachInner(f, Math.max(from - leftLen, 0), Math.min(this.length, to) - leftLen, start + leftLen) === false)
      return false
  };

  Append.prototype.forEachInvertedInner = function forEachInvertedInner (f, from, to, start) {
    var leftLen = this.left.length
    if (from > leftLen &&
        this.right.forEachInvertedInner(f, from - leftLen, Math.max(to, leftLen) - leftLen, start + leftLen) === false)
      return false
    if (to < leftLen &&
        this.left.forEachInvertedInner(f, Math.min(from, leftLen), to, start) === false)
      return false
  };

  Append.prototype.sliceInner = function sliceInner (from, to) {
    if (from == 0 && to == this.length) return this
    var leftLen = this.left.length
    if (to <= leftLen) return this.left.slice(from, to)
    if (from >= leftLen) return this.right.slice(from - leftLen, to - leftLen)
    return this.left.slice(from, leftLen).append(this.right.slice(0, to - leftLen))
  };

  Append.prototype.leafAppend = function leafAppend (other) {
    var inner = this.right.leafAppend(other)
    if (inner) return new Append(this.left, inner)
  };

  Append.prototype.leafPrepend = function leafPrepend (other) {
    var inner = this.left.leafPrepend(other)
    if (inner) return new Append(inner, this.right)
  };

  Append.prototype.appendInner = function appendInner (other) {
    if (this.left.depth >= Math.max(this.right.depth, other.depth) + 1)
      return new Append(this.left, new Append(this.right, other))
    return new Append(this, other)
  };

  return Append;
}(RopeSequence));

module.exports = RopeSequence

},{}],61:[function(require,module,exports){
var base = {
  8: "Backspace",
  9: "Tab",
  10: "Enter",
  12: "NumLock",
  13: "Enter",
  16: "Shift",
  17: "Control",
  18: "Alt",
  20: "CapsLock",
  27: "Escape",
  32: " ",
  33: "PageUp",
  34: "PageDown",
  35: "End",
  36: "Home",
  37: "ArrowLeft",
  38: "ArrowUp",
  39: "ArrowRight",
  40: "ArrowDown",
  44: "PrintScreen",
  45: "Insert",
  46: "Delete",
  59: ";",
  61: "=",
  91: "Meta",
  92: "Meta",
  106: "*",
  107: "+",
  108: ",",
  109: "-",
  110: ".",
  111: "/",
  144: "NumLock",
  145: "ScrollLock",
  160: "Shift",
  161: "Shift",
  162: "Control",
  163: "Control",
  164: "Alt",
  165: "Alt",
  173: "-",
  186: ";",
  187: "=",
  188: ",",
  189: "-",
  190: ".",
  191: "/",
  192: "`",
  219: "[",
  220: "\\",
  221: "]",
  222: "'",
  229: "q"
}
var shift = {
  48: ")",
  49: "!",
  50: "@",
  51: "#",
  52: "$",
  53: "%",
  54: "^",
  55: "&",
  56: "*",
  57: "(",
  59: ";",
  61: "+",
  173: "_",
  186: ":",
  187: "+",
  188: "<",
  189: "_",
  190: ">",
  191: "?",
  192: "~",
  219: "{",
  220: "|",
  221: "}",
  222: "\"",
  229: "Q"
}

// Fill in the digit keys
for (var i = 0; i < 10; i++) base[48 + i] = base[96 + i] = String(i)

// The function keys
for (var i = 1; i <= 24; i++) base[i + 111] = "F" + i

// And the alphabetic keys
for (var i = 65; i <= 90; i++) {
  base[i] = String.fromCharCode(i + 32)
  shift[i] = String.fromCharCode(i)
}

// For each code that doesn't have a shift-equivalent, copy the base name
for (var code in base) if (!shift.hasOwnProperty(code)) shift[code] = base[code]

function keyName(event) {
  return event.key || (event.shiftKey ? shift : base)[event.keyCode] || "Unidentified"
}

module.exports = keyName
keyName.base = base
keyName.shift = shift

},{}]},{},[1]);
