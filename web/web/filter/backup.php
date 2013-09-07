<?php


class Filter_Backup
{


  public static function file ()
  {
    $config_general = Database_Config_General::getInstance ();
    include ("paths/paths.php");
    $file = "$localStatePath/$location/backup.sql.gz";
    return $file;
  }

  
}


?>
