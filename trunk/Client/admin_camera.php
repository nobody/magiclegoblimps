<?php
			session_start();
			if($_SESSION['userlevel'] != 2)
			{
				header("Location: http://tinyurl.com/zoocam");
                exit();
			}
			
            include "includes/config.php";
            include "includes/opendb.php";
            
            $query = "SELECT * FROM `cameras` LIMIT 1";
			$result = mysql_query($query);
			if(mysql_num_rows($result) != 0) {
			
			    $cameraID= mysql_result($result,0,"camera_id");
			    $cameraType= mysql_result($result,0,"camera_type");
			    $cameraCurrX= mysql_result($result,0,"camera_curr_x");
			    $cameraCurrY= mysql_result($result,0,"camera_curr_y");
			    $cameraCurrObject= mysql_result($result,0,"camera_curr_object");
			    $cameraResolution= mysql_result($result,0,"camera_resolution");
			    $cameraFPS= mysql_result($result,0,"camera_fps");
			    $cameraIP= mysql_result($result,0,"camera_ip");
			    $cameraFeed= mysql_result($result,0,"camera_feed");
			}
   		    
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <title>Zoocam Admin - Cameras</title>
        <script type="text/javascript" src="js/jquery.js"></script>
        <script type="text/javascript" src="js/update_animal_info.js"></script>
        <script type="text/javascript" src="js/flowplayer.js"></script>
        <script type="text/javascript" src="js/mapfetch.js"></script>
        <script type="text/javascript" src="js/robot_controller_post.js"></script>      
        <script type="text/javascript" src="js/update_camera_info.js"></script>
        <script type="text/javascript" src="js/videoControls.js"></script>
        <link href="style.css" rel="stylesheet" type="text/css" />
        <link rel="icon" type="image/png" href="/images/favicon.png"></link>
<style type="text/css">
#map {
    background-image: url('images/animal subpage/island_map.png');
    width: 472px;
    height: 336px;
    margin-left: 20px;
    float: left;
    text-align: left;
    position: relative;
	margin-top: 0px;
}
</style>
    </head>
    <body onload="start_map(2);">
        

        <div id="main_div">
            <?php 
                $nav_items = array("home_nav" => "index.php", "users_nav"=> "admin_users.php" , "cameras_nav" => "admin_camera.php", "animal_nav" => "animals.php");
                $nomonkey = True;
                include "includes/header.php";
            ?>
            <div id="default_content_div">
                <div id="admin_sub_nav"><a href="admin_animals.php"></a></div>
            </div>
            <div id="live_feed">
                <h1>Camera <span id="camera_id"><?php echo $cameraID;?></span> Detail</h1>
                <a href="<?=$cameraFeed?>" id="player" style="display:block;width:413px;height:249px;"></a>
                <script language="JavaScript">flowplayer("player", "flowplayer/flowplayer-3.1.5.swf");</script> 
            </div>
            <div id="map"><div id="map_container"></div></div>
            <div id="admin_camera_controls">
                <h1>Camera Controls</h1>
                <div id="zoom_control">
                <div id="zoom_in" onclick="VideoZoomIn()"></div>
                <div id="zoom_out" onclick="VideoZoomOut()"></div>
                </div>
                <div id="tilt_control">
                <div id="tilt_up" onclick="VideoTiltUp()"></div>
                <div id="tilt_down" onclick="VideoTiltDown()"></div>
                </div>
                <div id="pan_control">
                <div id="pan_left"  onclick='sendRobotCommands(4,$("#camera_id").html(),1)'></div>
                <div id="pan_right" onclick='sendRobotCommands(4,$("#camera_id").html(),-1)'></div>
                </div>
            </div>
            <div id="admin_robot_controls">
                <h1>Robot Controls</h1>
                <div id="robot_up_arrow" onclick='sendRobotCommands(1,$("#camera_id").html(),0)'></div>
                <div id="robot_left_arrow" onclick='sendRobotCommands(2,$("#camera_id").html(),0)'> </div>
                <div style="float:left;"><img src="images/robot.png" /></div>
                <div id="robot_right_arrow" onclick='sendRobotCommands(3,$("#camera_id").html(),0)' > </div>
                <span id="submit_login" class="button_wrapper">
                <a href="#" onclick='sendRobotCommands(6,$("#camera_id").html(),0)'>
                    <span>Release</span>
                </a>
            </span>
            </div>
            <div id="admin_camera_statistics">
                <h1>Camera <span id="camera_ids"><?php echo $cameraID;?></span> Statistics</h1>
                <b>Grid Section: </b> <span id="grid_coords"><?php echo "(".$cameraCurrX.", ".$cameraCurrY.")";?></span> <br />
                <b>Last Object Viewed: </b> <span id="last_obj_viewed"><?php $cameraCurrObject;?></span><br />
                <b>Resolution: </b><span id="camera_resolution"><?php echo $cameraResolution;?></span><br />
                <b>FPS: </b><span id="camera_fps"><?php echo $cameraFPS;?></span><br />
                <b>Camera Type: </b> <span id="camera_type"><?php echo $cameraType?></span><br />
                <b>Camera IP: </b> <span id="ip_address"><?php echo $cameraIP;?></span><br />
                
            </div>
            <?php include "includes/footer.php" ?>
        </div>
        <?php include "includes/closedb.php" ?>
    </body>
</html>
