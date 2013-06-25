<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Style{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Style{/t}</h1>
    
    <table>

      <tr><td>{t}Stylesheet{/t}</td><td><a href="sheetm.php?name={$sheet}">{$sheet}</a></td></tr>

      <tr><td>{t}Style{/t}</td><td>{$style}</td></tr>

      <tr><td>{t}Name{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&name=">{$name}</a></td></tr>

      <tr><td>{t}Description{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&info=">{$info}</a></td></tr>

      <tr><td>{t}Category{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&category=">{$category}</a></td></tr>

      <tr><td>{t}This style{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&type=">{$type_text}</a></td></tr>

      {if $subtype_text != ""}
      <tr><td>{t}This one{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&subtype=">{$subtype_text}</a></td></tr>
      {/if}

      {if $fontsize_relevant}
      <tr><td>{t}Font size{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&fontsize=">{$fontsize} {t}points{/t}</a></td></tr>
      {/if}

      {if $ibus_relevant}
      <tr><td>{t}Italic{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&italic=">{$italic}</a></td></tr>
      <tr><td>{t}Bold{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&bold=">{$bold}</a></td></tr>
      <tr><td>{t}Underline{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&underline">{$underline}</a></td></tr>
      <tr><td>{t}Small caps{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&smallcaps=">{$smallcaps}</a></td></tr>
      {/if}

      {if $superscript_relevant}
      <tr><td>{t}Superscript{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&superscript={$superscript_toggle}">{$superscript_value}</a></td></tr>
      {/if}

      {if $paragraph_treats_relevant}
      <tr><td>{t}Text alignment{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&alignment=">{$justification}</a></td></tr>
      <tr><td>{t}Space before{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&spacebefore=">{$spacebefore} {t}mm{/t}</a></td></tr>
      <tr><td>{t}Space after{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&spaceafter=">{$spaceafter} {t}mm{/t}</a></td></tr>
      <tr><td>{t}Left margin{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&leftmargin=">{$leftmargin} {t}mm{/t}</a></td></tr>
      <tr><td>{t}Right margin{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&rightmargin=">{$rightmargin} {t}mm{/t}</a></td></tr>
      <tr><td>{t}First line indent{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&firstlineindent=">{$firstlineindent} {t}mm{/t}</a></td></tr>
      {/if}

      {if $columns_relevant}
      <tr><td>{t}Span columns{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&spancolumns={$spancolumns_toggle}">{$spancolumns}</a></td></tr>
      {/if}

      {if $color_relevant}
      <tr><td>{t}Color{/t}</td><td style="background-color:#{$color}"></td><td><a href="view.php?sheet={$sheet}&style={$style}&color=">[{t}edit{/t}]</a></td></tr>
      {/if}

      {if $print_relevant}
      <tr><td>{t}Print{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&print={$print_toggle}">{$print}</a></td></tr>
      {/if}

      {if $userbool1_function != ""}
      <tr><td>{$userbool1_function}</td><td><a href="view.php?sheet={$sheet}&style={$style}&userbool1={$userbool1_toggle}">{$userbool1_value}</a></td></tr>
      {/if}

      {if $userbool2_function != ""}
      <tr><td>{$userbool2_function}</td><td><a href="view.php?sheet={$sheet}&style={$style}&userbool2={$userbool2_toggle}">{$userbool2_value}</a></td></tr>
      {/if}

      {if $userbool3_function != ""}
      <tr><td>{$userbool3_function}</td><td><a href="view.php?sheet={$sheet}&style={$style}&userbool3={$userbool3_toggle}">{$userbool3_value}</a></td></tr>
      {/if}

      {if $userint1_notenumbering}
      <tr><td>{t}Note numbering{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&notenumbering=">{$userint1}</a></td></tr>
      {/if}

      {if $userint1_columnnumber}
      <tr><td>{t}Column number{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&userint1=">{$userint1}</a></td></tr>
      {/if}

      {if $userint2_notenumberingrestart}
      <tr><td>{t}Restart the note numbering{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&notenumberingrestart=">{$userint2}</a></td></tr>
      {/if}

      {if $userint2_endnoteposition}
      <tr><td>{t}Position of endnote{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&endnoteposition=">{$userint2}</a></td></tr>
      {/if}

      {if $userstring1_numberingsequence}
      <tr><td>{t}User-defined note numbering sequence{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&userstring1=">{$userstring1}</a></td></tr>
      {/if}

      {if $userstring1_wordlistaddition}
      <tr><td>{t}Addition after word list entry{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&userstring1=">{$userstring1}</a></td></tr>
      {/if}

      {if $userstring2_dumpendnotes}
      <tr><td>{t}Dump endnotes when encountering marker{/t}</td><td><a href="view.php?sheet={$sheet}&style={$style}&userstring2=">{$userstring2}</a></td></tr>
      {/if}

    </table>

