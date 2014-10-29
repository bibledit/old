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


class Filter_Archive
{


  // Compresses a file or folder identified by $source into zip format.
  // Returns the path to the zipfile it created.
  public static function zip ($source)
  {
    if (!extension_loaded ('zip') || !file_exists ($source)) return false;
    $zippedfile = uniqid (sys_get_temp_dir() . "/") . ".zip";
    $zip = new ZipArchive();
    if (!$zip->open ($zippedfile, ZIPARCHIVE::CREATE)) return false;
    if (is_dir ($source) === true)
    {
      $files = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($source), RecursiveIteratorIterator::SELF_FIRST);
      foreach ($files as $file)
      {
        // Ignore "." and ".." folders
        if (in_array (substr ($file, strrpos ($file, '/') + 1), array ('.', '..'))) continue;
        if (is_dir ($file) === true) {
          $zip->addEmptyDir (str_replace ($source . '/', '', $file . '/'));
        }
        else if (is_file($file) === true) {
          $zip->addFromString (str_replace ($source . '/', '', $file), file_get_contents ($file));
        }
      }
    }
    else if (is_file ($source) === true) {
      $zip->addFromString (basename ($source), file_get_contents ($source));
    }
    $zip->close ();
    return $zippedfile;
  }


  // Uncompresses a zip archive identified by $file.
  // Returns the path to the folder it created.
  // If $show_errors is true, it outputs errors in html.
  public static function unzip ($file, $show_errors)
  {
    $folder = uniqid (sys_get_temp_dir() . "/");
    mkdir ($folder);
    $zip = new ZipArchive;
    $res = $zip->open ($file);
    if ($res === true) {
      $zip->extractTo ($folder);
      $zip->close ();
    } else {
      Filter_Rmdir::rmdir ($folder);
      $folder = NULL;
      if ($show_errors) {
        Assets_Page::error (Locale_Translate::_("Failed to uncompress archive"));
      }
    }
    return $folder;
  }


  /**
  * Compresses a file identified by $filename into gzipped tar format.
  * Returns the path to the compressed archive it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public static function tarGzipFile ($filename, $show_errors)
  {
    $tarball = uniqid (sys_get_temp_dir() . "/") . ".tar.gz";
    $dirname = escapeshellarg (dirname ($filename));
    $basename = escapeshellarg (basename ($filename));
    exec ("cd $dirname && tar -czf $tarball $basename 2>&1", $output, $return_var);
    if ($return_var != 0) {
      @unlink ($tarball);
      $tarball = NULL;
      if ($show_errors) {
        Assets_Page::error (Locale_Translate::_("Failed to compress file"));
        foreach ($output as $line) {
          Assets_Page::error ($line);
        }
        Assets_Page::error ("Return status: $return_var");
      }
    }
    return $tarball;
  }


  /**
  * Compresses a $folder into gzipped tar format.
  * Returns the path to the compressed archive it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public static function tarGzipFolder ($folder, $show_errors)
  {
    $tarball = uniqid (sys_get_temp_dir() . "/") . ".tar.gz";
    $folder = escapeshellarg ($folder);
    exec ("cd $folder && tar -czf $tarball . 2>&1", $output, $return_var);
    if ($return_var != 0) {
      @unlink ($tarball);
      unset ($tarball);
      if ($show_errors) {
        Assets_Page::error (Locale_Translate::_("Failed to compress folder"));
        foreach ($output as $line) {
          Assets_Page::error ($line);
        }
        Assets_Page::error ("Return status: $return_var");
      }
    }
    return $tarball;
  }


  /**
  * Uncompresses a .tar.gz archive identified by $file.
  * Returns the path to the folder it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public static function untargz ($file, $show_errors)
  {
    $file = escapeshellarg ($file);
    $folder = uniqid (sys_get_temp_dir() . "/");
    mkdir ($folder);
    exec ("cd $folder && tar zxf $file 2>&1", $output, $return_var);
    if ($return_var != 0) {
      Filter_Rmdir::rmdir ($folder);
      $folder = NULL;
      if ($show_errors) {
        Assets_Page::error (Locale_Translate::_("Failed to uncompress archive"));
        foreach ($output as $line) Assets_Page::error ($line);
        Assets_Page::error ("Return status: $return_var");
      }
    }
    return $folder;
  }



  /**
  * Uncompresses a known archive identified by $file.
  * Returns the path to the folder it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public static function uncompress ($file, $show_errors)
  {
    // Tar (.tar) archives, including those compressed with gzip (.tar.gz, .tgz), bzip (.tar.bz, .tbz), bzip2 (.tar.bz2, .tbz2), compress (.tar.Z, .taz), lzop (.tar.lzo, .tzo) and lzma (.tar.lzma)
    // Zip archives (.zip)
    // Jar archives (.jar, .ear, .war)
    // 7z archives (.7z)
    // iso9660 CD images (.iso)
    // Lha archives (.lzh)
    // Single files compressed with gzip (.gz), bzip (.bz), bzip2 (.bz2), compress (.Z), lzop (.lzo) and lzma (.lzma)
    $suffix = pathinfo ($file, PATHINFO_EXTENSION);
    switch ($suffix) {
      case "tar.gz": // Never called because pathinfo would give the 'gz' suffix only.
      case "gz":
      case "tgz":
        return Filter_Archive::untargz ($file, $show_errors);
      case "zip":
        return Filter_Archive::unzip ($file, $show_errors);
      case "tar.bz":
      case "tbz":
      case "tar.bz2":
      case "tbz2":
    }
    return NULL;
  }


}

?>
