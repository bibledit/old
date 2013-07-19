<?php

require_once ("bootstrap/bootstrap.php");

class Resource_Logic
{

  public static function getExternal ($name, $book, $chapter, $verse) // Todo implement.
  {
    $database_resources = Database_Resources::getInstance ();
    $details = $database_resources->getDetails ($name);
    $command = $details ['command'];
    $code = $details ['code'];
    $filename = tempnam (sys_get_temp_dir (), "");
    file_put_contents ($filename, $code);
    $command = str_replace ("script", "$filename $book $chapter $verse", $command) . " 2>&1";
    $output = shell_exec ($command);
    unlink ($filename);
    return $output;
  }

}

?>
