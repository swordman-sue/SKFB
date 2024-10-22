<?php /* Smarty version 2.6.26, created on 2014-06-03 21:10:07
         compiled from main_default/client_ui.html */ ?>
<?php if (! empty ( $this->_tpl_vars['ui_client'] )): ?>

GameConfig = GameConfig or {}
GameConfig.<?php echo $this->_tpl_vars['cnf_name']; ?>
=
{
<?php $this->_tpl_vars['i'] = 1; ?>
<?php $_from = $this->_tpl_vars['ui_client_num']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['key_num'] => $this->_tpl_vars['num']):
?>
<?php 
    $special_return = dispose_cnf_data($this->_tpl_vars['ui_client'][$this->_tpl_vars['num']], $this->_tpl_vars['cnf_special']);
    $this->_tpl_vars['special_type'] = $special_return['type'];
    $this->_tpl_vars['special_extra'] = $special_return['extra'];
 ?>
<?php if ($this->_tpl_vars['special_type'] == 1): ?>
<?php $_from = $this->_tpl_vars['ui_client'][$this->_tpl_vars['num']]; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['key'] => $this->_tpl_vars['val']):
?>
    [<?php echo $this->_tpl_vars['key']; ?>
] = {
<?php $_from = $this->_tpl_vars['val']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['kk'] => $this->_tpl_vars['vv']):
?>
<?php if ($this->_tpl_vars['special_extra']['s_count'] == 3): ?>
<?php if (! is_numeric ( $this->_tpl_vars['kk'] )): ?>
<?php if ($this->_tpl_vars['vv'] != ""): ?>
<?php $this->_tpl_vars['type'] = dispose_cnf_type($this->_tpl_vars['vv']); ?>
<?php if ($this->_tpl_vars['type'] == 1): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = <?php echo $this->_tpl_vars['vv']; ?>
,
<?php elseif ($this->_tpl_vars['type'] == 2): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = <?php if ($this->_tpl_vars['vv'] == 'PHP_EMPTY'): ?>[[]]<?php else: ?>[[<?php echo $this->_tpl_vars['vv']; ?>
]]<?php endif; ?>,
<?php elseif ($this->_tpl_vars['type'] == 3): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = {<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?><?php echo $this->_tpl_vars['v2']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php elseif ($this->_tpl_vars['type'] == 4): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = {
<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
            {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?><?php echo $this->_tpl_vars['v3']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
        },
<?php elseif ($this->_tpl_vars['type'] == 5): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = {
<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
            {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?>{<?php $_from = $this->_tpl_vars['v3']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k4'] => $this->_tpl_vars['v4']):
?><?php echo $this->_tpl_vars['v4']; ?>
, <?php endforeach; endif; unset($_from); ?>}, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
        },
<?php endif; ?>
<?php endif; ?>
<?php else: ?>
        [<?php echo $this->_tpl_vars['kk']; ?>
] = {
<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['kkk'] => $this->_tpl_vars['vvv']):
?>
<?php if ($this->_tpl_vars['kkk'] != 'LIST' && $this->_tpl_vars['vvv'] != ""): ?>
<?php $this->_tpl_vars['type'] = dispose_cnf_type($this->_tpl_vars['vvv']); ?>
<?php if ($this->_tpl_vars['type'] == 1): ?>
            <?php echo $this->_tpl_vars['kkk']; ?>
 = <?php echo $this->_tpl_vars['vvv']; ?>
,
<?php elseif ($this->_tpl_vars['type'] == 2): ?>
            <?php echo $this->_tpl_vars['kkk']; ?>
 = <?php if ($this->_tpl_vars['vv'] == 'PHP_EMPTY'): ?>[[]]<?php else: ?>[[<?php echo $this->_tpl_vars['vvv']; ?>
]]<?php endif; ?>,
<?php elseif ($this->_tpl_vars['type'] == 3): ?>
            <?php echo $this->_tpl_vars['kkk']; ?>
 = {<?php $_from = $this->_tpl_vars['vvv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?><?php echo $this->_tpl_vars['v2']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php elseif ($this->_tpl_vars['type'] == 4): ?>
            <?php echo $this->_tpl_vars['kkk']; ?>
 = {
<?php $_from = $this->_tpl_vars['vvv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
                {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?><?php echo $this->_tpl_vars['v3']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
            },
<?php elseif ($this->_tpl_vars['type'] == 5): ?>
            <?php echo $this->_tpl_vars['kkk']; ?>
 = {
<?php $_from = $this->_tpl_vars['vvv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
                {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?>{<?php $_from = $this->_tpl_vars['v3']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k4'] => $this->_tpl_vars['v4']):
?><?php echo $this->_tpl_vars['v4']; ?>
, <?php endforeach; endif; unset($_from); ?>}, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
            },
<?php endif; ?>
<?php else: ?>
<?php if ($this->_tpl_vars['cnf_special']['extra'] != ""): ?>
            <?php echo $this->_tpl_vars['cnf_special']['extra']; ?>
 = {
<?php endif; ?>
<?php $_from = $this->_tpl_vars['vvv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['l_key'] => $this->_tpl_vars['l_val']):
?>
                [<?php echo $this->_tpl_vars['l_key']; ?>
] = {
<?php $_from = $this->_tpl_vars['l_val']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k'] => $this->_tpl_vars['v']):
?>
<?php if ($this->_tpl_vars['v'] != ""): ?>
<?php $this->_tpl_vars['type'] = dispose_cnf_type($this->_tpl_vars['v']); ?>
<?php if ($this->_tpl_vars['type'] == 1): ?>
                    <?php echo $this->_tpl_vars['k']; ?>
 = <?php echo $this->_tpl_vars['v']; ?>
,
<?php elseif ($this->_tpl_vars['type'] == 2): ?>
                    <?php echo $this->_tpl_vars['k']; ?>
 = <?php if ($this->_tpl_vars['v'] == 'PHP_EMPTY'): ?>[[]]<?php else: ?>[[<?php echo $this->_tpl_vars['v']; ?>
]]<?php endif; ?>,
<?php elseif ($this->_tpl_vars['type'] == 3): ?>
                    <?php echo $this->_tpl_vars['k']; ?>
 = {<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?><?php echo $this->_tpl_vars['v2']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php elseif ($this->_tpl_vars['type'] == 4): ?>
                    <?php echo $this->_tpl_vars['k']; ?>
 = {
<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
                        {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?><?php echo $this->_tpl_vars['v3']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
                    },
<?php elseif ($this->_tpl_vars['type'] == 5): ?>
                    <?php echo $this->_tpl_vars['k']; ?>
 = {
<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
                        {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?>{<?php $_from = $this->_tpl_vars['v3']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k4'] => $this->_tpl_vars['v4']):
?><?php echo $this->_tpl_vars['v4']; ?>
, <?php endforeach; endif; unset($_from); ?>}, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
                    },
<?php endif; ?>
<?php endif; ?>
<?php endforeach; endif; unset($_from); ?>
                },
<?php endforeach; endif; unset($_from); ?>
<?php if ($this->_tpl_vars['cnf_special']['extra'] != ""): ?>
            },
<?php endif; ?>
<?php endif; ?>
<?php endforeach; endif; unset($_from); ?>
        },
<?php endif; ?>
<?php endif; ?>
<?php if ($this->_tpl_vars['special_extra']['s_count'] == 2): ?>
<?php if ($this->_tpl_vars['kk'] != 'LIST' && $this->_tpl_vars['vv'] != ""): ?>
<?php $this->_tpl_vars['type'] = dispose_cnf_type($this->_tpl_vars['vv']); ?>
<?php if ($this->_tpl_vars['type'] == 1): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = <?php echo $this->_tpl_vars['vv']; ?>
,
<?php elseif ($this->_tpl_vars['type'] == 2): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = <?php if ($this->_tpl_vars['vv'] == 'PHP_EMPTY'): ?>[[]]<?php else: ?>[[<?php echo $this->_tpl_vars['vv']; ?>
]]<?php endif; ?>,
<?php elseif ($this->_tpl_vars['type'] == 3): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = {<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?><?php echo $this->_tpl_vars['v2']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php elseif ($this->_tpl_vars['type'] == 4): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = {
<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
            {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?><?php echo $this->_tpl_vars['v3']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
        },
<?php elseif ($this->_tpl_vars['type'] == 5): ?>
        <?php echo $this->_tpl_vars['kk']; ?>
 = {
<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
            {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?>{<?php $_from = $this->_tpl_vars['v3']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k4'] => $this->_tpl_vars['v4']):
?><?php echo $this->_tpl_vars['v4']; ?>
, <?php endforeach; endif; unset($_from); ?>}, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
        },
<?php endif; ?>
<?php else: ?>
<?php if ($this->_tpl_vars['cnf_special']['extra'] != ""): ?>
        <?php echo $this->_tpl_vars['cnf_special']['extra']; ?>
 = {
<?php endif; ?>
<?php $_from = $this->_tpl_vars['vv']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['l_key'] => $this->_tpl_vars['l_val']):
?>
            [<?php echo $this->_tpl_vars['l_key']; ?>
] = {
<?php $_from = $this->_tpl_vars['l_val']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k'] => $this->_tpl_vars['v']):
?>
<?php if ($this->_tpl_vars['v'] != ""): ?>
<?php $this->_tpl_vars['type'] = dispose_cnf_type($this->_tpl_vars['v']); ?>
<?php if ($this->_tpl_vars['type'] == 1): ?>
                <?php echo $this->_tpl_vars['k']; ?>
 = <?php echo $this->_tpl_vars['v']; ?>
,
<?php elseif ($this->_tpl_vars['type'] == 2): ?>
                <?php echo $this->_tpl_vars['k']; ?>
 = <?php if ($this->_tpl_vars['v'] == 'PHP_EMPTY'): ?>[[]]<?php else: ?>[[<?php echo $this->_tpl_vars['v']; ?>
]]<?php endif; ?>,
<?php elseif ($this->_tpl_vars['type'] == 3): ?>
                <?php echo $this->_tpl_vars['k']; ?>
 = {<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?><?php echo $this->_tpl_vars['v2']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php elseif ($this->_tpl_vars['type'] == 4): ?>
                <?php echo $this->_tpl_vars['k']; ?>
 = {
<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
                    {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?><?php echo $this->_tpl_vars['v3']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
                },
<?php elseif ($this->_tpl_vars['type'] == 5): ?>
                <?php echo $this->_tpl_vars['k']; ?>
 = {
<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
                    {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?>{<?php $_from = $this->_tpl_vars['v3']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k4'] => $this->_tpl_vars['v4']):
?><?php echo $this->_tpl_vars['v4']; ?>
, <?php endforeach; endif; unset($_from); ?>}, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
                },
<?php endif; ?>
<?php endif; ?>
<?php endforeach; endif; unset($_from); ?>
            },
<?php endforeach; endif; unset($_from); ?>
<?php if ($this->_tpl_vars['cnf_special']['extra'] != ""): ?>
        },
<?php endif; ?>
<?php endif; ?>
<?php endif; ?>
<?php endforeach; endif; unset($_from); ?>
<?php $this->_tpl_vars['i']++; ?>
    },
<?php endforeach; endif; unset($_from); ?>
<?php else: ?>
<?php $_from = $this->_tpl_vars['ui_client'][$this->_tpl_vars['num']]; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['key'] => $this->_tpl_vars['val']):
?>
    [<?php if (isset ( $this->_tpl_vars['val']['id'] )): ?><?php echo $this->_tpl_vars['val']['id']; ?>
<?php else: ?><?php echo $this->_tpl_vars['i']; ?>
<?php endif; ?>] = {
<?php $_from = $this->_tpl_vars['val']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k'] => $this->_tpl_vars['v']):
?>
<?php if ($this->_tpl_vars['v'] != "" && ( ( $this->_tpl_vars['special_type'] == 2 && $this->_tpl_vars['k'] != 'id' ) || $this->_tpl_vars['special_type'] != 2 )): ?>
<?php $this->_tpl_vars['type'] = dispose_cnf_type($this->_tpl_vars['v']); ?>
<?php if ($this->_tpl_vars['type'] == 1): ?>
        <?php echo $this->_tpl_vars['k']; ?>
 = <?php echo $this->_tpl_vars['v']; ?>
,
<?php elseif ($this->_tpl_vars['type'] == 2): ?>
        <?php echo $this->_tpl_vars['k']; ?>
 = <?php if ($this->_tpl_vars['v'] == 'PHP_EMPTY'): ?>[[]]<?php else: ?>[[<?php echo $this->_tpl_vars['v']; ?>
]]<?php endif; ?>,
<?php elseif ($this->_tpl_vars['type'] == 3): ?>
        <?php echo $this->_tpl_vars['k']; ?>
 = {<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?><?php echo $this->_tpl_vars['v2']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php elseif ($this->_tpl_vars['type'] == 4): ?>
        <?php echo $this->_tpl_vars['k']; ?>
 = {
<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
            {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?><?php echo $this->_tpl_vars['v3']; ?>
, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
        },
<?php elseif ($this->_tpl_vars['type'] == 5): ?>
        <?php echo $this->_tpl_vars['k']; ?>
 = {
<?php $_from = $this->_tpl_vars['v']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k2'] => $this->_tpl_vars['v2']):
?>
            {<?php $_from = $this->_tpl_vars['v2']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k3'] => $this->_tpl_vars['v3']):
?>{<?php $_from = $this->_tpl_vars['v3']; if (!is_array($_from) && !is_object($_from)) { settype($_from, 'array'); }if (count($_from)):
    foreach ($_from as $this->_tpl_vars['k4'] => $this->_tpl_vars['v4']):
?><?php echo $this->_tpl_vars['v4']; ?>
, <?php endforeach; endif; unset($_from); ?>}, <?php endforeach; endif; unset($_from); ?>},
<?php endforeach; endif; unset($_from); ?>
        },
<?php endif; ?>
<?php endif; ?>
<?php endforeach; endif; unset($_from); ?>
<?php $this->_tpl_vars['i']++; ?>
    },
<?php endforeach; endif; unset($_from); ?>
<?php endif; ?>
<?php endforeach; endif; unset($_from); ?>
};
<?php endif; ?>
