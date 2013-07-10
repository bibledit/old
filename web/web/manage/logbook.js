$(window).load (function () {
  $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 3000);
});

var getMore = function () 
{
  $.get ("logbook.php?id=" + nextID, function (response) {
    if (response == "") {
      setTimeout (getMore, 2000);
    } else {
      $ ("div").append ($ ("<p>" + response + "</p>"));
      $("body").animate({ scrollTop: $(document).height() - $(window).height() }, 100);
      nextID++;
      setTimeout (getMore, 100);
    }
  });  
}

setTimeout (getMore, 3000);
