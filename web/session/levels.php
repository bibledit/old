<?


// Contains the access control levels.


// Special Names and Level Constants for user access control.
// page will only be accessible to the user with
// the admin name and also to those users at the
// admin user level.
// Levels must be digits between 0-9.

define("ADMIN_NAME", gettext ("Site Administrator"));
define("GUEST_NAME", gettext ("Guest"));
define("ADMIN_LEVEL", 9);
define("MANAGER_LEVEL",  7);
define("TRANSLATOR_LEVEL",  5);
define("CONSULTANT_LEVEL",  3);
define("PUBLIC_LEVEL",  1);
define("GUEST_LEVEL", 0);



?>
