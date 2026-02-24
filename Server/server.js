const express = require("express");
const mongoose = require("mongoose");
const cors = require("cors");
const Data = require("./models/Data");

const app = express();
app.use(express.json());
app.use(cors());

// Connect MongoDB
mongoose.connect("mongodb+srv://iotuser:YOUR_PASSWORD@iot-cluster.xxxxx.mongodb.net/iotdb?retryWrites=true&w=majority")
  .then(() => console.log("MongoDB Connected"))
  .catch(err => console.log(err));

// POST route for batch upload
app.post("/data", async (req, res) => {
  try {
    const allData = req.body; // Expect array of sensor data
    await Data.insertMany(allData);
    res.status(200).json({ message: "Batch saved" });
  } catch (err) {
    res.status(500).json({ error: err });
  }
});

// GET latest data
app.get("/data/latest", async (req, res) => {
  const data = await Data.find().sort({ timestamp: -1 }).limit(50);
  res.json(data);
});

app.listen(3000, () => console.log("Server running on port 3000"));