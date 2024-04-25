//var gateway = `ws://${window.location.hostname}/ws`;
var gateway = `ws://${window.location.hostname}/ws`;
var stopStartButton = document.getElementById("stopstartbutton");
var reAverageButton = document.getElementById("reaveragebutton");
var statusHeader = document.getElementById("highlight");
var slider = document.getElementById("myrange");
var output = document.getElementById("valuenum");
output.innerHTML = parseFloat(slider.value).toFixed(1);

console.log(gateway);
var websocket;
// Init web socket when the page loads
window.addEventListener("load", onload);

reAverageButton.addEventListener("click", reAverage);

function onload(event) {
  initWebSocket();
  setEmail();
}

function getReadings() {
  websocket.send("getReadings");
}

function getSensitivity() {
  websocket.send("getSensitivity");
}

function getStopStart() {
  websocket.send("getStopStart");
}

slider.oninput = function () {
  output.innerHTML = parseFloat(slider.value).toFixed(1);
};

slider.onchange = function () {
  var sliderData = JSON.stringify({ sensitivity: parseFloat(slider.value) });
  setSensitivity(sliderData);
};

function setSensitivity(sliderData) {
  websocket.send(sliderData);
}

function setEmail() {
  document.getElementById("saveButton").addEventListener("click", function () {
    var userEmail = document.getElementById("email").value; // Retrieve email value
    websocket.send(userEmail);
  });
}

function setSensitivitySlider(initialSliderData) {
  output.innerHTML = parseFloat(initialSliderData.sensitivity[0]).toFixed(1);
  slider.value = initialSliderData.sensitivity[0].toFixed(1);
}

function setStopStart(initialStopStartData) {
  if (initialStopStartData.stopstartdata[0] == 0) {
    stopStartButton.addEventListener("click", Start);
    stopStartButton.value = "Start";
    statusHeader.innerText = "Stopped";
    statusHeader.style.color = "#b30000";
  }
  if (initialStopStartData.stopstartdata[0] == 1) {
    stopStartButton.addEventListener("click", Stop);
    stopStartButton.value = "Stop";
    statusHeader.innerText = "Running";
    statusHeader.style.color = "#00b300";
  }
}

function Start() {
  console.log("Started");
  stopStartButton.removeEventListener("click", Start);
  stopStartButton.addEventListener("click", Stop);
  stopStartButton.value = "Stop";
  statusHeader.innerText = "Running";
  statusHeader.style.color = "#00b300";
  websocket.send("Start");
}

function Stop() {
  console.log("Stopped");
  stopStartButton.removeEventListener("click", Stop);
  stopStartButton.addEventListener("click", Start);
  stopStartButton.value = "Start";
  statusHeader.innerText = "Stopped";
  statusHeader.style.color = "#b30000";
  websocket.send("Stop");
}

function reAverage() {
  websocket.send("reAverage");
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
  getSensitivity();
  getStopStart();
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
            backgroundColor: function (context) {
              const chart = context.chart;
              const { ctx, chartArea } = chart;

              if (!chartArea) {
                // This case happens on initial chart load
                return null;
              }
              return getGradient(ctx, chartArea);
            },
            borderColor: "rgb(75, 192, 192)",
            tension: 0.1,
            pointRadius: 0, // Remove data points
          },
        ],
      },
      options: {
        scales: {
          y: {
            ticks: {
              color: "#e5e5e5",
              font: {
                size: 18,
              },
            },
            grid: {
              color: "#3e3f3f",
            },
            beginAtZero: true,
            title: {
              display: true,
              text: "Magnitude",
              color: "#e5e5e5",
              font: {
                size: 18,
              },
            },
          },
          x: {
            ticks: {
              color: "#e5e5e5",
              font: {
                size: 18,
              },
            },
            grid: {
              color: "#3e3f3f",
            },
            type: "linear",
            position: "bottom",
            title: {
              display: true,
              text: "Frequency (Hz)",
              color: "#e5e5e5",
              font: {
                size: 18,
              },
            },
            max: 1022,
          },
        },
        plugins: {
          legend: {
            display: false, // Optionally hide the legend
          },
        },
        layout: {
          padding: 20,
        },
        elements: {
          line: {
            borderWidth: 3, // Make the line thicker
          },
        },
        backgroundColor: "#000000", // Dark background
      },
    });
  }
}

// Function to create a gradient
function getGradient(ctx, chartArea) {
  const gradient = ctx.createLinearGradient(
    0,
    chartArea.bottom,
    0,
    chartArea.top
  );
  gradient.addColorStop(0, "rgba(255, 0, 255, 0.2)");
  gradient.addColorStop(0.5, "rgba(0, 255, 255, 0.2)");
  gradient.addColorStop(1, "rgba(255, 255, 0, 0.2)");
  return gradient;
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
  //console.log("Event Data: ", event.data);
  var myObj = JSON.parse(event.data);
  var keys = Object.keys(myObj);
  console.log("Parsed Object: ", myObj);
  if (myObj.hasOwnProperty("magnitude")) {
    updateChart(myObj);
  }
  if (myObj.hasOwnProperty("sensitivity")) {
    setSensitivitySlider(myObj);
  }
  if (myObj.hasOwnProperty("stopstartdata")) {
    setStopStart(myObj);
  }
}
