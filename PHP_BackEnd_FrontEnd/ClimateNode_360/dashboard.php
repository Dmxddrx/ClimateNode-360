<?php
$conn = new mysqli("localhost", "root", "", "climatenode_360");

$nodeNames = [
    0 => "Outside - Court",
    1 => "Workshop AC",
    2 => "Workshop Non-AC",
    3 => "Court"
];

$nodes_query = "SELECT * FROM sensor_network s1 
                 WHERE created_at = (
                    SELECT MAX(created_at) 
                    FROM sensor_network s2 
                    WHERE s1.node_id = s2.node_id
                 )
                 ORDER BY node_id ASC";

$nodes_result = $conn->query($nodes_query);
?>

<!DOCTYPE html>
<html>
<head>
<title>ClimateNode 360</title>
<link rel="stylesheet" href="style.css">
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<style>

/* === SPLIT LAYOUT === */

.dashboard-container{
    display:flex;
    padding:20px 40px 40px 40px;
    gap:40px;
}

/* LEFT SIDE */
.left-column{
    width:350px;
    display:flex;
    flex-direction:column;
    gap:30px;
}

/* RIGHT SIDE */
.right-column{
    flex:1;
    display:flex;
    flex-direction:column;
    gap:30px;
}

/* Graph container matching card style */
.graph-card{
    background: linear-gradient(145deg, #1c1f2e, #141626);
    border-radius:25px;
    padding:30px;
    box-shadow: 0 15px 35px rgba(0,0,0,0.6);
}

.graph-card h3{
    margin-top:0;
    margin-bottom:20px;
}

canvas{
    height:260px !important;
}

.node-widget.active{
    box-shadow: 0 0 25px rgba(0,140,255,0.6);
}

</style>
</head>

<body>

<h1 class="glass-panel-title">ClimateNode 360</h1>

<div class="dashboard-container">

<!-- LEFT SIDE -->
<div class="left-column">

<?php while($row = $nodes_result->fetch_assoc()): ?>

<?php
$id = $row['node_id'];
$name = $nodeNames[$id] ?? "Node $id";
$temp = round($row['temp']);
?>

<div class="node-widget" onclick="selectNode(<?php echo $id; ?>, this)">

    <div class="node-info">

        <div class="node-name"><?php echo $name; ?></div>
        <div class="node-id-label">Node ID: <?php echo $id; ?></div>

        <div style="font-size:3em;">
            <?php echo ($temp >= 28) ? '☀️' : '❄️'; ?>
        </div>

        <div class="main-temp <?php echo ($temp >= 28) ? 'temp-hot' : 'temp-cold'; ?>">
            <?php echo $temp; ?>°
        </div>

        <div class="timestamp">
            <?php echo date('H:i:s', strtotime($row['created_at'])); ?>
        </div>

        <div class="sub-data">💧 <?php echo $row['hum']; ?>%</div>
        <div class="sub-data">🌫️ <?php echo $row['dust']; ?> µg/m³</div>

    </div>

</div>

<?php endwhile; ?>

</div>

<!-- RIGHT SIDE -->
<div class="right-column">

<div class="graph-card">
    <h3 id="title">Select a Node</h3>
</div>

<div class="graph-card">
    <h3>Today - Temperature</h3>
    <canvas id="tempChart"></canvas>
</div>

<div class="graph-card">
    <h3>Today - Humidity</h3>
    <canvas id="humChart"></canvas>
</div>

<div class="graph-card">
    <h3>Today - Dust</h3>
    <canvas id="dustChart"></canvas>
</div>

<div class="graph-card">
    <h3>Last 7 Days (Average Temperature)</h3>
    <canvas id="weekChart"></canvas>
</div>

</div>
</div>

<script>

let currentNode=null;
let charts={};

function selectNode(nodeId,element){

    currentNode=nodeId;

    document.querySelectorAll('.node-widget')
        .forEach(el=>el.classList.remove('active'));

    element.classList.add('active');

    document.getElementById("title").innerText="Node "+nodeId+" Analytics";

    loadToday();
    loadWeek();
}

async function loadToday(){
    const res=await fetch("api_today.php?node="+currentNode);
    const data=await res.json();

    drawChart("tempChart","Temperature (°C)",data.labels,data.temps);
    drawChart("humChart","Humidity (%)",data.labels,data.hums);
    drawChart("dustChart","Dust (µg/m³)",data.labels,data.dusts);
}

async function loadWeek(){
    const res=await fetch("api_week.php?node="+currentNode);
    const data=await res.json();

    drawChart("weekChart","Weekly Avg Temp",data.labels,data.avgTemps);
}

function drawChart(id,label,labels,data){

    if(charts[id]) charts[id].destroy();

    charts[id]=new Chart(document.getElementById(id),{
        type:'line',
        data:{
            labels:labels,
            datasets:[{
                data:data,
                borderColor:"#4cc9f0",
                tension:0.3,
                fill:false
            }]
        },
        options:{
            responsive:true,
            plugins:{legend:{display:false}},
            scales:{
                x:{ticks:{color:"#aaa"},grid:{color:"rgba(255,255,255,0.05)"}},
                y:{ticks:{color:"#aaa"},grid:{color:"rgba(255,255,255,0.05)"}}
            }
        }
    });
}

</script>

</body>
</html>