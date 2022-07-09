const template = document.createElement('template');
template.innerHTML = `
<style>
	.vent-input {
		width:100px;
		height:100px;
		padding:5px;
	}
	.v {
		font-size: 3em;
		color: #ffffff;
		display:block;
		width: 100%;
	}
	.name {
		font-size: 1em;
		color: #ffffff;
	}
	button {
		display: block;
		width: 100%;
		height: 100%;
		border: solid 1px #105955;
		background-color: #90D9D5;
		border-radius: 10px;
		cursor: pointer;
		padding: 14px 28px;
	}
	not-selected {
		background-color: #90D9D5;
	}
	button:focus {
		background-color: #C6ECE9;
		outline: none;
	}
	
</style>
<div class = "vent-input">
	<button ventinput>
		<div class="v">00</div>
		<div class="l">label</div>
		<div class="u">unit</div>
	</button>
</div>		
`;


class VentInput extends HTMLElement {
	// Specify observed attributes so that
  	// attributeChangedCallback will work
	static get observedAttributes() {
		return ['v', 'l', 'u'];
	}
	
	constructor() {
		super();
		this._ventbutton;
		this._isSelected = false;
		this._currentVal = 0;
		this.attachShadow({ mode: 'open' });
		this.shadowRoot.appendChild(template.content.cloneNode(true));
	}
	
	connectedCallback() {
		this._ventbutton = this.shadowRoot.querySelector(".vent-input");
		this.shadowRoot.querySelector("button").addEventListener('click', this._onClick.bind(this));
		this.shadowRoot.querySelector("button").addEventListener('wheel', this._scroll.bind(this));
		this.shadowRoot.querySelector("button").addEventListener("mouseleave", this._stopScroll.bind(this));
	}

	diconnectedCallback(){
		this.shadowRoot.querySelector("button").removeEventListener('click', this._onClick);
		this.shadowRoot.querySelector("button").removeEventListener('wheel', this._scroll);
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

	_stopScroll(){
		if (this._isSelected){ 
			this._isSelected = false;
			this.shadowRoot.querySelector("button").blur();
		}
	}
	
	_scroll(event) {
		if (this._isSelected){
			event.preventDefault();

			if (event.deltaY < 0) {
				this._currentVal++;
			} else {
				this._currentVal--;
			}
		
			this.shadowRoot.querySelector('.v').innerHTML = this._currentVal;
		}
	}
	  
	
	_onClick(){
		this._isSelected = true;;
	}
	
	
}
customElements.define('vent-input', VentInput);