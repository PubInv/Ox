const express = require('express');
const app = express();
const server = require('http').createServer(app);
const io = require('socket.io')(server);
const port = 3005;

/*var cookieSession = require("cookie-session");
app.set('trust proxy', 1)
app.use(
    cookieSession({
      name: "__session",
      keys: ["key1"],
        maxAge: 24 * 60 * 60 * 100,
        secure: true,
        httpOnly: true,
        sameSite: 'none'
    })
);*/



// Serve static files in the /public directory
app.use(express.static('public'))

io.on('connection', (socket) => {
    console.log(`a user connected ${socket.id}`);
});


var cnt = 0;
let freq = 0.1; // Hz
let amp = 2; // amplitude

let d_0 = new Date();
let t_0 = d_0.getTime();

setInterval(function () {

    var time = new Date();
    let t_val = (time.getTime() - t_0) / 1000;
    let w = 2 * Math.PI * freq;
    var y_val = amp * Math.sin(w * (t_val));

    point = { t: t_val, y: y_val };
    io.emit('plot', point);

    if (++cnt === 10000) clearInterval(interval);
}, 100);

// Start server
server.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
})
