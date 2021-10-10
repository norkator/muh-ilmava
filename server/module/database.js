'use strict';

const pgTools = require('pgtools');
const dotEnv = require('dotenv').config();

async function initDatabase() {
  return await createDatabase();
}

exports.initDatabase = initDatabase;

async function createDatabase() {
  pgTools.createdb({
    user: process.env.DB_USER,
    password: process.env.DB_PASSWORD,
    port: process.env.DB_PORT,
    host: process.env.DB_HOST
  }, process.env.DB_DATABASE, function (error, response) {
    if (!error) {
      console.log('db ' + process.env.DB_DATABASE + ' did not exists. Created it!')
    }
  });
  return true;
}
