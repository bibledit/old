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


class Filter_Upload
{


  public static function error2text ($error)
  {
    switch ($error) {
      case UPLOAD_ERR_OK:
        $message = "There is no error, the file uploaded with success";
        break;
      case UPLOAD_ERR_INI_SIZE:
        $message = "The uploaded file exceeds the upload_max_filesize directive in php.ini";
        break;
      case UPLOAD_ERR_FORM_SIZE:
        $message = "The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form";
        break;
      case UPLOAD_ERR_PARTIAL:
        $message = "The uploaded file was only partially uploaded";
        break;
      case UPLOAD_ERR_NO_FILE:
        $message = "No file was uploaded";
        break;
      case UPLOAD_ERR_NO_TMP_DIR:
        $message = "Missing a temporary folder";
        break;
      case UPLOAD_ERR_CANT_WRITE:
        $message = "Failed to write file to disk";
        break;
      case UPLOAD_ERR_EXTENSION:
        $message = "File upload stopped by extension";
        break;
      default:
        $message = "General upload error";
        break;
    }
    return $message;
  }




}

?>
