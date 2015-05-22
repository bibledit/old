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


class Filter_Merge
{

  /*
  This filter merges files.
  $base: Data for the merge base.
  $user: Data as modified by the user.
  $server: Data as modified by the collaboration server.
  The filter uses a three-way merge algorithm.
  There should be one unchanged segment (either a line or word) between the modifications.
  If necessary it converts the data into a new format with one character per line for more fine-grained merging.
  In case of a conflict, it favours the edition from the server.
  The filter returns the merged data.
  */
  public static function run ($base, $user, $server)
  {
    $success = false;

    // Try a standard line-based merge. Should be sufficient for most cases.
    $result = Filter_Merge::merge ($base, $user, $server);
    if ($result) $success = true;

    if (!$success) {
      // Convert the data to one word per line, and try to merge again.
      $baseWords = Filter_Merge::lines2words ($base);
      $userWords = Filter_Merge::lines2words ($user);
      $serverWords = Filter_Merge::lines2words ($server);
      $result = Filter_Merge::merge ($baseWords, $userWords, $serverWords);
      $result = Filter_Merge::words2lines ($result);
      if ($result) $success = true;
    }

    if (!$success) {
      // Convert the data so it has one grapheme per line, and try again.
      $baseGraphemes = Filter_Merge::lines2graphemes ($base);
      $userGraphemes = Filter_Merge::lines2graphemes ($user);
      $serverGraphemes = Filter_Merge::lines2graphemes ($server);
      $result = Filter_Merge::merge ($baseGraphemes, $userGraphemes, $serverGraphemes);
      $result = Filter_Merge::graphemes2lines ($result);
      if ($result) $success = true;
    }

    if (!$success) {
      // Everything failed. Return the server data.
      $result = $server;
    }

    // Merge result.
    return $result;
  }


  /*
  merge - three-way merge.
  merge is  useful  for combining separate changes to an original.
  The function normally returns the merged text.
  If case of conflicts, it returns NULL.
  */
  private static function merge ($base, $user, $server)
  {
    include_once ("merge_merge.php");
    $threewaymerge = new ThreeWayMerge ($base, $user, $server);
    $result = $threewaymerge->merge ();
    if (strpos ($result, "be:conflict") !== false) return NULL;
    return $result;
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
    $count = mb_strlen ($data);
    for ($i = 0; $i < $count; $i++) {
      $grapheme = mb_substr ($data, $i, 1);
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
