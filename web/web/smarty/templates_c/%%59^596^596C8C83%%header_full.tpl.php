<?php /* Smarty version 2.6.26, created on 2010-06-18 18:11:58
         compiled from ../assets/header_full.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('block', 't', '../assets/header_full.tpl', 6, false),)), $this); ?>
<table width="100%">
  <tr>
    <td>
        <?php if ($this->_tpl_vars['level'] >= 6): ?>
      <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
administration/index.php"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Administration<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?></a>
    <?php endif; ?>
        <?php if ($this->_tpl_vars['level'] >= 5): ?>
      <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
manage/index.php"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Management<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?></a>
    <?php endif; ?>
        <?php if ($this->_tpl_vars['level'] >= 4): ?>
      <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
translate/index.php"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Translation<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?></a>
    <?php endif; ?>
    <?php if ($this->_tpl_vars['level'] >= 3): ?>
      <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
consultations/index.php"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Consultations<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?></a>
    <?php endif; ?>
    <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
help/index.php"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Help<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?></a>
        <img border="0" src="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
timer/index.php" width="0" height="0" />
    </td>
    <td align="right">
    <?php if ($this->_tpl_vars['user'] == ""): ?>
      <?php if ($this->_tpl_vars['logging_in'] != true): ?>
        <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
session/login.php"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Login<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?></a>
      <?php endif; ?>
    <?php else: ?>
        <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
user/index.php"><?php echo $this->_tpl_vars['user']; ?>
</a>
        |
        <a href="<?php echo $this->_tpl_vars['header_path_modifier']; ?>
session/logout.php"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Logout<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?></a>
    <?php endif; ?>
    </td>
  </tr>
</table>