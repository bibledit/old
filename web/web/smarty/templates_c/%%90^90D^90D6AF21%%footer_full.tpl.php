<?php /* Smarty version 2.6.26, created on 2010-04-01 15:45:33
         compiled from ../assets/footer_full.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('block', 't', '../assets/footer_full.tpl', 7, false),)), $this); ?>
<div align="center">
<p>
<?php 
include ("version.php");
$this->assign ("version", $version);
 ?>
<font size="-2"><?php $this->_tag_stack[] = array('t', array()); $_block_repeat=true;smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], null, $this, $_block_repeat);while ($_block_repeat) { ob_start(); ?>Powered by<?php $_block_content = ob_get_contents(); ob_end_clean(); $_block_repeat=false;echo smarty_translate($this->_tag_stack[count($this->_tag_stack)-1][1], $_block_content, $this, $_block_repeat); }  array_pop($this->_tag_stack); ?> <a href="http://bibledit.org">Bibledit-Web</a> <?php echo $this->_tpl_vars['version']; ?>
</font>
</p>
</div>