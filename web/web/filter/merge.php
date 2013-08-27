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


class Filter_Merge
{

  /*
  This filter merges files.
  There are four arguments, all representing filenames:
  $base: Data for the merge base.
  $user: Data as modified by the user.
  $server: Data as modified by the collaboration server.
  The filter uses program "git" do do a three-way merge.
  There should be one unchanged segment (either a line or word) between the modifications.
  If necessary it converts the data into a new format with one character per line, 
  for more fine-grained merging.
  In case of a conflict, it favours the edition from the server.
  */
  public static function run ($base, $user, $server)
  {
    // First try a standard line-based merge. Should be sufficient for most cases.
    $repository = Filter_Merge::createRepository ();    
    $userClone = Filter_Merge::cloneRepository ($repository);
    Filter_Merge::commitData ($userClone, $base);
    Filter_Merge::pushData ($userClone);
    $serverClone = Filter_Merge::cloneRepository ($repository);
    Filter_Merge::commitData ($serverClone, $server);
    Filter_Merge::pushData ($serverClone);
    Filter_Merge::commitData ($userClone, $user);
    $exit_code = Filter_Merge::pullData ($userClone, "");
    $result = file_get_contents ("$userClone/data");
    Filter_Rmdir::rmdir ($repository);
    Filter_Rmdir::rmdir ($userClone);
    Filter_Rmdir::rmdir ($serverClone);
    if ($exit_code == 0) {
      return $result;
    }
    // The line-based merge didn't manage to merge properly. Convert the data with 
    // one word per line, and try to merge again.
    $baseWords = Filter_Merge::lines2words ($base);
    $userWords = Filter_Merge::lines2words ($user);
    $serverWords = Filter_Merge::lines2words ($server);
    $repository = Filter_Merge::createRepository ();    
    $userClone = Filter_Merge::cloneRepository ($repository);
    Filter_Merge::commitData ($userClone, $baseWords);
    Filter_Merge::pushData ($userClone);
    $serverClone = Filter_Merge::cloneRepository ($repository);
    Filter_Merge::commitData ($serverClone, $serverWords);
    Filter_Merge::pushData ($serverClone);
    Filter_Merge::commitData ($userClone, $userWords);
    $exit_code = Filter_Merge::pullData ($userClone, "");
    $result = file_get_contents ("$userClone/data");
    $result = Filter_Merge::words2lines ($result);
    Filter_Rmdir::rmdir ($repository);
    Filter_Rmdir::rmdir ($serverClone);
    Filter_Rmdir::rmdir ($userClone);
    if ($exit_code == 0) {
      return $result;
    }
    // The word-based merge mechanism didn't manage to merge without conflicts.
    // Convert the data so it has one grapheme per line, and try again.
    $baseGraphemes = Filter_Merge::lines2graphemes ($base);
    $userGraphemes = Filter_Merge::lines2graphemes ($user);
    $serverGraphemes = Filter_Merge::lines2graphemes ($server);
    $repository = Filter_Merge::createRepository ();    
    $userClone = Filter_Merge::cloneRepository ($repository);
    Filter_Merge::commitData ($userClone, $baseGraphemes);
    Filter_Merge::pushData ($userClone);
    $serverClone = Filter_Merge::cloneRepository ($repository);
    Filter_Merge::commitData ($serverClone, $serverGraphemes);
    Filter_Merge::pushData ($serverClone);
    Filter_Merge::commitData ($userClone, $userGraphemes);
    // Add a parameter so git will take the server's version in case of a merge conflict.
    $exit_code = Filter_Merge::pullData ($userClone, "-Xtheirs");
    $result = file_get_contents ("$userClone/data");
    $result = Filter_Merge::graphemes2lines ($result);
    Filter_Rmdir::rmdir ($repository);
    Filter_Rmdir::rmdir ($serverClone);
    Filter_Rmdir::rmdir ($userClone);
    if ($exit_code == 0) {
      return $result;
    }
    // Everthing failed. Return the server data.
    return $server;
  }


  // This function creates a file-based git repository.
  // It returns the path to the repository.
  private static function createRepository ()
  {
    $repository = uniqid (sys_get_temp_dir () . "/") . "repository";
    mkdir ($repository);
    $command = "cd $repository; git --bare init --shared 2>&1";
    exec ($command, $output, $exit_code);
    return $repository;
  }


  // This function clones a git $repository,
  // It returns the path to the clone.
  private static function cloneRepository ($repository)
  {
    $clone = uniqid (sys_get_temp_dir() . "/") . "clone";
    $command = "git clone file://$repository $clone 2>&1";
    exec ($command, $output, $exit_code);
    return $clone;
  }

  
  // The function commit $data to the git repository at $clone.
  private static function commitData ($clone, $data)
  {
    file_put_contents ("$clone/data", $data);
    $command = "cd $clone; git add . 2>&1";
    exec ($command, $output, $exit_code);
    $command = "cd $clone; git commit -a -m commit 2>&1";
    exec ($command, $output, $exit_code);
  }

   
  // The function pushes data in $clone to its server.
  private static function pushData ($clone)
  {
    $command = "cd $clone; git push 2>&1";
    exec ($command, $output, $exit_code);
  }


  // The function pulls data for $clone from its server.
  // It returns the exit code of the process.
  private static function pullData ($clone, $parameter)
  {
    $command = "cd $clone; git pull $parameter 2>&1";
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
