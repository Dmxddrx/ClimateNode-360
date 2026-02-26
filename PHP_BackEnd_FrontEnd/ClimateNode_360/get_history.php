<?php
$conn = new mysqli("localhost", "root", "", "climatenode_360");
$nodeId = intval($_GET['node']);

$query = "SELECT created_at, temp, hum, dust 
          FROM sensor_network 
          WHERE node_id = $nodeId 
          ORDER BY created_at DESC 
          LIMIT 20";
$result = $conn->query($query);

$labels = []; $temps = []; $hums = []; $dusts = [];

while($row = $result->fetch_assoc()) {
    $labels[] = date('H:i', strtotime($row['created_at']));
    $temps[] = $row['temp'];
    $hums[] = $row['hum'];
    $dusts[] = $row['dust'];
}

echo json_encode([
    'labels' => array_reverse($labels),
    'temps' => array_reverse($temps),
    'hums' => array_reverse($hums),
    'dusts' => array_reverse($dusts)
]);
?>