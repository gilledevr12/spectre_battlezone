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
let pickups = {};
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
let X_MAX = 12.23;
let Y_MAX = 9.2;
let PICKUP_LIFE = 20000;

function initAnchors() {
    //TODO put values here
    let a1_x_dist = 9.87,
        a1_y_dist = 4.04,
        a2_x_dist = 6.26,
        a2_y_dist = 6.75,
        a3_x_dist = 4.83,
        a3_y_dist = 2.46;
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
    equations.one.f = (dists.D1 * dists.D1) - (dists.D2 * dists.D2) - equations.one.c;
    equations.two.f = (dists.D1 * dists.D1) - (dists.D3 * dists.D3) - equations.two.c;
    let x = (equations.two.f - ((equations.two.y*equations.one.f)/equations.one.y)) /
        (equations.two.x - ((equations.two.y*equations.one.x)/equations.one.y));
    let y = (equations.one.f - (equations.one.x * x))/(equations.one.y);
    let position = {
        x: x,
        y: y
    };
    // console.log('x: ' + position.x + " y: " + position.y);
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
        let client = activeUsers[clientIp];
        //TODO update player info here
        //first 3 are acceleration, next mag, uwb, then shots
        if (args[10] === 1) {
            client.player.shotFired = 1;
            client.player.inventory.ammo--;
            console.log('got hit?');
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
    let MAG_NORTH = 47.0;
    let heading = Math.atan2(y, x);  // assume pitch, roll are 0
    heading *= (180 / Math.PI);
    heading -= MAG_NORTH;
    if (heading < 0){
        heading += 360;
    } else if (heading > 360) {
        heading -= 360;
    }
    set_direction(player, heading + 90); 
    // console.log('heading: ' + heading);
}

function update(elapsedTime) {
    //TODO game logic here
    for (let shot in shots){
        for (let others in activeUsers){
            if (isInTrajectory(shots[shot].stats.id, activeUsers[others].player.stats.id, shots[shot].position,
                    shots[shot].direction, activeUsers[others].player.position)) {
                console.log("A stupendous shot!!!");
                //TODO log a hit and health and stuff
                activeUsers[others].player.stats.health--;
                activeUsers[others].player.reportUpdate = true;

            } else {
                console.log("Missed teribbly");
            }
        }
    }
    shots.length = 0;

    for (let pick in pickups.pickupArray) {
        if (!pickups.pickupArray[pick].alive) {
            pickups.pickupArray[pick].life -= elapsedTime;
            if (pickups.pickupArray[pick].life < 0) pickups.pickupArray[pick].life = PICKUP_LIFE;
        }
    }

    //pickup crap here
    for (let index in activeUsers) {
        for (let pick in pickups.pickupArray){
            if (collided(activeUsers[index].player, pickups.pickupArray[pick].model)
                && pickups.pickupArray[pick].alive){

                //run pickup logic
                switch (pickups.pickupArray[pick].id) {
                    case 1:
                        activeUsers[index].player.stats.health += 25;
                        if (activeUsers[index].player.stats.health > 100) activeUsers[index].player.stats.health = 100;
                        pickups.pickupArray[pick].life -= elapsedTime;
                        break;
                    case 2:
                        activeUsers[index].player.inventory.armor += 50;
                        if (activeUsers[index].player.inventory.armor > 200)
                            activeUsers[index].player.inventory.armor = 200;
                        pickups.pickupArray[pick].life -= elapsedTime;
                        break;
                    case 3:
                        activeUsers[index].player.inventory.ammo += 10;
                        if (activeUsers[index].player.inventory.ammo > 50)
                            activeUsers[index].player.inventory.ammo = 50;
                        pickups.pickupArray[pick].life -= elapsedTime;
                        break;
                    case 4:
                        pickups.pickupArray[pick].life -= elapsedTime;
                        break;
                }
            }
        }
    }
}

function updatePlayers(elapsedTime) {
    //TODO send out the player update

    let pick_date = {};
    for (let index in pickups.pickupArray){
        if (pickups.pickupArray[index].life === PICKUP_LIFE && pickups.pickupArray[index].alive === false){
            pickups.pickupArray[index].alive = true;
            pick_date.msg = 'show';
            pick_date.pickup = (pickups.pickupArray[index].id)-1;
            io.emit(NetworkIds.UPDATE_PICKUPS, pick_date);
            ioServer.emit(NetworkIds.UPDATE_PICKUPS, pick_date);
        } else if (pickups.pickupArray[index].life !== PICKUP_LIFE && pickups.pickupArray[index].alive === true) {
            pickups.pickupArray[index].alive = false;
            pick_date.msg = 'taken';
            pick_date.pickup = (pickups.pickupArray[index].id)-1;
            io.emit(NetworkIds.UPDATE_PICKUPS, pick_date);
            ioServer.emit(NetworkIds.UPDATE_PICKUPS, pick_date);
        }
    }

    for (let index in activeUsers){
        if (activeUsers[index].player.reportUpdate){
            let name = activeUsers[index].userName;
            activeUsers[index].player.reportUpdate = false;
            let update = {
                userName: name,
                position: activeUsers[index].player.position,
                direction: activeUsers[index].player.direction,
                inventory: activeUsers[index].player.inventory,
                stats: activeUsers[index].player.stats,
                shotFired: activeUsers[index].player.shotFired
            };
            if (activeUsers[index].hasOwnProperty('socket')){
                activeUsers[index].socket.emit(NetworkIds.UPDATE_SELF, update);
            }
            ioServer.emit(NetworkIds.UPDATE_OTHER, update);
            ioServer.emit('log message', name + '- x: ' + update.position.x + ' y: ' +
                update.position.y + ' heading: ' + update.direction);

            for (let index in activeUsers){
                if (activeUsers[index].userName !== name && activeUsers[index].hasOwnProperty('socket')){
                    activeUsers[index].socket.emit(NetworkIds.UPDATE_OTHER, update);
                }
            }

        }
    }
    //reset the shots fired
    for (let index in activeUsers){
        activeUsers[index].shotFired = 0;
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
            activeUsers[index].player.stats.health--;
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

    testFunc();

    if (!quit) {
        setTimeout(() => {
            let now = present();
            gameLoop(now, now - currentTime);
        }, SIMULATION_UPDATE_RATE_MS);
    }
}

function initIo(http, http2) {

    var net = require('net');

    var HOST = '129.123.121.211';
    // var HOST = '144.39.203.228';
    var PORT = 3000;

    net.createServer(function(sock) {

        // console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

        sock.on('data', function(data) {

            // ioServer.emit('log message', sock.remoteAddress + ": " + data);

            inputQueue.enqueue({
                clientId: sock.remotePort,
                clientAddress: sock.remoteAddress,
                message: data
            });

            // console.log('DATA ' + sock.remoteAddress + ': ' + data);
            // Write the data back to the socket, the client will receive it as data from the server
            sock.write('You said "' + data + '"');

        });

        // Add a 'close' event handler to this instance of socket
        sock.on('close', function(data) {
            // console.log('CLOSED: ' + sock.remoteAddress +' '+ sock.remotePort);
        });

    }).listen(PORT, HOST);

    console.log('Server listening on ' + HOST +':'+ PORT);

    ioServer = require('socket.io')(http2);
    io = require('socket.io')(http);

    ioServer.on('connection', function (socket) {
        socket.on('join', function (data) {
            let msg = {
                pickups: pickups.pickupArray
            }
            ioServer.emit('ready', msg);
            connectPlayers();
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
            if(activeUsers[clientIp].joined) {
                activeUsers[clientIp].dead = false;
                activeUsers[clientIp].socket = socket;
                activeUsers[clientIp].id = socket.id;
                // activeUsers[clientIp].player.clientId = socket.id;
                data.name = activeUsers[clientIp].userName;
                let push = {
                    userName: data.name,
                    color: activeUsers[clientIp].player.color,
                    pickups: pickups.pickupArray
                };
                socket.emit('name player', push);
            } else {
                data.name = activeUsers[clientIp].userName;
                console.log(data.name + ' with id ' + socket.id + ' connected');

                activeUsers[clientIp].id = socket.id;
                activeUsers[clientIp].socket = socket;
                activeUsers[clientIp].joined = true;

                let push = {
                    userName: data.name,
                    color: activeUsers[clientIp].player.color,
                    pickups: pickups.pickupArray
                };

                socket.emit('name player', push);
                let client = activeUsers[clientIp];
                for (let index in activeUsers){
                    if (activeUsers[index].userName !== client.userName && activeUsers[index].hasOwnProperty('socket')){
                        activeUsers[index].socket.emit(NetworkIds.CONNECT_OTHER, push);
                        let other = {
                            userName: activeUsers[index].userName,
                            color: activeUsers[index].player.color
                        };
                        client.socket.emit(NetworkIds.CONNECT_OTHER, other);
                    }
                }
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

function connectPlayers() {
    let message;
    for (let index in activeUsers){
        message = {
            userName: activeUsers[index].userName,
            color: activeUsers[index].player.color
        };
        ioServer.emit(NetworkIds.CONNECT_OTHER, message);
    }
}

function createPlayers() {
    let p1 = '144.39.195.27';
    // let p1 = '144.39.203.228';

    let p2 = '144.39.105.156';
    let p3 = '144.39.251.161';

    let player1 = makePlayer(p1, 'green');
    let player2 = makePlayer(p2, 'red');
    let player3 = makePlayer(p3, 'blue');
    activeUsers[p1] = {
        userName: 'Tag_1',
        player: player1,
        joined: false
    };

    activeUsers[p2] = {
        userName: 'Tag_2',
        player: player2,
        joined: false
    };

    activeUsers[p3] = {
        userName: 'Tag_3',
        player: player3,
        joined: false
    };

}

function init(http, http2) {
    initAnchors();
    initIo(http, http2);
    testTrajectory();
    createPlayers();
    pickups = makePickups();
    gameLoop(present(),0);
}

module.exports.init = init;

function makePlayer(id, fill){
    let that = {};

    let reportUpdate = false;

    Object.defineProperty(that, 'color', {
        get: () => color,
        set: value => { color = value; }
    });

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

    Object.defineProperty(that, 'radius', {
        get: () => radius,
        set: value => { radius = value; }
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

    let radius = .02;

    let color = fill;

    let direction = 0;//spec.direction;
    let shotFired = 0;

    return that;
}

function add_weapon(player, weapon){
    player.weapons.push(weapon);
}

function set_direction(player, direction){
    direction *= (Math.PI/180);
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

function collided(obj1, obj2) {
    let distance = Math.sqrt(Math.pow(obj1.position.x - obj2.position.x, 2) + Math.pow(obj1.position.y - obj2.position.y, 2));
    //leeway of .05
    let radii = obj1.radius + obj2.radius + .01;

    return distance <= radii;
}

function makePickups(){
    let that = {};

    let pickupArray = [],
        pickupIndex = [ [0.8, 0.5], [0.5, 0.8], [0.1, 0.7], [0.3, 0.2] ],
        Pickups = {
            num: 4
        },
        pickupSize = {
            width: 0.05,
            height: 0.05
        };

    for(let i=0; i < Pickups.num; i++){
        var pickup_type;

        if(i === 0){ pickup_type = 'cross.png' }
        if(i === 1){ pickup_type = 'shield.png' }
        if(i === 2){
            pickup_type = 'shell.png';
            pickupSize.width = .025;
        }
        if(i === 3){
            pickup_type = 'shotgun.png';
            pickupSize.width = .07;
            pickupSize.height = .03;
        }

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
                radius: 0.025
            },
            id: i+1,
            type: pickup_type,
            life: PICKUP_LIFE,
            alive: true
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
