'use strict';

const dotEnv = require('dotenv').config();
require('console-stamp')(console);
const express = require('express');
const app = express();
const router = express.Router();
const initDb = require('./module/database');
const schedule = require('node-schedule');


const HEATING_COIL_NAME = 'heating_coil';


initDb.initDatabase().then(async () => {
  const sequelizeObjects = require('./module/sequelize');

  app.use(express.json());
  app.use(express.urlencoded({extended: true,}));

  app.use(function (req, res, next) {
    res.setHeader('Access-Control-Allow-Origin', '*');
    next();
  });

// Console log all
  router.use(function (req, res, next) {
    console.log(req.method + req.url);
    next();
  });

  app.use('/', router);
  router.get('/', async function (req, res) {
    console.log('device reported being alive');
    res.json({status: 'received'});
  });

  app.use('/device/v1', router);
  router.post('/measurements', async function (req, res) {
    try {
      const measurements = req.body;
      console.log(measurements);

      const incomingAir = measurements.data.filter(f => {
        return f.name === 'incoming_air'
      })[0];
      const outgoingAirToRooms = measurements.data.filter(f => {
        return f.name === 'outgoing_air_to_rooms'
      })[0];
      const returningRoomsAir = measurements.data.filter(f => {
        return f.name === 'returning_rooms_air'
      })[0];
      const wasteAir = measurements.data.filter(f => {
        return f.name === 'waste_air_out'
      })[0];

      sequelizeObjects.Measurement.create({
        incoming_air_temperature: incomingAir.temp,
        incoming_air_humidity: incomingAir.hum,
        incoming_air_carbon_dioxide: incomingAir.dioxide || null,
        incoming_air_carbon_monoxide: incomingAir.monoxide || null,

        outgoing_air_to_rooms_temperature: outgoingAirToRooms.temp,
        outgoing_air_to_rooms_humidity: outgoingAirToRooms.hum,
        outgoing_air_to_rooms_carbon_dioxide: outgoingAirToRooms.dioxide || null,
        outgoing_air_to_rooms_carbon_monoxide: outgoingAirToRooms.monoxide || null,

        returning_rooms_air_temperature: returningRoomsAir.temp,
        returning_rooms_air_humidity: returningRoomsAir.hum,
        returning_rooms_air_carbon_dioxide: returningRoomsAir.dioxide || null,
        returning_rooms_air_carbon_monoxide: returningRoomsAir.monoxide || null,

        waste_air_out_temperature: wasteAir.temp,
        waste_air_out_humidity: wasteAir.hum,
        waste_air_out_carbon_dioxide: wasteAir.dioxide || null,
        waste_air_out_carbon_monoxide: wasteAir.monoxide || null,

      });
    } catch (e) {
      console.error(e);
    }
    res.json({status: 'received'});
  });

  app.listen(8080, () => {
    console.log(`ilmava api listening at port 8080.`);
  });

  // await calculateCoilEnergyUse();
  schedule.scheduleJob('0 0 * * *', async () => {
    await calculateCoilEnergyUse();
  });

  async function calculateCoilEnergyUse() {
    const results = await sequelizeObjects.Query.query(`
      select started::date                        as date,
             500 * (duration_minutes / 60) / 1000 as used_kwh
      from (
             select min("createdAt")                       as started,
                    max("createdAt")                       as ended,
                    EXTRACT(EPOCH FROM (max("createdAt") - min("createdAt"))::INTERVAL) /
                    60                                     as "duration_minutes",
                    avg(outgoing_air_to_rooms_temperature) as "avg_temp"
             from measurements
             where outgoing_air_to_rooms_temperature > 21
               and outgoing_air_to_rooms_humidity < 27
               and "createdAt"::date != (SELECT CURRENT_DATE)
             group by "createdAt"::date
           ) as m
    `, null);
    for (const result of results[0]) {
      const exists = await sequelizeObjects.Consumption.findAll({
        where: {
          date: result.date
        }
      });
      if (exists.length === 0) {
        await sequelizeObjects.Consumption.create({
          name: HEATING_COIL_NAME,
          date: result.date,
          value: result.used_kwh,
        });
      }
    }
  }

});
