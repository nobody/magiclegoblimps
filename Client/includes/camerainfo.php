<?php  
//This file is called from the admin camera page using ajax whenever a new animal is
//selected to update the page with information specefic to that animal

include "config.php";
include "opendb.php";

$id = $_GET["id"];
$query = "SELECT * FROM cameras WHERE camera_id=".$id;

$result = mysql_query($query);
if(mysql_num_rows($result) == 0) {
    die();
}

$row = mysql_fetch_array($result, MYSQL_ASSOC);

//animal will be in JSON format (a string representation of an object that can
//be parsed by javascript
echo json_encode($row);

include "closedb.php";
?>
