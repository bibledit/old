$(document).ready (function () {
  $(document).on ("passage", loadNotes);
});


function loadNotes ()
{
  $.get ("notes.php", function (response) {
    var container = $ ("#noteslist");
    container.empty ();
    container.append (response);
  });  
}
