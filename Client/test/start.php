<?php
ignore_user_abort(true);
set_time_limit(0);

//$handle = popen("/usr/bin/python /var/www/test/simulation.py", "r")
//echo $handle;
if(exec('ps -Al | grep python -c') > 0){
    echo "Simulation already running";
    header('Location: ../admin_demo.php');
    die(1);
}
$proc = proc_open("/usr/bin/python /var/www/test/simulation.py",
	array(
		array("pipe", "r"),
		array("pipe", "w"),
		array("pipe", "w")
	),
	$pipes);

header('Location: ../admin_demo.php');	
#echo stream_get_contents($pipes[1]);

?>
