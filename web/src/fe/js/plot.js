'use strict';
//import Plotly from 'plotly.js-dist';

export class Plot {
    constructor (api_url, name) {
        this.api_url = api_url;
        this.name = name;
    }

    hello() {
        console.log(`${this.name} says hello!`);
    }

    function getData(table, startTime, endTime) {
        const query = `${this.api_url}/${table}/${startTime}/${endTime}`;
        fetch(query)
          .then((response) => response.json())
          .then((data) => {
            console.log(data);
            if (data == null) {
              console.warn("Data is null!");
              return;
            }
            const processedData = processData(data);
            updatePlot(processedData, "#80CAF6", timeViewMinutes, -2, 2);
          })
          .catch((error) => {
            console.warn(error);
          });
        }

    function processData(data) {
        const update = {
            x: [],
            y: [],
        };
        for (let i = 0; i < data.length; i++) {
            // Crude algorithm to break the line if the datapoint
            // time delta is 50% greater than the average delta.
            // This is probably not very robust.
            if (i > 3) {
            // Calculate the average of the last 3 deltas (4 points)
            const d0 = new Date(data[i].time).getTime();
            const d1 = new Date(data[i - 1].time).getTime();
            const d2 = new Date(data[i - 2].time).getTime();
            const d3 = new Date(data[i - 3].time).getTime();
            const avg = (d1 - d0 + (d2 - d1) + (d3 - d2)) / 3;

            // Check if last delta is 50% greater than the average
            // Which would indicate a data break
            if (d1 - d0 > avg * 1.5) {
                //console.log('avg:', avg);
                //console.log('delta: ', d1-d0);
                //console.log('gap!');

                // Push a null value to break the line
                update.x.push(null);
                update.y.push(null);
                continue;
            }
            }
            update.x.push(data[i].time); // This is a hard-coded PIRDS value
            update.y.push(data[i].val); // This is a hard-coded PIRDS value
        }
        return update;
    }

    function updatePlot(data, lineColor, timeWindow, yMin, yMax) {
        console.log("lineColor: ", lineColor);
        const plot = [
            {
            x: data.x,
            y: data.y,
            //mode: 'markers',
            //type: 'scattergl',
            mode: "lines",
            line: { color: lineColor }, //'#80CAF6' }
            },
        ];

        const minTime = new Date(
            Date.now() - timeWindow * 1000 * 60
        ).toISOString();
        const maxTime = new Date().toISOString();
        const viewWindow = setViewWindow(minTime, maxTime, yMin, yMax);
        Plotly.react("plotDiv", plot); // Update plot
        Plotly.relayout("plotDiv", viewWindow); // Update view
    }

    function setViewWindow(startTime, endTime, yMin, yMax) {
        const view = {
            xaxis: {
            type: "date",
            range: [startTime, endTime],
            },
            yaxis: {
            range: [yMin, yMax],
            },
        };
        return view;
    }
    
    

}
