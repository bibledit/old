<?php


class Dialog_Books
{
  private $smarty;
    
  /**
  * Constructs a book dialog
  * $query - The query values to keep. The remainder is removed from $caller.
  * $action - GET or POST action to take.
  * $inclusions - NULL (in which case it does nothing), or an array of book IDs to include.
  * $exclusions - NULL (in which case it does nothing), or an array of book IDs to exclude.
  */
  public function __construct ($query, $header, $info_top, $info_bottom, $action, $inclusions, $exclusions)
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $caller = $_SERVER["PHP_SELF"];
    if (is_array ($query)) {
      $full_query = array ();
      foreach ($query as $value) {
        $full_query = array_merge ($full_query, array ($value => $_GET[$value]));
      }
      $caller .= "?" . http_build_query ($full_query);
    }
    $this->smarty->assign ("caller", $caller);
    $this->smarty->assign ("header", $header);
    $this->smarty->assign ("info_top", $info_top);
    $this->smarty->assign ("info_bottom", $info_bottom);
    $this->smarty->assign ("action", $action);

    $database_books = Database_Books::getInstance();
    $book_ids = $database_books->getIDs ();
    if (is_array ($inclusions)) {
      $book_ids = $inclusions;
    }
    if (is_array ($exclusions)) {
      $book_ids = array_diff ($book_ids, $exclusions);
      $book_ids = array_values ($book_ids);
    }
    foreach ($book_ids as $id) {
      $book_names[] = $database_books->getEnglishFromId ($id);
    }
    $this->smarty->assign ("book_ids", $book_ids);
    $this->smarty->assign ("book_names", $book_names);
    $this->smarty->display("books.tpl");
  }
  
}
?>
