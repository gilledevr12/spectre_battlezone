Laser.main_hub = (function(logic, graphics) {

    let socketIO = null;

    let Y_MAX = 9.67;
    let X_MAX = 10.18;
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
                    // reconnectPlayer(message.data);
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
        otherUsers[data.userName].position = data.position;
        otherUsers[data.userName].stats = data.stats;
        otherUsers[data.userName].direction = data.direction;
        otherUsers[data.userName].inventory = data.inventory;
        otherUsers[data.userName].shotFired = data.shotFired;

    }

    // function assignColor(){
    //     if (Object.keys(otherUsers).length === 1){
    //         return 'green';
    //     }
    //     if (Object.keys(otherUsers).length === 2){
    //         return 'red';
    //     }
    //     if (Object.keys(otherUsers).length === 3){
    //         return 'yellow';
    //     }
    // }

    function connectPlayer(data) {
        otherUsers[data.userName] = logic.Player();
        otherUsers[data.userName].color = data.color;
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
            graphics.drawTriangle(otherUsers[index].color, otherUsers[index].position,
                otherUsers[index].direction, otherUsers[index].size);
            if (otherUsers[index].shotFired){
                graphics.drawLaser(otherUsers[index].position, otherUsers[index].direction);
            }

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
        graphics.drawText(logic.armor);
        graphics.drawText(logic.ammo);
        graphics.drawText(logic.p1HealthText);
        graphics.drawText(logic.p2HealthText);
        graphics.drawText(logic.p3HealthText);
        graphics.drawText(logic.p1ArmorText);
        graphics.drawText(logic.p2ArmorText);
        graphics.drawText(logic.p3ArmorText);
        graphics.drawText(logic.p1AmmoText);
        graphics.drawText(logic.p2AmmoText);
        graphics.drawText(logic.p3AmmoText);
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

