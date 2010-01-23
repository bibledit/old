<?php
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
    // Assign a few page variables. 
    $smarty->assign("pagetitle","Bibledit");
    // Render and display the template.
    $smarty->display("index.tpl");
?>