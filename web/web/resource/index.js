var passage = "";


$(document).ready (function () {
  pollPassage ();
  loadResources ();
});


function pollPassage () 
{
  if (newPassageAvailable) {
    newPassageAvailable = false;
    loadResources ();
  }
  setTimeout (pollPassage, 1000);
}


function loadResources ()
{
  for (i = 0; i < resourceCount; i++) {
    var container = $ ("#bibledit" + i);
    container.empty ();
    container.append ("↻");
    $.ajax ({
      context: container,
      url: "get.php",
      type: "GET",
      data: { resource: i, book: navigationBook, chapter: navigationChapter, verse: navigationVerse },
      success: function (response) {
        this.empty ();
        this.append ($ (response));
      },
    });
  }
}
