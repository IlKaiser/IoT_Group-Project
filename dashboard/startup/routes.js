const views = require("../routes/views");
const violations = require("../routes/violations");
var bodyParser = require('body-parser')


module.exports = function (app) {
  app.use(bodyParser.json());
  app.use(bodyParser.urlencoded({ extended: false }))
  
  app.use("/", views);
  app.use("/", violations);
};
