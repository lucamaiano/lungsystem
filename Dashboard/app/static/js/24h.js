var xmlhttp = new XMLHttpRequest();
var hour = [];
var pressure = []
var temperature = []
var humidity = []
var gas = []

xmlhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);

        for (var i = 0; i < myObj.length && hour.length <= 24; i++) {
        	console.log(myObj[i])
        	var date = myObj[i].date;
			var measures = myObj[i].measure[0];
			console.log(measures);
    		
    		day.push(date)
    		hour.push(measures.hour)
			pressure.push(measures.pressure)
			temperature.push(measures.temperature)
			humidity.push(measures.humidity)
			gas.push(measures.gas)
        }
        /*
        console.log(hour);
        console.log(pressure);
        console.log(temperature);
        console.log(humidity);
        console.log(gas);
        */
        
    }

    var d3 = Plotly.d3;
    var WIDTH_IN_PERCENT_OF_PARENT = 100,
    HEIGHT_IN_PERCENT_OF_PARENT = 70;

	var gd3 = d3.select("div[id='clusters']")
	    .style({
	        width: WIDTH_IN_PERCENT_OF_PARENT + '%',
	        'margin-left': (100 - WIDTH_IN_PERCENT_OF_PARENT) / 2 + '%',

	        height: HEIGHT_IN_PERCENT_OF_PARENT + 'vh',
	        'margin-top': (100 - HEIGHT_IN_PERCENT_OF_PARENT) / 10 + 'vh'
	    });

	var gd = gd3.node();

    var trace1 = {
	  x: hour, 
	  y: pressure,
	  
	  type: 'scatter',
	  name: 'Pressure',
	  marker: {
	    color: 'rgb(49,130,189)'
	  }
	};

	var trace2 = {
	  x: hour,
	  y: temperature,
	  
	  type: 'scatter',
	  name: 'Temperature',
	  marker: {
	    color: 'rgb(204,204,204)'
	  }
	};

	var trace3 = {
	  x: hour,
	  y: humidity,
	  
	  type: 'scatter',
	  name: 'Humidity',
	  marker: {
	    color: 'rgb(29,199,234)'
	  }
	};

	var trace4 = {
	  x: hour,
	  y: gas,
	  
	  type: 'scatter',
	  name: 'Gas',
	  marker: {
	    color: 'rgb(251,64,75)'
	  }
	};

	var data = [trace1, trace2, trace3, trace4];

	var layout = {
	  xaxis: {
	    tickangle: -45
	  },
	  legend: {
	  	orientation: 'h',
	  	y:-0.3
	  }
	};

	Plotly.newPlot(gd, data, layout);

	window.onresize = function() {
    Plotly.Plots.resize(gd);
	};
};
xmlhttp.open("GET", "api/measures", true);
xmlhttp.send();

