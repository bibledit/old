<font size="-1" color="grey"><hr /></font>
<p><font size="-1" color="grey">{$totalFound} {t}results{/t}</font></p>
{section name=offset loop=$urls} 
  <p style="margin-bottom: 0em"><a href="{$urls[offset]}" title="{$titles[offset]}">{$titles[offset]}</a></p>
  <p style="margin-top: 0em; margin-bottom: 0em"><font size="-1" color="green">{$urls[offset]}</font></p>
  <p>{$excerpts[offset]}</p>
{/section} 
<font size="-1" color="grey"><hr /></font>
