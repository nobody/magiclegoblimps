<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"></meta>
        <title></title>
        <link href="style.css" rel="stylesheet" type="text/css"></link>
        <link rel="icon" type="image/png" href="/images/favicon.png" />
    </head>
    <body>
        <div id="main_div">
            <?php include "includes/header.php" ?>
            <div id="default_content_div">
			<div id="demo_form">
				<h1>Administrator's Demo<h1>
				<form id="demo_start" action="test/start.php">
					<input class="button_wrapper" type="submit" name="Start" value="Start" />
				</form>
				<form id="demo_stop" action="test/stop.php">
					<input class="button_wrapper" type="submit" name="Stop" value="Stop" />
				</form>
			</div>
            </div>
            <?php include "includes/footer.php" ?>
        </div>
    </body>
</html>
