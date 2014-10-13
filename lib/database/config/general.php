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


class Database_Config_General
{
  // Singleton object instance logic.
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Config_General();
    }
    return self::$instance;
  }


  // Functions for getting and setting values or lists of values.


  public function file ($key)
  {
    $path = realpath (__DIR__ . "/../../databases/config/general");
    $file = "$path/$key";
    return $file;
  }


  public function getValue ($key, $default)
  {
    $file = $this->file ($key);
    @$value = file_get_contents ($file);
    if ($value !== false) return $value;
    return $default;
  }


  public function setValue ($key, $value)
  {
    $file = $this->file ($key);
    file_put_contents ($file, $value);
  }


  public function getList ($key)
  {
    $file = $this->file ($key);
    @$value = file_get_contents ($file);
    if ($value === false) return array ();
    $list = explode ("\n", $value);
    return $list;
  }


  public function setList ($key, $values)
  {
    $file = $this->file ($key);
    $value = implode ("\n", $values);
    file_put_contents ($file, $value);
  }


  // Named configuration function.
  

  public function getSiteMailName() {
    return $this->getValue ("site-mail-name", "Bible Translation");
  }
  public function setSiteMailName ($value) {
    $this->setValue ("site-mail-name", $value);
  }

  public function getSiteMailAddress() {
    return $this->getValue ("site-mail-address", "");
  }
  public function setSiteMailAddress ($value) {
    $this->setValue ("site-mail-address", $value);
  }

  public function getMailStorageHost() {
    return $this->getValue ("mail-storage-host", "");
  }
  public function setMailStorageHost ($value) {
    $this->setValue ("mail-storage-host", $value);
  }

  public function getMailStorageUsername() {
    return $this->getValue ("mail-storage-username", "");
  }
  public function setMailStorageUsername ($value) {
    $this->setValue ("mail-storage-username", $value);
  }

  public function getMailStoragePassword() {
    return $this->getValue ("mail-storage-password", "");
  }
  public function setMailStoragePassword ($value) {
    $this->setValue ("mail-storage-password", $value);
  }

  public function getMailStorageSecurity() {
    return $this->getValue ("mail-storage-security", "");
  }
  public function setMailStorageSecurity ($value) {
    $this->setValue ("mail-storage-security", $value);
  }

  public function getMailStoragePort() {
    return $this->getValue ("mail-storage-port", "");
  }
  public function setMailStoragePort ($value) {
    $this->setValue ("mail-storage-port", $value);
  }

  public function getMailSendHost() {
    return $this->getValue ("mail-send-host", "");
  }
  public function setMailSendHost ($value) {
    $this->setValue ("mail-send-host", $value);
  }

  public function getMailSendAuthentication() {
    return $this->getValue ("mail-send-authentication", "");
  }
  public function setMailSendAuthentication ($value) {
    $this->setValue ("mail-send-authentication", $value);
  }

  public function getMailSendUsername() {
    return $this->getValue ("mail-send-username", "");
  }
  public function setMailSendUsername ($value) {
    $this->setValue ("mail-send-username", $value);
  }

  public function getMailSendPassword() {
    return $this->getValue ("mail-send-password", "");
  }
  public function setMailSendPassword ($value) {
    $this->setValue ("mail-send-password", $value);
  }

  public function getMailSendSecurity() {
    return $this->getValue ("mail-send-security", "");
  }
  public function setMailSendSecurity ($value) {
    $this->setValue ("mail-send-security", $value);
  }

  public function getMailSendPort() {
    return $this->getValue ("mail-send-port", "");
  }
  public function setMailSendPort ($value) {
    $this->setValue ("mail-send-port", $value);
  }

  public function getTimerMinute() {
    return $this->getValue ("timer-minute", "");
  }
  public function setTimerMinute ($value) {
    $this->setValue ("timer-minute", $value);
  }

  public function getTimezone() {
    return $this->getValue ("timezone", "UTC");
  }
  public function setTimezone ($value) {
    $this->setValue ("timezone", $value);
  }

  public function getSiteURL() {
    return $this->getValue ("site-url", "");
  }
  public function setSiteURL ($value) {
    $this->setValue ("site-url", $value);
  }

  public function getSiteLanguage () {
    return $this->getValue ("site-language", "");
  }
  public function setSiteLanguage ($value) {
    $this->setValue ("site-language", $value);
  }

  public function getClientMode () {
    return $this->getValue ("client-mode", false);
  }
  public function setClientMode ($value) {
    $this->setValue ("client-mode", $value);
  }

  public function getServerAddress () {
    return $this->getValue ("server-address", "");
  }
  public function setServerAddress ($value) {
    $this->setValue ("server-address", $value);
  }

  public function getRepeatSendReceive () {
    return $this->getValue ("repeat-send-receive", 0);
  }
  public function setRepeatSendReceive ($value) {
    $this->setValue ("repeat-send-receive", $value);
  }

}


?>
