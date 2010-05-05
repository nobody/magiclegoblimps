<?php
include('config.php');

$connect = mysql_connect(DBHOST, DBUSER, DBPASS) or die("Error connecting to database.");
mysql_select_db(DBNAME);
?>