const Sequelize = require('sequelize');
const dotEnv = require('dotenv').config();


// Models
const MeasurementModel = require('../models/measurement');
const ConsumptionModel = require('../models/consumption');


// Sequelize instance
const sequelize = new Sequelize(process.env.DB_DATABASE, process.env.DB_USER, process.env.DB_PASSWORD, {
  port: process.env.DB_PORT,
  host: process.env.DB_HOST,
  dialect: process.env.DB_DIALECT,
  pool: {
    max: 10,
    min: 0,
    idle: 10000
  },
  logging: function (str) {
    // console.log(str);
  },
});


// Initialize models
const Measurement = MeasurementModel(sequelize, Sequelize);
const Consumption = ConsumptionModel(sequelize, Sequelize);

// Sync with database
sequelize.sync()
  .then(() => {
  }).catch(error => {
});


// Export models
module.exports = {
  Measurement,
  Consumption,
};
