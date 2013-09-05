$(document).ready (function () {
  $ (document).on ("passage", usfmEditorNewPassage);
  $ ("#usfmeditor").on ("paste cut keydown", usfmEditorChanged);
  $ (window).on ("unload beforeunload", usfmEditorSaveChapter);
  $ ("#usfmeditor").on ("paste", function (e) {
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    document.execCommand ("insertHTML", false, data);
  });
  usfmIdPoller ();
  $ ("#usfmeditor").on ("paste cut click", usfmCaretChanged);
  $ ("#usfmeditor").on ("keydown", usfmHandleKeyDown);
  $ ("#usfmeditor").focus ();
  rangy.init ();
});


var usfmBible;
var usfmBook;
var usfmChapter;
var usfmEditorChangedTimeout;
var usfmLoadedText;
var usfmIdChapter = 0;
var usfmIdTimeout;
var usfmCaretTimeout;


function usfmEditorNewPassage ()
{
  usfmEditorSaveChapter ();
  usfmEditorLoadChapter (false);
}

function usfmEditorLoadChapter (reload)
{
  if ((navigationBible != usfmBible) || (navigationBook != usfmBook) || (navigationChapter != usfmChapter) || reload) {
    usfmBible = navigationBible;
    usfmBook = navigationBook;
    usfmChapter = navigationChapter;
    usfmIdChapter = 0;
    $ ("#usfmeditor").empty ();
    $.ajax ({
      url: "load.php",
      type: "GET",
      data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter },
      success: function (response) {
        $ ("#usfmeditor").empty ();
        $ ("#usfmeditor").append (response);
        usfmEditorStatus (usfmEditorChapterLoaded);
        usfmLoadedText = response;
      },
    });
  }
}


function usfmEditorSaveChapter ()
{
  if (!usfmBible) return;
  if (!usfmBook) return;
  var usfm = $ ("#usfmeditor").text ();
  if (usfm == usfmLoadedText) return;
  usfmEditorStatus (usfmEditorChapterSaving);
  usfmLoadedText = usfm;
  usfmIdChapter = 0;
  $.ajax ({
    url: "save.php",
    type: "POST",
    data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter, usfm: usfm },
    success: function (response) {
      usfmEditorStatus (response);
    },
  });
}


function usfmEditorChanged ()
{
  if (usfmEditorChangedTimeout) {
    clearTimeout (usfmEditorChangedTimeout);
  }
  usfmEditorChangedTimeout = setTimeout (usfmEditorSaveChapter, 1000);
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
          usfmEditorLoadChapter (true);
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
  var sel = rangy.getSelection ();
  var range = sel.rangeCount ? sel.getRangeAt(0) : null;
  var offset = range.startOffset;
  $.ajax ({
    url: "offset.php",
    type: "GET",
    data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter, offset: offset },
    success: function (response) {
      response = $.parseJSON (response);
      var verse = response ["verse"];
    }
  });
}


