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
  If necessary it converts the data into a new format with one character per line, 
  for more fine-grained merging.
  In case of a conflict, it favours the edition from the server.
  */
  public static function run ($base, $user, $server) // Todo
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
    $exit_code = Filter_Merge::pullData ($userClone);
    var_dump ($exit_code); // Todo
    $result = file_get_contents ("$userClone/data");
    if ($exit_code == 0) {

    }

    Filter_Rmdir::rmdir ($repository);
    // Todo Filter_Rmdir::rmdir ($userClone);
    Filter_Rmdir::rmdir ($serverClone);
        
    return $result;





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
  private static function pullData ($clone)
  {
    $command = "cd $clone; git pull 2>&1";
    exec ($command, $output, $exit_code);
    return $exit_code;
  }


}


?>
