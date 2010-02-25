<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


/**
* This prepares the statements for the list page which will list all possible roles to be chosen from.
*/
$user = $_GET['user'];
$header = gettext ("Would you like to change the role given to user $user?");
$info_top = gettext ("You can give another role to the user. These are the possible roles:");
$info_bottom = gettext ("Please select a role.");
$dialog_list = new Dialog_List ($_SERVER['HTTP_REFERER'], $header, $info_top, $info_bottom);
for ($i = GUEST_LEVEL; $i <= ADMIN_LEVEL; $i++) {
  $database = Database_Users::getInstance ();
  $parameter = "user=$user&level=$i";
  $dialog_list->add_row ($roles[$i], $parameter);
}
$dialog_list->run ();


?>
