var curAnimalID;
var voteStarted=false;
var voteIntervalID;

var curCameraFeed=-1;
var curFeedType=-1;
var cameraStarted=false;
var tmp = -1;

function updateInfo(animalID) {
    $.ajax({url: 'includes/animalinfo.php',
            data: {id: animalID},
            dataType: 'json',
            success: function(data){
                $("#animal_info_middle span, #animal_info_middle p").html("");
                $("#animal_info_title").html("About the " + data.animal_name);
                $("#vote_button_text").html("Vote Here for " + data.animal_name + "!");
                $("#kingdom").html(data.animal_kingdom);
                $("#phylum").html(data.animal_phylum);
                $("#subphylum").html(data.animal_subphylum);
                $("#class").html(data.animal_class);
                $("#order").html(data.animal_order);
                $("#suborder").html(data.animal_suborder);
                $("#animal_summary").html(data.animal_description);
                if(data.animal_image_path) {
                    $("#animal_image").css('display','');
                    $("#animal_image").attr('src', data.animal_image_path);
                }
                else
                    $("#animal_image").css('display', 'none');
                $("#vote_text_header").html("Our cameras are controlled through user votes.");
                $("#vote_text_detail").html("If you don't see a camera near your favorite animal,<br />cast your vote to help change camera locations.");
                $("#vote_button").css({'opacity':'1', 'display':''});
                $("#vote_button").css('filter','alpha(opacity=100)');  /* for ie browser */
                $("#vote_button").css('background-color','white');    /* for ie browser */
                $("#vote_button > a").css('display','');
                $("#edit_animal").attr('href', 'admin_animals.php?mode=edit&id=' + animalID);
                curAnimalID = animalID;
                if(voteIntervalID) {
                    clearInterval(voteIntervalID);
                    voteIntervalID = null;
                    voteStarted = false;
                }
                hideVideo();
            }
        });
}

function VideoCheck(){
    $.ajax({url: 'includes/videoinfo.php',
        data: {id: curAnimalID},
        dataType: 'json',
        async: false,
        success: function(data){
            if(data.num > 0){
                //console.log("data.num: "+data.num);
                //console.log("camera_feed: "+data.camera_feed);
                tmp = data.camera_feed;

                //camera_feed is live
                if(data.num == 1) {
                    $("#video_text").html("You are watching a live view of this animal!");
                }
                else { //camera_feed is archived
                    curFeedType = data.num;
                    $("#video_text").html("Enjoy this archived video while we try our best to service your request!");
                }
                //console.log("tmp: "+data.tmp);
            }else{
                tmp = -1;
            }
         }
    });
    return tmp;
}

function showVideo(video_url){
    $('#no_camera').hide();
	document.getElementById("player").href = video_url;
    $('#yes_camera').show();
    loadVideoFeed();
    curCameraFeed=video_url;
    cameraStarted=true;
}

function loadVideoFeed(){
    if(curFeedType == 1){ live = true; } else { live = false;}
    flowplayer("player", "flowplayer/flowplayer-3.1.5.swf", { 
        clip:  { 
            autoplay: true,
            autoBuffering: true,
       		live: live
        }
    });
}

function hideVideo(){
    $('#no_camera').show();
    $('#yes_camera').hide();
    cameraStarted=false;
}

function startVote() {
    if(!voteStarted) {
        vote();
        voteIntervalID = setInterval("vote()", 30*1000);
        voteStarted=true;
    }
}

function vote() {
    $.ajax({url: "includes/request_animal.php",
            data: {animalID: curAnimalID},
            success: function(data) { 
                $("#vote_text").fadeOut(400, function(){
                    $("#vote_text_header").html("Your vote has been sent!<br />We are currently hard at work sending cameras to this animal.");
                    $("#vote_text_detail").html("Please be patient, your vote will be counted as long as you stay on this animal's page.");
                    $("#vote_text").fadeIn(400);
                    $("#vote_button").fadeTo(400, 0);
                    $("#vote_button > a").css('display','none');
                })
            }
    });
    var tmpVideoFeed = VideoCheck();
    //console.log("curAnimalID: " +curAnimalID);
    //console.log("tmpVideoFeed: " +tmpVideoFeed);
    //console.log("curCameraFeed: " +curCameraFeed);
    //console.log("cameraStarted: " +cameraStarted);
    if(tmpVideoFeed == -1){ //NO VIDEO!
        if(cameraStarted) // that's a change, stop the video
            hideVideo();
    }else if(cameraStarted && tmpVideoFeed != curCameraFeed){ // change the video feed
        showVideo(tmpVideoFeed);
    }else if(!cameraStarted){ // change the video feed
        showVideo(tmpVideoFeed);
    }
}
