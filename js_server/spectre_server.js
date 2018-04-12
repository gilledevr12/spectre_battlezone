'use strict'

let express = require('express');
let sqlite = require('sqlite3');
let fs = require('fs');
let path=require('path');
var bodyParser = require('body-parser');
let connections = 0;
let TARGET_USERS_NUM = 1;
let game_started = false;
let activeUsers = [];

let app = express();
let http = require('http').Server(app);
var io = require('socket.io')(http);

let mimeTypes = {
    '.js' : 'text/javascript',
    '.html' : 'text/html',
    '.css' : 'text/css',
    '.png' : 'image/png',
    '.jpg' : 'image/jpeg',
    '.mp3' : 'audio/mpeg3'
};

var net = require('net');

var HOST = '144.39.206.251';
var PORT = 3000;

// Create a server instance, and chain the listen function to it
// The function passed to net.createServer() becomes the event handler for the 'connection' event
// The sock object the callback function receives UNIQUE for each connection
net.createServer(function(sock) {

    // We have a connection - a socket object is assigned to the connection automatically
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

    // Add a 'data' event handler to this instance of socket
    sock.on('data', function(data) {

        console.log('DATA ' + sock.remoteAddress + ': ' + data);
        // Write the data back to the socket, the client will receive it as data from the server
        sock.write('You said "' + data + '"');

    });

    // Add a 'close' event handler to this instance of socket
    sock.on('close', function(data) {
        console.log('CLOSED: ' + sock.remoteAddress +' '+ sock.remotePort);
    });

}).listen(PORT, HOST);

console.log('Server listening on ' + HOST +':'+ PORT);

//sql
let db = new sqlite.Database('users.db');

db.serialize(function() {
  db.run("CREATE TABLE IF NOT EXISTS users (user TEXT PRIMARY KEY, pass TEXT)");
});
db.serialize(function() {
  db.run("CREATE TABLE IF NOT EXISTS highscores (score INT, user TEXT)");
});


app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

app.use('/css',express.static(path.join(__dirname,'css')));
app.use('/scripts',express.static(path.join(__dirname,'scripts')));
app.use('/images', express.static(path.join(__dirname, 'images')));

app.get('/', function(request, response){

    console.log('okay');
  response.sendFile(path.join(__dirname, 'page.html'));

});

app.post('/signin', function(request, response) {
  db.get('SELECT pass FROM users WHERE user = ?', request.body['username'],function(err, row){
    if(err){
      response.json({error:err});
    }else if(typeof row === 'undefined') {
      response.json({error:'User does not exist'});
    } else if(row.pass === request.body['password']){
      response.json({error:null,authenticated:true});
    }else{
      response.json({error:'Invalid Password'});
    }
  })
});

app.post('/signup', function(request, response) {

  db.run('INSERT INTO users(user,pass) VALUES (?,?)', [request.body['username'], request.body['password']],function(err, row){
    if(err){
      if(err.errno === 19) {
        response.json({error:"User already exists"});
      } else {
        response.json({error:err});
      }
    }else {
      response.json({error:null,authenticated:true});
    }
  })
});

app.get('/highscores/add/:score/:user', function(request, response) {

  db.run('INSERT INTO highscores(score,user) VALUES (?,?)', [request.params['score'], request.params['user']],function(err, row){
    if(err){
      response.json({error:err});
    }else {
      response.json({'inserted':true});
    }
  })
});

app.get('/highscores', function(request, response){
  db.all('SELECT * FROM highscores ORDER BY score DESC', (err, rows) => {
    if(err){
      response.json({error:err});
    } else {
      response.json(rows);
    }
  });
});

app.use('*', function(request, response){
    console.log("okay");
  //response.sendfile(path.join(__dirname, 'page.html'));
  response.status(404).send("Not found");
})

function runCountdown() {
    var target_date = new Date().getTime() + (10*1000);
    var seconds, pastSeconds = 0;
// update the tag with id "countdown" every 1 second
    var refresh = setInterval(function () {

        // find the amount of "seconds" between now and target
        var current_date = new Date().getTime();
        var seconds_left = (target_date - current_date + 1) / 1000;
        seconds = parseInt(seconds_left % 60);

        if (pastSeconds !== seconds) {
            io.emit('start game', seconds.toString());
            pastSeconds = seconds;
        }
        if (seconds === 0){
            io.emit('start game', 'countdown finished');
            clearInterval(refresh);
        }
    }, 1000);
}

io.on('connection', function(socket){
  socket.on('join', function(data){
    console.log(data.name + ' with id ' + socket.id + ' connected');
    if(game_started) {
      if(typeof activeUsers[data.name] !== 'undefined') {
          activeUsers[data.name] = socket.id;
          io.sockets.sockets[socket.id].emit('start game', "player reconnect"); io.emit('chat message',data.name + ' has rejoined the game.');
      } else {
        io.emit('chat message', "Game already in progress.")
      }
    } else {
      io.emit('chat message',data.name + ' has joined the game.');
      activeUsers[data.name] = socket.id;
      connections++;
      if (connections >= TARGET_USERS_NUM) {
        if (!game_started) runCountdown();
        game_started = true;
      }
    }

    socket.on('chat message', function(msg){
      io.emit('chat message', data.name + ": " + msg);
    });

    socket.on('input', (keyInput) => {
      console.log(data.name + ': ' + keyInput);
    });

    socket.on('disconnect', function(){
        connections--;
        console.log(data.name + ' with id ' + socket.id + ' disconnected');
        io.emit('chat message', data.name + ' has left the game');
    });
  });
});

http.listen(3001, function() {
  console.log('Server running at http://localhost:3001/');
});