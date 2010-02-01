<?php


class Validate_Email {

  public function valid ($email)
  {
    require_once ("is_email.php");
    return is_email ($email);
  }
} 


?>
