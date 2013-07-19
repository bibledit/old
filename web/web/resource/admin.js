var href;

$(document).ready (function () {

  $(".deleteresource").on ("click", function (event) {
    href = event.currentTarget.href
    event.preventDefault ();
    $("#dialog-confirm").dialog ("open");
  });

  $( "#dialog-confirm" ).dialog({
    autoOpen: false,
    modal: true,
    buttons: {
      Confirm: function () {
        window.location.href = href;
      },
      Cancel: function() {
        $(this).dialog ("close");
      }
    }
  });

});
