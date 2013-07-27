var container;


$(document).ready (function () {
  container = $ ("#bibleditnavigation");
  $.get ("../navigation/update.php?bible=" + bible, function (response) {
    container.append (response);
    bindClickHandlers ();
  });
  navigationPollPassage ();
});


function bindClickHandlers () {
  $("#selectbible").on ("click", function (event) {
    selectBible (event);
  });
  $("#selectbook").on ("click", function (event) {
    selectBook (event);
  });
  $("#selectchapter").on ("click", function (event) {
    selectChapter (event);
  });
  $("#selectverse").on ("click", function (event) {
    selectVerse (event);
  });
}


function selectBible (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + bible + "&getbibles", function (response) {
    container.empty ();
    container.append (response);
    $("#selectbibles").on ("click", function (event) {
      selectBibles (event);
    });
  });  
}


function selectBook (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + bible + "&getbooks", function (response) {
    container.empty ();
    container.append (response);
    $("#selectbooks").on ("click", function (event) {
      selectBooks (event);
    });
  });  
}


function selectChapter (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + bible + "&getchapters", function (response) {
    container.empty ();
    container.append (response);
    $("#selectchapters").on ("click", function (event) {
      selectChapters (event);
    });
  });  
}


function selectVerse (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + bible + "&getverses", function (response) {
    container.empty ();
    container.append (response);
    $("#selectverses").on ("click", function (event) {
      selectVerses (event);
    });
  });  
}


function selectBibles (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    bible = event.target.innerText;
    $.get ("../navigation/update.php?bible=" + bible + "&setbible", function (response) {
      container.empty ();
      container.append (response);
      bindClickHandlers ();
    });  
  }
}


function selectBooks (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?bible=" + bible + "&setbook=" + event.target.id, function (response) {
      container.empty ();
      container.append (response);
      bindClickHandlers ();
    });  
  }
}


function selectChapters (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?bible=" + bible + "&setchapter=" + event.target.id, function (response) {
      container.empty ();
      container.append (response);
      bindClickHandlers ();
    });  
  }
}


function selectVerses (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?bible=" + bible + "&setverse=" + event.target.id, function (response) {
      container.empty ();
      container.append (response);
      bindClickHandlers ();
    });  
  }
}


function navigationPollPassage () // Todo
{
  $.ajax ({
    url: "../navigation/passage.php",
    type: "GET",
    data: { pollpassage: "" },
    success: function (response) {
      console.log (response);
      /* 
      if (response != passage) {
        passage = response;
        loadResources ();
      }
      */
    },
    complete: function (xhr, status) {
      setTimeout (navigationPollPassage, 1000);
    }
  });
}


