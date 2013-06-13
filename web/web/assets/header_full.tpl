<table width="100%">
  <tr>
    <td>
    {* Administration menu is only applicable to the site administrator *}
    {if $level >= 6}
      <a href="{$header_path_modifier}administration/index.php">{t}Administration{/t}</a>
    {/if}
    {* Management menu is only applicable to the manager and higher roles *}
    {if $level >= 5}
      <a href="{$header_path_modifier}manage/index.php">{t}Management{/t}</a>
    {/if}
    {* Translation menu is only applicable to the translator and higher roles *}
    {if $level >= 4}
      <a href="{$header_path_modifier}translate/index.php">{t}Translation{/t}</a>
    {/if}
    {if $level >= 3}
      <a href="{$header_path_modifier}consultations/index.php">{t}Consultations{/t}</a>
    {/if}
    {if $level >= 2}
      <a href="{$header_path_modifier}members/index.php">{t}Members{/t}</a>
    {/if}
    <a href="{$header_path_modifier}help/index.php">{t}Help{/t}</a>
    </td>
    <td align="right">
    {if $user == ""}
      { if $logging_in != true }
        <a href="{$header_path_modifier}session/login.php">{t}Login{/t}</a>
      {/if}
    {else}
      {if $level >= 2}
        <form action="{$header_path_modifier}search/search.php" method="get" name="search" id="search">
          <input name="q" type="text" value="{$query}"/>
          <input type="submit" value="{t}Search{/t}" onClick="this.value = '{t}Searching...{/t}'; return true;" />
        </form>
      {/if}
      <a href="{$header_path_modifier}user/index.php">{$user}</a>
    {/if}
    </td>
  </tr>
</table>
