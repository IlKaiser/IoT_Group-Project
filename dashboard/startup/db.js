const config = require("config");
const AWS = require("aws-sdk");

require("dotenv").config();

let awsConfig = {
    "region": config.get("aws_region"),
    "endpoint": config.get("aws_endpoint"),
    "accessKeyId": process.env.AWS_ACCESS_KEY_ID,
    "secretAccessKey": process.env.AWS_SECRET_ACCESS_KEY,
    "sessionToken": process.env.AWS_SESSION_TOKEN
};

AWS.config.update(awsConfig);

const docClient = new AWS.DynamoDB.DocumentClient();

const paramsViolation = {
    TableName: config.get("violation_table_name"),
};

exports.docClient = docClient;
exports.paramsViolation = paramsViolation;
