<?php
$conn = new mysqli("localhost", "root", "", "climatenode_360");

$nodeNames = [
    0 => "Outside - Court",
    1 => "Workshop AC",
    2 => "Workshop Non-AC",
    3 => "Court"
];

$nodes_query = "SELECT * FROM sensor_network s1 
                 WHERE created_at = (SELECT MAX(created_at) FROM sensor_network s2 WHERE s1.node_id = s2.node_id)
                 ORDER BY node_id ASC";
$nodes_result = $conn->query($nodes_query);
?>

<!DOCTYPE html>
<html>
<head>
    <title>ClimateNode 360 | Interactive</title>
    <link rel="stylesheet" href="style.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>

<script>
setInterval(() => {
    location.reload();
}, 60000); // refresh every 60 seconds
</script>


<body>

<h1 class="glass-panel-title">ClimateNode 360</h1>

<div class="nodes-grid" id="nodeGrid">
    <?php while($row = $nodes_result->fetch_assoc()): ?>
        <div class="node-widget" id="node-<?php echo $row['node_id']; ?>" onclick="toggleNode(<?php echo $row['node_id']; ?>)">
            
            <div class="node-info">

                <div class="node-name">
                    <?php 
                        $id = $row['node_id'];
                        echo isset($nodeNames[$id]) ? $nodeNames[$id] : "Node $id";
                    ?>
                </div>

                <div class="node-id-label">
                    Node ID: <?php echo $row['node_id']; ?>
                </div>
                
                <div style="font-size: 3em;"><?php echo ($row['temp'] > 28) ? '☀️' : '❄'; ?></div> 
                <div class="main-temp"><?php echo round($row['temp']); ?>°</div>

                <div class="timestamp"><?php echo date('H:i:s', strtotime($row['created_at'])); ?></div>
                
                <div class="sub-data">
                    <div class="data-label">💧 Humidity</div>
                    <div class="data-value"><?php echo $row['hum']; ?>%</div>
                </div>
                <div class="sub-data">
                    <div class="data-label">🌫️ Dust</div>
                    <div class="data-value"><?php echo $row['dust']; ?> µg/m³</div>
                </div>
                
            </div>

            <div class="inner-chart-container">
                <canvas id="canvas-<?php echo $row['node_id']; ?>"></canvas>
            </div>
        </div>
    <?php endwhile; ?>
</div>

<script>
let activeChart = null;
let activeNodeId = null;
let updateInterval = null;

async function toggleNode(nodeId) {

    const target = document.getElementById('node-' + nodeId);
    const expanded = target.classList.contains('expanded');

    if (updateInterval) clearInterval(updateInterval);

    document.querySelectorAll('.node-widget')
        .forEach(el => el.classList.remove('expanded'));

    if (!expanded) {
        target.classList.add('expanded');
        await refreshData(nodeId);

        updateInterval = setInterval(() => {
            refreshData(nodeId);
        }, 5000);
    } else {
        if (activeChart) activeChart.destroy();
        activeChart = null;
        activeNodeId = null;
    }
}

async function refreshData(nodeId) {
    try {
        const res = await fetch(`get_history.php?node=${nodeId}`);
        const data = await res.json();

        if (!data.labels || !data.labels.length) return;

        updateText(nodeId, data);
        renderChart(nodeId, data.labels, data.temps, data.hums);
    } catch (err) {
        console.log("Fetch error:", err);
    }
}

function updateText(nodeId, data) {
    const box = document.getElementById('node-' + nodeId);
    const last = data.temps.length - 1;
    const temp = Math.round(data.temps[last]);

    const tempEl = box.querySelector('.main-temp');
    tempEl.innerText = temp + "°";
    tempEl.classList.add("pop");
    setTimeout(() => tempEl.classList.remove("pop"), 400);

    tempEl.classList.remove("temp-hot","temp-cold");
    if (temp >= 28) tempEl.classList.add("temp-hot");
    else tempEl.classList.add("temp-cold");

    box.querySelector('.timestamp').innerText =
        "Live: " + data.labels[last];

    box.querySelectorAll('.data-value')[1].innerText =
        data.dusts[last];
}

function renderChart(nodeId, labels, temps, hums) {

    const ctx = document
        .getElementById('canvas-' + nodeId)
        .getContext('2d');

    if (activeChart && activeNodeId === nodeId) {
        activeChart.data.labels = labels;
        activeChart.data.datasets[0].data = temps;
        activeChart.data.datasets[1].data = hums;
        activeChart.update();
        return;
    }

    if (activeChart) activeChart.destroy();

    activeNodeId = nodeId;

    activeChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [
                {
                    data: temps,
                    tension: 0.4,
                    fill: true,
                    borderColor: '#ff4d6d',
                    backgroundColor: (context) => {
                        const chart = context.chart;
                        const {ctx, chartArea} = chart;
                        if (!chartArea) return null;
                        const gradient = ctx.createLinearGradient(0, chartArea.top, 0, chartArea.bottom);
                        gradient.addColorStop(0, "rgba(255,77,109,0.5)");
                        gradient.addColorStop(1, "rgba(255,77,109,0)");
                        return gradient;
                }
                },
                {
                    data: hums,
                    borderColor: '#4cc9f0',
                    tension: 0.4
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: {
                duration: 900,
                easing: 'easeOutQuart'
            },
            plugins: { legend: { display: false } },
            scales: {
                y: {
                    grid: { color: 'rgba(255,255,255,0.05)' },
                    ticks: { color: '#aaa' }
                },
                x: {
                    grid: { color: 'rgba(255,255,255,0.05)' },
                    ticks: { color: '#aaa' }
                }
            }
        }
    });
}
</script>


</body>
</html>