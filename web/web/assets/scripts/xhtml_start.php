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
</head>
<?php  
echo "<body";
if (isset ($this->onLoadStatement)) {
  echo ' onload="' . $this->onLoadStatement . '"';
}
echo ">\n";
?>
