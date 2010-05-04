<?php 
require('includes/session.php'); 
$robot = $_GET["robot"];
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>ZooCam Mobile</title>
<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=UTF-8" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Cache-Control" content="no-cache" />
<meta name="HandheldFriendly" content="true" />

<link href="mobile.css" rel="stylesheet" type="text/css" />
<meta name="viewport" content="width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=0" />
<meta name="viewport" content="width=device-width; initial-scale=1.0; maximum-scale=1; minimum-scale=1; user-scalable=0;" />
</head>

<body>
    <div id="main">
<?
include "includes/config.php";
include "includes/opendb.php";
if($_SESSION['username'] == GUEST_NAME){
    if($_GET["loginfail"] == 1)
        $error = "<font color=\"red\">Incorrect username/password.</font>";
    echo <<<BLOCK
    <div class="header">
        <span><b>Please login to control robots</b></span>
    </div>
    <div id="robot">
		<form name="test" id="test" method="POST" action="includes/process.php">    
        <input type="hidden" name="redirect" value="/mobile.php" />
		<div id="login_block">
		    <div id="block_middle">
                $error <br />
            <div id="login_label">E-mail:</div>
		    <input id="email" name="email1" type="email" size="18"/> <br />
            <div id="login_label">Password:</div>
		    <input id="password" name="password1" type="password" size="18" autocomplete="off"
 /> <br/>   
            <div id=\"submit_login\" class=\"button_wrapper\">
                <input class="button_wrapper" type="submit" name="Submit" value="Login"><br />
            </div>
		    </div>
		</div>
		</form>
    </div>
BLOCK;
}else if($_SESSION['userlevel'] == 2){ // ADMIN AND LOGGED IN
    echo"<script language=\"Javascript\" type=\"text/javascript\"> setTimeout(\"location.reload(true);\", 10*1000); </script>";
    if($robot != 0){ // ROBOT SELECTED
        $query = "SELECT camera_id FROM `cameras` WHERE camera_id=".$robot." AND camera_curr_x >= 0  AND camera_curr_y >= 0 LIMIT 1";
    	$result = mysql_query($query);
    	if(mysql_num_rows($result) != 0) {
            $online = true;
            $robot_status = "<span id=\"status\" style=\"color:green\">Robot $robot is online.</span>";
        }else{
            $online = false;
            $robot_status = "<span id=\"status\" style=\"color:red\">Robot $robot is offline.</span>";
        }
        echo <<<BLOCK
    <div class="header">
        <span class="button_wrapper"> <a href="mobile.php"> <span>Robots</span> </a> </span>
        <span class="button_wrapper"> <a href="includes/logout.php?redirect=/mobile.php"> <span>Logout</span> </a> </span>
    </div>
BLOCK;
        if($online){
            echo <<<BLOCK
    <div id="robot">
        <div id="admin_robot_controls">
            <a href="includes/robot_controller.php?robot=$robot&action=1&redirect=1"><div id="robot_up_arrow"></div></a>
            <a href="includes/robot_controller.php?robot=$robot&action=2&redirect=1"><div id="robot_left_arrow"> </div></a>
            <div style="float:left;"><img src="images/robot.png" alt="Robot Control" /></div>
            <a href="includes/robot_controller.php?robot=$robot&action=3&redirect=1"><div id="robot_right_arrow"> </div></a>
            <div id="pan_control">
                <a href="includes/robot_controller.php?robot=$robot&action=4&degrees=10&redirect=1"><div id="pan_left"></div></a>
                <a href="includes/robot_controller.php?robot=$robot&action=4&degrees=-10&redirect=1"><div id="pan_right"></div></a>
            </div>
            <span class="button_wrapper"> <a href="includes/robot_controller.php?robot=$robot&action=6&redirect=1"> <span>Release Control</span> </a> </span>
        </div>
        Status: $robot_status
    </div>
BLOCK;
        }else{ // ROBOT IS OFFLINE!!
            echo <<<BLOCK
    <div id="robot">
        Status: $robot_status
    </div>
BLOCK;
        }   
    }else{ // NO ROBOT SELECTED, SHOW LIST
        $robotlist = "";
        $query = "SELECT camera_id, camera_curr_x, camera_curr_y FROM cameras";
        $result = mysql_query($query);
        if(mysql_numrows($result) == 0)
            $robot_status = "<div id=\"status\" style=\"color:red\">All robots are offline.</div>";
        else
            $robot_status = "<div id=\"status\">Choose a robot to control.</div>";
        while($row = mysql_fetch_array($result, MYSQL_ASSOC)){
            $robotid = $row['camera_id'];
            $x = $row['camera_curr_x'];
            $y = $row['camera_curr_y'];
            if($x < 0 || $y < 0){
                continue;
            }
            $robotlist .= "<a href=\"mobile.php?robot=$robotid\"><li>Robot $robotid - location: ($x, $y)</li></a>\n";
        }
        echo <<<BLOCK
    <div class="header">
      <span class="button_wrapper"> <a href="mobile.php"> <span>Refresh</span> </a> </span>
      <span class="button_wrapper"> <a href="includes/logout.php?redirect=/mobile.php"> <span>Logout</span> </a> </span>
    </div>
        $robot_status
        <div id="robot">
            <ul id="robotlist">
                $robotlist
            </ul>
        </div>
BLOCK;
    }
}elseif($_SESSION['userlevel'] != 2){ // NOT ADMIN
    echo "You do not have the user level to control robots. <a href=\"includes/logout.php?redirect=/mobile.php\">Logout</a>";
}else{ // NOT LOGGED IN
        echo "If you see this message you are a magical fairy that can defy logic. You win the game.";
}

include "includes/closedb.php";
?>
    </div>
</body>
</html>
