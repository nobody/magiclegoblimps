<?php

$cache_file = '../tmp/twittercache.csv';

try{
    updateTwitterCache($cache_file);
    printTweets($cache_file);
} catch (Exception $e){
    echo '<div id="tweet_top"> </div>';
    echo '<div id="tweet_middle">';
    echo '<div id="tweet_message">';
    echo '<b>There was an error retrieving twitter data.</b>';
    echo '</div></div>';
    echo '<div id="tweet_bottom"></div>';
}
function updateTwitterCache($cache_file){
    if(!file_exists($cache_file)){
        $file=fopen($cache_file,"w"); //changed r to w
        fclose($file);
    }
    if(time()-filemtime($cache_file) >= 90){
        require "twitter.lib.php";
        require "config.php";
        $twitter = new Twitter($twitteruser, $twitterpass);
        $xml = $twitter->getMentions();

        $twitter_status = new SimpleXMLElement($xml);
        $fp = fopen($cache_file, 'w'); //changed twitter.csv to $cache_file
        foreach($twitter_status->status as $status)
            foreach($status->user as $user){
                fputcsv($fp, array($status->created_at, $status->id, $status->text, $status->source, $user->name, $user->screen_name, $user->description, $user->profile_image_url, 'http://www.twitter.com/'.$user->name, $user->followers_count));
            }
        fclose($fp);
    }
}

function printTweets($cache_file){
    if (($handle = fopen($cache_file, "r")) !== FALSE) {
        while (($data = fgetcsv($handle, 1000, ",")) !== FALSE) {
            echo '<div id="tweet_top"> </div>';
            echo '<div id="tweet_middle">';
            echo '<div id="tweet_message">';
            echo '<b><a href="http://www.twitter.com/'.$data[4].'">'.$data[5].'</a></b><br />';
            echo $data[2];
            echo '</div></div>';
            echo '<div id="tweet_bottom"></div>';
        }
        fclose($handle);
    }
}
?>
