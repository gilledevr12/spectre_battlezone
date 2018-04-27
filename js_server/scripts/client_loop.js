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
    }

    function connectPlayerSelf(data) {
        myPlayer.model.userId = data.userId;
        // myPlayer.model.position = data.position;
        // myPlayer.model.stats = data.stats;
        // myPlayer.model.direction = data.direction;
        // myPlayer.model.inventory = data.inventory;
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

        // console.log(myPlayer.model)
        graphics.drawTriangle('green', myPlayer.model.position, myPlayer.model.direction, myPlayer.model.size);
        if (myPlayer.model.shotFired){
            graphics.drawLaser(myPlayer.model.position, myPlayer.model.direction);
        }
        let text = {
            font: '48px serif',
            text: "100",
            position: {
                x: .5,
                y: .02
            }
        }
        graphics.drawText(text);
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

        network();
        requestAnimationFrame(gameLoop);
    }

    return {
        init : init
    };

}(Laser.logic, Laser.graphics));

