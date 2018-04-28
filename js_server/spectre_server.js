'use strict'

let express = require('express');
let fs = require('fs');
let path=require('path');
var bodyParser = require('body-parser');
let game = require('./gameloop');

let app = express();
let app2 = express();
let http = require('http').Server(app);
let http2 = require('http').Server(app);

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

app.use('/css',express.static(path.join(__dirname,'css')));
app.use('/scripts',express.static(path.join(__dirname,'scripts')));
app.use('/images',express.static(path.join(__dirname,'images')));

app.get('/', function(request, response){
  response.sendFile(path.join(__dirname, 'page.html'));
});

app.use('*', function(request, response){
  response.status(404).send("Not found");
});

http.listen(3001, function() {
    // http2.listen(3002, function() {
        game.init(http, http2);
        console.log('Server running at http://localhost:3001/');
        console.log('game logger running at http://localhost:3002/');
    // });
});