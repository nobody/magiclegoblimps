<?php

include('session.php');

if(isset($_POST['redirect']))
    $redirect = "http://".$_SERVER['HTTP_HOST'].$_POST["redirect"];
else
    $redirect = "http://".$_SERVER['HTTP_HOST'].$_GET["redirect"];

if($_REQUEST['editflag'])
{
	$query = "UPDATE ".TBL_USERS." SET user_firstname='".$_POST['firstname']."', user_lastname='".$_POST['lastname']."', user_address1='".$_POST['address1']."', user_address2='".$_POST['address2']."', user_city='".$_POST['city']."', user_state='".$_POST['state']."', user_zip='".$_POST['zip']."', user_phone='".$_POST['phone']."', user_dob='".$_POST['dob']."', user_type='".$_POST['usertype']."' WHERE user_email='".$_REQUEST['useredit']."'";
	$result = mysql_query($query);
	if($result && ($_SESSION['userlevel'] == 2))
	{
        if(!$redirect)
            $redirect = "/admin_users.php";
		header("Location: ".$redirect);
	}
	else if($result)
	{
        if(!$redirect)
            $redirect = "/";
		header("Location: ".$redirect);
	}
	else
	{
		echo "OOOH FUCK ".$_POST['firstname'];
	}
}
else if($_REQUEST['delflag'] && ($_SESSION['userlevel'] == 2))
{
	$query = "DELETE FROM ".TBL_USERS." WHERE user_email = '".$_REQUEST['userdel']."'";
	$result = mysql_query($query);
	if($result)
	{
        if(!$redirect)
            $redirect = "/admin_users.php";
		header("Location: ".$redirect);
	}
	else
	{
		echo $_REQUEST['userdel'];
		echo "God damn it";
	}
}
else if($_REQUEST['regflag'])
{
	if(empty($_POST['firstname']) || 
		empty($_POST['lastname']) ||
		empty($_POST['address1']) ||
		empty($_POST['city']) ||
		empty($_POST['state']) ||
		empty($_POST['zip']) ||
		empty($_POST['phone']) ||
		empty($_POST['dob']) ||
		empty($_POST['email1']) ||
		empty($_POST['email2']) ||
		empty($_POST['password1']) ||
		empty($_POST['password2']))
	{
		echo "AWWWW Shit! they're all empty fool";
	}
	else if($_POST['email1'] != $_POST['email2'])
	{
        if(!$redirect)
            $redirect = "/register.php";
		header("Location: ".$redirect);
	}
	else if($_POST['password1'] != $_POST['password2'])
	{
        if(!$redirect)
            $redirect = "/register.php";
		header("Location: ".$redirect);
	}
	else
	{
		$query = "INSERT INTO `user` (`user_id`, `user_email`, `user_password`, `user_type`, `user_last_logon`, `user_last_logout`, `user_created`, `user_firstname`, `user_lastname`, `user_address1`, `user_address2`, `user_city`, `user_state`, `user_zip`, `user_phone`, `user_dob`, `user_loggedin`) VALUES (NULL, '".strtolower($_POST['email1'])."', '".sha1($_POST['password1'])."', '1', NOW(), NOW(), '0000-00-00 00:00:00', '".$_POST['firstname']."', '".$_POST['lastname']."', '".$_POST['address1']."', '".$_POST['address2']."', '".$_POST['city']."', '".$_POST['state']."', '".$_POST['zip']."', '".$_POST['phone']."', '".$_POST['dob']."', NULL)";
		$result = mysql_query($query);
		if($result)
		{
			$_SESSION['username'] = $_POST['email1'];
            if(!$redirect)
                $redirect = "/admin_users.php";
		    header("Location: ".$redirect);
		}
		else
		{
			echo "Failed to create user?";
		}
	}
}
else if($_REQUEST['addflag'])
{
	if(empty($_POST['firstname']) || 
		empty($_POST['lastname']) ||
		empty($_POST['address1']) ||
		empty($_POST['city']) ||
		empty($_POST['state']) ||
		empty($_POST['zip']) ||
		empty($_POST['phone']) ||
		empty($_POST['dob']) ||
		empty($_POST['email1']) ||
		empty($_POST['email2']) ||
		empty($_POST['password1']) ||
		empty($_POST['password2']))
	{
		echo "AWWWW Shit!";
	}
	else if($_POST['email1'] != $_POST['email2'])
	{
        if(!$redirect)
            $redirect = "/add_user.php";
		header("Location: ".$redirect);
	}
	else if($_POST['password1'] != $_POST['password2'])
	{
        if(!$redirect)
            $redirect = "/add_user.php";
		header("Location: ".$redirect);
	}
	else
	{
		$insertpass = $_POST['password1'];
		$insertpass = sha1($insertpass);
		$query = "INSERT INTO `user` (`user_id`, `user_email`, `user_password`, `user_type`, `user_last_logon`, `user_last_logout`, `user_created`, `user_firstname`, `user_lastname`, `user_address1`, `user_address2`, `user_city`, `user_state`, `user_zip`, `user_phone`, `user_dob`, `user_loggedin`) VALUES (NULL, '".strtolower($_POST['email1'])."', '".$insertpass."', ".$_POST['usertype'].", NOW(), NOW(), '0000-00-00 00:00:00', '".$_POST['firstname']."', '".$_POST['lastname']."', '".$_POST['address1']."', '".$_POST['address2']."', '".$_POST['city']."', '".$_POST['state']."', '".$_POST['zip']."', '".$_POST['phone']."', '".$_POST['dob']."', NULL)";
		$result = mysql_query($query);
		if($result)
		{
            if(!$redirect)
                $redirect = "../admin_users.php";
		    header("Location: ".$redirect);
		}
		else
		{	
			echo "Failed to create user?";
		}
	}
}
else if(empty($_POST['email1']) || empty($_POST['password1']))
{
    if(!$redirect)
        $redirect = "/";
		header("Location: ".$redirect."?loginfail=1");
}
else
{
	$user = strtolower(addslashes($_POST['email1']));
	$pass = $_POST['password1'];
	
	if(confirmUserPass($user, $pass) == 0)
	{
		/* Username and password correct, register session variables */
		$userinfo = getUserInfo($user);
		$username = $_SESSION['username'] = $userinfo['user_email'];
		$userlevel = $_SESSION['userlevel'] = $userinfo['user_type'];
		
		/* Update active users table */
		addActiveUser($username, $time);
		removeActiveGuest($_SERVER['REMOTE_ADDR']);

        if(!$redirect)
            $redirect = "/";
		header("Location: ".$redirect);
	}
	else
	{
        if(!$redirect)
            $redirect = "/";
		header("Location: ".$redirect."?loginfail=1");
	}
}
?>
