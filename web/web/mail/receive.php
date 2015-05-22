<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


class Mail_Receive
{
  public $storage; // Mail object.
  public $count;   // Amount of messages on the server.
  public function __construct()
  {
    $config_general = Database_Config_General::getInstance ();
    $storagehost =     $config_general->getMailStorageHost ();
    $storageusername = $config_general->getMailStorageUsername ();
    $storagepassword = $config_general->getMailStoragePassword ();
    $storagesecurity = $config_general->getMailStorageSecurity ();
    $storageport     = $config_general->getMailStoragePort ();
    $config = array('host' => $storagehost, 'user' => $storageusername, 'password' => $storagepassword);
    if ($storagesecurity != "NONE") {
      $config = array_merge ($config, array ('ssl' => $storagesecurity));
    }
    if ($storageport != "") {
      $config = array_merge ($config, array ('port' => $storageport));
    }
    $this->storage = new Zend_Mail_Storage_Pop3($config);
    $this->count = $this->storage->countMessages();
  }
}


?>
