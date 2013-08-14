$(window).load (function () {
  $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 3000);
});

var getMore = function () 
{
  $.get ("logbook.php?id=" + lastID, function (response) {
    if (response == "") {
      setTimeout (getMore, 2000);
    } else {
      response = $.parseJSON (response);
      lastID = response ["id"];
      $ ("div").append ($ ("<p>" + response ["entry"] + "</p>"));
      $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 100);
      setTimeout (getMore, 100);
    }
  });  
}

setTimeout (getMore, 3000);
