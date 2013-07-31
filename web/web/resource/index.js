$(document).ready (function () {
  $(document).on ("passage", loadResources);
});


function loadResources ()
{
  for (i = 0; i < resourceCount; i++) {
    var container = $ ("#bibledit" + i);
    container.empty ();
    container.append ("↻");
    $.ajax ({
      context: container,
      url: "get.php",
      type: "GET",
      data: { resource: i, book: navigationBook, chapter: navigationChapter, verse: navigationVerse },
      success: function (response) {
        this.empty ();
        this.append ($ (response));
      },
    });
  }
}
