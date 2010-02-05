<?php


/**
* SQL Injection projection.
*/
class Database_SQLInjection
{
  public function no ($value) {
    if(get_magic_quotes_gpc())
      return $value;
    return addslashes ($value);
  }
}


?>
