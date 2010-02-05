<?


// Contains the access control levels.


// Special Names and Level Constants for user access control.
// page will only be accessible to the user with
// the admin name and also to those users at the
// admin user level.
// Levels must be digits between 0-9.

define("GUEST_LEVEL",       1);
define("MEMBER_LEVEL",      2);
define("CONSULTANT_LEVEL",  3);
define("TRANSLATOR_LEVEL",  4);
define("MANAGER_LEVEL",     5);
define("ADMIN_LEVEL",       6);

$roles = array(
               GUEST_LEVEL      => gettext ("Guest"),
               MEMBER_LEVEL     => gettext ("Member"),
               CONSULTANT_LEVEL => gettext ("Consultant"),
               TRANSLATOR_LEVEL => gettext ("Translator"),
               MANAGER_LEVEL    => gettext ("Manager"),
               ADMIN_LEVEL      => gettext ("Administrator")
               );


?>
