const express = require("express");
const router = express.Router();
const path = require("path");

router.get("/", async (req, res) => {
  res.sendFile(path.join(__dirname, "../views", "login.html"));
});

router.get("/violations", async (req, res) => {
  res.sendFile(path.join(__dirname, "../views", "violations.html"));
});


module.exports = router;
