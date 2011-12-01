<?php


class Filter_Backup
{

  public function file () // Todo
  {
    $config_general = Database_Config_General::getInstance ();
    $value = $config_general->getBackupFile ();
    if ($value == "") {
      return $value;
    }
    include ("paths/paths.php");
    $file = $localStatePath . "/" . $value . ".sql.gz";
    return $file;
  }
  
  public function url ()
  {
    $config_general = Database_Config_General::getInstance ();
    $value = $config_general->getBackupFile ();
    if ($value == "") {
      return $value;
    }
    $url = $_SERVER["HTTP_REFERER"];
    $url = dirname (dirname ($url));
    $url = $url . "/" . $value . ".sql.gz";
    return $url;
  }
    

}


?>
