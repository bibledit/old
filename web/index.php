<?php

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
    
    // Reference the Smarty class library.
    require_once("smarty/Smarty.class.php");
    // Reference the Smarty gettext plugin.
    require_once('smarty-gettext/smarty-gettext.php');
    // Create a new instance of the Smarty class.
    $smarty = new Smarty;
    // Register the gettext plugin as a block function named 't'.
    $smarty->register_block('t', 'smarty_translate');
    //** un-comment the following line to show the debug console 
    //$smarty->debugging = true; 
    // Assign a page variable. 
    $smarty->assign("pagetitle",gettext ("Bible editor"));
    // Render and display the template.
    $smarty->display("index.tpl");
?>