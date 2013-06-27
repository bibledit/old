<h2>{$header}</h2>
<p>{$info_top}</p>
<p>
{section name=offset loop=$book_ids} 
<a href="{$caller}&{$action}={$book_ids[offset]}">{$book_names[offset]}</a>
{/section} 
</p>
<p>{$info_bottom}</p>
<h3><a href="{$caller}">{t}Cancel{/t}</a></h3>
