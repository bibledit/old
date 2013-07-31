$(document).ready (function () {
  usfmEditorPollPassage ();
  usfmEditorLoadChapter ();
});


function usfmEditorPollPassage () 
{
  if (newPassageAvailable) {
    newPassageAvailable = false;
    usfmEditorSaveChapter ();
    usfmEditorLoadChapter ();
  }
  setTimeout (usfmEditorPollPassage, 1000);
}


function usfmEditorLoadChapter ()
{
  console.log ("load chapter");
  
}


function usfmEditorSaveChapter ()
{
  console.log ("save chapter");
  
}
