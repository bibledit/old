var navigationBible;
var navigationBook;
var navigationChapter;
var navigationVerse;


var navigatorContainer;
var navigatorTimeout;
var navigationNewBible;


$(document).ready (function () {
  navigatorContainer = $ ("#bibleditnavigation");
  buildNavigator ();
  navigationPollPassage ();
  $("body").on ("keydown", navigationHandleKeyDown);
});


function buildNavigator () {
  $.get ("../navigation/update.php?bible=" + navigationBible, function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    bindClickHandlers ();
  });
}


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
  $.get ("../navigation/update.php?bible=" + navigationBible + "&getbibles", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    $("#selectbibles").on ("click", function (event) {
      selectBibles (event);
    });
  });  
}


function selectBook (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + navigationBible + "&getbooks", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    $("#selectbooks").on ("click", function (event) {
      selectBooks (event);
    });
  });  
}


function selectChapter (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + navigationBible + "&getchapters", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    $("#selectchapters").on ("click", function (event) {
      selectChapters (event);
    });
  });  
}


function selectVerse (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + navigationBible + "&getverses", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    $("#selectverses").on ("click", function (event) {
      selectVerses (event);
    });
  });  
}


function selectBibles (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    navigationNewBible = event.target.innerText;
    $.get ("../navigation/update.php?bible=" + navigationNewBible + "&setbible", function (response) {
      navigatorContainer.empty ();
      navigatorContainer.append (response);
      bindClickHandlers ();
      if (navigationNewBible != navigationBible) {
        navigationBible = navigationNewBible;
        navigationNewBible = undefined;
        $(document).trigger ("passage");
      }
      navigationPollPassage ();
    });  
  }
}


function selectBooks (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?bible=" + navigationBible + "&setbook=" + event.target.id, function (response) {
      navigatorContainer.empty ();
      navigatorContainer.append (response);
      bindClickHandlers ();
      navigationPollPassage ();
    });  
  }
}


function selectChapters (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?bible=" + navigationBible + "&setchapter=" + event.target.id, function (response) {
      navigatorContainer.empty ();
      navigatorContainer.append (response);
      bindClickHandlers ();
      navigationPollPassage ();
    });  
  }
}


function selectVerses (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?bible=" + navigationBible + "&setverse=" + event.target.id, function (response) {
      navigatorContainer.empty ();
      navigatorContainer.append (response);
      bindClickHandlers ();
      navigationPollPassage ();
    });  
  }
}


function navigationPollPassage ()
{
  if (navigatorTimeout) {
    clearTimeout (navigatorTimeout);
  }
  $.ajax ({
    url: "../navigation/passage.php",
    type: "GET",
    success: function (response) {
      var ref = $.parseJSON (response);
      var book = ref ["book"];
      var chapter = ref ["chapter"];
      var verse = ref ["verse"];
      if ((book != navigationBook) || (chapter != navigationChapter) || (verse != navigationVerse)) {
        navigationBook = book;
        navigationChapter = chapter;
        navigationVerse = verse;
        $(document).trigger ("passage");
        buildNavigator ();
      }
    },
    complete: function (xhr, status) {
      navigatorTimeout = setTimeout (navigationPollPassage, 1000);
    }
  });
}


function navigationHandleKeyDown (event) {
  // Ctrl-G:
  if ((event.ctrlKey == true) && (event.keyCode == 71)) {
    event.preventDefault ();
    navigationGetEntry ();
  }
  // Escape:
  if (event.keyCode == 27) {
    event.preventDefault ();
    buildNavigator ();
  }
  // Enter:
  if (event.keyCode == 13) {
    if (event.target.id == "selectpassage") {
      event.preventDefault ();
      navigationSubmitEntry ();
    }
  }
}


function navigationGetEntry () {
  $.get ("../navigation/update.php?bible=" + navigationBible + "&getentry", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    $("#selectpassage").focus ();
    $("#submitpassage").on ("click", function (event) {
      navigationSubmitEntry ();
    });
  });  
}


function navigationSubmitEntry () {
  var passage = $("#selectpassage").val ();
  $.ajax ({
    url: "../navigation/update.php",
    type: "GET",
    data: { bible: navigationBible, passage: passage },
    complete: function (xhr, status) {
      buildNavigator ();
    }
  });
}
