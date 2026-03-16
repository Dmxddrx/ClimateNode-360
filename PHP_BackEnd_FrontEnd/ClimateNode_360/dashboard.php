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

/* === LAYOUT === */
.header-area {
    text-align: center;
    padding-top: 10px;
}

.dashboard-container {
    display: flex;
    padding: 20px 40px 40px 40px;
    gap: 40px;
}

.left-column {
    width: 350px;
    display: flex;
    flex-direction: column;
    gap: 30px;
}

.right-column {
    flex: 1;
    display: flex;
    flex-direction: column;
    gap: 30px;
}

/* === GRAPH CARDS === */
.graph-card {
    background: linear-gradient(145deg, #1c1f2e, #141626);
    border-radius: 25px;
    padding: 30px;
    box-shadow: 0 15px 35px rgba(0,0,0,0.6);
}

.graph-card h3 {
    margin-top: 0;
    margin-bottom: 20px;
    font-weight: 400;
    letter-spacing: 1px;
}

canvas {
    height: 220px !important;
    /*width: 100% !important;*/
}

.chart-wrapper {
    position: relative;
    width: 100%;
    height: 220px;
}

/* === ACTIVE CARD === */
.node-widget.active {
    box-shadow: 0 0 25px rgba(0,140,255,0.6);
}

/* === LIVE BADGE === */
.live-badge {
    display: inline-flex;
    align-items: center;
    gap: 6px;
    font-size: 12px;
    color: #4cc9f0;
    letter-spacing: 2px;
    margin-top: 4px;
    opacity: 0.8;
}

.live-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: #4cc9f0;
    animation: livePulse 1.5s infinite;
}

@keyframes livePulse {
    0%, 100% { opacity: 1;   transform: scale(1);   }
    50%       { opacity: 0.3; transform: scale(0.7); }
}

#last-refreshed {
    font-size: 12px;
    opacity: 0.35;
    margin-top: 5px;
    letter-spacing: 1px;
}

/* === PLACEHOLDER === */
.placeholder {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    height: 280px;
    opacity: 0.3;
    font-size: 15px;
    gap: 12px;
    letter-spacing: 1px;
}

.placeholder-icon {
    font-size: 48px;
}

/* === CHARTS AREA === */
#charts-area {
    display: none;
    flex-direction: column;
    gap: 30px;
}

/* === SUMMARY ROW === */
.summary-row {
    display: flex;
    gap: 16px;
}

.summary-pill {
    flex: 1;
    background: rgba(255,255,255,0.05);
    border-radius: 16px;
    padding: 14px 18px;
    font-size: 13px;
    text-align: center;
    letter-spacing: 0.5px;
}

.summary-pill .pill-value {
    font-size: 22px;
    font-weight: 300;
    display: block;
    margin-top: 4px;
}

</style>
</head>

<body>

<!-- HEADER -->
<div class="header-area">
    <h1 class="glass-panel-title">ClimateNode 360</h1>
    <div class="live-badge">
        <div class="live-dot"></div>
        LIVE
    </div>
    <div id="last-refreshed">Connecting…</div>
</div>

<div class="dashboard-container">

    <!-- LEFT: NODE CARDS -->
    <div class="left-column">

    <?php while ($row = $nodes_result->fetch_assoc()): ?>
    <?php
        $id   = $row['node_id'];
        $name = $nodeNames[$id] ?? "Node $id";
        $temp = round($row['temp']);
    ?>
    <div class="node-widget"
         id="node-card-<?php echo $id; ?>"
         onclick="selectNode(<?php echo $id; ?>, this)">

        <div class="node-info">
            <div class="node-name"><?php echo $name; ?></div>
            <div class="node-id-label">NODE <?php echo $id; ?></div>

            <div id="node-icon-<?php echo $id; ?>" style="font-size:3em; margin-top:10px;">
                <?php echo ($temp >= 28) ? '☀️' : '❄️'; ?>
            </div>

            <div id="node-temp-<?php echo $id; ?>"
                 class="main-temp <?php echo ($temp >= 28) ? 'temp-hot' : 'temp-cold'; ?>">
                <?php echo $temp; ?>°
            </div>

            <div id="node-time-<?php echo $id; ?>" class="timestamp">
                <?php echo date('d M Y, H:i', strtotime($row['created_at'])); ?>
            </div>

            <div id="node-hum-<?php echo $id; ?>"  class="sub-data">💧 <?php echo $row['hum']; ?>%</div>
            <div id="node-dust-<?php echo $id; ?>" class="sub-data">🌫️ <?php echo $row['dust']; ?> µg/m³</div>
        </div>

    </div>
    <?php endwhile; ?>

    </div>

    <!-- RIGHT: CHARTS -->
    <div class="right-column">

        <!-- Shown when no node is selected -->
        <div id="select-placeholder" class="graph-card">
            <div class="placeholder">
                <div class="placeholder-icon">📡</div>
                <div>Select a node to view analytics</div>
            </div>
        </div>

        <!-- Charts (hidden until a node is selected) -->
        <div id="charts-area">

            <!-- Summary pills -->
            <div class="graph-card">
                <h3 id="title">Node Analytics</h3>
                <div class="summary-row">
                    <div class="summary-pill">
                        🌡️ Temperature
                        <span class="pill-value" id="pill-temp">—</span>
                    </div>
                    <div class="summary-pill">
                        💧 Humidity
                        <span class="pill-value" id="pill-hum">—</span>
                    </div>
                    <div class="summary-pill">
                        🌫️ Dust
                        <span class="pill-value" id="pill-dust">—</span>
                    </div>
                </div>
            </div>

            <div class="graph-card">
                <h3>Today — Temperature (°C)</h3>
                <canvas id="tempChart"></canvas>
            </div>

            <div class="graph-card">
                <h3>Today — Humidity (%)</h3>
                <canvas id="humChart"></canvas>
            </div>

            <div class="graph-card">
                <h3>Today — Dust (µg/m³)</h3>
                <canvas id="dustChart"></canvas>
            </div>

            <div class="graph-card">
                <h3>Last 7 Days — Avg Temperature</h3>
                <canvas id="weekChart"></canvas>
            </div>

        </div>

    </div>
</div>

<script>

let currentNode = null;
let charts      = {};

const CARD_REFRESH_MS  = 10000;   // refresh cards every 10 s
const CHART_REFRESH_MS = 30000;   // refresh charts every 30 s

// ─── Select a node ────────────────────────────────────────────────────────────
function selectNode(nodeId, element) {
    currentNode = nodeId;

    document.querySelectorAll('.node-widget').forEach(el => el.classList.remove('active'));
    element.classList.add('active');

    const nodeName = element.querySelector('.node-name').innerText;
    document.getElementById('title').innerText = nodeName + ' — Analytics';

    document.getElementById('select-placeholder').style.display = 'none';
    document.getElementById('charts-area').style.display        = 'flex';

    loadToday();
    loadWeek();
}

// ─── Fetch & draw today's charts ─────────────────────────────────────────────
async function loadToday() {
    try {
        const res  = await fetch("api_today.php?node=" + currentNode);
        const data = await res.json();

        // Update summary pills with latest reading
        const last = data.temps.length - 1;
        if (last >= 0) {
            document.getElementById('pill-temp').textContent  = data.temps[last]  + ' °C';
            document.getElementById('pill-hum').textContent   = data.hums[last]   + ' %';
            document.getElementById('pill-dust').textContent  = data.dusts[last]  + ' µg';
        }

        drawChart("tempChart",  data.labels, data.temps,  "#ff4d6d", "rgba(255,77,109,0.15)");
        drawChart("humChart",   data.labels, data.hums,   "#4cc9f0", "rgba(76,201,240,0.12)");
        drawChart("dustChart",  data.labels, data.dusts,  "#f8961e", "rgba(248,150,30,0.12)");
    } catch(e) {
        console.warn("loadToday error:", e);
    }
}

async function loadWeek() {
    try {
        const res  = await fetch("api_week.php?node=" + currentNode);
        const data = await res.json();
        drawChart("weekChart", data.labels, data.avgTemps, "#90e0ef", "rgba(144,224,239,0.1)");
    } catch(e) {
        console.warn("loadWeek error:", e);
    }
}

// ─── Draw / redraw a Chart.js line chart ─────────────────────────────────────
function drawChart(id, labels, data, borderColor, bgColor) {
    if (charts[id]) charts[id].destroy();

    charts[id] = new Chart(document.getElementById(id), {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                data:            data,
                borderColor:     borderColor,
                backgroundColor: bgColor,
                tension:         0.35,
                fill:            true,
                pointRadius:     3,
                pointHoverRadius: 6,
                borderWidth:     2
            }]
        },
        options: {
            responsive: true,
            interaction: { mode: 'index', intersect: false },
            plugins: {
                legend:  { display: false },
                tooltip: {
                    backgroundColor: 'rgba(20,22,38,0.95)',
                    titleColor:      '#aaa',
                    bodyColor:       '#fff',
                    borderColor:     borderColor,
                    borderWidth:     1,
                    padding:         10
                }
            },
            scales: {
                x: {
                    ticks: { color: "#aaa", maxTicksLimit: 8 },
                    grid:  { color: "rgba(255,255,255,0.05)" }
                },
                y: {
                    ticks: { color: "#aaa" },
                    grid:  { color: "rgba(255,255,255,0.05)" }
                }
            }
        }
    });
}

// ─── Auto-refresh all node cards via api_nodes.php ───────────────────────────
async function refreshCards() {
    try {
        const res   = await fetch("api_nodes.php");
        const nodes = await res.json();

        nodes.forEach(node => {
            const id    = node.node_id;
            const isHot = node.temp >= 28;

            const tempEl = document.getElementById('node-temp-'  + id);
            const iconEl = document.getElementById('node-icon-'  + id);
            const timeEl = document.getElementById('node-time-'  + id);
            const humEl  = document.getElementById('node-hum-'   + id);
            const dustEl = document.getElementById('node-dust-'  + id);

            if (tempEl) {
                tempEl.textContent = node.temp + '°';
                tempEl.className   = 'main-temp ' + (isHot ? 'temp-hot' : 'temp-cold');
                // trigger pop animation
                tempEl.classList.remove('pop');
                void tempEl.offsetWidth;             // force reflow
                tempEl.classList.add('pop');
            }
            if (iconEl)  iconEl.textContent  = isHot ? '☀️' : '❄️';
            if (timeEl)  timeEl.textContent  = node.created_at;
            if (humEl)   humEl.textContent   = '💧 ' + node.hum  + '%';
            if (dustEl)  dustEl.textContent  = '🌫️ ' + node.dust + ' µg/m³';
        });

        document.getElementById('last-refreshed').textContent =
        'Updated at ' + new Date().toLocaleString('en-GB', {
            day: '2-digit', month: 'short', year: 'numeric',
            hour: '2-digit', minute: '2-digit', second: '2-digit'
        });

    } catch (e) {
        document.getElementById('last-refreshed').textContent = '⚠️ Connection error';
    }
}

// ─── Timers ───────────────────────────────────────────────────────────────────
setInterval(refreshCards, CARD_REFRESH_MS);

setInterval(() => {
    if (currentNode !== null) {
        loadToday();
        loadWeek();
    }
}, CHART_REFRESH_MS);

// Run once on load so "last updated" shows immediately
refreshCards();

</script>
</body>
</html>