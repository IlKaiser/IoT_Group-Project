const express = require("express");
const router = express.Router();
const path = require("path");
 

const check_login = require("../helper/check_login");

router.get("/", async (req, res) => {
  res.sendFile(path.join(__dirname, "../views", "login.html"));
});

router.get("/redirect_to_violations", async (req, res) => {
  res.sendFile(path.join(__dirname, "../views", "redirect_to_violations.html"));
});

router.get("/violations", async (req, res) => {
  res.sendFile(path.join(__dirname, "../views", "violations.html"));
});

router.post("/login-request", async (req,res) => {
  let username = req.body.username;
  let password = req.body.password;
 
  check_login.check(username,password,
    /*success*/  function(){ res.redirect("/redirect_to_violations"); },
    /*failure*/  function(){ res.redirect("/"); }
  );
});

module.exports = router;
