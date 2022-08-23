var intervalIdSound = window.setInterval(updateSound, 1000);
var intervalIdHistory = window.setInterval(updateHistory, 5000);
var historyData = [6, 6, 6, 6, 6];
var ctx = document.getElementById("historyChart");
var myLineChart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: ["-55 s", "-50 s", "-45 s", "-40 s", "-35 s", "-30 s", "-25 s", "-20 s", "-15 s", "-10 s", "-5 s", "0 s"],
    datasets: [{
      label: "Volume",
      lineTension: 0.3,
      backgroundColor: "rgba(78, 115, 223, 0.05)",
      borderColor: "rgba(78, 115, 223, 1)",
      pointRadius: 3,
      pointBackgroundColor: "rgba(78, 115, 223, 1)",
      pointBorderColor: "rgba(78, 115, 223, 1)",
      pointHoverRadius: 3,
      pointHoverBackgroundColor: "rgba(78, 115, 223, 1)",
      pointHoverBorderColor: "rgba(78, 115, 223, 1)",
      pointHitRadius: 10,
      pointBorderWidth: 2,
      data: historyData
    }],
  },
  options: {
    maintainAspectRatio: false,
    layout: {
      padding: {
        left: 10,
        right: 25,
        top: 25,
        bottom: 0
      }
    },
    scales: {
      xAxes: [{
        time: {
          unit: 'sec'
        },
        gridLines: {
          display: false,
          drawBorder: false
        },
        ticks: {
          maxTicksLimit: 7
        }
      }],
      yAxes: [{
        ticks: {
          maxTicksLimit: 5,
          padding: 10,
          suggestedMax : 100,
          suggestedMin : 40,
          // Include a dollar sign in the ticks
          callback: function(value, index, values) {
            return number_format(value) + ' dB';
          }
        },
        gridLines: {
          color: "rgb(234, 236, 244)",
          zeroLineColor: "rgb(234, 236, 244)",
          drawBorder: false,
          borderDash: [2],
          zeroLineBorderDash: [2]
        }
      }],
    },
    legend: {
      display: false
    },
    tooltips: {
      backgroundColor: "rgb(255,255,255)",
      bodyFontColor: "#858796",
      titleMarginBottom: 10,
      titleFontColor: '#6e707e',
      titleFontSize: 14,
      borderColor: '#dddfeb',
      borderWidth: 1,
      xPadding: 15,
      yPadding: 15,
      displayColors: false,
      intersect: false,
      mode: 'index',
      caretPadding: 10,
      callbacks: {
        label: function(tooltipItem, chart) {
          var datasetLabel = chart.datasets[tooltipItem.datasetIndex].label || '';
          return datasetLabel + ': ' + number_format(tooltipItem.yLabel) + ' dB';
        }
      }
    }
  }
});

async function updateSound(){
    const response = await fetch("/sound");
    const respJson = await response.json();
    document.getElementById("soundText").textContent = respJson.sound + " dB";
}

async function updateHistory(){
  const response = await fetch("/history");
  const respJson = await response.json();
  console.log(respJson);
  for (let i = 0; i < respJson.size; i++){
    historyData[i] = respJson.history[i];
  }
  myLineChart.update();
}