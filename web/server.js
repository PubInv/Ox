"use strict";
const express = require("express");
const app = express();
const bodyParser = require("body-parser");
const server = require("http").createServer(app);
const io = require("socket.io")(server);
const port = 3020;

const { Op } = require("sequelize");

// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: false }));
// parse application/json
app.use(bodyParser.json());

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

const Sequelize = require("sequelize");
const sequelize = new Sequelize(
  "postgres://postgres:password@localhost:5432/database_development",
  {
    freezeTableName: true,
    dialect: "postgres",
    protocol: "postgres",
    dialectOptions: {
      //ssl: {
      //    require: true,
      //    rejectUnauthorized: false
      //}
    },
  }
);

sequelize
  .authenticate()
  .then(() => {
    console.log("Connection has been established successfully.");
  })
  .catch((err) => {
    console.error("Unable to connect to the database:", err);
  });

/*let PageLoads = sequelize.define('page_loads', {
    userAgent: Sequelize.STRING,
    time: Sequelize.DATE
});*/

let Pimd = sequelize.define(
  "pimd",
  {
    location: Sequelize.STRING,
    value: Sequelize.INTEGER,
    time: Sequelize.DATE,
  },
  {
    freezeTableName: true,
    createdAt: false,
    updatedAt: false,
  }
);


/*app.get('/', async (req, res) => {
    // get the user agent and current time
    const userAgent = req.get('user-agent');
    const time = new Date().getTime();

    try {
        // insert the record
        await PageLoads.create({
            userAgent, time
        });

        // now display everything in the table
        const messages = await PageLoads.findAll();
        res.send(messages);
    } catch (e) {
        console.log('Error inserting data', e)
    }
})*/

app.post("/api/pimd", async (req, res) => {
  //res.send('Got a POST request ' + JSON.stringify(req.body));
  res.sendStatus(200);
  console.log(req.body);
  const location = req.body.location;
  const value = req.body.value;
  const time = new Date(req.body.time);

  try {
    // insert the record
    await Pimd.create({
      location,
      value,
      time,
    });
  } catch (e) {
    console.log("Error inserting data", e);
  }
});

app.get("/api/pimd", async (req, res) => {
  try {
    const data = await Pimd.findAll();
    res.send(data);
  } catch (e) {
    console.log("Error inserting data", e);
  }
});

app.get("/api/pimd/:from_time/:to_time", async (req, res) => {
  try {
    //const data = await Pimd.findAll();
    const data = await Pimd.findAll({
      where: {
        time: {
          [Op.lt]: req.params.to_time, //new Date(),
          [Op.gt]: req.params.from_time, //new Date(Date.now() - 1000 * 60)
        },
      },
    });
    res.send(data);
  } catch (e) {
    console.log("Error inserting data", e);
  }
});


// PIRDS:
/*
{ "event" : "M",
  "type" : "T",
  "loc" : "B",
  "num" : 2,
  "ms" : 35,
  "val" : 250
  "sht" : 6754328
  "pid" : "834f44a2-4bf3-40ba-827b-ba2a8cc59531"
  }
*/

let Pirds = sequelize.define(
  "pirds",
  {
    event: Sequelize.STRING,
    type: Sequelize.STRING,
    loc: Sequelize.STRING,
    num: Sequelize.INTEGER,
    ms: Sequelize.INTEGER,
    val: Sequelize.INTEGER,
    sht: Sequelize.INTEGER,
    pid: Sequelize.STRING,
    time: Sequelize.DATE,
  },
  {
    freezeTableName: true,
    createdAt: false,
    updatedAt: false,
  }
);

app.post("/api/pirds", async (req, res) => {
  //res.send('Got a POST request ' + JSON.stringify(req.body));
  res.sendStatus(200);
  console.log(req.body);
  const event = req.body.event;
  const type = req.body.type;
  const loc = req.body.loc;
  const num = parseInt(req.body.num);
  const ms = parseInt(req.body.ms);
  const val = parseFloat(req.body.val);
  const sht = parseInt(req.body.sht);
  const pid = req.body.pid;
  const time = new Date(req.body.time);

  try {
    // insert the record
    await Pirds.create({
      event,
      type,
      loc,
      num,
      ms,
      val,
      sht,
      pid,
      time,
    });
  } catch (e) {
    console.log("Error inserting data", e);
  }
});

app.get("/api/pirds", async (req, res) => {
  try {
    const data = await Pirds.findAll();
    res.send(data);
  } catch (e) {
    console.log("Error inserting data", e);
  }
});

app.get("/api/pirds/:from_time/:to_time", async (req, res) => {
  try {
    const data = await Pirds.findAll({
      where: {
        time: {
          [Op.lt]: req.params.to_time, //new Date(),
          [Op.gt]: req.params.from_time, //new Date(Date.now() - 1000 * 60)
        },
      },
    });
    res.send(data);
  } catch (e) {
    console.log("Error getting data", e);
  }

});

// PIRCS
/*
{ "ack": "S",
  "err": 0,
  "com": "C",
  "par" : "P",
  "int" : "T",
  "mod" : 0,
  "val" : 400
  }
*/



// /api/pircs/C/P/T/0/400
// /api/pircs?com=C&par=P&int=T&mod=0&val=400

// Serve static files in the /public directory
app.use(express.static("public"));

io.on("connection", (socket) => {
  console.log(`a user connected ${socket.id}`);
  /*socket.emit('plot', 
    await Pimd.findAll({
        where: {
          time: {
            [Op.lt]: new Date(),
            [Op.gt]: new Date(Date.now() - 1000 * 60)
          }
        }
      })
    );*/
});

/*var cnt = 0;
let freq = 0.1; // Hz
let amp = 2; // amplitude

let d_0 = new Date();
let t_0 = d_0.getTime();

let interval = setInterval(function () {

    var time = new Date();
    let t_val = (time.getTime() - t_0) / 1000;
    let w = 2 * Math.PI * freq;
    var y_val = amp * Math.sin(w * (t_val));

    point = { t: t_val, y: y_val };
    io.emit('plot', point);
    console.log(point);
    if (++cnt === 10000) clearInterval(interval);
}, 100);
*/

// Start server
server.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`);
});
