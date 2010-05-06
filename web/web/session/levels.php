<?

// Contains the access control levels.

// Special Names and Level Constants for user access control.
// Levels must be digits between 0-9.

if (!defined ("GUEST_LEVEL"))      define("GUEST_LEVEL",      1);
if (!defined ("MEMBER_LEVEL"))     define("MEMBER_LEVEL",     2);
if (!defined ("CONSULTANT_LEVEL")) define("CONSULTANT_LEVEL", 3);
if (!defined ("TRANSLATOR_LEVEL")) define("TRANSLATOR_LEVEL", 4);
if (!defined ("MANAGER_LEVEL"))    define("MANAGER_LEVEL",    5);
if (!defined ("ADMIN_LEVEL"))      define("ADMIN_LEVEL",      6);

$roles = array
(
  GUEST_LEVEL      => gettext ("Guest"),
  MEMBER_LEVEL     => gettext ("Member"),
  CONSULTANT_LEVEL => gettext ("Consultant"),
  TRANSLATOR_LEVEL => gettext ("Translator"),
  MANAGER_LEVEL    => gettext ("Manager"),
  ADMIN_LEVEL      => gettext ("Administrator")
);

?>
