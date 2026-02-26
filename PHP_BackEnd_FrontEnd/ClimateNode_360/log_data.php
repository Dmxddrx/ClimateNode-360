<?php
$servername = "localhost";
$username = "root"; 
$password = "";     
$dbname = "climatenode_360"; 

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // These MUST match your serverconfig.cpp String postData
    $node = $_POST['node_id'];
    $t    = $_POST['temp'];
    $h    = $_POST['hum'];
    $d    = $_POST['dust'];

    // These MUST match your phpMyAdmin column names exactly
    $sql = "INSERT INTO sensor_network (node_id, temp, hum, dust) 
            VALUES ('$node', '$t', '$h', '$d')";

    if ($conn->query($sql) === TRUE) {
        echo "ClimateNode_" . $node . " sync successful";
    } else {
        echo "Error: " . $conn->error;
    }
} else {
    echo "ClimateNode_360 Backend is Active.";
}
$conn->close();
?>