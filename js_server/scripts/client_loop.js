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
            }
        }
    }

    function updateOther(data) {
        otherUsers[data.userName].position = data.position;
        otherUsers[data.userName].stats = data.stats;
        otherUsers[data.userName].direction = data.direction;
        otherUsers[data.userName].inventory = data.inventory;
        otherUsers[data.userName].shotFired = data.shotFired;
    }

    function updatePickups(data) {

    }

    function updateSelf(data) {
        // if(data.hasOwnProperty('winner')) {
        //     if(data.winner){
        //         // alert("Congratulations! You are the winner!");
        //     }
        // }
        // if (data.hasOwnProperty('weapon')){
        //     myPlayer.model.weapon = data.weapon;
        // }
        // if (data.hasOwnProperty('health')){
        //     myPlayer.model.health = data.health;
        // }
        // if (data.hasOwnProperty('ammo')){
        //     myPlayer.model.ammo = data.ammo;
        // }
        // if (data.dead){
        //     myPlayer.model.dead = data.dead;
        // }
        // gameTime = data.gameTime;
        // // pickups = data.pickups;
        // console.log(data)
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
        otherUsers[data.userName].stats.id = data.userName;
        otherUsers[data.userName] = logic.Player();
        otherUsers[data.userName].color = data.color;
    }

    function connectPlayerSelf(data) {
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
        for (let index in otherUsers){
            // otherUsers[index].model.update(elapsedTime);
        }
    }

    function render(){
        graphics.clear();
        graphics.drawBorder();

        for (let index in otherUsers){
            graphics.drawTriangle(otherUsers[index].color, otherUsers[index].position, otherUsers[index].direction,
                otherUsers[index].size);
        }

        for (let pickup in pickups){
            // let position = pickups[pickup].position;
            // if (position.hasOwnProperty('x')){
            //     // graphics.draw(pickups[pickup].texture, position, {width: pickups[pickup].width,height: pickups[pickup].height},0,false);
            // }
        }

        graphics.drawTriangle(myPlayer.model.color, myPlayer.model.position, myPlayer.model.direction, myPlayer.model.size);
        if (myPlayer.model.shotFired){
            graphics.drawLaser(myPlayer.model.position, myPlayer.model.direction);
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

    function init(socket, userId, color) {
        myId = userId;
        socketIO = socket;
        myPlayer.model.color = color;
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

