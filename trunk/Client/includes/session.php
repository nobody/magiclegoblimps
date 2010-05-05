<?php 

include('config.php');
include('opendb.php');

$username;				// user logs in with email
$firstname;
$userlevel;				// the level of the user
$time;					// time user was last active
$logged_in;				// true if user is logged in
$userinfo = array();	// array holding user info
$url;					// the page url currently being viewed
$referrer;				// last recorded site page viewed
$num_active_users;
$num_active_guests;

startSession();

function startSession()
{
	session_start();
	$time = time();
	$logged_in = checkLogin();
	
	if(!$logged_in)
	{
		$username = $_SESSION['username'] = GUEST_NAME;
		$userlevel = GUEST_LEVEL;
		addActiveGuest($_SERVER['REMOTE_ADDR'], $time);
	}
	else
	{
		addActiveUser($username, $time);
	}	
	
	// remove inactive users
	removeInactiveUsers();
	removeInactiveGuests();
	
	// set referrer page
	if(isset($_SESSION['url'])) 
	{
		$referrer = $_SESSION['url'];
	}
	else
	{
		$referrer = "/";
	}
	
	//set current url
	$url = $_SESSION['url'] = $_SERVER['PHP_SELF'];
}

function checkLogin()
{
	if(isset($_COOKIE['cookname']))
	{
		$username = $_SESSION['username'] = $_COOKIE['cookname'];
		$_SESSION['firstname'] = $_COOKIE['cookfn'];
	}
	if(isset($_SESSION['username']) && $_SESSION['username'] != GUEST_NAME)
	{
		if(confirmUsername($_SESSION['username']) != 0)
		{
			unset($_SESSION['username']);
			unset($_SESSION['firstname']);
			unset($_SESSION['userlevel']);
			return false;
		}
		$userinfo = getUserInfo($_SESSION['username']);
		$username = $userinfo['user_email'];
		$_SESSION['firstname'] = $userinfo['user_firstname'];
		$userlevel = $userinfo['user_level'];
		return true;
	}
	else
	{
		return false;
	}
}

function confirmUsername($username)
{
    /* Add slashes if necessary (for query) */
    if(!get_magic_quotes_gpc()) 
	{
		$username = addslashes($username);
    }

	/* Verify that user is in database */
	$q = "SELECT * FROM ".TBL_USERS." WHERE user_email = '$username'";
	$result = mysql_query($q);
	if(!$result || (mysql_num_rows($result) < 1))
	{
		return 1; //Indicates username failure
	}
	return 0;
}

function confirmUserPass($username, $password)
{
	/* Add slashes if necessary (for query) */
	if(!get_magic_quotes_gpc()) 
	{
		$username = addslashes($username);
	}

	/* Verify that user is in database */
	$q = "SELECT user_password FROM ".TBL_USERS." WHERE user_email = '$username'";
	$result = mysql_query($q);
	if(!$result || (mysql_numrows($result) < 1)){
		return 1; //Indicates username failure
	}

	/* Retrieve password from result, strip slashes */
	$dbarray = mysql_fetch_array($result);
	$dbarray['user_password'] = stripslashes($dbarray['user_password']);
	$password = stripslashes($password);
	$password = sha1($password);

	/* Validate that password is correct */
	if($password == $dbarray['user_password']){
		return 0; //Success! Username and password confirmed
	}
	else{
		return 2; //Indicates password failure
	}
}

function getUserInfo($username)
{
	$q = "SELECT * FROM ".TBL_USERS." WHERE user_email = '$username'";
	$result = mysql_query($q);
	/* Error occurred, return given name by default */
	if(!$result || (mysql_numrows($result) < 1))
	{
		return NULL;
	}
	/* Return result array */
	$dbarray = array();
	$dbarray = mysql_fetch_array($result);
	return $dbarray;
}

function logout()
{
	/**
	* Delete cookies - the time must be in the past,
	* so just negate what you added when creating the
	* cookie.
	*/
	if(isset($_COOKIE['cookname']) && isset($_COOKIE['cookfn'])){
	 setcookie("cookname", "", time()-COOKIE_EXPIRE, COOKIE_PATH);
	 setcookie("cookfn",   "", time()-COOKIE_EXPIRE, COOKIE_PATH);
	}

	/* Unset PHP session variables */
	unset($_SESSION['username']);
	unset($_SESSION['firstname']);

	/* Reflect fact that user has logged out */
	$logged_in = false;

	/**
	* Remove from active users table and add to
	* active guests tables.
	*/
	removeActiveUser($username);
	addActiveGuest($_SERVER['REMOTE_ADDR'], $time);

	/* Set user level to guest */
	$username  = GUEST_NAME;
	$userlevel = GUEST_LEVEL;
}

function updateUserField($username, $field, $value)
{
	$q = "UPDATE ".TBL_USERS." SET ".$field." = '$value' WHERE user_email = '$username'";
	return mysql_query($q);
}

function addActiveUser($username, $time)
{
	$q = "UPDATE ".TBL_USERS." SET user_last_logon = '$time' WHERE user_email = '$username'";
	mysql_query($q);

	if(!TRACK_VISITORS) return;
	$q = "REPLACE INTO ".TBL_ACTIVE_USERS." VALUES ('$username', '$time')";
	mysql_query($q);
	$q = "UPDATE ".TBL_USERS." SET user_loggedin = 1 WHERE user_email = '$username'";
	mysql_query($q);
	calcNumActiveUsers();
}

function calcNumActiveUsers()
{
	/* Calculate number of users at site */
	$q = "SELECT * FROM ".TBL_ACTIVE_USERS;
	$result = mysql_query($q);
	$num_active_users = mysql_numrows($result);
}

function addActiveGuest($ip, $time)
{
	if(!TRACK_VISITORS) return;
	$q = "REPLACE INTO ".TBL_ACTIVE_GUESTS." VALUES ('$ip', '$time')";
	mysql_query($q);
	calcNumActiveGuests();
}

function calcNumActiveGuests()
{
	/* Calculate number of guests at site */
	$q = "SELECT * FROM ".TBL_ACTIVE_GUESTS;
	$result = mysql_query($q);
	$num_active_guests = mysql_numrows($result);
}

function removeInactiveUsers()
{
	if(!TRACK_VISITORS) return;
	$timeout = time() - USER_TIMEOUT * 60;
	$q = "DELETE FROM ".TBL_ACTIVE_USERS." WHERE timestamp < $timeout";
	mysql_query($q);
	$q = "UPDATE ".TBL_USERS." SET user_loggedin = 0 WHERE timestamp < $timeout";
	mysql_query($q);
	calcNumActiveUsers();
}

function removeInactiveGuests()
{
	if(!TRACK_VISITORS) return;
	$timeout = time() - GUEST_TIMEOUT * 60;
	$q = "DELETE FROM ".TBL_ACTIVE_GUESTS." WHERE timestamp < $timeout";
	mysql_query($q);
	calcNumActiveGuests();
}

function removeActiveUser($username)
{
	if(!TRACK_VISITORS) return;
	$q = "DELETE FROM ".TBL_ACTIVE_USERS." WHERE user_email = '$username'";
	mysql_query($q);
	$q = "UPDATE ".TBL_USERS." SET user_loggedin = 0 WHERE user_email = '$username'";
	mysql_query($q);
	calcNumActiveUsers();
}
   
function removeActiveGuest($ip)
{
	if(!TRACK_VISITORS) return;
	$q = "DELETE FROM ".TBL_ACTIVE_GUESTS." WHERE ip = '$ip'";
	mysql_query($q);
	calcNumActiveGuests();

}

function gtfo() 
{
	location.replace('index.php');
}
?>