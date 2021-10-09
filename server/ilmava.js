'use strict';

const dotEnv = require('dotenv').config();
require('console-stamp')(console);
const express = require('express');
const app = express();
const router = express.Router();


app.use(express.json());
app.use(express.urlencoded({extended: true,}));

app.use(function (req, res, next) {
    res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, PATCH, DELETE, OPTIONS ');
    res.header(
        'Access-Control-Allow-Headers',
        'Origin, X-Requested-With,' +
        ' Content-Type, Accept,' +
        ' Authorization,' +
        ' Access-Control-Allow-Credentials'
    );
    res.header('Access-Control-Allow-Credentials', 'true');
    res.header("Access-Control-Allow-Origin", "*");
    next();
});

app.use('/device/v1', router);
router.post('/measurements', async function (req, res) {
    console.log(req.body);
    res.json({status: 'received'});
});

app.listen(3800, () => {
    console.log(`ilmava api listening at port 3800.`);
});
