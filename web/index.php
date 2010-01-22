<?php
    // Reference the Smarty class library.
    require_once("smarty/Smarty.class.php");
    // Create a new instance of the Smarty class.
    $smarty = new Smarty;
    //** un-comment the following line to show the debug console 
    //$smarty->debugging = true; 
    // Assign a few page variables. 
    $smarty->assign("pagetitle","Bibledit");
    // Render and display the template.
    $smarty->display("index.tpl");
?>