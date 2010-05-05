<?php

// path to ffmpeg
$ffmpeg = '?/ffmpeg';

// the input video file
$video  = dirname(__FILE__) . '/?.flv';

// where to save the image
$image  = dirname(__FILE__) . '../images/?.jpg';

// default time in the video to grab the image
$second = 1;

// get the duration and a random place within that
$cmd = "$ffmpeg -i $video 2>&1";
if (preg_match('/Duration: ((\d+):(\d+):(\d+))/s', `$cmd`, $time)) {
    $total = ($time[2] * 3600) + ($time[3] * 60) + $time[4];
    $second = rand(1, ($total - 1));
}

// grab the screenshot
$cmd = "$ffmpeg -i $video -an -ss $second -t 00:00:01 -r 1 -y -vcodec flv -f mjpeg $image 2>&1";
$return = `$cmd`;

?>