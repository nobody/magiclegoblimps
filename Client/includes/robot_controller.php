<?

require('config.php');

// 3 Parameters in $_GET:
    // action
    // robot
    // degrees (to turn)

//$host = "titus.collegiumv.org";
$host = ROBOT_CONTROLLER_URL;
$port = ROBOT_CONTROLLER_PORT;
$action = $_GET["action"];
$robot = $_GET["robot"];
$degrees = $_GET["degrees"];

if($action == null)
    die("Invalid action.");
if($robot == null)
    die("Invalid robot.");
if($degrees == null)
    $degrees = 0;
if($action < 1 || $action > 6)
    die("Invalid action.");
if($robot < 1 || $robot > 10)
    die("Invalid robot.");
if($degrees < -360 || $degrees > 360)
    die("Invalid degree value.");

switch($action){
    case 1:
        $action_name = "forward";
        break;
    case 2:
        $action_name = "left";
        break;
    case 3:
        $action_name = "right";
        break;
    case 4:        
        if($degrees < 0)
            $action_name = "turn right";
        else
            $action_name = "turn left";
        break;
    case 5:
        $action_name = "delete";
        break;
    case 6:
        $action_name = "release";
        break;
    default:
        $action_name = "unknown command";
        break;
}
$redirect = $_GET["redirect"];

try{
    @$sock=fsockopen($host, $port, $errnum, $errstr, 30) ;
    if (!is_resource($sock)) {        
        if($redirect == 1){
            header("Location: ".$_SERVER['HTTP_REFERER']);
            print $_SERVER['HTTP_REFERER'];
        }
        else
            exit("connection fail: ".$errnum." ".$errstr) ;
    } else {
        if($action == 4){
            fwrite($sock, $action."$".$robot."$".$degrees."$\n");
            $message = "Command sent to robot ".$robot.": $action_name";
            //echo  $action."$".$robot."$".$degrees."$\n";
        }else{
            fwrite($sock, $action."$".$robot."$\n");
            $message = "Command sent to robot ".$robot.": $action_name";
            //echo  $action."$".$robot."$\n";
        }
    }
}catch (Exception $e) {
    $message = 'Error: Robot control server is not responding.';
}
if($redirect == 1)
    header("Location: ".$_SERVER['HTTP_REFERER']);
echo $message;
fclose($sock) ;
?>
