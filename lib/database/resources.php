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


// Robustness: Data is stored on the filesystem.


class Database_Resources
{

  private static $instance;
  private $db;
  private function __construct () {
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Resources ();
    }
    return self::$instance;
  }


  private function folder ()
  {
    return realpath (__DIR__ . "/../resources");
  }
  
  
  public function getFile ($name)
  {
    $name = str_replace (" ", "_", $name);
    $file = $this->folder () . "/$name.php";
    return $file;
  }


  public function getInclude ($name)
  {
    $name = str_replace (" ", "_", $name);
    $file = "../resources/$name.php";
    return $file;
  }


  public function getMetaFile ($name)
  {
    $file = $this->getFile ($name);
    $file = str_replace (".php", "_meta.php", $file);
    return $file;
  }


  public function getVersification ($name)
  {
    $versification = "English";
    @include ($this->getMetaFile ($name));
    return $versification;
  }


  public function getMapping ($name)
  {
    $mapping = "English";
    @include ($this->getMetaFile ($name));
    return $mapping;
  }


  public function save ($name, $code)
  {
    $file = $this->getFile ($name);
    file_put_contents ($file, $code);
  }


  // Gets the names of the available external resource scripts.
  // The names are derived from the filenames of the resource scripts, as follows:
  // * An underscore in the filenames becomes a space in the resource name.
  // * The .php extension is removed.
  // * Scripts ending on _meta.php are not considered. They contain meta information for that resource.
  public function getNames ()
  {
    $names = array ();
    $files = scandir ($this->folder ());
    $files = Filter_Folders::cleanup ($files);
    foreach ($files as $file) {
      if ($file == strtolower ($file)) continue;
      if (strpos ($file, "_meta.php") != false) continue;
      $suffix = pathinfo ($file, PATHINFO_EXTENSION);
      if ($suffix != "php") continue;
      $name = substr ($file, 0, -1 - strlen ($suffix));
      $name = str_replace ("_", " ", $name);
      $names [] = $name;
    }
    $names = array_values ($names);
    return $names;
  }


  public function delete ($name)
  {
    $file = $this->getFile ($name);
    @unlink ($file);
  }


}


?>
