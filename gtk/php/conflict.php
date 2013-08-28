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


class Filter_Conflict
{

  // This function resolves any conflicts in a git repository.
  // $repository: Path to the repository.
  public static function run ($repository)
  {
    $unmerged_paths = Filter_Conflict::getUnmergedPaths ($repository);
    foreach ($unmerged_paths as $path) {
      Filter_Conflict::mergeUnmergedPath ($repository, $path);
    }
    if (count ($unmerged_paths)) {
      Filter_Conflict::commitMergedData ($repository);
    }
  }
  
  
  private static function getUnmergedPaths ($repository)
  {
    $unmerged_paths = array ();
    $shellrepo = escapeshellarg ($repository);
    $command = "cd $shellrepo; git status 2>&1";
    exec ($command, $output, $exit_code);
    foreach ($output as $line) {
      if (strpos ($line, "both modified:") !== false) {
        $line = trim ($line);
        $line = substr ($line, 20);
        $line = trim ($line);
        $unmerged_paths [] = $line;
      }
    }
    return $unmerged_paths;
  }
  
  
  private static function mergeUnmergedPath ($repository, $path)
  {
    $shellrepo = escapeshellarg ($repository);

    $common_ancestor = uniqid (sys_get_temp_dir () . "/") . "ancestor";
    $argument = escapeshellarg (":1:$path");
    $command = "cd $shellrepo; git show $argument > $common_ancestor";
    exec ($command, $output, $exit_code);
    
    $head_version = uniqid (sys_get_temp_dir () . "/") . "head";
    $argument = escapeshellarg (":2:$path");
    $command = "cd $shellrepo; git show $argument > $head_version";
    exec ($command, $output, $exit_code);
    
    $merge_head_version = uniqid (sys_get_temp_dir () . "/") . "merge_head";
    $argument = escapeshellarg (":3:$path");
    $command = "cd $shellrepo; git show $argument > $merge_head_version";
    exec ($command, $output, $exit_code);

    $mergeBase = file_get_contents ($common_ancestor);
    $userData = file_get_contents ($head_version);
    $serverData = file_get_contents ($merge_head_version);
    
    $mergedData = Filter_Merge::run ($mergeBase, $userData, $serverData);

    file_put_contents ("$repository/$path", $mergedData);    

    unlink ($common_ancestor);
    unlink ($head_version);
    unlink ($merge_head_version);
  }
  
  
  private static function commitMergedData ($repository)
  {
    $shellrepo = escapeshellarg ($repository);
    $command = "cd $shellrepo; git commit -a -m FixMergeConflicts 2>&1";
    exec ($command, $output, $exit_code);
  }
  

}


?>
