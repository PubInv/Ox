/*       */

//import Plotly from 'plotly.js-dist';

export class Plot {
    name        ;

    constructor (name        ) {
        this.name = name;
    }

    hello() {
        console.log(`${this.name} says hello!`);
    }

    boo(foo        ) {
        return 5;
    }
}
