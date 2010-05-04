<?php
	session_start();
	if($_SESSION['userlevel'] != 2)
	{
		header("Location: http://tinyurl.com/zoocam");
        exit();
    }

    include "includes/config.php";
    include "includes/opendb.php";

if($_GET['clearanimalvotes'] == 1)
{
    mysql_query("UPDATE animals SET animal_num_views='0'");
    mysql_query("UPDATE total_votes SET total_votes_num='0'");
    // CLEAR ALL ANIMAL INFO!
	header("Location: admin_animals_stats.php");
    exit();
}
        
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <title>Zoocam - Animals</title>
        <script type="text/javascript" src="js/jquery.js"></script>
        <link href="style.css" rel="stylesheet" type="text/css" />
        <link rel="icon" type="image/png" href="/images/favicon.png"></link>
<script type="text/javascript">
function show_delete_confirm()
{
var r=confirm("Are you sure you want to delete all animal voting data?");
if (r==true)
  {
    window.location = "admin_animals_stats.php?clearanimalvotes=1";
  }
}
</script>

    </head>
    <body>
        <div id="main_div">
            <?php
                $nomonkey = true;
                $nav_items = array("home_nav" => "index.php", "users_nav"=> "admin_users.php", "cameras_nav" => "admin_camera.php", "animal_nav" => "animals.php");
                include "includes/header.php";
            ?>
            <div id="default_content_div">
                <div id="admin_sub_nav">
                    <a id='edit_animal' href='admin_animals.php?mode=edit'>Edit an Animal</a>
                    <a id='add_animal' href='admin_animals.php?mode=add'>Add an Animal</a>
                    <a id="view_animal_stats" href="admin_stats_animals.php">View Statistics</a>
                </div>
                <div id="animal_info">
                    <div id="animal_info_top"></div>
                    <div id="animal_info_middle">
                        <img id="animal_image"/>   
                        <h1 id="animal_info_title">Animal Votes</h1>
                        <table width="100%">
                        <tr><td></td><th>Animal</th><th>Votes</th><th>Vote Percent (%)</th></tr>
<?
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
					echo "<tr><td><img src=\"images/viewerschoice_animals_".$filename.".png\" /></td><td><a href=\"animals.php?id={$row['animal_id']}\">{$row['animal_name']}</a></td><td>".$row['animal_num_views']."</td><td>".round($popularity, 2)."%</td></tr>";
				}
                echo "<tr><td></td><td>Total</td><td>$total_votes</td><td>100%</td></tr>";
?>
                        </table>
                        <a href="#" onclick="show_delete_confirm()">Reset Animal Votes</a>
                        <p id="animal_summary"></p>
                    </div>
                    <div id="animal_info_bottom"></div>
                </div>

                <div id="animal_info" style="float:right">
                    <div id="animal_info_top"></div>
                    <div id="animal_info_middle">
                        <img id="animal_image"/>
                        <h1 id="animal_info_title">Animal Service</h1>
                        <table width="100%">
<?

// Complicated... Get all the cameras. With each camera, get all animals being serviced. With each animal being serviced, get their name.
$query = "SELECT camera_id FROM `cameras`";
$result = mysql_query($query);
while($row = mysql_fetch_array($result, MYSQL_ASSOC)){
    $camera_id = $row['camera_id'];
    echo "<tr><th>Camera $camera_id</th> <td></td></tr>";
    $query = "SELECT * FROM `service` WHERE service_camera_id=$camera_id";
    $result1 = mysql_query($query);
    if(mysql_num_rows($result1) != 0) {
        while($servicerow = mysql_fetch_array($result1, MYSQL_ASSOC)) {
            $animal_id = $servicerow['service_object_id'];
            $query = "SELECT animal_name FROM animals WHERE animal_id='$animal_id' LIMIT 1";
            $result2 = mysql_query($query);
            if(mysql_num_rows($result2) != 0) {
                $animalrow = mysql_fetch_array($result2, MYSQL_ASSOC);
                $animalname = $animalrow['animal_name'];
                $filename = strtolower($animalrow['animal_name']);
                echo "<tr><td><img src=\"images/viewerschoice_animals_".$filename.".png\" /></td> <td><a href=\"animals.php?id=$animal_id\">$animalname</a></td></tr>";
            }
        }
    }else{
        echo "<tr><td colspan=\"2\">no animals</td></tr>";
    }
}
?>
                        </table>
                        <p id="animal_summary"></p>
                    </div>
                    <div id="animal_info_bottom"></div>
                </div>
            </div>
            <?php include "includes/footer.php" ?>
        </div>
    </body>
</html>
