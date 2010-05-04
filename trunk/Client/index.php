<?php
	include('includes/session.php');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<!--<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">-->
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head> 
<title>Zoocam Home Page</title>
<link href="style.css" rel="stylesheet" type="text/css" />
<link rel="icon" type="image/png" href="/images/favicon.png" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<script type="text/javascript" src="js/jquery.js"></script>
<script type="text/javascript" src="js/twitterfetch.js"></script>
<script type="text/javascript" src="js/watermark_util.js"></script>
<script type="text/javascript" src="js/carousel.js"></script>
<script type="text/javascript" src="js/flowplayer.js"></script>
<script type="text/javascript" src="js/mapfetch.js"></script>
<script type="text/javascript">
$(document).ready(function() {
    setInterval('updateViewersChoice()', 30000);
});

function updateViewersChoice() {
    $.ajax({url: 'includes/viewerschoice.php',
           success: function(data){
                $("#viewerschoice_middle").html(data);
           }
       });
}
</script>
</head>
<body onload="start_map(0);start_twitter();">

<div id="main_div">
    <?php
        $nav_items = array("home_nav" => "index.php", "animal_nav" => "animals.php");
        include "includes/header.php";
    ?>
<!--        <div id="header">
        <div id="monkey">
            <img src="images/monkey_hanging.png" /></div>
            <div id="nav_bar">
                <span id="nav_left">&nbsp;</span>
                <span id="nav_middle">
                    <a id="home_nav" href="index.php"></a>
                    <a id="animal_nav" href="animals.php"></a>
                </span>                
                <span id="nav_right">&nbsp;</span>
            </div>
        </div> -->
<div id="content_div">
<div id="left_column">

<div id="viewerschoice_block"> 
	<div id="viewerschoice_top"></div> 
	<div id="viewerschoice_left"></div> 
	<div id="viewerschoice_right"></div> 
	<div id="viewerschoice_middle">
		<table>
			<?php
				$query = "SELECT total_votes_num FROM total_votes WHERE total_votes_id = 1";
				$result = mysql_query($query);
				$row = mysql_fetch_array($result);
				$total_votes = $row['total_votes_num'];
				
				$query = "SELECT animal_name, animal_num_views, animal_id FROM animals ORDER BY animal_num_views DESC";
				$result = mysql_query($query);
				
				while($row = mysql_fetch_array($result, MYSQL_ASSOC))
				{
                    if($total_votes == 0)
                        $popularity = 0;
                    else
					    $popularity = (100 * ($row['animal_num_views']/$total_votes));
					$filename = strtolower($row['animal_name']);
					echo "<tr><td><img src=\"images/viewerschoice_animals_".$filename.".png\" /></td><td><a href=\"animals.php?id={$row['animal_id']}\">{$row['animal_name']}</a></td><td>".round($popularity, 2)."%</td></tr>";
				}
			?>
		</table>
	</div>
	<div id="viewerschoice_bottom"></div> 
</div>

        <div id="twitter_block"> 
            <div id="twitter_top"></div> 
            <div id="twitter_left"></div> 
            <div id="twitter_right"></div>
	<div id="twitter_middle">            
		<div id="twitter_container"></div></div>
            <div id="twitter_bottom"></div> 
        </div>

</div>
<div id="middle_column">
    <div id="map_index"><div id="map_container"></div></div>

</div>
	<div id="right_column">
		<?php
			if($_SESSION['username'] == GUEST_NAME)
			{
				echo "<form name=\"test\" id=\"test\" method=\"POST\" action=\"includes/process.php\">";
				echo "<div id=\"login_block\">";
					echo "<div id=\"login_top\"></div>";
					echo "<div id=\"login_left\"></div>";
					echo "<div id=\"login_right\"></div>";
					echo "<div id=\"block_middle\">";
                    if($_GET["loginfail"] == 1)
    					echo "<font color=\"red\">Incorrect username/password.</font>";
					echo "<input id=\"email\" name=\"email1\" type=\"text\" value=\"email\" class=\"watermark\" onfocus=\"watermark_onfocus(this);\" onblur=\"watermark_onblur(this, 'email');\"  onmouseup=\"watermark_onmouseup(event);\" /> <br />";
					echo "<input id=\"password\" name=\"password1\" type=\"password\" value=\"password\" class=\"watermark\" onfocus=\"watermark_onfocus(this);\" onblur=\"watermark_onblur(this, 'password');\" onmouseup=\"watermark_onmouseup(event);\"/> <br/>";
					echo "<div id=\"submit_login\" class=\"button_wrapper\">";
													//echo "<a href=\"\" onclick=\"$('#test').submit();\">";
														//echo "<span>Submit</span>";
													//echo "</a>";
						echo "<input class=\"button_wrapper\" type=\"submit\" name=\"Submit\" value=\"Login\">";
					echo "</div><br />";
					echo "<a href=\"register.php\">Sign Up</a>&nbsp;|&nbsp;<a href=\"forgotpass.php\">Forgot Password</a>";
					echo "</div>";
					echo "<div id=\"login_bottom\"></div>";
				echo "</div>";
				echo "</form>";
				
				
//				echo "<form name=\"test\" method=\"POST\" action=\"includes/process.php\">";
//				echo "<input name=\"email1\" type=\"text\" id=\"email1\">";
//				echo "<input name=\"password1\" type=\"text\" id=\"password1\">";
//				echo "<input type=\"submit\" name=\"Submit\" value=\"Login\">";
//				echo "</form>";
			}
		?>
        <!--<div id="login_block">
            <div id="login_top"></div>
            <div id="login_left"></div>
            <div id="login_right"></div>
            <div id="block_middle">           
            <input id="email" type="text" value="email" class="watermark" onfocus="watermark_onfocus(this);" onblur="watermark_onblur(this, 'email');" onmouseup="watermark_onmouseup(event);"/> <br />
            <input id="password" type="text" value="password" class="watermark" onfocus="watermark_onfocus(this);" onblur="watermark_onblur(this, 'password');" onmouseup="watermark_onmouseup(event);"/> <br/>
            <span id="submit_login" class="button_wrapper">
                <a href="#">
                    <span>Submit</span>
                </a>
            </span> <br />
            <a href="#">Sign Up</a>&nbsp;|&nbsp;<a href="#">Forgot Password</a>
            </div>
            <div id="login_bottom"></div>
        </div>-->
        <div id="videopick_block"> 
            <div id="videopick_top"></div> 
            <div id="videopick_left"></div> 
            <div id="videopick_right"></div> 
			<div id="videopick_middle">
				<div id="videopick_videowindow">
					<a href="" id="player"></a>
					<!--<a href="http://0-22-19-13-ab-d8.dyn.utdallas.edu/test.flv" id="player"></a>-->
					<!--<video src="test.ogv" autoplay="autoplay" controls="controls" width="216" height="162"></video>-->
					
					<script language="JavaScript">flowplayer("player", "flowplayer/flowplayer-3.1.5.swf");</script>
				</div>
				<div id="videopick_carousel">
					<div id="videopick_left_arrow">
						<a href=# id="videopick_previous"><img src="images/left_arrow.gif" /></a>
					</div>
					<div id="videopick_right_arrow">
						<a href=# id="videopick_next"><img src="images/right_arrow.gif" /></a>
					</div>
					<div id="videopick_slides">
						<ul>
							<?php 
								$query = "SELECT * FROM archives ORDER BY archive_qos DESC";
								$result = mysql_query($query);
								while($row = mysql_fetch_array($result, MYSQL_ASSOC))
								{
									echo "<li><a onclick=\"videopick_changeVideo('".$row['archive_url']."')\"><img src=\"".$row['archive_thumb_url']."\" /></a></li>";
								}
							/*<li><a href=# onclick="videopick_changeVideo('http://10.176.14.65:8090/demo.flv')"><img src="images/videothumb-live.png"></img></a></li>
							<li><a href=# onclick="videopick_changeVideo('http://10.176.14.65/archives/video/f1.flv')"><img src="images/videothumb-archive1.png"></img></a></li>
							<li><a href=# onclick="videopick_changeVideo('http://10.176.14.65/archives/video/f3.flv')"><img src="images/videothumb-archive2.png"></img></a></li>
							<li><a href=# onclick="videopick_changeVideo('http://10.176.14.65/archives/video/f4.flv')"><img src="images/videothumb-archive3.png"></img></a></li>
							<li><a href=# onclick="videopick_changeVideo('http://10.176.14.65/archives/video/f5.flv')"><img src="images/videothumb-archive4.png"></img></a></li>
							<li><a href=# onclick="videopick_changeVideo('http://10.176.14.65/archives/video/f6.flv')"><img src="images/videothumb-archive5.png"></img></a></li>
							<li><a href=# onclick="videopick_changeVideo('http://10.176.14.65/archives/video/f6.flv')"><img src="images/videothumb-archive6.png"></img></a></li>*/
							?>
						</ul>
					</div>
				</div>
			</div>
            <div id="videopick_morevids"><a href="#">View More Videos</a></div>
            <div id="videopick_bottom"></div> 
        </div>
        </div>

</div>
            <?php include "includes/footer.php" ?>
</div>

<?php
	include 'includes/closedb.php';
?>

</body>
</html>
