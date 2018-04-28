Laser.main_hub = (function(logic, graphics) {

    let socketIO = null;

    let X_MAX = 15;
    let Y_MAX = 15;
    let lastTimeStamp,
        jobQueue = logic.createQueue(),
        otherUsers = [],
        gameTime = 10 * 60, //seconds
        pickups = [];

    function network() {
        // socketIO.on(NetworkIds.CONNECT_ACK, data => {
        //     jobQueue.enqueue({
        //         type: NetworkIds.CONNECT_ACK,
        //         data: data
        //     });
        // });

        socketIO.on(NetworkIds.CONNECT_OTHER, data => {
            jobQueue.enqueue({
                type: NetworkIds.CONNECT_OTHER,
                data: data
            });
        });

        socketIO.on(NetworkIds.RECONNECT_OTHER, data => {
            jobQueue.enqueue({
                type: NetworkIds.RECONNECT_OTHER,
                data: data
            });
        });

        socketIO.on(NetworkIds.UPDATE_OTHER, data => {
            jobQueue.enqueue({
                type: NetworkIds.UPDATE_OTHER,
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

    function updateMsgs(){
        let processMe = jobQueue;
        jobQueue = jobQueue = logic.createQueue();
        while (!processMe.empty) {
            let message = processMe.dequeue();
            switch (message.type) {
                case NetworkIds.CONNECT_OTHER:
                    connectPlayer(message.data);
                    break;
                case NetworkIds.RECONNECT_OTHER:
                    reconnectPlayer(message.data);
                    break;
                case NetworkIds.UPDATE_OTHER:
                    updateOther(message.data);
                    break;
                case NetworkIds.UPDATE_PICKUPS:
                    updatePickups(message.data);
                    break;
            }
        }
    }

    function updateOther(data){
        otherUsers[data.userId].position = data.position;
        otherUsers[data.userId].stats = data.stats;
        otherUsers[data.userId].direction = data.direction;
        otherUsers[data.userId].inventory = data.inventory;
        otherUsers[data.userId].shotFired = data.shotFired;

    }

    function connectPlayer(data) {
        otherUsers[data.userId] = logic.Player();
    }

    function updatePickups(data) {

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

        graphics.drawText(logic.p1);
        graphics.drawText(logic.p2);
        graphics.drawText(logic.p3);
        graphics.drawText(logic.health1);
        graphics.drawText(logic.ammo);
    }

    function gameLoop(time) {
        let elapsedTime = time - lastTimeStamp;
        lastTimeStamp = time;

        update(elapsedTime);
        render();

        requestAnimationFrame(gameLoop);
    };

    function init(socket) {
        socketIO = socket;
        graphics.initGraphics();
        network();
        requestAnimationFrame(gameLoop);
    }

    return {
        init : init
    };

}(Laser.logic, Laser.graphics));

