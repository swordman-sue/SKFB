<?php /* Smarty version 2.6.26, created on 2014-06-26 14:43:48
         compiled from main_default/hrl_server.html */ ?>
-ifdef(MACRO_<?php echo $this->_tpl_vars['erl_format']['fun']; ?>
).
-else.
-define(MACRO_<?php echo $this->_tpl_vars['erl_format']['fun']; ?>
, 1).
-record(<?php echo $this->_tpl_vars['erl_format']['fun']; ?>
, {
<?php $this->_tpl_vars['i'] = 1; ?>
<?php $_from = $this->_tpl_vars['erl_server_num']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['key_num'] => $this->_tpl_vars['num']):
?>
<?php $_from = $this->_tpl_vars['hrl_server'][$this->_tpl_vars['num']]; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['key'] => $this->_tpl_vars['val']):
?>
<?php if ($this->_tpl_vars['i'] == 1): ?>
    <?php echo $this->_tpl_vars['val']; ?>

<?php else: ?>
    ,<?php echo $this->_tpl_vars['val']; ?>

<?php endif; ?>
<?php $this->_tpl_vars['i']++; ?>
<?php endforeach; endif; unset($_from); ?>
<?php endforeach; endif; unset($_from); ?>
}).
-endif.