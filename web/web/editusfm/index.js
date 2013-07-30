$(document).ready (function () {
  pollPassage ();
});


function pollPassage () 
{
  if (newPassageAvailable) {
    newPassageAvailable = false;
    $('input[type=submit]').click();
  }
  setTimeout (pollPassage, 1000);
}


