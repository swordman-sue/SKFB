<?php

$_cfg = array();
if ($handle = opendir(EXCEL_PATH)) {
    while ( false !== ( $item = readdir( $handle ) ) ) {
        if (!strpos($item, '.xml')) {
            continue;
        }
        $item = basename($item, ".xml");
		if(PHP_OS == 'Linux')
		{
			// linux
		}
		else
		{
			$item = iconv('GBK', 'UTF-8', $item);
		}
		$_cfg[] = $item;
    }
    closedir($handle);
}
sort($_cfg);
?>
