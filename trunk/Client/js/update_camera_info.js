function updateInfo(cameraID) {
/*	alert("camera");*/
    $.ajax({url: 'includes/camerainfo.php',
            data: {id: cameraID},
            dataType: 'json',
            success: function(data){
                $("#camera_id").html(data.camera_id);
                $("#camera_ids").html(data.camera_id);
                $("#grid_coords").html("("+data.camera_curr_x+", "+ data.camera_curr_y+")");
                $("#last_grid_coords").html("("+data.camera_last_x+", "+ data.camera_last_y+")");
                $("#last_obj_viewed").html(data.camera_curr_object);
                $("#camera_type").html(data.camera_type);
                $("#camera_fps").html(data.camera_fps);
                $("#camera_resolution").html(data.camera_resolution);
                $("#ip_address").html(data.camera_ip);
                            
	            document.getElementById("player").href = data.camera_feed;
	            flowplayer("player", "flowplayer/flowplayer-3.1.5.swf", { 
                    clip:  { 
                        autoplay: true,
                        autoBuffering: true 
                    }
                } );
            }
        });

    currentlyselected = cameraID;
    map_update();
}
