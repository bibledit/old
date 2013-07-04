<?php echo '<?xml version="1.0" encoding="UTF-8"?>' . "\n" ?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title><?php echo "$this->title" ?></title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
    <?php if (isset ($this->document_ready_functions) || isset ($this->wysiwyg_editor)) { ?>
      <script type="text/javascript" src="../jquery/jquery.js"></script>
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
