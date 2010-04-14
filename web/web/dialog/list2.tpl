<h2>{$header}</h2>
{if !$horizontal}
  <p>{$info_top}</p>
{/if}
{if !$horizontal}
  <ul>
{/if}
{section name=offset loop=$text_lines} 
  {if !$horizontal}<li>{/if}<a href="{$caller_url}{$get_parameters[offset]}">{$text_lines[offset]}</a>{if !$horizontal}</li>{/if}
{/section}
{if !$horizontal}
  </ul> 
{/if}
{if !$horizontal}
  <p>{$info_bottom}</p>
{/if}
<h3><a href="{$caller_url}">{t}Cancel{/t}</a></h3>
