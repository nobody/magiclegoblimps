<?

include('config.php');
include('opendb.php');

// $page : 0=index.php, 1=animal.php, 2=admin_camera
$page = $_GET["page"];
$selected = $_GET["selected"];

$ypositions = array(29, 60, 101, 150, 213);
$xpositions = array(
        array( 104, 156, 207, 259, 311, 362 ),
        array( 93, 148, 205, 260, 317, 373 ),
        array( 77, 139, 202, 264, 325, 388 ),
        array( 59, 128, 198, 268, 337, 406 ),
        array( 35, 114, 193, 271, 351, 429 )
    );

$query = "SELECT * FROM `cameras`";
$result = mysql_query($query);
if(mysql_num_rows($result) == 0) {
    exit();
}
while($row = mysql_fetch_array($result, MYSQL_ASSOC)){
    if($row['camera_curr_x'] < 0 || $row['camera_curr_y'] < 0){
        continue;
    }
    if(time() - $row['camera_last_changed'] < 3)
        $animate = true;
    if($animate){
        $x_position = $row['camera_curr_x'];
        $y_position = 4 - $row['camera_curr_y'];
        drawMapItem($row,$xpositions, $ypositions, $page, $selected, $animate);
    }else{
        drawMapItem($row,$xpositions, $ypositions, $page, $selected, $animate);
    }
}


function drawMapItem($row, $xpositions, $ypositions, $page, $selected, $animate){
    
    if($animate){
        $x_position = $row['camera_last_x'];
        $y_position = 4 - $row['camera_last_y'];
    }else{
        $x_position = $row['camera_curr_x'];
        $y_position = 4 - $row['camera_curr_y'];
    }
    $x_map_position = $xpositions[$y_position][$x_position];
    $y_map_position = $ypositions[$y_position];
    
    
    echo "<div style=\"position:absolute; top:".($y_map_position-45)."px; left: ".($x_map_position-25)."px;\"";
    echo " id=\"camera_marker_".$row['camera_id']."\">";

    // if the camera just moved:
    if($animate){
        $x_position = $row['camera_curr_x'];
        $y_position = 4 - $row['camera_curr_y'];
        $x_map_position = $xpositions[$y_position][$x_position];
        $y_map_position = $ypositions[$y_position];
        ?>

<script type="text/javascript"> 
animateMap('camera_marker_<?=$row['camera_id']?>',
            <?=$x_map_position-25?>, <?=$y_map_position-45?>) 
</script>
<?
        
        setPosNotChanged($row['camera_id']);
    }

    switch($page){
    case 0:
        // index page:
        // show animals, no arrows
        // no link on camera
        // links of animals go to animal page with animals.php?id=$animal_id
        drawCameraMarker($row['camera_id'], '');
        drawSeenAnimals($row['camera_id']);
        break;
    case 1:        
        // animals page:
        // show animals, no arrows
        // no link on camera
        // links of animals update current animal
        drawCameraMarker($row['camera_id'], '');
        drawSeenAnimals($row['camera_id']);
        break;
    case 2:        
        // admin page:
        // show arrows
        // link on camera to update current camera
        // links of animals go to animal page with animals.php?id=$animal_id
        
        echo "<a onclick=\"updateInfo(".$row["camera_id"].")\">\n";
        if($selected == $row['camera_id'])
            drawCameraMarker($row['camera_id'], '');
        else
            drawCameraMarker($row['camera_id'], '_50');
        drawCameraArrow($row['camera_orientation']);
        echo "</a>";
        break;
    }
    echo "</div>\n";
    
    
}

function drawSeenAnimals($camera_id){
    $query = "SELECT * FROM `service` WHERE service_camera_id=$camera_id";
    $result = mysql_query($query);
    if(mysql_num_rows($result) == 0) {
        // NO ANIMALS :(
    }else{
        $offset = 0;
        $i = 0;
        while($i < 3 && $row = mysql_fetch_array($result, MYSQL_ASSOC)) {
            $animal_name = getAnimalNameFromId($row['service_object_id']);
            echo "<div style=\"position:absolute; top:".(-20-$offset)."px; left: ".(10)."px; z-index:1000;\">";
            echo "<a href=\"animals.php?id=".$row['service_object_id']."\"><img src=\"images/map_animals/".strtolower($animal_name).".png\" /></a>";
            echo "</div>\n";
            $offset += 28;
            $i++;
        }
    }
}

function drawCameraMarker($camera_id, $imagename){
	echo "<div id=\"camera_marker\" style=\"position:absolute; top:".(13)."px; left: ".(1)."px; background-image: url('images/CameraMarker".($camera_id).$imagename.".png')\"></div>\n";
	    
}

function getAnimalNameFromId($animal_id){
    if(!isset($animal_id))
        return "";
    $query = "SELECT animal_name FROM `animals` WHERE animal_id=$animal_id";
    $result = mysql_query($query);
    if(mysql_num_rows($result) == 0) {
        return;
    }
    $row = mysql_fetch_row($result);
    return $row[0];
}

function drawCameraArrow($camera_orientation){
    switch($camera_orientation){
        case 0:
            echo "<div style=\"position:absolute; top:".(0)."px; left: ".(17)."px;\"><img src=\"images/camera_direction_north.png\" /></div>\n";
            break;
        case 1:
            echo "<div style=\"position:absolute; top:".(20)."px; left: ".(37)."px;\"><img src=\"images/camera_direction_east.png\" /></div>\n";
            break;
        case 2:
            echo "<div style=\"position:absolute; top:".(40)."px; left: ".(17)."px;\"><img src=\"images/camera_direction_south.png\" /></div>\n";
            break;
        case 3:
            echo "<div style=\"position:absolute; top:".(25)."px; left: ".(0)."px;\"><img src=\"images/camera_direction_west.png\" /></div>\n";
            break;
    }
}

function setPosNotChanged($camera_id){
    $query = sprintf("UPDATE zoocam.cameras SET camera_pos_changed = '0' WHERE cameras.camera_id = %s LIMIT 1", mysql_real_escape_string($camera_id));
    $result = mysql_query($query);
}


include('closedb.php');

?>
