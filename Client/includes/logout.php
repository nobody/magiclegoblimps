<?php
include('session.php');
$redirect = $_GET["redirect"];
if(!$redirect)
    $redirect = "/";
//session_start();
removeActiveUser($_SESSION['username']);
session_destroy();
header("Location: ".$redirect);
?>
