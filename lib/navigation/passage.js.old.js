/*
Copyright (©) 2003-2013 Teus Benschop.

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
  // Alt-Left Arrow.
  if ((event.altKey == true) && (event.keyCode == 37)) {
    navigateBack (event);
  }
  // Alt-Right Arrow.
  if ((event.altKey == true) && (event.keyCode == 39)) {
    navigateForward (event);
  }
  // Alt-Down Arrow.
  if ((event.altKey == true) && (event.keyCode == 40)) {
    navigateNextVerse (event);
  }
  // Alt-Up Arrow.
  if ((event.altKey == true) && (event.keyCode == 38)) {
    navigatePreviousVerse (event);
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
