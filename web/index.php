<?php
    
    // Page bootstrapping.
    require_once ("bootstrap/bootstrap.php");

    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->display("index.tpl");

/*


    // Specify the target language
    $language = "nl_NL.utf8";
    // Assign the appropriate locale
    putenv("LANG=$language"); 
    echo setlocale(LC_ALL, $language);
    echo "<br>";
    echo setlocale(LC_ALL, 0);
    echo "<br>";
    // echo setlocale(LC_ALL, "");

    // Identify the location of the translation files
    echo bindtextdomain("bibledit", "locale");
    echo "<br>";

    // Tell the script which domain to search within when translating text
    echo textdomain("bibledit");
    echo "<br>";

    echo gettext ("chapter");
    echo "<br>";
    echo gettext ("Bible");
    echo "<br>";
    echo gettext ("Bible editor");
    echo "<br>";
*/
    
?>
