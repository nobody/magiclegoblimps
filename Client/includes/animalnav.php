<?php
include "config.php";
include "opendb.php";

$query = "SELECT animal_id, animal_name FROM animals ORDER BY animal_name";
$result = mysql_query($query);
while($row = mysql_fetch_array($result, MYSQL_ASSOC)){
	if($row['animal_kingdom'] != NULL)
	{
		echo "<li><a href=\"#\" onclick=\"updateInfo(".$row["animal_id"].")\"><span>".$row["animal_name"]."</span></a></li>";
	}
}

include "closedb.php";
?>
