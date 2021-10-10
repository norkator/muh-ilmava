'use strict';

const dotEnv = require('dotenv').config();
require('console-stamp')(console);
const express = require('express');
const app = express();
const router = express.Router();
const initDb = require('./module/database');


initDb.initDatabase().then(() => {
    const sequelizeObjects = require('./module/sequelize');

    app.use(express.json());
    app.use(express.urlencoded({extended: true,}));

// app.use(function (req, res, next) {
//     res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, PATCH, DELETE, OPTIONS ');
//     res.header(
//         'Access-Control-Allow-Headers',
//         'Origin, X-Requested-With,' +
//         ' Content-Type, Accept,' +
//         ' Authorization,' +
//         ' Access-Control-Allow-Credentials'
//     );
//     res.header('Access-Control-Allow-Credentials', 'true');
//     res.header("Access-Control-Allow-Origin", "*");
//     next();
// });

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
                outgoing_air_to_rooms_temperature: outgoingAirToRooms.temp,
                outgoing_air_to_rooms_humidity: outgoingAirToRooms.hum,
                returning_rooms_air_temperature: returningRoomsAir.temp,
                returning_rooms_air_humidity: returningRoomsAir.hum,
                waste_air_out_temperature: wasteAir.temp,
                waste_air_out_humidity: wasteAir.hum,
            });
        } catch (e) {
            console.error(e);
        }
        res.json({status: 'received'});
    });

    app.listen(8080, () => {
        console.log(`ilmava api listening at port 8080.`);
    });


});
