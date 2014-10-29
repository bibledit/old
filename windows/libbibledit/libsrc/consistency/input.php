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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$database_volatile = Database_Volatile::getInstance ();


$id = $_POST ['id'];
$passages = $_POST ['passages'];
$translations = $_POST ['translations'];


$database_volatile->setValue ($id, "passages", $passages);
$database_volatile->setValue ($id, "translations", $translations);


$logic = new Consistency_Logic ($id);


$response = $logic->response ();


$database_volatile->setValue ($id, "response", $response);


echo $response;


?>
