var href;

$(document).ready (function () {

  $(".deleteresource").on ("click", function (event) {
    href = event.currentTarget.href
    event.preventDefault ();
    $("#confirm-delete").dialog ("open");
  });

  $( "#confirm-delete" ).dialog({
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

  $(".convertresource").on ("click", function (event) {
    href = event.currentTarget.href
    event.preventDefault ();
    $("#confirm-convert").dialog ("open");
  });

  $( "#confirm-convert" ).dialog({
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
