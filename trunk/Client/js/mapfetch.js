var currentpage = 0;
var currentlyselected = 1;

function map_update()
{
    var wrapperId 	=	'#map_container';
    var postFile	=	'../includes/mapfetch.php?page='+currentpage+'&selected='+currentlyselected;

    $.post( postFile, {}, function(data){$(wrapperId).html(data); } );
}

function start_map(page){
    currentpage = page;
    map_update(currentpage);
    setInterval('map_update()', 3*1000);
}

function animateMap(wrapperId, newX, newY){
    $("#"+wrapperId).animate({
        top: newY+"px",
        left: newX+"px"
    }, 1500 );
}
