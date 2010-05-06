<?php

class Filter_Archive
{

  /**
  * Compresses a file identified by $filename into zip format.
  * Returns the path to the compressed archive it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public function zipFile ($filename, $show_errors)
  {
    $zippedfile = tempnam (sys_get_temp_dir(), '') . ".zip";
    $dirname = escapeshellarg (dirname ($filename));
    $basename = escapeshellarg (basename ($filename));
    exec ("cd $dirname && zip $zippedfile $basename 2>&1", $output, &$return_var);
    if ($return_var != 0) {
      $zippedfile = NULL;
      if ($show_errors) {
        Assets_Page::error (gettext ("Failed to compress file"));
        foreach ($output as $line) {
          Assets_Page::error ($line);
        }
        Assets_Page::error ("Return status: $return_var");
      }
    }
    return $zippedfile;
  }
  

  /**
  * Compresses a $folder into zip format.
  * Returns the path to the compressed archive it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public function zipFolder ($folder, $show_errors)
  {
    $zippedfile = tempnam (sys_get_temp_dir(), '') . ".zip";
    $folder = escapeshellarg ($folder);
    exec ("cd $folder && zip -r $zippedfile * 2>&1", $output, &$return_var);
    if ($return_var != 0) {
      unset ($zippedfile);
      if ($show_errors) {
        Assets_Page::error (gettext ("Failed to compress folder"));
        foreach ($output as $line) {
          Assets_Page::error ($line);
        }
        Assets_Page::error ("Return status: $return_var");
      }
    }
    return $zippedfile;
  }


  /**
  * Uncompresses a zip archive identified by $file.
  * Returns the path to the folder it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public function unzip ($file, $show_errors)
  {
    $file = escapeshellarg ($file);
    $folder = tempnam (sys_get_temp_dir(), '');
    unlink ($folder);
    mkdir ($folder);
    exec ("unzip -o -d $folder $file 2>&1", $output, &$return_var);
    if ($return_var != 0) {
      $folder = NULL;
      if ($show_errors) {
        Assets_Page::error (gettext ("Failed to uncompress archive"));
        foreach ($output as $line) Assets_Page::error ($line);
        Assets_Page::error ("Return status: $return_var");
      }
    }
    return $folder;
  }


  /**
  * Compresses a file identified by $filename into gzipped tar format.
  * Returns the path to the compressed archive it created.
  * If $show_errors is true, it outputs errors in html.
  */
  public function tarGzipFile ($filename, $show_errors)
  {
    $tarball = tempnam (sys_get_temp_dir(), '') . ".tar.gz";
    $dirname = escapeshellarg (dirname ($filename));
    $basename = escapeshellarg (basename ($filename));
    exec ("cd $dirname && tar -czf $tarball $basename 2>&1", $output, &$return_var);
    if ($return_var != 0) {
      unset ($tarball);
      if ($show_errors) {
        Assets_Page::error (gettext ("Failed to compress file"));
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
  public function tarGzipFolder ($folder, $show_errors)
  {
    $tarball = tempnam (sys_get_temp_dir(), '') . ".tar.gz";
    $folder = escapeshellarg ($folder);
    exec ("cd $folder && tar -czf $tarball . 2>&1", $output, &$return_var);
    if ($return_var != 0) {
      unset ($tarball);
      if ($show_errors) {
        Assets_Page::error (gettext ("Failed to compress folder"));
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
  public function untargz ($file, $show_errors)
  {
    $file = escapeshellarg ($file);
    $folder = tempnam (sys_get_temp_dir(), '');
    unlink ($folder);
    mkdir ($folder);
    exec ("cd $folder && tar zxf $file 2>&1", $output, &$return_var);
    if ($return_var != 0) {
      $folder = NULL;
      if ($show_errors) {
        Assets_Page::error (gettext ("Failed to uncompress archive"));
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
  public function uncompress ($file, $show_errors)
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
      case "tar.gz": // Never called because pathinfo would give the 'gz' only.
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
