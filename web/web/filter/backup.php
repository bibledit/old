<?php


class Filter_Backup
{


  public static function folder ()
  {
    include ("paths/paths.php");
    $folder = "$localStatePath/$location";
    return $folder;
  }


  public static function file ()
  {
    $file = Filter_Backup::folder () . "/backup.sql.gz";
    return $file;
  }

  
}


?>
