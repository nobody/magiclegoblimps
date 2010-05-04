<?php include('includes/session.php'); 
	if($_SESSION['userlevel'] != 2)
	{
		header("Location: index.php");
	}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <title>Zoocam Users</title>
        <link href="style.css" rel="stylesheet" type="text/css" />
        <link rel="icon" type="image/png" href="/images/favicon.png"></link>
		<link href="users_table.css" rel="stylesheet" type="text/css" />
		<script type="text/javascript" src="js/jquery.js"></script>
		<script type="text/javascript" src="js/jquery.dataTables.js"></script>
		<script type="text/javascript">
			$(document).ready(function() {
				$('#users_table').dataTable();
			});
		</script>
    </head>
    <body>
        <div id="main_div">
            <?php 
				$nomonkey = true;
				$nav_items = array("home_nav" => "index.php", "users_nav"=> "admin_users.php", "cameras_nav" => "admin_camera.php", "animal_nav" => "animals.php");
				include "includes/header.php" 
			?>
            <div id="default_content_div">
                <div id="admin_sub_nav">
					<a id="add_user" href="add_user.php">Add User</a>
				</div>
				<table cellpadding="0" cellspacing="0" border="0" class="display" id="users_table">
					<thead>
						<tr>
							<th>Name</th>
							<th>Type</th>
							<th>Description</th>
							<th>Login Status</th>
							<th></th>
						</tr>
					</thead>
					<tbody>
						<?php 
							$query = "SELECT * FROM user ORDER BY user_lastname";
							$result = mysql_query($query);
							
							while($row = mysql_fetch_array($result, MYSQL_ASSOC))
							{
								echo "<tr>";
								echo "<td>".$row['user_lastname'].", ".$row['user_firstname']."</td>";
								echo "<td>";
									if($row['user_type'] == 0)
									{
										echo "Guest";
									}
									else if($row['user_type'] == 1)
									{
										echo "Registered";
									}
									else if($row['user_type'] == 2)
									{
										echo "Admin";
									}
									else
									{
										echo "Oh Fuck!?";
									}
								echo "</td>";
								echo "<td width='25px'>".$row['user_description']."</td>";
                                if($row['user_loggedin'] == 1)
								    echo "<td>yes</td>";
                                else
								    echo "<td>no</td>";
								echo "<td><a href=\"edit_user.php?editflag=true&useredit=".$row['user_email']."\">edit</a> | <a href=\"includes/process.php?delflag=true&?editflag=true&userdel=".$row['user_email']."\">delete</a></td>";
								echo "</tr>";
							}
						?>
					</tbody>
				</table>
            </div>
            <?php include "includes/footer.php" ?>
        </div>
    </body>
</html>
