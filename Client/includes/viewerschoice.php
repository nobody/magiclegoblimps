<?php
include "config.php";
include "opendb.php";

echo "<table>";
$query = "SELECT total_votes_num FROM total_votes WHERE total_votes_id = 1";
$result = mysql_query($query);
$row = mysql_fetch_array($result);
$total_votes = $row['total_votes_num'];

$query = "SELECT animal_name, animal_num_views, animal_id FROM animals ORDER BY animal_num_views DESC";
$result = mysql_query($query);

while($row = mysql_fetch_array($result, MYSQL_ASSOC))
{
        $popularity = (100 * ($row['animal_num_views']/$total_votes));
        $filename = strtolower($row['animal_name']);
        echo "<tr><td><img src=\"images/viewerschoice_animals_".$filename.".png\"></img></td><td><a href=\"animals.php?id={$row['animal_id']}\">{$row['animal_name']}</a></td><td>".round($popularity, 2)."%</td></tr>";
}
echo "</table>";

include "closedb.php";
?>