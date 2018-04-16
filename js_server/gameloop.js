/*
*
* This file is the GameLoop for the server side
* Everything to do with game logic goes here
*
* This also houses the tcp wifi packet server and http server
*
 */

let connections = 0;
let TARGET_USERS_NUM = 5;
let game_started = false;
let activeUsers = [];
let Queue = require('./queue');
let inputQueue = Queue.createQueue();
let present = require('present');
let quit = false;
const SIMULATION_UPDATE_RATE_MS = 100; // 1/10 of a second update
let io = null;

function processInput(elapsedTime) {
    //
    // Double buffering on the queue so we don't asynchronously receive inputs
    // while processing.
    let processMe = inputQueue;
    inputQueue = Queue.createQueue();

    while (!processMe.empty) {
        let input = processMe.dequeue();
        let args = input.message.toString().split(" ");
        //get the client
        console.log(args[0]);
        let client = activeUsers[args[0]];
        //TODO update player info here
        // client.player.someFunction();
    }

}

function createPlayerInfo(user_id) {
    //TODO make a player here
    let that = {};

    that.someFunction = function () {
        console.log('processing')
    };

    that.health = 100;

    return that;
}

function update(elapsedTime) {
    //TODO game logic here
    //EX: if(hit) player.health--;  *that type of stuff ya know?
}

function updatePlayers(elapsedTime) {
    //TODO send out the player update
}

function gameLoop(currentTime, elapsedTime) {
    processInput(elapsedTime);
    update(elapsedTime, currentTime);
    updatePlayers(elapsedTime);

    if (!quit) {
        setTimeout(() => {
            let now = present();
            gameLoop(now, now - currentTime);
        }, SIMULATION_UPDATE_RATE_MS);
    }
}

function initIo(http) {

    var net = require('net');

    var HOST = '144.39.206.247'; //192.168.1.67
    var PORT = 3000;

    net.createServer(function(sock) {

        console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

        sock.on('data', function(data) {

            io.emit('chat message', sock.remoteAddress + ": " + data);

            inputQueue.enqueue({
                clientId: sock.remotePort,
                clientAddress: sock.remoteAddress,
                message: data
            });

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

    io = require('socket.io')(http);

    io.on('connection', function (socket) {
        socket.on('join', function (data) {
            console.log(data.name + ' with id ' + socket.id + ' connected');
            if (game_started) {
                console.log('too many');
            } else {
                io.emit('chat message', 'Tag_' + (connections + 1) + ' has joined the game: ' +
                    data.name);
                var name_id = "Tag_" + (connections+1);

                //used to send specific messages
                let player = createPlayerInfo(name_id);
                activeUsers[name_id] = {
                    id: socket.id,
                    socket: socket,
                    player: player
                };
                connections++;
                data.name = 'Tag_' + connections;
                if (connections >= TARGET_USERS_NUM) {
                    game_started = true;
                }
            }

            socket.on('chat message', function (msg) {
                io.emit('chat message', data.name + ": " + msg);
            });

            socket.on('disconnect', function () {
                connections--;
                console.log(data.name + ' with id ' + socket.id + ' disconnected');
                io.emit('chat message', data.name + ' has left the game');
            });
        });
    });

}

function init(http) {
    initIo(http);
    gameLoop(present(),0);
}

module.exports.init = init;
