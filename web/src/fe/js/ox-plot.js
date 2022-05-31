class OxPlot extends HTMLElement {
  // Specify observed attributes so that
  // attributeChangedCallback will work
  // static get observedAttributes() {
  // 	return ['v', 'l', 'u'];
  // }

  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    this.shadowRoot.innerHTML = `
		<script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
		<style>
			
		</style>
		<p>Hello</p>
        <div id="ox-plot" style="width: 800px; height: 500px"></div>
		
		`;
  }

  connectedCallback() {}

  diconnectedCallback() {}

  attributeChangedCallback(name, oldValue, newValue) {}

  doSomething(arg) {
    this.arg = arg;
    console.log("doSomething: " + arg);
  }

  setDataUrl(url) {
	  this.dataUrl = url;
  }

  setTimeViewMinutes(mins) {
	  this.timeViewMinutes = mins;
  }

  setRefreshRate() {
    const interval = setInterval(this.boo(), 1000);
	
  }

  boo() {
	this.startTime = new Date(
        Date.now() - 1 * 1000 * 60
      ).toISOString();
    this.endTime = new Date().toISOString();
	this.getData2("pirds", this.startTime, this.endTime);
  }

  getData2(table, startTime, endTime) {
	  console.log("GetData");
    const query = `${this.dataUrl}/${table}/${startTime}/${endTime}`;
	console.log(query);
    fetch(query)
      .then((response) => response.json())
      .then((data) => {
        console.log(data);
        if (data == null || data.length === 0) {
          console.warn("No data received!");
          return;
        }
        const processedData = this.processData(data);
        this.updatePlot(processedData, "#80CAF6", this.timeViewMinutes, -2, 2);
      })
      .catch((error) => {
        console.warn(error);
      });
  }

  // Take only the x and y values from the JSON data
  processData(data) {
	  console.log("processData");
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

  updatePlot(data, lineColor, timeWindow, yMin, yMax) {
    const plot = [
      {
        x: data.x,
        y: data.y,
        mode: "lines",
        line: { color: lineColor },
      },
    ];

    const minTime = new Date(Date.now() - timeWindow * 1000 * 60).toISOString();
    const maxTime = new Date().toISOString();
    const viewWindow = this.setViewWindow(minTime, maxTime, yMin, yMax);
	console.log("id: " + this.id)
    Plotly.react(this.id, plot); // Update plot
    Plotly.relayout(this.id, viewWindow); // Update view
	console.log("Updating plot...");
  }

  setViewWindow(startTime, endTime, yMin, yMax) {
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
customElements.define("ox-plot", OxPlot);
