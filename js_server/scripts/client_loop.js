let cross = { 
    texture: 'cross.png',
    position: {
        x: 0.25, 
        y: 0.10,
    },
    size: {
        width: 0.22,
        height: 0.19
    }
};

let shield = { 
    texture: 'shield.png',
    position: {
        x: 0.25, 
        y: 0.35,
    },
    size: {
        width: 0.22,
        height: 0.19
    }
};

let shell = { 
    texture: 'shell.png',
    position: {
        x: 0.25, 
        y: 0.60,
    },
    size: {
        width: 0.09,
        height: 0.12
    }
};

let shotgun = { 
    texture: 'shotgun.png',
    position: {
        x: 0.50, 
        y: 0.87,
    },
    size: {
        width: 0.35,
        height: 0.08
    }
};

let player_red = { 
    texture: 'player_red.png',
    position: {
        x: 0.15, 
        y: 0.55,
    },
    size: {
        width: 0.05,
        height: 0.05
    }
};

let player_blue = { 
    texture: 'player_blue.png',
    position: {
        x: 0.15, 
        y: 0.60,
    },
    size: {
        width: 0.05,
        height: 0.05
    }
};

let player_green = { 
    texture: 'player_green.png',
    position: {
        x: 0.15, 
        y: 0.65,
    },
    size: {
        width: 0.05,
        height: 0.05
    }
};

Laser.main = (function(logic, graphics) {

    let socketIO = null;

    let lastTimeStamp, messageId = 1,
        myPlayer = {
            model: logic.Player(),
        },
        jobQueue = logic.createQueue(),
        otherUsers = [],
        gameTime = 10 * 60, //seconds
        pickups = [],
        myId;

    function network() {
        socketIO.on(NetworkIds.CONNECT_ACK, data => {
            jobQueue.enqueue({
                type: NetworkIds.CONNECT_ACK,
                data: data
            });
        });

        // socketIO.on(NetworkIds.CONNECT_OTHER, data => {
        //     jobQueue.enqueue({
        //         type: NetworkIds.CONNECT_OTHER,
        //         data: data
        //     });
        // });

        socketIO.on(NetworkIds.RECONNECT_SELF, data => {
            jobQueue.enqueue({
                type: NetworkIds.RECONNECT_SELF,
                data: data
            });
        });
        //
        // socketIO.on(NetworkIds.RECONNECT_OTHER, data => {
        //     jobQueue.enqueue({
        //         type: NetworkIds.RECONNECT_OTHER,
        //         data: data
        //     });
        // });

        // socketIO.on(NetworkIds.UPDATE_OTHER, data => {
        //     jobQueue.enqueue({
        //         type: NetworkIds.UPDATE_OTHER,
        //         data: data
        //     });
        // });

        socketIO.on(NetworkIds.UPDATE_SELF, data => {
            jobQueue.enqueue({
                type: NetworkIds.UPDATE_SELF,
                data: data
            });
        });

        socketIO.on(NetworkIds.UPDATE_PICKUPS, data => {
            jobQueue.enqueue({
                type: NetworkIds.UPDATE_PICKUPS,
                data: data
            });
        });
    }

    function playerHits(data) {
        if (data.userId === myPlayer.model.userId) {

        }
    }

    function updateMsgs(){
        let processMe = jobQueue;
        jobQueue = jobQueue = logic.createQueue();
        while (!processMe.empty) {
            let message = processMe.dequeue();
            switch (message.type) {
                case NetworkIds.CONNECT_ACK:
                    connectPlayerSelf(message.data);
                    break;
                case NetworkIds.RECONNECT_SELF:
                    reconnectPlayerSelf(message.data);
                    break;
                case NetworkIds.UPDATE_SELF:
                    updateSelf(message.data);
                    break;
                case NetworkIds.UPDATE_PICKUPS:
                    updatePickups(message.data);
                    break;
            }
        }
    }

    function updatePickups(data) {

    }

    function updateSelf(data) {
        if(data.hasOwnProperty('winner')) {
            if(data.winner){
                // alert("Congratulations! You are the winner!");
            }
        }
        if (data.hasOwnProperty('weapon')){
            myPlayer.model.weapon = data.weapon;
        }
        if (data.hasOwnProperty('health')){
            myPlayer.model.health = data.health;
        }
        if (data.hasOwnProperty('ammo')){
            myPlayer.model.ammo = data.ammo;
        }
        if (data.dead){
            myPlayer.model.dead = data.dead;
        }
        gameTime = data.gameTime;
        // pickups = data.pickups;
    }

    function reconnectPlayerSelf(data) {
        myPlayer.model.orientation = data.orientation;
        myPlayer.model.position.x = data.position.x;
        myPlayer.model.position.y = data.position.y;
    }

    function connectPlayerSelf(data) {
        myPlayer.model.userId = data.userId;
        // myPlayer.model.position.x = data.position.x;
        // myPlayer.model.position.y = data.position.y;
    }

    function update(elapsedTime){
        updateMsgs();
        for (let index in otherUsers){
            // otherUsers[index].model.update(elapsedTime);
        }
    }

    function render(){
        graphics.clear();
        graphics.drawBorder();

        for (let index in otherUsers){

        }

        for (let pickup in pickups){
            // let position = pickups[pickup].position;
            // if (position.hasOwnProperty('x')){
            //     // graphics.draw(pickups[pickup].texture, position, {width: pickups[pickup].width,height: pickups[pickup].height},0,false);
            // }
        }

        let position = {
            x: .2, y: .5
        }

        let size = {
            width: .04, height: .04
        }
        graphics.drawTriangle('green', position, Math.PI * (3/4), size);
        graphics.drawLaser(position,Math.PI * (3/4));
        let healthText = {
            font: '130px serif',
            text: "100",
            position: {
                x: .5,
                y: .0001
            }
        }

        let armorText = {
            font: '130px serif',
            text: "100",
            position: {
                x: .5,
                y: .25
            }
        }

        let ammoText = {
            font: '130px serif',
            text: "20",
            position: {
                x: .5,
                y: .50
            }
        }

        graphics.drawText(healthText);
        graphics.drawText(armorText);
        graphics.drawText(ammoText);
        graphics.drawStatsImage(cross.texture, cross.position, cross.size);
        graphics.drawStatsImage(shield.texture, shield.position, shield.size);
        graphics.drawStatsImage(shell.texture, shell.position, shell.size);
        graphics.drawStatsImage(shotgun.texture, shotgun.position, shotgun.size);
        //graphics.drawStatsImage(player_red.texture, player_red.position, player_red.size);
        //graphics.drawStatsImage(player_blue.texture, player_blue.position, player_blue.size);
        //graphics.drawStatsImage(player_green.texture, player_green.position, player_green.size);
        // draw self
        if (myPlayer.model.dead) {
            // graphics.draw('tombstone.png', myPlayer.model.position, myPlayer.model.size, myPlayer.model.orientation, false);
        } else {
            // myPlayer.sprite.render(myPlayer.model.position, myPlayer.model.orientation);
        }

        // graphics.drawText();
    }

    function gameLoop(time) {
        let elapsedTime = time - lastTimeStamp;
        lastTimeStamp = time;

        update(elapsedTime);
        render();

        requestAnimationFrame(gameLoop);
    };

    //
    // function printMousePos(event) {
    //     let message = {
    //         id: messageId++,
    //         type: NetworkIds.CLICK,
    //         userId: myId,
    //         x: event.clientX,
    //         y: event.clientY,
    //         width: document.getElementById('canvas-pregame').width,
    //         height: document.getElementById('canvas-pregame').height
    //     };
    // }

    function init(socket, userId) {
        myId = userId;
        socketIO = socket;
        graphics.initGraphics();
        graphics.createImage('cross.png');
        graphics.createImage('shield.png');
        graphics.createImage('shell.png');
        graphics.createImage('shotgun.png');
        graphics.createImage('player_red.png');
        graphics.createImage('player_blue.png');
        graphics.createImage('player_green.png');
        network();
        requestAnimationFrame(gameLoop);
    }

    return {
        init : init
    };

}(Laser.logic, Laser.graphics));

