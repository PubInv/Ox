class VentOutput extends HTMLElement {
	// Specify observed attributes so that
  	// attributeChangedCallback will work
	static get observedAttributes() {
		return ['v', 'l', 'u'];
	}
	
	constructor() {
		super();
		this._ventbutton;
		this.attachShadow({ mode: 'open' });
		this.shadowRoot.innerHTML = `
		<style>
			.vent-output {
				border: solid 1px #105955;
				background-color: #90D9D5;
				border-radius: 10px;
				cursor: default;
				padding: 8px 20px 8px 20px;
				margin: 5px;
			}
		
			.v {
				font-size: 4em;
				color: #ffffff;
				margin:0;
				padding:0;
				line-height: 90%;
			}
			.labels div {
				margin-right:10px;
			}
			.labels {
				display:flex;
			}			
		</style>
		
		<div class="vent-output">
		
		<div class="v">00</div>
			<div class="labels">		
				<div class="l">label</div>
				<div class="u">unit</div>
			</div>
		</div>		
		`
	}
	
	connectedCallback() {
		this._ventbutton = this.shadowRoot.querySelector(".vent-output");
	}

	diconnectedCallback(){
		
	}

	attributeChangedCallback(name, oldValue, newValue) {
		if (name == 'v') {
			this.shadowRoot.querySelector('.v').innerHTML = newValue;
			this._currentVal = newValue;		
		}
		else if (name == 'l') {
			this.shadowRoot.querySelector('.l').innerHTML = newValue;
		}
		else if (name == 'u') {
			this.shadowRoot.querySelector('.u').innerHTML = newValue;
		}
	}	
	
}
customElements.define('vent-output', VentOutput);