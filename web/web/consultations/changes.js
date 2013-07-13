var idCount = 0;


$(document).ready(function() {
  updateIdCount ();
  selectEntry ();
  $("body").on ("keydown", keyDown);
  $("div").on ("click", handleClick);
});


function keyDown (event) {
  //console.log (event);
  // Down arrow: Go to next entry.
  if (event.keyCode == 40) {
    event.preventDefault ();
    nextEntry ();
    selectEntry ();
  }
  // Up arrow: Go to previous entry.
  if (event.keyCode == 38) {
    event.preventDefault ();
    previousEntry ();
    selectEntry ();
  }
  // Delete the entry.
  if (event.keyCode == 46) {
    removeEntry ();
    nextEntry ();
    selectEntry ();
  }
  // Right arrow: Expand entry.
  if (event.keyCode == 39) {
    event.preventDefault ();
    expandEntry ();
  }
  // Left arrow: Collapse entry.
  if (event.keyCode == 37) {
    event.preventDefault ();
    collapseEntry ();
  }
}


function handleClick (event) {
  selectedID = $(event.currentTarget).attr ("id").substring (5, 100);
  selectEntry ();
  var eventTarget = $(event.target);
  var actionID = eventTarget.attr ("id");
  if (!actionID) return;
  // Remove the change notification.
  if (actionID == ("remove" + selectedID)) {
    removeEntry ();
    nextEntry ();
    selectEntry ();
    event.preventDefault ();
  }
  // Expand / collapse the change notification.
  if (actionID == ("expand" + selectedID)) {
    toggleEntry ();
    event.preventDefault ();
  }
  // Unsubscribe from the note.
  if (actionID.substring (0, 11) == ("unsubscribe")) {
    $.post ("change.php", { unsubscribe:actionID });
    eventTarget.fadeOut ();
    event.preventDefault ();
  }
  // Unassign the note.
  if (actionID.substring (0, 8) == ("unassign")) {
    $.post ("change.php", { unassign:actionID });
    eventTarget.fadeOut ();
    event.preventDefault ();
  }
  // Delete the note.
  if (actionID.substring (0, 6) == ("delete")) {
    $.post ("change.php", { delete:actionID });
    console.log (eventTarget.parent ());
    eventTarget.parent ().parent ().fadeOut ();
    event.preventDefault ();
  }
}


function nextEntry () {
  if (selectedID == 0) return;
  while (selectedID++ <= lastID) {
    if ($("div#entry" + selectedID).length) {
      return;
    }
  }
  if ($("div").length) {
    selectedID = $("div").last ().attr ("id").substring (5, 100);
  } else {
    selectedID = 0;
  }
}


function previousEntry () {
  if (selectedID == 0) return;
  while (selectedID-- >= firstID) {
    if ($("div#entry" + selectedID).length) {
      return;
    }
  }
  if ($("div").length) {
    selectedID = $("div").first ().attr ("id").substring (5, 100);
  } else {
    selectedID = 0;
  }

}


function selectEntry () {
  $(".selected").removeClass ("selected");
  var selectedElement = $("div#entry" + selectedID);
  if (selectedElement.length) {
    selectedElement.addClass ("selected");
    var elementOffset = selectedElement.offset ();
    $("body").scrollTop (elementOffset.top + (selectedElement.height () / 2) - ($(window).height () / 2));
  }
}


function removeEntry () {
  if (selectedID == 0) return;
  $.post ("changes.php", { remove:selectedID });
  $(".selected").remove ();
  updateIdCount ();
  
}


function updateIdCount () {
  idCount = $("div[id^='entry']").length;
  $("#count").html (idCount);
}


function expandEntry () {
  // Bail out if nothing has been selected.
  if (selectedID == 0) return;
  // Bail out if the entry is already expanded.
  if ($(".selected > div").length > 0) return;
  // Navigate to the passage of the entry.
  $.post ("changes.php", { navigate:selectedID });
  // Get extra information through AJAX calls.
  $.get ("change.php?get=" + selectedID, function (response) {
    var extraInfo = $ ("<div>" + response + "</div>");
    $(".selected").append (extraInfo);
    var viewportHeight = $(window).height ();
    var infoHeight = extraInfo.height ();
    var infoOffset = extraInfo.offset ();
    $("body").animate({ scrollTop: infoOffset.top + (infoHeight / 2) - (viewportHeight / 2) }, 500);
  });
}


function collapseEntry () {
  if (selectedID == 0) return;
  $(".selected > div").remove ();
  selectEntry ();
}


function toggleEntry () {
  if (selectedID == 0) return;
  if ($(".selected > div").length > 0) {
    collapseEntry ();
  } else {
    expandEntry ();
  }
}
