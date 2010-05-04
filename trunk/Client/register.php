<?php include('includes/session.php'); ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"></meta>
        <title>Zoocam - User Registration</title>
        <link href="style.css" rel="stylesheet" type="text/css"></link>
		<style type="text/css">
			div#registration_blurb {
				padding: 10px;
				font-size: 12px;
				width: 600px;
			}
			div#registration_blurb p {
				padding-top: 20px;
				line-height: 16px;
			}
			span#tent{
				display: block;
				text-align: center;
			}
			#registration_form>table {
				font-size: 12px;
				padding: 50px;
				margin-left: 75px;
				valign: middle;
			}
			#registration_form>table td {
				
				padding: 10px;
			}
			#registration_form input.button_wrapper {
				margin-left: 5px;
				margin-right: 5px;
			}
		</style>
    </head>
    <body>
        <div id="main_div">
            <?php include "includes/header.php" ?>
            <div id="default_content_div">
				<div id="registration_blurb">
					<h1>Register for Zoocam!</h1>
					<p>Enter the following information and you'll be registered as one of our users. As a registered user you'll be able to stay on our site longer and have some control over our cameras... it's so so amazing!!</p>
				</div>
				<div id="reg_form">
					<form id="registration_form" action="includes/process.php?regflag=true" method="post">
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
								<!--<td>User Type</td>
								<td>
									<select name="usertype">
										<option value="1" selected="selected">User</option>
										<option value="2">Admin</option>
									</select>
								</td>-->
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
							<input class="button_wrapper" type="submit" name="Submit" value="Submit" />
							<input class="button_wrapper" type="button" name="Cancel" value="Cancel" onclick=gtfo() />
						</span>
					</form>
				</div>
            </div>
            <?php include "includes/footer.php" ?>
        </div>
    </body>
</html>
