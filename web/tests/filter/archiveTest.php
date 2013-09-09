<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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


class archiveTest extends PHPUnit_Framework_TestCase
{


  private $file1;
  private $file2;
  private $data1;
  private $data2;


  protected function setUp () 
  {
    for ($i = 0; $i < 1000; $i++) {
      $this->data1 .= "Data One\n";
      $this->data2 .= "Data Two\n";
    }
    $this->file1 = tempnam (sys_get_temp_dir(), '');
    file_put_contents ($this->file1, $this->data1);
    $this->file2 = tempnam (sys_get_temp_dir(), '');
    file_put_contents ($this->file2, $this->data2);
  }


  protected function tearDown () 
  {
    unlink ($this->file1);
    unlink ($this->file2);
  }


  public function testFilterArchiveZipFile()
  {
    // Test zip compression.
    $zipfile = Filter_Archive::zip ($this->file1, false);
    $this->assertTrue(file_exists ($zipfile));
    $this->assertEquals(159, filesize ($zipfile));
    unlink ($zipfile);
    // Test that compressing a non-existing file returns NULL.
    $zipfile = Filter_Archive::zip ("xxxxx", false);
    $this->assertEquals (NULL, $zipfile);
  }


  public function testFilterArchiveZipFolder()
  {
    $folder = uniqid (sys_get_temp_dir() . "/");
    mkdir ($folder);
    file_put_contents ("$folder/file1", $this->data1);
    file_put_contents ("$folder/file2", $this->data2);
    // Test zip compression.
    $zipfile = Filter_Archive::zip ($folder, false);
    $this->assertTrue(file_exists ($zipfile));
    $this->assertEquals(292, filesize ($zipfile));
    // Clean up the mess.
    unlink ($zipfile);
    Filter_Rmdir::rmdir ($folder);
  }


  public function testFilterArchiveUnzip()
  {
    $zipfile = Filter_Archive::zip ($this->file1, false);
    // Test unzip.
    $folder = Filter_Archive::unzip ($zipfile, false);
    $this->assertTrue(file_exists ($folder));
    foreach (new DirectoryIterator ($folder) as $fileInfo) {
      if($fileInfo->isDot()) continue;
      $path = $fileInfo->getFilename();
      $path = "$folder/$path";
      $this->assertEquals(9000, filesize ($path));
    }
    unlink ($zipfile);
    Filter_Rmdir::rmdir ($folder);
    // Test that unzipping garbage returns NULL.
    $folder = Filter_Archive::unzip ("xxxxx", false);
    $this->assertEquals(NULL, $folder);
  }


  public function testFilterArchiveTarGzipFile()
  {
    // Test gzipped tarball compression.
    $tarball = Filter_Archive::tarGzipFile ($this->file1, false);
    $this->assertTrue(file_exists ($tarball));
    $this->assertGreaterThan(140, filesize ($tarball));
    $this->assertLessThan(180, filesize ($tarball));
    // Clean up tarball from /tmp folder.
    unlink ($tarball);
    // Test that compressing a non-existing file returns NULL.
    $tarball = Filter_Archive::tarGzipFile ("xxxxx", false);
    $this->assertEquals(NULL, $tarball);
  }


  public function testFilterArchiveTarGzipFolder()
  {
    $folder = uniqid (sys_get_temp_dir() . "/");
    mkdir ($folder);
    file_put_contents ("$folder/file1", $this->data1);
    file_put_contents ("$folder/file2", $this->data2);
    // Test compression.
    $tarball = Filter_Archive::tarGzipFolder ($folder, false);
    $this->assertTrue(file_exists ($tarball));
    $this->assertGreaterThan(220, filesize ($tarball));
    $this->assertLessThan(260, filesize ($tarball));
    // Clean up.
    unlink ($tarball);
    Filter_Rmdir::rmdir ($folder);
    // Test that compressing a non-existing folder returns NULL.
    //$tarball = Filter_Archive::tarGzipFolder ("$folder/x", false);
    //$this->assertEquals(NULL, $tarball);
  }


  public function testFilterArchiveUntargz()
  {
    $tarball = Filter_Archive::tarGzipFile ($this->file1, false);
    // Test decompression.
    $folder = Filter_Archive::untargz ($tarball, false);
    $this->assertTrue(file_exists ($folder));
    Filter_Rmdir::rmdir ($folder);
    $folder = Filter_Archive::uncompress ($tarball, false);
    $this->assertTrue(file_exists ($folder));
    foreach (new DirectoryIterator ($folder) as $fileInfo) {
      if($fileInfo->isDot()) continue;
      $path = $fileInfo->getFilename();
      $path = "$folder/$path";
      $this->assertEquals(9000, filesize ($path));
    }
    Filter_Rmdir::rmdir ($folder);
    unlink ($tarball);
    // Test that unzipping garbage returns NULL.
    $folder = Filter_Archive::untargz ("xxxxx", false);
    $this->assertEquals(NULL, $folder);
  }


}


?>
