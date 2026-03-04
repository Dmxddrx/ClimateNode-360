<?php
$conn = new mysqli("localhost", "root", "", "climatenode_360");

$nodeId = intval($_GET['node']);

$query = "SELECT created_at, temp, hum, dust
          FROM sensor_network
          WHERE node_id = $nodeId
          AND DATE(created_at) = CURDATE()
          ORDER BY created_at ASC";

$result = $conn->query($query);

$labels = [];
$temps = [];
$hums  = [];
$dusts = [];

while($row = $result->fetch_assoc()){
    $labels[] = date('H:i', strtotime($row['created_at']));
    $temps[]  = (float)$row['temp'];
    $hums[]   = (float)$row['hum'];
    $dusts[]  = (float)$row['dust'];
}

echo json_encode([
    "labels"=>$labels,
    "temps"=>$temps,
    "hums"=>$hums,
    "dusts"=>$dusts
]);
?>