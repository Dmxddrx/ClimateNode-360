<?php
header('Content-Type: application/json');

$conn = new mysqli("localhost", "admin", "admin", "climatenode_360");

$nodeId = intval($_GET['node']);

$query = "SELECT DATE(created_at) as day,
                 AVG(temp) as avgTemp,
                 AVG(hum)  as avgHum,
                 AVG(dust) as avgDust
          FROM sensor_network
          WHERE node_id = $nodeId
          AND created_at >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)
          GROUP BY DATE(created_at)
          ORDER BY day ASC";

$result   = $conn->query($query);
$labels   = [];
$avgTemps = [];
$avgHums  = [];
$avgDusts = [];

while ($row = $result->fetch_assoc()) {
    $labels[]   = $row['day'];
    $avgTemps[] = round($row['avgTemp'], 2);
    $avgHums[]  = round($row['avgHum'],  2);
    $avgDusts[] = round($row['avgDust'], 2);
}

echo json_encode([
    "labels"   => $labels,
    "avgTemps" => $avgTemps,
    "avgHums"  => $avgHums,
    "avgDusts" => $avgDusts
]);