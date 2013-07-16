var passage = "";


$(document).ready (function () {
  $( "#sortable" ).sortable({ update: sorted });
  $( "#sortable" ).disableSelection();
});


var sorted = function (event, ui) {
  var list = new Array ();
  $ ("#sortable").children ().each (function (i) {
    list.push (this.childNodes [1].data);
  });
  $.post ("settings.php", {'resources': list.toString()});
}

