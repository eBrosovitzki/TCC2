var eventHistory = [];
var socket = io.connect();
var amplitude = [['Tempo', 'Amplitude']];							// Variável global para armazenar dados de amplitude ao longo do tempo
var angulo = [['Tempo', 'Amplitude']];
var optionsAmp = {
	title: 'Histórico do momento linear',
	curveType: 'function',
	legend: { position: 'bottom' }
};
var optionsAngle = {
	title: 'Histórico do momento angular',
	curveType: 'function',
	legend: { position: 'bottom' }
};
var battGaugeOptions = {
	width: 200, height: 100,
	redFrom: 0, redTo: 20,
	yellowFrom: 20, yellowTo: 50,
	greenFrom: 50, greenTo: 100,
	minorTicks: 5
};
var maxDataPoints = 500;											// Limita o tamanho do histórico
var dataAmp;
var dataAngle;
var dataBattery;
var botaoEmergenciaAtivado = false;
var quedaDetectadaAtivada = false;
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
var nodeIP;
//setInterval(updateSensorData, 500);

google.charts.load('current', { 'packages': ['corechart', 'gauge'] });			// Carrega a biblioteca do Google Charts
google.charts.setOnLoadCallback(function () {
	drawAmpChart();
	drawAngleChart();
	//drawBatGauge();
});


// Recebe os dados do dispositivo através do servidor NODEjs
socket.on('accelData', function (accelData) {
	accelX = accelData.data.accel.accelX
	accelY = accelData.data.accel.accelY
	accelZ = accelData.data.accel.accelZ
	temp = accelData.data.temp
	gyroX = accelData.data.gyro.gyroX
	gyroY = accelData.data.gyro.gyroY
	gyroZ = accelData.data.gyro.gyroZ
	ampData = accelData.data.infoDev.ampData
	botaoEmergencia = accelData.data.infoDev.botaoEmergencia
	quedaDetectada = accelData.data.infoDev.quedaDetectada
	battLevel = accelData.data.battery.battLevel
	battVoltage = accelData.data.battery.battVoltage
	angleData = accelData.data.infoDev.angleData
	//nodeIP = accelData.data.nodeID

	document.getElementById("txt-gauge-accelX").innerHTML = accelX.toFixed(2);
	document.getElementById("txt-gauge-accelY").innerHTML = accelY.toFixed(2);
	document.getElementById("txt-gauge-accelZ").innerHTML = accelZ.toFixed(2);
	document.getElementById("txt-gauge-temp").innerHTML = temp.toFixed(0) + "°C";
	document.getElementById("txt-gauge-gyroX").innerHTML = gyroX.toFixed(2);
	document.getElementById("txt-gauge-gyroY").innerHTML = gyroY.toFixed(2);
	document.getElementById("txt-gauge-gyroZ").innerHTML = gyroZ.toFixed(2);
	document.getElementById("info-battery-level").innerHTML = "Nível da bateria: " + battLevel + "%";
	document.getElementById("info-battery-voltage").innerHTML = "Voltagem da bateria: " + battVoltage + "V";
	//document.getElementById("info-node-ip").innerHTML = "IP do NodeMCU: " + nodeIP;
	
	//updateBatteryGauge(battLevel);
	//updateBatteryBar(battLevel);

	// Atualize o gráfico com os novos dados
	updateAmpChart([new Date(), ampData]);
	updateAngleChart([new Date(), angleData]);

	// Atualiza os dados dos sensores
	updateSensorData();

	// Se ocorrer uma queda, adicione uma entrada ao histórico de eventos
	if (quedaDetectada && !quedaDetectadaAtivada) {
		addHistoricoEvento("Queda detectada!");
		quedaDetectadaAtivada = true;  // Defina o estado como verdadeiro
	} else if (!quedaDetectada) {
		quedaDetectadaAtivada = false;  // Reset para falso 
	}

	// Se o botão de emergência for acionado, adicione uma entrada ao histórico de eventos
	if (botaoEmergencia && !botaoEmergenciaAtivado) {
		addHistoricoEvento("Botão de emergência acionado, verificar imediatamente!");
		botaoEmergenciaAtivado = true;  // Defina o estado como verdadeiro
	} else if (!botaoEmergencia) {
		botaoEmergenciaAtivado = false;  // Reset para falso quando o botão é desativado
	}
});

function drawBatGauge() {
	dataBattery = google.visualization.arrayToDataTable([
		['Label', 'Value'],
		['Bateria', battLevel]
	]);
}

// Renderiza o gráfico de amplitude inicial
function drawAmpChart() {
	dataAmp = new google.visualization.DataTable();
	dataAmp.addColumn('datetime', 'Tempo');
	dataAmp.addColumn('number', 'Amplitude');
	var chart = new google.visualization.LineChart(document.getElementById('amp-chart'));
	chart.draw(dataAmp, optionsAmp);
}

// Atualiza o gráfico de amplitude com novos dados
function updateAmpChart(newData) {
	dataAmp.addRow(newData);
	if (dataAmp.getNumberOfRows() > maxDataPoints) {
		dataAmp.removeRow(0);
	}
	var chart = new google.visualization.LineChart(document.getElementById('amp-chart'));
	chart.draw(dataAmp, optionsAmp);
}

// Renderiza o gráfico do ângulo inicial
function drawAngleChart() {
	dataAngle = new google.visualization.DataTable();
	dataAngle.addColumn('datetime', 'Tempo');
	dataAngle.addColumn('number', 'Amplitude');
	var chart = new google.visualization.LineChart(document.getElementById('angle-chart'));
	chart.draw(dataAngle, optionsAngle);
}

// Atualiza o gráfico do ângulo com novos dados
function updateAngleChart(newData) {
	dataAngle.addRow(newData);
	if (dataAngle.getNumberOfRows() > maxDataPoints) {
		dataAngle.removeRow(0);
	}
	var chart = new google.visualization.LineChart(document.getElementById('angle-chart'));
	chart.draw(dataAngle, optionsAngle);
}

// Mantém um histórico dos eventos ocorridos (máx. 10)
function addHistoricoEvento(infoEvento) {
	var currentTime = new Date();
	var formattedTime = currentTime.toLocaleString();
	var listItem = formattedTime + ": " + infoEvento;

	eventHistory.unshift(listItem);

	if (eventHistory.length > 10) {
		eventHistory.pop();
	}

	updateEventHistoryList();
}

// Atualiza a lista de histórico de eventos
function updateEventHistoryList() {
	var eventHistoryList = document.getElementById("event-history");
	eventHistoryList.innerHTML = "";

	for (var i = 0; i < eventHistory.length; i++) {
		var listItem = document.createElement("li");
		listItem.className = "list-group-item";
		listItem.innerHTML = eventHistory[i];
		eventHistoryList.appendChild(listItem);
	}
}

// Atualiza os dados dos sensores
function updateSensorData() {
	gauge_accelX.set(accelX);
	gauge_accelY.set(accelY);
	gauge_accelZ.set(accelZ);
	gauge_gyroX.set(gyroX);
	gauge_gyroY.set(gyroY);
	gauge_gyroZ.set(gyroZ);
	gauge_temp.set(temp);
}

// Atualiza o indicador de bateria
function updateBatteryGauge() {
	var chart = new google.visualization.Gauge(document.getElementById('battery-gauge'));
	chart.draw(dataAmp, battGaugeOptions);
}

function updateBatteryBar(battLevel) {
	var batteryBar = document.querySelector(".battery-fill");
	var batteryText = document.querySelector(".battery-text");

	if (batteryBar) {
		batteryBar.style.width = battLevel + "%";
		//batteryBar.classList.add("battery-fill");
	}
	if (batteryText) {
		batteryText.textContent = "Nível da bateria: " + battLevel + "%";
	}
}