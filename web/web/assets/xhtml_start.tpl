<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{$title}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
    {if isset ($document_ready_functions) || isset ($wysiwyg_editor) }
      <script type="text/javascript" src="../jquery/jquery.js"></script>
      {section name=offset loop=$document_ready_functions} 
        <script>
        {literal}
          $(document).ready(function() {
          {/literal}
            {$document_ready_functions[offset]}
          {literal}
          });
        {/literal}
        </script>
      {/section} 
    {/if}
      <link rel="stylesheet" href="../jwysiwyg/jwysiwyg/jquery.wysiwyg.css" type="text/css" />
      <script type="text/javascript" src="../jwysiwyg/jwysiwyg/jquery.wysiwyg.js"></script>
      <script type="text/javascript">
        {literal}
          $(function()
          {
              $('#wysiwyg').wysiwyg();
          });
        {/literal}
      </script>
    {if isset ($wysiwyg_editor) }
    {/if}
  </head>
  <body>
