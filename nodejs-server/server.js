const express = require('express');
const app = express();
var bodyParser = require('body-parser');
var server = require('http').Server(app);
var io = require('socket.io')(server);
var path = require('path');
var os = require('os');
var accelData;
var accelX;
var accelY;
var accelZ;
var temp;
var gyroX;
var gyroY;
var gyroZ;
var ampData;
var angleData;
var botaoEmergencia;
var quedaDetectada;
var battLevel;
var battVoltage;
var interfaces = os.networkInterfaces();
var addresses = [];
var nodeIP;
for (var k in interfaces) {
  for (var k2 in interfaces[k]) {
    var address = interfaces[k][k2];
    if (address.family === 'IPv4' && !address.internal) {
      addresses.push(address.address);
    }
  }
}

app.use(bodyParser.json())
app.use(express.static(path.join(__dirname, '/public')));

server.listen(3000, process.argv[2], function () {
  var host = server.address().address
  var port = server.address().port
  console.log("Server listening on " + host + ":" + port)
});

app.get('/', function (req, res) {
  res.sendFile(__dirname + '/index.html');
});

app.post('/accel', function (req, res) {
  //console.log("Tensão da bateria: " + req.body.data.infoDev.nodeID)
  //console.log("Tensão da bateria (variável): " + req.body.data.infoDev.quedaDetectada)

  nodeIP = req.body.data.infoDev.nodeIP
  accelData = req.body
  accelX = req.body.data.accel.accelX
  accelY = req.body.data.accel.accelY
  accelZ = req.body.data.accel.accelZ
  temp = req.body.data.temp
  gyroX = req.body.data.gyro.gyroX
  gyroY = req.body.data.gyro.gyroY
  gyroZ = req.body.data.gyro.gyroZ
  ampData = req.body.data.infoDev.ampData
  botaoEmergencia = req.body.data.infoDev.botaoEmergencia
  quedaDetectada = req.body.data.infoDev.quedaDetectada
  battVoltage = req.body.data.battery.battVoltage
  battLevel = req.body.data.battery.battLevel
  angleData = req.body.data.infoDev.angleData

  io.emit('accelData', accelData);
  io.emit('accelX', accelX);
  io.emit('accelY', accelY);
  io.emit('accelZ', accelZ);
  io.emit('temp', temp);
  io.emit('gyroX', gyroX);
  io.emit('gyroY', gyroY);
  io.emit('gyroZ', gyroZ);
  io.emit('ampData', ampData);
  io.emit('botaoEmergencia', botaoEmergencia);
  io.emit('quedaDetectada', quedaDetectada);
  io.emit('battLevel', battLevel);
  io.emit('battVoltage', battVoltage);
  io.emit('angleData', angleData);
  io.emit('nodeIP', nodeIP);
});


io.on('connection', function (socket) {
  socket.on('location', function (data) {
    console.log(data);
  });
  socket.on('accelData', function (data) {
    console.log(data);
  });
});