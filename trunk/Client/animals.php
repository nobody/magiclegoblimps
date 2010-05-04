<?php include('includes/session.php'); ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <title>Zoocam - Animals</title>
        <script type="text/javascript" src="js/jquery.js"></script>
        <script type="text/javascript" src="js/flowplayer.js"></script>
        <script type="text/javascript" src="js/update_animal_info.js"></script>
        <script type="text/javascript" src="js/mapfetch.js"></script>
		<script type="text/javascript" src="js/videoControls.js"></script>          
        <link href="style.css" rel="stylesheet" type="text/css" />
        <link rel="icon" type="image/png" href="/images/favicon.png" />
<style type="text/css">

#map {
	background: url('images/island_map_wo_grid.png') no-repeat;
	width: 472px;
	height: 336px;
	z-index:1000;
    margin: auto;
    position: relative;
    float:right;
    margin-top: 75px;
}

#animalCameraControl {

	float: left;
	margin-top:25px;
    text-align: center;
    margin-left: 150px;
  	

}

</style> 
<script type= "text/javascript">/*<![CDATA[*/
$(function(){
	//Get our elements for faster access and set overlay width
	var div = $('div.animal_bar_container'),
		ul = $('ul.animal_bar_container'),
		ulPadding = 30;
	
	//Get menu width
	var divWidth = div.width();
 
	//Remove scrollbars	
	div.css({overflow: 'hidden'});
	
	//Find last image container
	var lastLi = ul.find('li:last-child');
	
	//When user move mouse over menu
	div.mousemove(function(e){
		//As images are loaded ul width increases,
		//so we recalculate it each time
		var ulWidth = lastLi[0].offsetLeft + lastLi.outerWidth() + ulPadding;	
		var left = (e.pageX - div.offset().left) * (ulWidth-divWidth) / divWidth;
		div.scrollLeft(left);
        //div.animate({ scrollLeft: 200 }, "slow");
	});
        
        //animate font change of animal bar
        var names =  $("div.animal_bar span")
        names.mouseover(function(){
           $(this).animate({fontSize: "32px"}, 150);
        });
        names.mouseout(function(){
            $(this).animate({fontSize: "24px"}, 150)
        });
});
/*]]>*/</script> 
    </head>
    <body onload="start_map(1);">
        <?php
            include "includes/config.php";
            include "includes/opendb.php";

            startSession();

            //check the query string for the animal to display at first, if nothing provided the monkey is displayed
            $id = $_GET["id"];
            if($id)
                echo "<script type=\"text/javascript\">updateInfo(".$id.");</script>";
            else
                echo "<script type=\"text/javascript\">updateInfo(6);</script>";
        ?>

        <div id="main_div">
            <?php
                if($_SESSION['userlevel']==ADMIN_LEVEL) {
                    $nomonkey=true;
                    $nav_items = array("home_nav" => "index.php", "users_nav"=> "admin_users.php", "cameras_nav" => "admin_camera.php", "animal_nav" => "animals.php");
                }
                else
                    $nav_items = array("home_nav" => "index.php", "animal_nav" => "animals.php");

                include "includes/header.php";
            ?>
            <div id="default_content_div">
                <?php
                if($_SESSION['userlevel']==ADMIN_LEVEL) {
                   echo "<div id='admin_sub_nav'>
                        <a id='edit_animal' href='admin_animals.php?mode=edit'>Edit an Animal</a>
                        <a id='add_animal' href='admin_animals.php?mode=add'>Add an Animal</a>
                        <a id='view_animal_stats' href='admin_stats_animals.php'>View Statistics</a>
                        </div>";
                }
                ?>
                <a name="animal_bar"></a>
                <div class="animal_bar">
                    <div class="animal_bar_container">
	                    <ul class="animal_bar_container">
                            <?php
                            $query = "SELECT animal_id, animal_name, animal_description FROM animals ORDER BY animal_name";
                            $result = mysql_query($query);
                            while($row = mysql_fetch_array($result, MYSQL_ASSOC)){
								if($_SESSION['userlevel'] == 2)
                                {
									echo "<li><a onclick=\"updateInfo(".$row["animal_id"].")\"><span>".$row["animal_name"]."</span></a></li>";
								}
								else if($row["animal_description"] != NULL)
								{
									echo "<li><a onclick=\"updateInfo(".$row["animal_id"].")\"><span>".$row["animal_name"]."</span></a></li>";
								}
                            }
                            ?>
	                    </ul>
                    </div>
                </div>
                <div id="animal_info">
                    <div id="animal_info_top"></div>
                    <div id="animal_info_middle">
                        <h1 id="animal_info_title">
                        </h1>
                        <br />
                        <img id="animal_image"/>
                        Kingdom:&nbsp;<span id="kingdom"></span> <br />
                        Phylum:&nbsp;<span id="phylum"></span> <br />
                        Subphylum:&nbsp;<span id="subphylum"></span> <br />
                        Class:&nbsp;<span id="class"></span> <br />
                        Order:&nbsp;<span id="order"></span> <br />
                        Suborder:&nbsp;<span id="suborder"></span> <br /> <br />
                        <p id="animal_summary"></p>
                    </div>
                    <div id="animal_info_bottom"></div>
                </div>
                <div id="yes_camera">
                    
                    <div id="videoplayer">
                        <div id="video_text" style="text-align: center"></div>
                        <a href="" id="player" style="display:block;width:400px;height:300px;" style="display:block;"></a>
                    </div>
					<?php 
						if($_SESSION['userlevel'] == 1 || $_SESSION['userlevel'] == 2)
						{
//							echo "<div id=\"animalCameraControls\">";
							echo "<div id=\"animalCameraControl\">";
								echo "<h1>Camera Controls</h1>";
								echo "<div id=\"zoom_control\">";
								echo "<div id=\"zoom_in\" onclick=\"VideoZoomIn()\"></div>";
								echo "<div id=\"zoom_out\" onclick=\"VideoZoomOut()\"></div>";
								echo "</div>";
								echo "<div id=\"tilt_control\">";
								echo "<div id=\"tilt_up\" onclick=\"VideoTiltUp()\"></div>";
								echo "<div id=\"tilt_down\" onclick=\"VideoTiltDown()\"></div>";
								echo "</div>";
							echo "</div>";
//							echo "</div>";
						}
					?>
                </div>
                <div id="no_camera">
                    <div id="vote_div">
                        <div id="vote_text">
                            <span id="vote_text_header"style="font: 10pt arial;">Our cameras are controlled through user votes.</span>
                            <br />
                            <span id="vote_text_detail" style="font: 8pt arial; color: gray;">If you don't see a camera near your favorite animal,<br />cast your vote to help change camera locations.</span>
                        </div>
                        <br /> <br />
                        <span id="vote_button" class="button_wrapper" onclick="startVote();">
                            <a href="#">
                                <span id="vote_button_text"></span>
                            </a>
                        </span>
                    </div>
                </div>
                <div id="map"><div id="map_container"></div></div>
            </div>
            <?php include "includes/footer.php" ?>
        </div>
        <?php include "includes/closedb.php" ?>
    </body>
</html>  
