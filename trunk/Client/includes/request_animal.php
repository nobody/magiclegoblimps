<?php
include "config.php";
include "opendb.php";

$id = $_GET["animalID"];
$query = "INSERT INTO requests SET request_animal=".$id.", request_ip='1.1.1.1', request_zone=1, request_time=NOW(); ";
mysql_query($query);
echo mysql_error();

$query = "UPDATE total_votes SET total_votes_num = total_votes_num + 1 WHERE total_votes_id=1; ";
mysql_query($query);
$query = "UPDATE animals SET animal_num_views = animal_num_views + 1 WHERE animal_id=".$id;
mysql_query($query);
echo mysql_error();

include "closedb.php";
?>
