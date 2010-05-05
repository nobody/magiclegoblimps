$(document).ready(function() {

	//rotation speed and timer
	var speed = 8000;
	var run = setInterval('rotate()', speed);	
	
	//grab the width and calculate left value
	var item_width = $('#videopick_slides li').outerWidth(); 
	var left_value = item_width * (-1); 
        
    //move the last item before first item, just in case user click previous button
	$('#videopick_slides li:first').before($('#videopick_slides li:last'));
	
	//set the default item to the correct position 
	$('#videopick_slides ul').css({'left' : left_value});

    //if user clicked on previous button
	$('#videopick_previous').click(function() {

		//get the right position            
		var left_indent = parseInt($('#videopick_slides ul').css('left')) + item_width;

		//slide the item            
		$('#videopick_slides ul').animate({'left' : left_indent}, 200, function(){    

            //move the last item and put it as first item            	
			$('#videopick_slides li:first').before($('#videopick_slides li:last'));           

			//set the default item to correct position
			$('#videopick_slides ul').css({'left' : left_value});
		
		});

		//cancel the link behavior            
		return false;
            
	});

 
    //if user clicked on next button
	$('#videopick_next').click(function() {
		
		//get the right position
		var left_indent = parseInt($('#videopick_slides ul').css('left')) - item_width;
		
		//slide the item
		$('#videopick_slides ul').animate({'left' : left_indent}, 200, function () {
            
            //move the first item and put it as last item
			$('#videopick_slides li:last').after($('#videopick_slides li:first'));                 	
			
			//set the default item to correct position
			$('#videopick_slides ul').css({'left' : left_value});
		
		});
		         
		//cancel the link behavior
		return false;
		
	});        
	
	//if mouse hover, pause the auto rotation, otherwise rotate it
	$('#videopick_slides').hover(
		
		function() {
			clearInterval(run);
		}, 
		function() {
			run = setInterval('rotate()', speed);	
		}
	);         
});

//a simple function to click next link
//a timer will call this function, and the rotation will begin :)  
function rotate() {
	$('#videopick_next').click();
}

//a simple function to change archive videos
function videopick_changeVideo(link) {
	document.getElementById("player").href = link;
	flowplayer("player", "flowplayer/flowplayer-3.1.5.swf");
}
