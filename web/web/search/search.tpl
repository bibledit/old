<div style="float:left; width:50%;">
<font size="-1" color="grey"><hr /></font>
<p><font size="-1" color="grey">{$noteCount} {t}notes{/t}</font></p>
{section name=offset loop=$noteUrls} 
  <p style="margin-top: 0.75em; margin-bottom: 0em"><a href="{$noteUrls[offset]}">{$noteTitles[offset]}</a></p>
  {$noteExcerpts[offset]}
{/section} 
</div>
<div style="float:right; width:50%; ">
<font size="-1" color="grey"><hr /></font>
<p><font size="-1" color="grey">{$textCount} {t}chapters{/t}</font></p>
{section name=offset loop=$textUrls} 
  <p style="margin-top: 0.75em; margin-bottom: 0em"><a href="{$textUrls[offset]}">{$textTitles[offset]}</a></p>
  {$textExcerpts[offset]}
{/section} 
</div>
