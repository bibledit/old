var passage = "";


$(document).ready (function () {
  pollPassage ();
});


function pollPassage () 
{
  return;
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
  $.ajax ({
    url: "get.php",
    type: "GET",
    data: { book: ref ["book"], chapter: ref ["chapter"], verse: ref ["verse"] },
    success: function (response) {
      var container = $ ("#bibledit");
      container.empty ();
      container.append ($ ("<div>" + response + "</div>"));
    },
  });
}

