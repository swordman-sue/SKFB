<?php /* Smarty version 2.6.26, created on 2016-07-18 21:46:26
         compiled from main_default/erl_server.html */ ?>
<?php echo $this->_tpl_vars['erl_head']; ?>

<?php $all_arr = array(); ?>
<?php $_from = $this->_tpl_vars['erl_server_num']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['key_num'] => $this->_tpl_vars['num']):
?>
<?php $_from = $this->_tpl_vars['erl_server'][$this->_tpl_vars['num']]; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['key'] => $this->_tpl_vars['val']):
?>
<?php 
    $get_arr = array();
    foreach($this->_tpl_vars['erl_format']['get'] as $v)
    {
        $get_arr[] = $this->_tpl_vars['val'][$v];
    }
    
    $all_arr[] = (count($get_arr) == 1) ? $get_arr[0] : $get_arr;
    
    $this->_tpl_vars['get_str'] = implode(",", $get_arr);
 ?>
get(<?php echo $this->_tpl_vars['get_str']; ?>
) -> #<?php echo $this->_tpl_vars['erl_format']['fun']; ?>
{
<?php $this->_tpl_vars['i'] = 1; ?>
<?php $_from = $this->_tpl_vars['val']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k'] => $this->_tpl_vars['v']):
?>
<?php if ($this->_tpl_vars['i'] > 1): ?>
<?php if (is_numeric ( $this->_tpl_vars['v'] ) || $this->_tpl_vars['v'] == 'true' || $this->_tpl_vars['v'] == 'false' || $this->_tpl_vars['v'] == '[]'): ?>
    ,<?php echo $this->_tpl_vars['k']; ?>
 = <?php echo $this->_tpl_vars['v']; ?>

<?php elseif (is_array ( $this->_tpl_vars['v'] )): ?>
    ,<?php echo $this->_tpl_vars['k']; ?>
 = <?php echo dispose_erl_array($this->_tpl_vars['v'], 1);echo "\n"; ?>
<?php else: ?>
    ,<?php echo $this->_tpl_vars['k']; ?>
 = <?php if ($this->_tpl_vars['v'] == 'PHP_EMPTY'): ?><?php else: ?><?php echo $this->_tpl_vars['v']; ?>
<?php endif; ?>

<?php endif; ?>
<?php else: ?>
<?php if (is_numeric ( $this->_tpl_vars['v'] ) || $this->_tpl_vars['v'] == 'true' || $this->_tpl_vars['v'] == 'false' || $this->_tpl_vars['v'] == '[]'): ?>
    <?php echo $this->_tpl_vars['k']; ?>
 = <?php echo $this->_tpl_vars['v']; ?>

<?php elseif (is_array ( $this->_tpl_vars['v'] )): ?>
    <?php echo $this->_tpl_vars['k']; ?>
 = <?php echo dispose_erl_array($this->_tpl_vars['v'], 1); ?>
<?php else: ?>
    ,<?php echo $this->_tpl_vars['k']; ?>
 = <?php if ($this->_tpl_vars['v'] == 'PHP_EMPTY'): ?><?php else: ?><?php echo $this->_tpl_vars['v']; ?>
<?php endif; ?>

<?php endif; ?>
<?php endif; ?>
<?php $this->_tpl_vars['i']++; ?>
<?php endforeach; endif; unset($_from); ?>
};
<?php endforeach; endif; unset($_from); ?>
<?php endforeach; endif; unset($_from); ?>

<?php echo $this->_tpl_vars['erl_end']; ?>


all() -> <?php echo dispose_erl_array($all_arr, 1); ?>.
