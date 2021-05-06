const views = require("../routes/views");
const violations = require("../routes/violations");
var express = require('express')


module.exports = function (app) {
  app.use(express.json());
  app.use(express.urlencoded({ extended: true }))
  
  app.use("/", views);
  app.use("/", violations);
};
