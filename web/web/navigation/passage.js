/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


var navigationBible;
var navigationBook;
var navigationChapter;
var navigationVerse;


var navigatorContainer;
var navigatorTimeout;


var navigationPickedBook;
var navigationPickedChapter;


$(document).ready (function () {
  navigatorContainer = $ ("#versepickerwrapper");
  buildNavigator ();
  navigationPollPassage ();
});


function buildNavigator () {
  $.get ("../navigation/update.php?bible=" + navigationBible, function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    bindClickHandlers ();
  });
}


function bindClickHandlers () {
  $("#navigateback").on ("click", function (event) {
    navigateBack (event);
  });
  $("#navigateforward").on ("click", function (event) {
    navigateForward (event);
  });
  $("#submitpassage").on ("click", function (event) {
    navigationSubmitEntry ();
  });
  $ ("#booklist").on ("click", function (event) {
    selectBook (event);
  });
  $ ("#chapterlist").on ("click", function (event) {
    selectChapter (event);
  });
  $ ("#verselist").on ("click", function (event) {
    selectVerse (event);
  });
}


function navigateBack (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + navigationBible + "&goback", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    bindClickHandlers ();
    navigationPollPassage ();
  });  
}


function navigateForward (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + navigationBible + "&goforward", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    bindClickHandlers ();
    navigationPollPassage ();
  });  
}


function selectBook (event) {
  event.preventDefault ();
  navigationPickedBook = event.target.value;
  $.ajax ({
    url: "../navigation/update.php",
    type: "GET",
    data: { bible: navigationBible, book: navigationPickedBook, getchapters: null },
    success: function (response) {
      var chapterlist = $ ("#chapterlist");
      chapterlist.empty ();
      chapterlist.append (response);
    },
  });
}


function selectChapter (event) {
  event.preventDefault ();
  navigationInitializePickers ();
  navigationPickedChapter = event.target.value;
  $.ajax ({
    url: "../navigation/update.php",
    type: "GET",
    data: { bible: navigationBible, book: navigationPickedBook, chapter: navigationPickedChapter, getverses: null },
    success: function (response) {
      var verselist = $ ("#verselist");
      verselist.empty ();
      verselist.append (response);
    },
  });
}


function selectVerse (event) {
  event.preventDefault ();
  buildNavigator ();
  navigationInitializePickers ();
  $.ajax ({
    url: "../navigation/update.php",
    type: "GET",
    data: { bible: navigationBible, book: navigationPickedBook, chapter: navigationPickedChapter, verse: event.target.value },
    success: function (response) {
    },
  });
}


function navigatePreviousVerse (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + navigationBible + "&previousverse", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    bindClickHandlers ();
    navigationPollPassage ();
  });  
}


function navigateNextVerse (event) {
  event.preventDefault ();
  $.get ("../navigation/update.php?bible=" + navigationBible + "&nextverse", function (response) {
    navigatorContainer.empty ();
    navigatorContainer.append (response);
    bindClickHandlers ();
    navigationPollPassage ();
  });  
}


function navigationPollPassage ()
{
  if (navigatorTimeout) {
    clearTimeout (navigatorTimeout);
  }
  $.ajax ({
    url: "../navigation/poll.php",
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
        navigationCallNewPassage ();
        buildNavigator ();
      }
    },
    complete: function (xhr, status) {
      navigatorTimeout = setTimeout (navigationPollPassage, 1000);
    }
  });
}


function navigationHandleKeyDown (event) 
{
  // Ctrl-G
  if ((event.ctrlKey == true) && (event.keyCode == 71)) {
    event.preventDefault ();
    $("#selectpassage").focus().select();
  }
  // Escape
  if (event.keyCode == 27) {
    event.preventDefault ();
    buildNavigator ();
  }
  // Enter
  if (event.keyCode == 13) {
    if (event.target.id == "selectpassage") {
      event.preventDefault ();
      navigationSubmitEntry ();
    }
  }
  // Alt-Left Arrow
  if ((event.altKey == true) && (event.keyCode == 37)) {
    navigateBack (event);
  }
  // Alt-Right Arrow
  if ((event.altKey == true) && (event.keyCode == 39)) {
    navigateForward (event);
  }
  // Alt-Down Arrow
  if ((event.altKey == true) && (event.keyCode == 40)) {
    navigateNextVerse (event);
  }
  // Alt-Up Arrow
  if ((event.altKey == true) && (event.keyCode == 38)) {
    navigatePreviousVerse (event);
  }
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


function navigationCallNewPassage () {
  try  {
    navigationNewPassage ();
  }
  catch (err) {
  }
  $ ("iframe").each (function (index) {
    try {
      $ (this)[0].contentWindow.navigationNewPassage ();
    }
    catch (err) {
    }
  });
}


function navigationInitializePickers ()
{
  if (navigationPickedBook == undefined) navigationPickedBook = navigationBook;
  if (navigationPickedChapter == undefined) navigationPickedChapter = navigationChapter;
}

