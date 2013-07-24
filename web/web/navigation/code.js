var container;


$(document).ready (function () {
  container = $ ("#bibleditnavigation");
  bindClickHandlers ();
});


function bindClickHandlers () {
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


function selectBook (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?getbooks", function (response) {
    container.empty ();
    container.append (response);
    $("#selectbooks").on ("click", function (event) {
      selectBooks (event);
    });
  });  
}


function selectChapter (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?getchapters", function (response) {
    container.empty ();
    container.append (response);
    $("#selectchapters").on ("click", function (event) {
      selectChapters (event);
    });
  });  
}


function selectVerse (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?getverses", function (response) {
    container.empty ();
    container.append (response);
    $("#selectverses").on ("click", function (event) {
      selectVerses (event);
    });
  });  
}


function selectBooks (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?setbook=" + event.target.id, function (response) {
      container.empty ();
      container.append (response);
      bindClickHandlers ();
    });  
  }
}


function selectChapters (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?setchapter=" + event.target.id, function (response) {
      container.empty ();
      container.append (response);
      bindClickHandlers ();
    });  
  }
}


function selectVerses (event) {
  event.preventDefault ();
  if (event.target.localName == "a") {
    $.get ("../navigation/update.php?setverse=" + event.target.id, function (response) {
      container.empty ();
      container.append (response);
      bindClickHandlers ();
    });  
  }
}

