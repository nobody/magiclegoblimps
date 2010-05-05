		<?php
			if($_SESSION['username'] != GUEST_NAME)
			{
				if($_SESSION['userlevel'] == 2)
				{
					$nomonkey = true;
					$nav_items = array("home_nav" => "index.php", "users_nav"=> "admin_users.php", "cameras_nav" => "admin_camera.php", "animal_nav" => "animals.php");
				} 
				echo "<div id=\"loggedin_window\">";
					echo "Welcome back, ".$_SESSION['firstname']."&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
					echo "<span id=\"edit_profile_button\">";
						echo "<a href=\"edit_user.php?editflag=true&useredit=".$_SESSION['username']."\">Edit Profile</a>";
					echo "</span>";
					echo " | ";
					echo "<span id=\"sign_out_button\" onclick=\"session_destroy()\">";
						echo "<a href=\"includes/logout.php\">Sign Out</a>";
					echo "</span>";
				echo "</div>";
			}
		?>
        <div id="header">
            <?php if(!$nomonkey)
                echo "<div id=\"monkey\"><img src=\"images/monkey_hanging.png\" /></div>"; 
            ?>
            <div id="nav_bar">
                <span id="nav_left">&nbsp;</span>
                <span id="nav_middle">
                    <?php
                        if($nav_items) {
                            $cur_url = $_SERVER['REQUEST_URI'];
                            if(!strstr($cur_url, ".php"))
                                    $cur_url .= "index.php";
                            foreach($nav_items as $nav_id => $url) {
                                if(strstr($cur_url, $url))
                                    echo "<a id=\"".$nav_id."\" href=\"".$url."\" class=\"currentPage\"></a>";
                                else
                                    echo "<a id=\"".$nav_id."\" href=\"".$url."\" class=\"notCurrentPage\"></a>";
                            }
                        }
                    ?>
                </span>
                <span id="nav_right">&nbsp;</span>
            </div>
        </div>
