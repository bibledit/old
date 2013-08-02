var usfmEditorContainer;
var usfmStatusContainer;


$(document).ready (function () {
  usfmEditorContainer = $ ("#usfmeditor");
  usfmStatusContainer = $ ("#usfmstatus");
  $(document).on ("passage", usfmEditorNewPassage);
  usfmEditorContainer.on ("paste cut keydown", usfmEditorChanged);
  $(window).on ("unload beforeunload", usfmEditorSaveChapter);
});


var usfmBible;
var usfmBook;
var usfmChapter;


var usfmChangedFlag = false;
var usfmEditorTimeout;


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
    usfmEditorContainer.empty ();
    $.ajax ({
      url: "load.php",
      type: "GET",
      data: { bible: usfmBible, book: usfmBook, chapter: usfmChapter },
      success: function (response) {
        usfmEditorContainer.empty ();
        usfmEditorContainer.append (response);
        usfmEditorStatus (usfmEditorChapterLoaded);
      },
    });
  }
}


function usfmEditorSaveChapter ()
{
  if (!usfmBible) return;
  if (!usfmBook) return;
  if (!usfmChangedFlag) return;
  usfmChangedFlag = false;
  var usfm = usfmEditorContainer.text ();
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
  usfmChangedFlag = true;
  if (usfmEditorTimeout) {
    clearTimeout (usfmEditorTimeout);
  }
  usfmEditorTimeout = setTimeout (usfmEditorSaveChapter, 1000);
  usfmEditorStatus (usfmEditorChapterModified);
}


function usfmEditorStatus (text)
{
  usfmStatusContainer.empty ();
  usfmStatusContainer.append (text);
}

