const mongoose = require('mongoose');

const DataSchema = new mongoose.Schema({
  nodeId: Number,          // Master = 0, slaves = 1,2,...
  temperature: Number,
  humidity: Number,
  dust: Number,
  timestamp: { type: Date, default: Date.now }
});

module.exports = mongoose.model("SensorData", DataSchema);