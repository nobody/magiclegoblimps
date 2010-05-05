<?php
/*
// INPUT
$animalId = $_GET['id'];

Format for the json object:

// OUTPUT
    video type: 0=no video, 1=live feed, 2=archived footage
    array of video camera_ids that see the object (if any)
    array of video camera URLs (if any)

*/


include "config.php";
include "opendb.php";

$animalId = $_GET['id'];
$camera_id = -1;
$camera_feed = -1;

$query = "SELECT * FROM service WHERE service_object_id=".$animalId." ORDER BY service_qos DESC  LIMIT 1";
$result = mysql_query($query);
if(mysql_num_rows($result) == 0) {
    //check for archive
    $query = "SELECT * FROM archives WHERE archive_obj_id=".$animalId." ORDER BY archive_qos DESC LIMIT 1";
    $result = mysql_query($query);

    //no archive found
    if(mysql_num_rows($result) == 0)
        printJSON(0, $camera_id, $camera_feed);
    else {
        $row = mysql_fetch_array($result);
        printJSON(2, $row['archive_id'], $row['archive_url']);
    }
    exit();
}
$num = mysql_num_rows($result);
while($row = mysql_fetch_array($result, MYSQL_ASSOC)){
    $camera_id = $row['service_camera_id'];
    $camera_feed = getFeedFromCameraId($row['service_camera_id']);
}
printJSON($num, $camera_id, $camera_feed);

function getFeedFromCameraId($CameraId){
    $query = "SELECT * FROM cameras WHERE camera_id=".$CameraId." LIMIT 1";
    $result = mysql_query($query);
    if(mysql_num_rows($result) == 0) {
        printJSON(0, $CameraId, $camera_feed);
        exit();
    }
    $row = mysql_fetch_array($result, MYSQL_ASSOC);
    return $row['camera_feed'];
}

// print out info
function printJSON($num, $id, $feed){
    $jsonarray = Array();
    $jsonarray['num'] = $num;
    $jsonarray['camera_id'] = $id;
    $jsonarray['camera_feed'] = $feed;
    echo json_encode($jsonarray);
}

include "closedb.php";

?>
