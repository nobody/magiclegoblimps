<?php
/* database connection variables */
define("DBHOST", "localhost");
define("DBUSER", "root");
define("DBPASS", "1EA96F710");
define("DBNAME", "zoocam");

#define("ROBOT_CONTROLLER_URL", "localhost"); # Test server
define("ROBOT_CONTROLLER_URL", "0-22-19-17-37-ad.dyn.utdallas.edu");
define("ROBOT_CONTROLLER_PORT", "10000");

/* table constants */
define(TBL_USERS, "user");
define(TBL_ACTIVE_USERS, "active_users");
define(TBL_ACTIVE_GUESTS, "active_guests");
define(TBL_BANNED_USERS, "banned_users");

/* user-type constants */
define(ADMIN_NAME, "admin");
define(GUEST_NAME, "Guest");
define(ADMIN_LEVEL, 2);
define(USER_LEVEL, 1);
define(GUEST_LEVEL, 0);

/* track visitors switch */
define(TRACK_VISITORS, true);

/* timeout constant to determine how long,
 * in minutes from user's last page 
 * refresh, before a user is considered
 * inactive. */
define(USER_TIMEOUT, 10);
define(GUEST_TIMEOUT, 5);
 
/* cookies */
define(COOKIE_EXPIRE, 60*60*24*100); // 100 days
define(COOKIE_PATH, "/"); // available throughout the domain

/* email constants for system emails,
 * and new users */
define(EMAIL_FROM_NAME, "Zoocam Administrator");
define(EMAIL_FROM_ADDRESS, "admin@zoocam.com");
define(EMAIL_WELCOME, false);

/* twitter variables */
$twitteruser = "zoocamera";
$twitterpass = "1EA96F710";
?>
