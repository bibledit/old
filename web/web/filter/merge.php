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


/*

Note: This object should not call other objects within the Bibledit-Web source,
because this object is also called from Bibledit-Gtk, and does not have access
to those other objects.
Calling other objects would result in faral errors that break Bibledit-Gtk.

*/


class Filter_Merge
{

  /*
  This filter merges files.
  $base: Data for the merge base.
  $user: Data as modified by the user.
  $server: Data as modified by the collaboration server.
  The filter uses program "merge" do do a three-way merge.
  There should be one unchanged segment (either a line or word) between the modifications.
  If necessary it converts the data into a new format with one character per line, 
  for more fine-grained merging.
  In case of a conflict, it favours the edition from the server.
  The filter returns the merged data.
  */
  public static function run ($base, $user, $server)
  {
    // The three files for merging.
    $mergeBaseFile = uniqid (sys_get_temp_dir () . "/") . "mergebase.txt";
    $userModificationFile = uniqid (sys_get_temp_dir () . "/") . "usermodification.txt";
    $serverModificationFile = uniqid (sys_get_temp_dir () . "/") . "servermodification.txt";
    
    $success = false;
    
    // Try a standard line-based merge. Should be sufficient for most cases.
    file_put_contents ($mergeBaseFile, $base);
    file_put_contents ($userModificationFile, $user);
    file_put_contents ($serverModificationFile, $server);
    $exit_code = Filter_Merge::merge ($mergeBaseFile, $userModificationFile, $serverModificationFile);
    $result = file_get_contents ($userModificationFile);
    if ($exit_code == 0) $success = true;

    if (!$success) {
      // Convert the data to one word per line, and try to merge again.
      $baseWords = Filter_Merge::lines2words ($base);
      $userWords = Filter_Merge::lines2words ($user);
      $serverWords = Filter_Merge::lines2words ($server);
      file_put_contents ($mergeBaseFile, $baseWords);
      file_put_contents ($userModificationFile, $userWords);
      file_put_contents ($serverModificationFile, $serverWords);
      $exit_code = Filter_Merge::merge ($mergeBaseFile, $userModificationFile, $serverModificationFile);
      $result = file_get_contents ($userModificationFile);
      $result = Filter_Merge::words2lines ($result);
      if ($exit_code == 0) $success = true;
    }

    if (!$success) {
      // Convert the data so it has one grapheme per line, and try again.
      $baseGraphemes = Filter_Merge::lines2graphemes ($base);
      $userGraphemes = Filter_Merge::lines2graphemes ($user);
      $serverGraphemes = Filter_Merge::lines2graphemes ($server);
      file_put_contents ($mergeBaseFile, $baseGraphemes);
      file_put_contents ($userModificationFile, $userGraphemes);
      file_put_contents ($serverModificationFile, $serverGraphemes);
      $exit_code = Filter_Merge::merge ($mergeBaseFile, $userModificationFile, $serverModificationFile);
      $result = file_get_contents ($userModificationFile);
      $result = Filter_Merge::graphemes2lines ($result);
      if ($exit_code == 0) $success = true;
    }

    if (!$success) {
      // Everthing failed. Return the server data.
      $result = $server;
    }
    
    // Clear up.
    @unlink ($mergeBaseFile);
    @unlink ($userModificationFile);
    @unlink ($serverModificationFile);

    // Merge result.
    return $result;
  }


  /*
  merge - three-way file merge
  merge [ options ] file1 file2 file3
  merge  incorporates  all  changes  that  lead from file2 to file3 into file1.
  The result ordinarily goes into file1.  
  merge  is  useful  for combining separate changes to an original.
  Suppose file2 is the original, and both file1 and file3 are modifications of file2.
  Then merge combines both changes.
  Exit status is 0 for no conflicts, 1 for some conflicts, 2 for trouble.
  The function returns the exit status.
  */
  private static function merge ($base, $user, $server)
  {
    $command = "merge $user $base $server 2>&1";
    exec ($command, $output, $exit_code);
    return $exit_code;
  }
  
  private static function lines2words ($data)
  {
    $data = str_replace ("\n", " new__line ", $data);
    $data = str_replace (" ", "\n", $data);
    return $data;
  }
  
  
  private static function words2lines ($data)
  {
    $data = str_replace ("\n", " ", $data);
    $data = str_replace (" new__line ", "\n", $data);
    return $data;
  }


  private static function lines2graphemes ($data)
  {
    $data = str_replace ("\n", " new__line ", $data);
    $data2 = "";
    $count = grapheme_strlen ($data);
    for ($i = 0; $i < $count; $i++) {
      $grapheme = grapheme_substr ($data, $i, 1);
      $data2 .= $grapheme;
      $data2 .= "\n";
    }
    return $data2;
  }
  
  
  private static function graphemes2lines ($data)
  {
    $data = str_replace ("\n", "", $data);
    $data = str_replace (" new__line ", "\n", $data);
    return $data;
  }
  

}


?>
