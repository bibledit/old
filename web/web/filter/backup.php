<?php


class Filter_Backup
{

  public function file ()
  {
    $config_general = Database_Config_General::getInstance ();
    $file = dirname (dirname (__FILE__));
    $file = $file . "/" . $config_general->getBackupFile () . ".sql.gz";
    return $file;
  }
  
  public function url ()
  {
    $config_general = Database_Config_General::getInstance ();
    $url = $_SERVER["HTTP_REFERER"];
    $url = dirname (dirname ($url));
    $url = $url . "/" . $config_general->getBackupFile () . ".sql.gz";
    return $url;
  }
  
  
  
  
    

}


?>
