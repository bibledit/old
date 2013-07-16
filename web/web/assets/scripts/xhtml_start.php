<!doctype html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
<meta charset="utf-8" />
<title><?php echo "$this->title" ?></title>
<link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
<?php if ($this->include_jquery) { ?>
<script type="text/javascript" src="../jquery/jquery-1.10.2.min.js"></script>
<?php } ?>
<?php if ($this->include_jquery_ui) { ?>
<link rel="stylesheet" type="text/css" href="../jquery/jquery-ui-1.10.3.css"></link>
<script type="text/javascript" src="../jquery/jquery-ui-1.10.3.min.js"></script>
<?php } ?>
<?php if (isset ($this->wysiwyg_editor)) { ?>
  <link rel="stylesheet" href="../jwysiwyg/jquery.wysiwyg.css" type="text/css" />
  <script type="text/javascript" src="../jwysiwyg/jquery.wysiwyg.js"></script>
  <script type="text/javascript">
    $(function()
    {
      $('#wysiwyg').wysiwyg();
    });
  </script>
<?php } ?>
<?php 
  if (isset ($this->document_ready_functions) || isset ($this->wysiwyg_editor)) { 
    foreach ($this->document_ready_functions as $document_ready_function) {
      echo "<script>\n";
        echo '$(document).ready(function() {' . "\n";
          echo "$document_ready_function\n";
        echo "});\n";
      echo "</script>\n";
    }
  } 
?>
</head>
<?php  
echo "<body";
if (isset ($this->onLoadStatement)) {
  echo ' onload="' . $this->onLoadStatement . '"';
}
echo ">\n";
?>
