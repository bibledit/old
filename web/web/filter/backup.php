<?php


class Filter_Backup
{


  public static function file ()
  {
    $config_general = Database_Config_General::getInstance ();
    $value = $config_general->getBackupFile ();
    if ($value == "") {
      return $value;
    }
    include ("paths/paths.php");
    $file = "$localStatePath/$location/$value.sql.gz";
    return $file;
  }

  
}


?>
