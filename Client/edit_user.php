<?php 
	include('includes/session.php'); 
	if(($_SESSION['userlevel'] != 2) && ($_SESSION['username'] != $_REQUEST['useredit']))
	{
		header("Location: index.php");
	}
	$forminfo = getUserInfo($_REQUEST['useredit']);
	$usertochange = $forminfo['user_email'];
	$returntoadmin = $_REQUEST['returntoadmin'];
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <title>Edit User</title>
        <link href="style.css" rel="stylesheet" type="text/css" />
        <link rel="icon" type="image/png" href="/images/favicon.png" />
		<style type="text/css">
			div#user_blurb {
				padding: 10px;
				font-size: 12px;
				width: 600px;
			}
			div#user_blurb p {
				padding-top: 20px;
				line-height: 16px;
			}
			span#tent{
				display: block;
				text-align: center;
			}
			#edit_user_form>table {
				font-size: 12px;
				padding: 50px;
				margin-left: 75px;
				valign: middle;
			}
			#edit_user_form>table td {
				padding: 10px;
			}
			#edit_user_form input.button_wrapper {
				margin-left: 5px;
				margin-right: 5px;
			}
		</style>
		<script type="text/javascript">
			function gtfo() 
			{
				location.replace('index.php');
			};
		</script>
    </head>
    <body>
        <div id="main_div">
            <?php include "includes/header.php" ?>
            <div id="default_content_div">
                <div id="admin_sub_nav">
					<a id="add_user" href="#">Add User</a>
				</div>
				<div id="user_blurb">
					<h1>Edit Your Profile</h1>
					<p>Keep us up to date with your personal information.</p>
				</div>
				<div id="user_form">
					<form id="edit_user_form" action="includes/process.php?editflag=true&useredit=<?php echo $usertochange; ?>" method="post">
						<table>
							<tr>
								<td>First Name*</td>
								<td><input name="firstname" type="text" value="<?php echo $forminfo['user_firstname'] ?>" /></td>
								<td>Email*</td>
								<td><input name="email1" type="text" value="<?php echo $forminfo['user_email'] ?>" /></td>
							</tr>
							<tr>
								<td>Last Name*</td>
								<td><input name="lastname" type="text" value="<?php echo $forminfo['user_lastname'] ?>" /></td>
								<td>Confirm Email*</td>
								<td><input name="email2" type="text" value="" /></td>
							</tr>
							<tr>
								<td>Address 1*</td>
								<td><input name="address1" type="text" value="<?php echo $forminfo['user_address1'] ?>" /></td>
								<td></td>
								<td></td>
							</tr>
							<tr>
								<td>Address 2</td>
								<td><input name="address2" type="text" value="<?php echo $forminfo['user_address2'] ?>" /></td>
								<td>Password*</td>
								<td><input name="password1" type="text" value="<?php echo $forminfo['user_password'] ?>" /></td>
							</tr>
							<tr>
								<td>City*</td>
								<td><input name="city" type="text" value="<?php echo $forminfo['user_city'] ?>" /></td>
								<td>Confirm Password*</td>
								<td><input name="password2" type="text" value="" /></td>
							</tr>
							<tr>
								<td>State*</td>
								<td><input name="state" type="text" value="<?php echo $forminfo['user_state'] ?>" /></td>
								<td></td>
								<td></td>
							</tr>
							<tr>
								<td>ZIP*</td>
								<td><input name="zip" type="text" value="<?php echo $forminfo['user_zip'] ?>" /></td>
								<?php
									if($_SESSION['userlevel'] == 2)
									{
										echo "<td>User Type</td>";
										echo "<td>";
											echo "<select name=\"usertype\">";
												echo "<option value=\"1\" selected=\"selected\">User</option>";
												echo "<option value=\"2\">Admin</option>";
											echo "</select>";
										echo "</td>";
									}
									else
									{
										echo "<input type=\"hidden\" name=\"usertype\" value=\"1\"";
									}
								?>
							</tr>
							<tr>
								<td>Phone*</td>
								<td><input name="phone" type="text" value="<?php echo $forminfo['user_phone'] ?>" /></td>
								<td></td>
								<td></td>
							</tr>
							<tr>
								<td>Date of Birth*</td>
								<td><input name="dob" type="text" value="<?php echo $forminfo['user_dob'] ?>" /></td>
								<td></td>
								<td></td>
							</tr>
						</table>
						<span id="tent">
							<input class="button_wrapper" type="submit" name="Save" value="Save"></input>
							<input class="button_wrapper" type="button" name="Cancel" value="Cancel" onclick=gtfo() />
						</span>
					</form>
				</div>
            </div>
            <?php include "includes/footer.php" ?>
        </div>
    </body>
</html>
