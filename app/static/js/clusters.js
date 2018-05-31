var xmlhttp = new XMLHttpRequest();
var day = [];
var hour = [];
var pressure = []
var temperature = []
var humidity = []
var gas = []


function sum(array) {
	var sum = 0.0;
    for (var i = 0; i < array.length-1; i++) {
		sum += parseFloat(array[i]);
	}
	return sum;
};

function mean(array) {
	var arraySum = sum(array);
  	return parseFloat(arraySum / parseFloat(array.length));
};


xmlhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        var last_week = new Set();
        for (var i = 0; i < myObj.length; i++) {
        	//console.log(myObj[i])
        	var date = myObj[i].date;
        	if (last_week.length <= 7) {
        		if (!last_week.has(date)) 
        			last_week.add(date);
        	}
        	if (last_week.has(date)) {
				var measures = myObj[i].measure[0];
				//console.log(measures);
	    		
	    		day.push(date)
	    		hour.push(measures.hour)
				pressure.push(measures.pressure)
				temperature.push(measures.temperature)
				humidity.push(measures.humidity)
				gas.push(measures.gas)
			}	
        }
        
    }

    var d3 = Plotly.d3;
    var WIDTH_IN_PERCENT_OF_PARENT = 100,
    HEIGHT_IN_PERCENT_OF_PARENT = 60;

	var gd3 = d3.select("div[id='24h']")
	    .style({
	        width: WIDTH_IN_PERCENT_OF_PARENT + '%',
	        'margin-left': (100 - WIDTH_IN_PERCENT_OF_PARENT) / 2 + '%',

	        height: HEIGHT_IN_PERCENT_OF_PARENT + 'vh',
	        'margin-top': (100 - HEIGHT_IN_PERCENT_OF_PARENT) / 10 + 'vh'
	    });

	var gd = gd3.node();

    var trace1 = {
	  x: day, 
	  y: pressure,
	  type: 'bar',
	  name: 'Pressure',
	  marker: {
	    color: 'rgb(49,130,189)'
	  }
	};

	var trace2 = {
	  x: day,
	  y: temperature,
	  type: 'bar',
	  name: 'Temperature',
	  marker: {
	    color: 'rgb(204,204,204)'
	  }
	};

	var trace3 = {
	  x: day,
	  y: humidity,
	  type: 'bar',
	  name: 'Humidity',
	  marker: {
	    color: 'rgb(29,199,234)'
	  }
	};

	var trace4 = {
	  x: day,
	  y: gas,
	  type: 'bar',
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
	  barmode: 'group',
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
xmlhttp.open("GET", 'api/measures', true);
xmlhttp.send();

