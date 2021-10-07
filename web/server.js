const express = require('express');
const { createServer } = require("http");
const Server = require("socket.io");

const app = express();
var cookieSession = require("cookie-session");
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
);
const httpServer = createServer(app);
const io = new Server(httpServer, { serveClient: false });
const port = 3005;



// Serve static files in the /public directory
app.use(express.static('public'))

io.on('connection', (socket) => {
    console.log(`a user connected ${socket.id}`);
});

// Start server
httpServer.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)
})
