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

    var HOST = '192.168.0.5';
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
    testTrajectory();
    gameLoop(present(),0);
}

module.exports.init = init;

function makePlayer(spec){
    let that = {};

    Object.defineProperty(that, 'position', {
        get: () => spec.position
    })

    let stats = {
        id: spec.id,
        alive: true,
        health: 100,
        armor: 0,
    }

    let weapons = [];
    weapons.push("pea_shooter");

    let position = {
        x: spec.position.x,
        y: spec.position.y
    }
    
    let direction = spec.direction;

    return that;
}

function add_weapon(player, weapon){
    player.weapons.push(weapon);
}

function set_direction(player, spec){
    player.direction = spec.direction;
}

function get_direction(player){
    return player.direction;
}

function set_position(player, spec){
    player.position.x = spec.position.x;
    player.position.y = spec.position.y;
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

function isInTrajectory(id1, id2, me, myTheta, you){
    console.log(id1 + ": " + me.position.x + "," + me.position.y + " firing at " + id2 + ": " + you.position.x + "," + you.position.y + " with trajectory: " + myTheta);
    let temp_theta = Math.atan2(you.position.y - me.position.y, you.position.x - me.position.x) * 180 / Math.PI;
    console.log("calcd theta: " + temp_theta);
    let temp_distance = Math.sqrt((you.position.y - me.position.y)*(you.position.y - me.position.y) + (you.position.x - me.position.x)*(you.position.x - me.position.x));
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

    let ret = isInTrajectory(1, 3, p1, 55.0, p3);
    if(ret)
        console.log("HIT!");
    else
        console.log("MISS!");

    ret = isInTrajectory(3, 1, p3, -200.0, p1);
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

