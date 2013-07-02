<?php


class Dialog_Books2
{
  private $view;
    
  /**
  * Constructs a book dialog
  * $action - GET or POST action to take.
  * $inclusions - NULL (in which case it does nothing), or an array of book IDs to include.
  * $exclusions - NULL (in which case it does nothing), or an array of book IDs to exclude.
  */
  public function __construct ($header, $info_top, $info_bottom, $action, $inclusions, $exclusions)
  {
    $this->view = new Assets_View (__FILE__);
    $database_sessions = Database_Sessions::getInstance();
    $caller = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ("session" => $database_sessions->getCurrentSessionId ()));
    $this->view->view->caller = $caller;
    $this->view->view->header = $header;
    $this->view->view->info_top = $info_top;
    $this->view->view->info_bottom = $info_bottom;
    $this->view->view->action = $action;

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
    $this->view->view->book_ids = $book_ids;
    $this->view->view->book_names = $book_names;
    $this->view->render ("books2.php");
    Assets_Page::footer();
    die;
  }
  
}
?>
