function ajax_update()
{
    var wrapperId 	=	'#twitter_container';
    var postFile	=	'../includes/twitterfetch.php';

    $.post(postFile, {}, function(data){$(wrapperId).fadeOut('2000',function(){$(this).html(data).fadeIn();}).html();});
}

function start_twitter(){
    ajax_update();
    setInterval('ajax_update()', 30*1000);
}

