var idCount = 0;


$(document).ready(function() {
  selectEntry ();
  $("body").bind ("keydown", keyDown);
  updateIdCount ();
});


function keyDown (event) {
  //console.log (event);
  // Handle Arrow Down.
  if (event.keyCode == 40) {
    event.preventDefault ();
    nextEntry ();
    selectEntry ();
  }
  // Handle Arrow Up.
  if (event.keyCode == 38) {
    event.preventDefault ();
    previousEntry ();
    selectEntry ();
  }
  // Handle Delete
  if (event.keyCode == 46) {
    removeEntry ();
    nextEntry ();
    selectEntry ();
  }
}


function nextEntry () {
  if (selectedID == 0) return;
  while (selectedID++ <= lastID) {
    if ($("div#" + selectedID).length) {
      return;
    }
  }
  if ($("div").length) {
    selectedID = $("div").last ().attr ("id");
  } else {
    selectedID = 0;
  }
}


function previousEntry () {
  if (selectedID == 0) return;
  while (selectedID-- >= firstID) {
    if ($("div#" + selectedID).length) {
      return;
    }
  }
  if ($("div").length) {
    selectedID = $("div").first ().attr ("id");
  } else {
    selectedID = 0;
  }

}


function selectEntry () {
  $(".selected").removeClass ("selected");
  var selectedElement = $("div#" + selectedID);
  if (selectedElement.length) {
    selectedElement.addClass ("selected");
    var viewportHeight = $(window).height ();
    var elementHeight = selectedElement.height ();
    var elementOffset = selectedElement.offset ();
    $("body").scrollTop (elementOffset.top + (elementHeight / 2) - (viewportHeight / 2));
  }
}


function removeEntry () {
  if (selectedID == 0) return;
  $.post ("changes.php", { remove:selectedID });
  $(".selected").remove ();
  updateIdCount ();
  
}


function updateIdCount () {
  idCount = $("div").length;
  $("#count").html (idCount);
}
