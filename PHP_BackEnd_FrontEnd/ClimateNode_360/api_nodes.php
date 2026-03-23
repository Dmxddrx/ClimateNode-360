<?php
header('Content-Type: application/json');

$conn = new mysqli("localhost", "admin", "admin", "climatenode_360");

if ($conn->connect_error) {
    http_response_code(500);
    echo json_encode(["error" => "DB connection failed: " . $conn->connect_error]);
    exit;
}

$nodeNames = [
    0 => "Outside - Court",
    1 => "Workshop AC",
    2 => "Workshop Non-AC",
    3 => "Court"
];

$query = "SELECT * FROM sensor_network s1
          WHERE created_at = (
              SELECT MAX(created_at)
              FROM sensor_network s2
              WHERE s1.node_id = s2.node_id
          )
          ORDER BY node_id ASC";

$result = $conn->query($query);

if (!$result) {
    http_response_code(500);
    echo json_encode(["error" => "Query failed: " . $conn->error]);
    exit;
}

$nodes = [];

while ($row = $result->fetch_assoc()) {
    $id      = (int) $row['node_id'];
    $nodes[] = [
        'node_id'    => $id,
        'name'       => $nodeNames[$id] ?? "Node $id",
        'temp'       => (int) round($row['temp']),
        'hum'        => (float) $row['hum'],
        'dust'       => (float) $row['dust'],
        'created_at' => date('d M Y, H:i', strtotime($row['created_at']))
    ];
}

$conn->close();
echo json_encode($nodes);