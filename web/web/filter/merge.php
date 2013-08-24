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
  $base: Filename with the merge base.
  $user: Filename with the modifications made by the user.
  $server: Filename with the modifications coming from the collaboration server.
  $output: Fileame for the merged output.
  The filter uses program "merge" do do a three-way merge.
  It converts the data into a new format with one character per line, 
  for better merging and far fewer conflicts.
  In case of a conflict, it favours the modification from the server.
  */
  public static function files ($base, $user, $server, $output) // Todo
  {
    // First try a standard line-based merge. Should be sufficient for most cases.


  }
  

}


?>
