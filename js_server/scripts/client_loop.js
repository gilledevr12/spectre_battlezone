Laser.main = (function(logic, graphics) {

    let socketIO = null;

    let lastTimeStamp, messageId = 1,
        myPlayer = {},
        // myPlayer = {
        //     model: logic.Player(),
        // },
        jobQueue = logic.createQueue(),
        otherUsers = [],
        gameTime = 10 * 60, //seconds
        pickups = [],
        theDead = [],
        myId;

    let cross = logic.cross, shell = logic.shell, shotgun = logic.shotgun, shield = logic.shield, time = logic.time();

    function network() {
        socketIO.on(NetworkIds.CONNECT_ACK, data => {
            jobQueue.enqueue({
                type: NetworkIds.CONNECT_ACK,
                data: data
            });
        });

        socketIO.on(NetworkIds.CONNECT_OTHER, data => {
            jobQueue.enqueue({
                type: NetworkIds.CONNECT_OTHER,
                data: data
            });
        });

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

        socketIO.on(NetworkIds.UPDATE_OTHER, data => {
            jobQueue.enqueue({
                type: NetworkIds.UPDATE_OTHER,
                data: data
            });
        });

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

        socketIO.on(NetworkIds.DEATH, data => {
            jobQueue.enqueue({
                type: NetworkIds.DEATH,
                data: data
            });
        });
    }

    function playerHits(data) {
        if (data.userName === myPlayer.model.userName) {

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
                case NetworkIds.CONNECT_OTHER:
                    connectPlayer(message.data);
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
                case NetworkIds.UPDATE_OTHER:
                    updateOther(message.data);
                    break;
                case NetworkIds.DEATH:
                    updateKill(message.data);
                    break;
            }
        }
    }

    function updateOther(data) {
        // otherUsers[data.userName].position = data.position;
        // otherUsers[data.userName].stats = data.stats;
        // otherUsers[data.userName].direction = data.direction;
        // otherUsers[data.userName].inventory = data.inventory;
        // otherUsers[data.userName].shotFired = data.shotFired;
    }

    function updatePickups(data) {
        if (data.msg === 'show') {
            pickups[data.pickup].alive = true;
        } else if (data.msg === 'taken') {
            pickups[data.pickup].alive = false;
        }
    }

    function updateKill(data) {
        //do something if me? maybe not needed
        if (data.userName !== myId) {
            let deadPerson = {
                time: 7000,
                position: data.player.position,
                size: myPlayer.model.size,
            }
            theDead.push(deadPerson);
        }
    }

    function updateSelf(data) {
        myPlayer.model.position = data.position;
        myPlayer.model.stats = data.stats;
        myPlayer.model.direction = data.direction;
        myPlayer.model.inventory = data.inventory;
        myPlayer.model.shotFired = data.shotFired;
    }

    function reconnectPlayerSelf(data) {
        myPlayer.model.direction = data.direction;
        myPlayer.model.position = data.position;
        myPlayer.model.color = data.color;
    }

    function connectPlayer(data) {
        otherUsers[data.userName] = logic.Player();
        otherUsers[data.userName].stats.id = data.userName;
        otherUsers[data.userName].color = data.color;
    }

    function connectPlayerSelf(data) {
        //TODO delete or use
        myPlayer.model.stats.id = data.userName;
        myPlayer.model.color = data.color;
        // myPlayer.model.position = data.position;
        // myPlayer.model.stats = data.stats;
        // myPlayer.model.direction = data.direction;
        // myPlayer.model.inventory = data.inventory;
        // myPlayer.model.position.x = data.position.x;
        // myPlayer.model.position.y = data.position.y;
    }

    function update(elapsedTime){
        updateMsgs();
        var parseTime = (time.getTime()).split(" ");
        time.text = parseTime[0];
        for (let index in theDead){
            theDead[index].time -= elapsedTime;
            if (theDead[index].time < 0){
                delete theDead[index];
            }
        }
        logic.healthText.text = myPlayer.model.stats.health;
        logic.ammoText.text = myPlayer.model.inventory.ammo;
        logic.armorText.text = myPlayer.model.inventory.armor;
    }

    function render(){
        graphics.clear();
        graphics.drawBorder();

        for (let index in otherUsers){
            graphics.drawTriangle(otherUsers[index].color, otherUsers[index].position, otherUsers[index].direction,
                otherUsers[index].size);
        }

        for (let pickup in pickups){
            let position = pickups[pickup].model.position;
            if (position.hasOwnProperty('x') && pickups[pickup].alive){
                graphics.drawMapImage(pickups[pickup].type, position, pickups[pickup].model.size)
            }
        }

        for (let index in theDead){
            graphics.drawMapImage('skull.png', theDead[index].position, theDead[index].size)
        }

        if (myPlayer.model.stats.alive){
            graphics.drawTriangle(myPlayer.model.color, myPlayer.model.position, myPlayer.model.direction, myPlayer.model.size);
            if (myPlayer.model.shotFired){
                graphics.drawLaser(myPlayer.model.position, myPlayer.model.direction);
            }
        } else {
            graphics.drawMapImage('skull.png', myPlayer.model.position, myPlayer.model.size)
        }

        graphics.drawText(logic.healthText);
        graphics.drawText(logic.armorText);
        graphics.drawText(logic.ammoText);
        graphics.drawText(time);
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

    function init(socket, userId, color, server_pickups) {
        myId = userId;
        socketIO = socket;
        for (let index in server_pickups){
            pickups.push(server_pickups[index]);
        }
        myPlayer.model = logic.Player();
        graphics.initGraphics();
        graphics.createImage('skull.png');
        graphics.createImage('cross.png');
        graphics.createImage('shield.png');
        graphics.createImage('shell.png');
        graphics.createImage('shotgun.png');
        graphics.createImage('player_red.png');
        graphics.createImage('player_blue.png');
        graphics.createImage('player_green.png');
        myPlayer.model.color = color;
        network();
        requestAnimationFrame(gameLoop);
    }

    return {
        init : init
    };

}(Laser.logic, Laser.graphics));

