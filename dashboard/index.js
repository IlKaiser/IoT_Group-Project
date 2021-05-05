const express = require("express");
const app = express();
const config = require("config");

const port = config.get("port");

require("./startup/routes")(app);

const server = app.listen(port, () => {
  console.log("Server listening on port : ", port);
});

module.exports = server;
