
function sendRobotCommands(actionID, cameraID, degreeMultiplier) {

//    $.post("includes/robot_controller.php",
//            {robot: cameraID, action: actionID, degrees: degreeMultiplier*10 },
//            function(data){if(data) alert(data);}
//        );
    $.ajax({        
        data: {robot: cameraID, action: actionID, degrees: degreeMultiplier*10},
        type: "GET",
        url: "includes/robot_controller.php",
        success: function(data){if(data) alert(data )}
    });
}

