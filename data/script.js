//var gateway = `ws://${window.location.hostname}/ws`;
var gateway = `ws://${window.location.hostname}/ws`;
console.log(gateway);
var websocket;
// Init web socket when the page loads
window.addEventListener("load", onload);

function onload(event) {
  initWebSocket();
}

function getReadings() {
  websocket.send("getReadings");
}

function initWebSocket() {
  console.log("Trying to open a WebSocket connection…");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

// When websocket is established, call the getReadings() function
function onOpen(event) {
  console.log("Connection opened");
  getReadings();
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

function updateChart(data) {
  // Assuming data is an object with freq and magnitude arrays
  var chartData = data.freq.map(function (d, i) {
    return { x: d, y: data.magnitude[i] }; // Create an object for each data point
  });

  if (window.chart) {
    // Update chart if it exists
    window.chart.data.datasets[0].data = chartData; // Update the chart data
    console.log(
      "Chart updated with new dataset: ",
      window.chart.data.datasets[0].data
    );
    window.chart.update();
  } else {
    // Create chart if it doesn't exist
    const ctx = document.getElementById("myChart");
    window.chart = new Chart(ctx, {
      type: "line",
      data: {
        datasets: [
          {
            label: "Frequency vs Magnitude",
            data: chartData, // Use the transformed chart data here
            fill: true,
            borderColor: "rgb(75, 192, 192)",
            tension: 0.1,
          },
        ],
      },
      options: {
        scales: {
          y: {
            beginAtZero: true,
            title: {
              display: true,
              text: "Magnitude",
            },
          },
          x: {
            type: "linear",
            position: "bottom",
            title: {
              display: true,
              text: "Frequency (Hz)",
            },
          },
        },
      },
    });
  }
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
  //console.log("Event Data: ", event.data);
  var myObj = JSON.parse(event.data);
  var keys = Object.keys(myObj);
  var data = [myObj.freq, myObj.magnitude];
  console.log("Parsed Object: ", myObj);
  updateChart(myObj);
}
