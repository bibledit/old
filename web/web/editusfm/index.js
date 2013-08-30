$(document).ready (function () {
  $ (document).on ("passage", usfmEditorNewPassage);
  $ ("#usfmeditor").on ("paste cut keydown", usfmEditorChanged);
  $ (window).on ("unload beforeunload", usfmEditorSaveChapter);
});


var usfmBible;
var usfmBook;
var usfmChapter;
var usfmEditorTimeout;
var usfmLoadedText;


function usfmEditorNewPassage ()
{
  usfmEditorSaveChapter ();
  usfmEditorLoadChapter ();
}

function usfmEditorLoadChapter ()
{
  if ((navigationBible != usfmBible) || (navigationBook != usfmBook) || (navigationChapter != usfmChapter)) {
    usfmBible = navigationBible;
    usfmBook = navigationBook;
    usfmChapter = navigationChapter;
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
  if (usfmEditorTimeout) {
    clearTimeout (usfmEditorTimeout);
  }
  usfmEditorTimeout = setTimeout (usfmEditorSaveChapter, 1000);
}


function usfmEditorStatus (text)
{
  $ ("#usfmstatus").empty ();
  $ ("#usfmstatus").append (text);
}

