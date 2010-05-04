<?php
	session_start();
	if($_SESSION['userlevel'] != 2)
	{
		header("Location: http://tinyurl.com/zoocam");
        exit();
    }

    include "includes/config.php";
    include "includes/opendb.php";
        
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <title>Zoocam - Animals</title>
        <script type="text/javascript" src="js/jquery.js"></script>
        <link href="style.css" rel="stylesheet" type="text/css" />
        <link rel="icon" type="image/png" href="/images/favicon.png"></link>
        <style type="text/css">
            #animal_info_entry {
                float: left;
                margin: 0px 20px;
            }
            #animal_info_entry input {
                width: 150px;
                margin-bottom: 5px;
            }
            #animal_info_entry select {
                width: 154px;
                margin-bottom: 5px;
            }
            #animal_info_entry label {
                display: inline-block;
                width: 80px;
            }
            #animal_description {
                height: 232px;
                width: 275px;
                margin-bottom: 10px;
                margin-right: 20px;
                float:left;
            }
            #edit_animal_title {
                margin-left: 20px;
            }
        </style>

<script type= "text/javascript">/*<![CDATA[*/
function updateInfo(animalID) {
    $.ajax({url: 'includes/animalinfo.php',
            data: {id: animalID},
            dataType: 'json',
            success: function(data){
                resetForm();
                //populate form fields
                $("#edit_animal_title").html("Edit the " + data.animal_name);
                $("#name").val(data.animal_name);
                $("#animal_id").val(data.animal_id);
                $("#object_id").val(data.animal_id);
                $("#kingdom").val(data.animal_kingdom);
                $("#phylum").val(data.animal_phylum);
                $("#subphylum").val(data.animal_subphylum);
                $("#class").val(data.animal_class);
                $("#order").val(data.animal_order);
                $("#suborder").val(data.animal_suborder);
                $("#animal_description").val(data.animal_description);
                $("#color").val(data.animal_ballcolor);
                $("#delete_button").attr('disabled','');
                if(data.animal_image_path)
                    $("#image_display").attr('src', data.animal_image_path).css('display','');
                else
                    $("#image_display").css('display','none');
            }
        });
}

function resetForm(){
    $(":input","#save_animal").not(":submit").val("");
    $("#image_display").attr('src', '');
    $("#save_button span").css('color','');
}

$(function(){
	//Get our elements for faster access and set overlay width
	var div = $('div.animal_bar_container'),
		ul = $('ul.animal_bar_container'),
		ulPadding = 30;

	//Get menu width
	var divWidth = div.width();

	//Remove scrollbars
	div.css({overflow: 'hidden'});

	//Find last image container
	var lastLi = ul.find('li:last-child');

	//When user move mouse over menu
	div.mousemove(function(e){
		//As images are loaded ul width increases,
		//so we recalculate it each time
		var ulWidth = lastLi[0].offsetLeft + lastLi.outerWidth() + ulPadding;
		var left = (e.pageX - div.offset().left) * (ulWidth-divWidth) / divWidth;
		div.scrollLeft(left);
	});

        //animate font change in animal bar
        var names =  $("div.animal_bar span")
        names.mouseover(function(){
           $(this).animate({fontSize: "32px"}, 150);
        });
        names.mouseout(function(){
            $(this).animate({fontSize: "24px"}, 150)
        });

        //send animal information for saving
        /*$("#save_button").click(function() {
            $.ajax({url: "includes/save_animal.php",
                   data: $("#save_animal").serialize(),
                   success: function(data){
                       alert(data);
                       //Update animal navigation
                       $.ajax({url: "includes/animalnav.php",
                               success: function(data){
                                   $("ul.animal_bar_container").html(data);
                                   if($("#animal_id").html() != "0")
                                       $("#edit_animal_title").html("Edit the " + $("#name").val());
                               }
                           });

                   }
            });
        });*/

        //show animal bar
        $("#edit_animal").click(function (){
            resetForm();
            $("#edit_animal_title").html("Edit an Animal");
            $("div.animal_bar, div.animal_bar_container").animate({height: 75}, 1000);
            return false;
        });

        //hide animal bar
        $("#add_animal").click(function (){
            if($("#animal_id").val() != "0") {
                resetForm();
                $("#animal_id").val("0");
                $("#delete_button").attr('disabled', 'disabled');
                $("#image_display").css('display','none');
                $("#edit_animal_title").html("Add an Animal");
                $("div.animal_bar, div.animal_bar_container").animate({height: 0}, 1000);
                return false;
            }
        });

        //Add nice interactions with form inputs
        var inputs = $(":input");

        //bold inputs label when in focus
        inputs.focus(function (){
            $(this).prev().css('font-weight', 'bold');
        });
        inputs.blur(function (){
            $(this).prev().css('font-weight', 'normal');
        });

        //turn save button red when changes have been made and not saved
        inputs.change(function() {
            $("#save_button span").css('color', 'red');
        });

});
/*]]>*/</script>
    </head>
    <body>
        <?php
            //Save animal button clicked
           if(isset($_POST['save_button'])){
            //check for image to upload
            $uploadFilename;
            if(is_uploaded_file($_FILES['image']['tmp_name']) && getimagesize($_FILES['image']['tmp_name'])) {
                //$imagesDirectory = $_SERVER['DOCUMENT_ROOT']."/images/animals/";
                //echo $imagesDirectory;
                $now = time();
                while(file_exists($uploadFilename = "./images/animals/".$now.$_FILES['image']['name']))
                {
                    $now++;
                }
                move_uploaded_file($_FILES['image']['tmp_name'], $uploadFilename);
            }

            $id = $_POST['animal_id'];
            $animal_name = $_POST['name'];
            $kingdom = $_POST['kingdom'];
            $phylum = $_POST['phylum'];
            $subphylum = $_POST['subphylum'];
            $class = $_POST['class'];
            $order = $_POST['order'];
            $suborder = $_POST['suborder'];
            $description = $_POST['animal_description'];
            $color = $_POST['color'];

            $query;
            //Adding new animal
            if(strcmp($id,"0") == 0) {
                $query = "INSERT INTO animals SET
                            animal_name='$animal_name',
                            animal_kingdom='$kingdom',
                            animal_phylum='$phylum',
                            animal_subphylum='$subphylum',
                            animal_class='$class',
                            animal_order='$order',
                            animal_suborder='$suborder',
                            animal_description='$description',
                            animal_ballcolor='$color',
                            animal_num_views=0,
                            animal_curr_x=0,
                            animal_curr_y=0,
                            animal_last_x=0,
                            animal_last_y=0";
                if($uploadFilename)
                    $query .= ", animal_image_path='$uploadFilename'";
            }
            else { //Updating existing animal
                $query = "UPDATE animals SET
                            animal_name='$animal_name',
                            animal_kingdom='$kingdom',
                            animal_phylum='$phylum',
                            animal_subphylum='$subphylum',
                            animal_class='$class',
                            animal_order='$order',
                            animal_suborder='$suborder',
                            animal_description='$description',
                            animal_ballcolor='$color'";
                if($uploadFilename)
                    $query .= ", animal_image_path='$uploadFilename'";
                $query .= " WHERE animal_id=$id";
            }
            mysql_query($query);
        }
        else if(isset($_POST['delete_button'])) {
            $query = "DELETE FROM animals WHERE animal_id=".$_POST['animal_id'];
            mysql_query($query);
        }

        ?>
        <div id="main_div">
            <?php
                $nomonkey = true;
                $nav_items = array("home_nav" => "index.php", "users_nav"=> "admin_users.php", "cameras_nav" => "admin_camera.php", "animal_nav" => "animals.php");
                include "includes/header.php";
            ?>
            <div id="default_content_div">
                <div id="admin_sub_nav">
                    <a id="edit_animal" href="" style="cursor: hand;">Edit an Animal</a>
                    <a id="add_animal" href="" style="cursor: hand">Add an Animal</a>
                    <a id="view_animal_stats" href="admin_stats_animals.php">View Statistics</a>
                </div>
                <div class="animal_bar">
                    <div class="animal_bar_container">
	                    <ul class="animal_bar_container">
                            <?php
                            $query = "SELECT animal_id, animal_name FROM animals ORDER BY animal_name";
                            $result = mysql_query($query);
                            while($row = mysql_fetch_array($result, MYSQL_ASSOC)){
                                echo "<li><a onclick=\"updateInfo(".$row["animal_id"].")\"><span>".$row["animal_name"]."</span></a></li>";
                            }
                            ?>
	                    </ul>
                    </div>
                </div>
                <h1 id="edit_animal_title">
                    Edit an Animal
                </h1>

                <form id="save_animal" action="admin_animals.php" method="post" enctype="multipart/form-data">
                    <input id="animal_id" name="animal_id" style="display: none" />
                    <div id="animal_info_entry">
                        <label for="name">Animal Name:&nbsp;</label><input id="name" name="name" /> <br />
                        <label for="kingdom">Kingdom:&nbsp;</label><input id="kingdom" name="kingdom" /> <br />
                        <label for="phylum">Phylum:&nbsp;</label><input id="phylum" name="phylum" /> <br />
                        <label for="subphylum">Subphylum:&nbsp;</label><input id="subphylum" name="subphylum" /> <br />
                        <label for="class">Class:&nbsp;</label><input id="class" name="class" /> <br />
                        <label for="order">Order:&nbsp;</label><input id="order" name="order" /> <br />
                        <label for="suborder">Suborder:&nbsp;</label><input id="suborder" name="suborder" /> <br />
                        <label for="color">Ball Color:&nbsp;</label>
                        <select id="color" name="color" style="position:relative; left:-3px;">
                            <option value=""></option>
                            <option value="Red">Red</option>
                            <option value="Yellow">Yellow</option>
                            <option value="Orange">Orange</option>
                            <option value="Blue">Blue</option>
                            <option value="Green">Green</option>
                            <option value="Purple">Purple</option>
                        </select> <br />
                        <label for="object_id">Object ID:&nbsp;</label><input id="object_id" name="object_id" disabled="disabled" />
                    </div>
                    <label for="animal_description" style="float:left; margin-top:5px; width: 70px;">Description:&nbsp;</label><textarea id="animal_description" name="animal_description"></textarea>
                    <label for="image">Image:&nbsp;</label><input id="image" type="file" name="image" />
                    <br />
                    <img id="image_display" alt="Image Preview" style="margin-top: 5px; margin-left: 20px;" />
                    <br style="clear:both" />
                    <div style="text-align: center; clear:both">
                        <input id="save_button" name="save_button" type="submit" value="Save" />
                        <input id="delete_button" name="delete_button" type="submit" value="Delete" disabled="disabled" />
                    </div>
                </form>
            </div>
            <?php include "includes/footer.php" ?>
        </div>
        <?php
            //check query string for mode, disappear animal bar if its add
            if(strcmp($_GET["mode"], "add") == 0) {
                echo "<script> 
                        $('div.animal_bar, div.animal_bar_container').css({'height': '0px'});
                        $('#edit_animal_title').html('Add an Animal');
                        $('#image_display').css('display','none');
                        $('#animal_id').val('0');
                      </script>";
            }
            else if($_GET["id"]) {
                echo "<script>
                        updateInfo(".$_GET['id'].");
                      </script>";
            }

            include "includes/closedb.php"
        ?>
    </body>
</html>
