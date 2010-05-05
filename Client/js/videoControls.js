var zoomAmt = 100;
var zoomStep = 10;
var yoffset = 0;
var ystep = 10;
var videoWidth = 413;
var videoHeight = 249;

function VideoZoomIn(){
    if(zoomAmt+zoomStep <= 1000){
        zoomAmt += zoomStep;
        VideoZoomSet(zoomAmt);
    }
}
function VideoZoomOut(){
    if(zoomAmt-zoomStep >= 100){
        zoomAmt -= zoomStep;
        VideoZoomSet(zoomAmt);
    }
}
function VideoZoomReset(){
    zoomAmt = 100;
    VideoZoomSet(zoomAmt);
}
function VideoZoomSet(z){
    $f('player').getScreen().animate({width:z+'%',height:z+'%'});
}
function VideoTiltUp(){
    if(yoffset+ystep <= videoHeight/2){
        yoffset += ystep;
        VideoTilt(yoffset);
    }
}
function VideoTiltDown(){
    if(yoffset-ystep >= -videoHeight/2){
        yoffset -= ystep;
        VideoTilt(yoffset);
    }
}
function VideoTilt(Y){
    if(Y < 0){
        iY = Math.abs(Y);
        $f('player').getScreen().animate({bottom:iY});
    }else if(Y > 0){
        $f('player').getScreen().animate({top:Y});
    }else{
        $f('player').getScreen().animate({top: 0, bottom: 0});
    }
}
