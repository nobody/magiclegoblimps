<?php

include "includes/config.php";
include "includes/opendb.php";

$id = $_GET['animal_id'];
$animal_name = $_GET['name'];
$kingdom = $_GET['kingdom'];
$phylum = $_GET['phylum'];
$subphylum = $_GET['subphylum'];
$class = $_GET['class'];
$order = $_GET['order'];
$suborder = $_GET['suborder'];
$description = $_GET['animal_description'];

echo $_GET['image'];

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
                animal_num_views=0,
                animal_curr_x=0,
                animal_curr_y=0,
                animal_last_x=0,
                animal_last_y=0";
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
                animal_description='$description'
              WHERE animal_id=$id";
}
mysql_query($query);

include 'includes/closedb.php';
?>
