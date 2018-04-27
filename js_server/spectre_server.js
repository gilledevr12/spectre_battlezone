'use strict'

let express = require('express');
let sqlite = require('sqlite3');
let fs = require('fs');
let path=require('path');
var bodyParser = require('body-parser');
let game = require('./gameloop');

let app = express();
let http = require('http').Server(app);

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
})

http.listen(3001, function() {
    game.init(http);
    console.log('Server running at http://localhost:3001/');
});