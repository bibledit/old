var passage = "";


$(document).ready (function () {
  pollPassage ();
});


function pollPassage () 
{
  $.ajax ({
    url: "get.php",
    type: "GET",
    data: { pollpassage: "" },
    success: function (response) {
      if (response != passage) {
        passage = response;
        loadResources ();
      }
    },
    complete: function (xhr, status) {
      setTimeout (pollPassage, 1000);
    }
  });
}


function loadResources ()
{
  var ref = $.parseJSON (passage);
  for (i = 0; i < resourceCount; i++) {
    var container = $ ("#bibledit" + i);
    container.empty ();
    container.append ("↻");
    $.ajax ({
      context: container,
      url: "get.php",
      type: "GET",
      data: { resource: i, book: ref ["book"], chapter: ref ["chapter"], verse: ref ["verse"] },
      success: function (response) {
        this.empty ();
        this.append ($ (response));
      },
    });
  }
}
