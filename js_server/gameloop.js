/*
*
* This file is the GameLoop for the server side
* Everything to do with game logic goes here
*
* This also houses the tcp wifi packet server and http server
*
 */

let connections = 0;
let activeUsers = [];
let shots = [];
let updates_messages = [];
let Queue = require('./queue');
let NetworkIds = require('./scripts/network-ids');
let inputQueue = Queue.createQueue();
let present = require('present');
let quit = false;
const SIMULATION_UPDATE_RATE_MS = 100; // 1/10 of a second update
let io = null;
let ioServer = null;
let count = 0;
let anchors = {};
let X_MAX = 9.67;
let Y_MAX = 10.18;

function initAnchors() {
    //TODO put values here
    let a1_x_dist = 4.90,
        a1_y_dist = 4.42,
        a2_x_dist = 3.55,
        a2_y_dist = 3.23,
        a3_x_dist = 4.27,
        a3_y_dist = 5.26;
    anchors.a1 = {
        x: a1_x_dist * (-1) * 2,
        y: a1_y_dist * (-1) * 2,
        c: Math.pow(a1_x_dist,2) + Math.pow(a1_y_dist,2)
    };
    anchors.a2 = {
        x: a2_x_dist * (-1) * 2,
        y: a2_y_dist * (-1) * 2,
        c: Math.pow(a2_x_dist,2) + Math.pow(a2_y_dist,2)
    };
    anchors.a3 = {
        x: a3_x_dist * (-1) * 2,
        y: a3_y_dist * (-1) * 2,
        c: Math.pow(a3_x_dist,2) + Math.pow(a3_y_dist,2)
    };
}

function calculatePosition(player, dists) {
    let equations = {
        one: {
            x: anchors.a1.x - anchors.a2.x,
            y: anchors.a1.y - anchors.a2.y,
            c: anchors.a1.c - anchors.a2.c
        },
        two: {
            x: anchors.a1.x - anchors.a3.x,
            y: anchors.a1.y - anchors.a3.y,
            c: anchors.a1.c - anchors.a3.c
        }
    };
    equations.one.f = dists.D1 - dists.D2 - equations.one.c;
    equations.two.f = dists.D1 - dists.D3 - equations.two.c;
    console.log(equations.one.x);
    console.log(equations.one.y);
    console.log(equations.one.c);
    console.log(equations.one.f);
    let x = (equations.two.f - ((equations.two.y*equations.one.f)/equations.one.y)) /
        (equations.two.x - ((equations.two.y*equations.one.x)/equations.one.y));
    let y = (equations.one.f - (equations.one.x * x))/(equations.one.y);
    let position = {
        x: x,
        y: y
    };
    console.log('x: ' + position.x + " y: " + position.y);
    set_position(player, position);

}

function processInput(elapsedTime) {
    //
    // Double buffering on the queue so we don't asynchronously receive inputs
    // while processing.
    let processMe = inputQueue;
    inputQueue = Queue.createQueue();

    while (!processMe.empty) {
        let input = processMe.dequeue();
        let clientIp = input.clientAddress;
        let args = input.message.toString().split(" ");
        //get the client
        for (let index in args){
            // console.log(args[index]);
        }
        let client = activeUsers[clientIp];
        //TODO update player info here
        //first 3 are acceleration, next mag, uwb, then shots
        if (args[10] === 0) {
            console.log('got hit?')
            shots.push(client.player);
        }
        findHeading(client.player, args[4], args[5]);
        let dists = {
            D1: args[7],
            D2: args[8],
            D3: args[9]
        };
        calculatePosition(client.player, dists);
        client.player.reportUpdate = true;
    }
}

function findHeading(player, x, y) {
    let MAG_NORTH = 130.0;
    let heading = Math.atan2(y, x);  // assume pitch, roll are 0
    heading *= (180 / Math.PI);
    if (heading < -MAG_NORTH){
        heading += 360 + MAG_NORTH;
    } else {
        heading += MAG_NORTH;
    }
    set_direction(player, heading); 
    console.log('heading: ' + heading);
}

function update(elapsedTime) {
    //TODO game logic here
    for (let shot in shots){
        for (let others in shots){
            if (isInTrajectory(shots[shot].stats.id, shots[others].stats.id, shots[shot].position,
                    shots[shot].direction, shots[others].position)) {
                console.log("A stupendous shot!!!");
                //here for now, in the send messages later
                // shots[shot].socket.emit("You hit someone!")
                // shots[others].socket.emit("You were hit")
                //TODO log a hit and health and stuff
                shots[others].player.stats.health--;
                shots[others].player.reportUpdate = true;

            } else {
                console.log("Missed teribbly");
            }
        }
    }
    shots.length = 0;
}

function updatePlayers(elapsedTime) {
    //TODO send out the player update
    // for (let index in updates_messages){
    //     //this type of thing here
    //     //shots[shot].socket.emit("You hit someone!")
    //     //shots[others].socket.emit("You were hit")
    // }
    for (let index in activeUsers){
        if (activeUsers[index].player.reportUpdate){
            let name = activeUsers[index].userName;
            activeUsers[index].player.reportUpdate = false;
            let update = {
                name: name,
                position: activeUsers[index].player.position,
                direction: activeUsers[index].player.direction,
                inventory: activeUsers[index].player.inventory,
                stats: activeUsers[index].player.stats,
                shotFired: activeUsers[index].player.shotFired
            }
            activeUsers[index].socket.emit(NetworkIds.UPDATE_SELF, update)
            // activeUsers[index].socket.emit(NetworkIds.UPDATE_SELF, activeUsers[index].player)

            for (let index in activeUsers){
                if (activeUsers[index].userName !== name){
                    activeUsers[index].socket.emit(NetworkIds.UPDATE_OTHER, update);
                    ioServer.emit(NetworkIds.UPDATE_OTHER, update);
                }
            }

        }
    }
}

function testFunc() {
    if (count === 0) {
        for (let index in activeUsers){
            activeUsers[index].player.shotFired = 0;
        }
    }
    count++;
    for (let index in activeUsers){
        activeUsers[index].player.reportUpdate = true;
        activeUsers[index].player.position.x += .001;
        activeUsers[index].player.direction += .1;
        if (count === 30) {
            activeUsers[index].player.shotFired = 1;
        }
    }
    if (count === 30) {
        count = 0;
    }
}

function gameLoop(currentTime, elapsedTime) {
    processInput(elapsedTime);
    update(elapsedTime, currentTime);
    updatePlayers(elapsedTime);

    // testFunc();

    if (!quit) {
        setTimeout(() => {
            let now = present();
            gameLoop(now, now - currentTime);
        }, SIMULATION_UPDATE_RATE_MS);
    }
}

function initIo(http, http2) {

    var net = require('net');

    var HOST = '192.168.1.5';
    var PORT = 3000;

    net.createServer(function(sock) {

        console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

        sock.on('data', function(data) {

            ioServer.emit('log message', sock.remoteAddress + ": " + data);

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

    ioServer = require('socket.io')(http2);
    io = require('socket.io')(http);

    ioServer.on('connection', function (socket) {
        socket.on('join', function (data) {
            ioServer.emit('ready', data.name + ' has joined the game: ');


            // socket.on('chat message', function (msg) {
            //     io.emit('chat message', data.name + ": " + msg);
            // });
        });
    });

    io.on('connection', function (socket) {
        socket.on('join', function (data) {
            var remoteConnection = socket.request.connection.remoteAddress;
            let args = remoteConnection.toString().split(":");
            let clientIp = args[args.length - 1];
            if(typeof activeUsers[clientIp] !== 'undefined') {
                activeUsers[clientIp].dead = false;
                activeUsers[clientIp].socket = socket;
                activeUsers[clientIp].id = socket.id;
                // activeUsers[clientIp].player.clientId = socket.id;
                data.name = "Tag_" + clientIp[clientIp.length - 1];

                // notifyReconnect(socket, activeUsers[data.name].user);
                // io.sockets.sockets[socket.id].emit('start game', "player reconnect");
                io.emit('name player', data.name + ' has rejoined the game.');
            } else {
                data.name = "Tag_" + clientIp[clientIp.length - 1];
                console.log(data.name + ' with id ' + socket.id + ' connected');

                io.emit('name player', data.name + ' has joined the game: ');

                //used to send specific messages
                let player = makePlayer(clientIp);
                activeUsers[clientIp] = {
                    userName: data.name,
                    id: socket.id,
                    socket: socket,
                    player: player
                };
            }
            connections++;

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

function init(http, http2) {
    initAnchors();
    initIo(http, http2);
    testTrajectory();
    gameLoop(present(),0);
}

module.exports.init = init;

function makePlayer(id){
    let that = {};

    let reportUpdate = false;

    Object.defineProperty(that, 'position', {
        get: () => position,
        set: value => { position = value; }
    });

    Object.defineProperty(that, 'reportUpdate', {
        get: () => reportUpdate,
        set: value => { reportUpdate = value; }
    });

    Object.defineProperty(that, 'direction', {
        get: () => direction,
        set: value => { direction = value; }
    });

    Object.defineProperty(that, 'stats', {
        get: () => stats,
        set: value => { stats = value; }
    });

    Object.defineProperty(that, 'inventory', {
        get: () => inventory,
        set: value => { inventory = value; }
    });

    Object.defineProperty(that, 'shotFired', {
        get: () => shotFired ,
        set: value => { shotFired = value; }
    });

    let stats = {
        id: id,
        alive: true,
        health: 100
    };

    let inventory = {
        armor: 0,
        ammo: 0,
        weapon: "pea_shooter"
    };

    // let weapons = [];
    // weapons.push("pea_shooter");

    let position = {
        x: .2, //spec.position.x,
        y: .2 //spec.position.y
    };

    let direction = 0;//spec.direction;
    let shotFired = 0;

    return that;
}

function add_weapon(player, weapon){
    player.weapons.push(weapon);
}

function set_direction(player, direction){
    player.direction = direction;
}

function get_direction(player){
    return player.direction;
}

function set_position(player, position){
    player.position.x = position.x/X_MAX;
    player.position.y = position.y/Y_MAX;
}

function get_postion(player){
    return player.position;
}

function player_hit(distance, weapon){
    console.log("Player was hit!");
    let power = weapon.power;
    if(weapon === 'shotgun'){
        power = weapon.power - distance;
    }
    if(player.armor > 0){
        let remainder = 0;
        if(player.armor - power < 0){
            player.armor = 0;
            remainder = -1 * (player.armor - power);
            player.health -= remainder;
        }
    }
    else{
        player.health -= power;
    }
    if(player.health <= 0){
        player.alive = false;
    }
}

function makePickups(){
    let that = {};

    let pickupArray = [],
        pickupIndex = [ [1, 0.5], [0.5, 1], [0.1, 0], [0, 0.1] ],
        Pickups = {
            num: 4
        },
        pickupSize = {
            width: 0.1,
            height: 0.15
        };

    for(let i=0; i < Pickups.num; i++){
        pickupArray.push( {
            model: {
                position: {
                    x: pickupIndex[i][0],
                    y: pickupIndex[i][1]
                },
                size: {
                    height: pickupSize.height,
                    width: pickupSize.width
                },
                radius: 0.1
            },
            id: i+1,
            /*type: function(i+1){
                id = i+1;
                let type;
                if(id === 1){ type = health },
                if(id === 2){ type = armor },
                if(id === 3){ type = shotgun },
                if(id === 4){ type = shot_ammo }
                return type;
            }*/
        });
    }

    Object.defineProperty(that, 'pickupArray', {
        get: () => pickupArray,
        set: value => { pickupArray = value; }
    });
    return that;
}

function isInTrajectory(id1, id2, me, myTheta, you){
    console.log(id1 + ": " + me.x + "," + me.y + " firing at " + id2 + ": " + you.x + "," + you.y + " with trajectory: " + myTheta);
    let temp_theta = Math.atan2(you.y - me.y, you.x - me.x) * 180 / Math.PI;
    console.log("calcd theta: " + temp_theta);
    let temp_distance = Math.sqrt((you.y - me.y)*(you.y - me.y) + (you.x - me.x)*(you.x - me.x));
    let theta_tolerance = temp_distance * 0.25;
    console.log("distance: " + temp_distance + " tolerance " + theta_tolerance);
    if((temp_theta < myTheta + theta_tolerance) && (temp_theta > myTheta - theta_tolerance))
        return 1;
    else
        return 0;
}

function testTrajectory(){
    let spec1 = { position: { x: 2,  y: 2 }, direction:   55 }
    let spec2 = { position: { x: 17, y: 4 }, direction: -200 }
    let spec3 = { position: { x: 13,  y: 18 }, direction: -200 }
    let p1 = makePlayer(spec1);
    let p2 = makePlayer(spec2);
    let p3 = makePlayer(spec3);

    let ret = isInTrajectory(1, 3, p1.position, 55.0, p3.position);
    if(ret)
        console.log("HIT!");
    else
        console.log("MISS!");

    ret = isInTrajectory(3, 1, p3.position, -200.0, p1.position);
    if(ret)
        console.log("HIT!");
    else
        console.log("MISS!");
}


/*#define WEAPON_COUNT    4

//WEAPON CLASS
#define PEA_SHOOTER     0
#define SHOTGUN         1
#define ASSAULT_RIFLE   2
#define SPECTRE_RIFLE   3

//RELOAD DELAY, FIRE DELAY
#define FAST            0
#define AVERAGE         1
#define SLOW            2

//FIRE WEIGHT (multiplier)
#define VARIES          0
#define LOW             0.5
#define MID             1
#define HIGH            2

typedef struct {
  char CLASS;
  char RELOAD_DELAY;
  char FIRE_DELAY;
  char FIRE_WEIGHT;
  char CLIP_SIZE;
} WEAPON;

#endif
#ifndef INVENTORY_H
#define INVENTORY_H
#include "rifle_variables.h"

typedef struct {
    WEAPON WEAPON_NAME;
    bool EQUIPPED;
    char AMMO_REMAINING;
    char MAX_AMMO;
} INVENTORY_ITEM;
*/
