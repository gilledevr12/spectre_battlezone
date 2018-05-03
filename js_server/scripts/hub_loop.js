Laser.main_hub = (function (logic, graphics) {

    let socketIO = null;

    let Y_MAX = 9.67;
    let X_MAX = 10.18;
    let lastTimeStamp,
        jobQueue = logic.createQueue(),
        otherUsers = [],
        gameTime = 10 * 60, //seconds
        pickups = [],
        theDead = [],
        time = logic.time(),
        // start = ((time.getTime()).split(" "))[0];
        start = time.getTime(),
        MILLISECONDS_TO_ZERO = 3600000*17;

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

        socketIO.on(NetworkIds.DEATH, data => {
            jobQueue.enqueue({
                type: NetworkIds.DEATH,
                data: data
            });
        });
    }

    function updateMsgs() {
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
                case NetworkIds.DEATH:
                    updateKill(message.data);
                    break;
            }
        }
    }

    function updateKill(data) {
        //do something if me? maybe not needed
        let deadPerson = {
            time: 7000,
            position: data.position,
            size: otherUsers['Tag_1'].size
        }
        theDead[data.userName] = deadPerson;
    }

    function updateOther(data) {
        otherUsers[data.userName].position = data.position;
        otherUsers[data.userName].stats = data.stats;
        otherUsers[data.userName].direction = data.direction;
        otherUsers[data.userName].inventory = data.inventory;
        otherUsers[data.userName].shotFired = data.shotFired;
    }

    function connectPlayer(data) {
        otherUsers[data.userName] = logic.Player();
        otherUsers[data.userName].color = data.color;
    }

    function updatePickups(data) {
        if (data.msg === 'show') {
            pickups[data.pickup].alive = true;
            console.log('showme')
        } else if (data.msg === 'taken') {
            pickups[data.pickup].alive = false;
            console.log('pickme')
        }
    }

    function update(elapsedTime) {
        var timeNow = time.getTime() - start;
        var date = new Date(timeNow-MILLISECONDS_TO_ZERO);
        time.text = (date.toTimeString().split(" "))[0];
        updateMsgs();
        logic.p1HealthText.text = otherUsers['Tag_1'].stats.health;
        logic.p1AmmoText.text = otherUsers['Tag_1'].inventory.ammo;
        logic.p1ArmorText.text = otherUsers['Tag_1'].inventory.armor;
        logic.p1KillText.text = otherUsers['Tag_1'].stats.kills;
        logic.p1DeathText.text = otherUsers['Tag_1'].stats.deaths;
        logic.p2HealthText.text = otherUsers['Tag_2'].stats.health;
        logic.p2AmmoText.text = otherUsers['Tag_2'].inventory.ammo;
        logic.p2ArmorText.text = otherUsers['Tag_2'].inventory.armor;
        logic.p2KillText.text = otherUsers['Tag_2'].stats.kills;
        logic.p2DeathText.text = otherUsers['Tag_2'].stats.deaths;
    }

    function render() {
        graphics.clear();
        graphics.drawBorder();

        for (let pickup in pickups) {
            let position = pickups[pickup].model.position;
            if (position.hasOwnProperty('x') && pickups[pickup].alive) {
                graphics.drawMapImage(pickups[pickup].type, position, pickups[pickup].model.size)
            }
        }

        for (let index in otherUsers) {
            if (otherUsers[index].stats.alive){
                graphics.drawMapImage(otherUsers[index].color, otherUsers[index].position, otherUsers[index].size, otherUsers[index].direction)
                // graphics.drawTriangle(otherUsers[index].color, otherUsers[index].position,
                //     otherUsers[index].direction, otherUsers[index].size);
                if (otherUsers[index].shotFired) {
                    graphics.drawLaser(otherUsers[index].position, otherUsers[index].direction);
                }
            } else {
                if (theDead[index] !== undefined) {
                    console.log(index)
                    graphics.drawMapImage('skull.png', theDead[index].position, theDead[index].size)
                }
            }
        }

        graphics.drawText(logic.p1);
        graphics.drawText(logic.p2);
        // graphics.drawText(logic.p3);
        graphics.drawText(logic.health);
        graphics.drawText(logic.armor);
        graphics.drawText(logic.ammo);
        graphics.drawText(logic.kills);
        graphics.drawText(logic.deaths);
        graphics.drawText(logic.p1HealthText);
        graphics.drawText(logic.p2HealthText);
        // graphics.drawText(logic.p3HealthText);
        graphics.drawText(logic.p1ArmorText);
        graphics.drawText(logic.p2ArmorText);
        // graphics.drawText(logic.p3ArmorText);
        graphics.drawText(logic.p1AmmoText);
        graphics.drawText(logic.p2AmmoText);
        // graphics.drawText(logic.p3AmmoText);
        graphics.drawText(logic.p1KillText);
        graphics.drawText(logic.p2KillText);
        // graphics.drawText(logic.p3KillText);
        graphics.drawText(logic.p1DeathText);
        graphics.drawText(logic.p2DeathText);
        // graphics.drawText(logic.p3DeathText);
        graphics.drawText(time);
    }

    function gameLoop(time) {
        let elapsedTime = time - lastTimeStamp;
        lastTimeStamp = time;

        update(elapsedTime);
        render();

        requestAnimationFrame(gameLoop);
    };

    function init(socket, server_pickups) {
        socketIO = socket;
        for (let index in server_pickups) {
            pickups.push(server_pickups[index])
        }
        graphics.initGraphics();
        graphics.createImage('skull.png');
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
        init: init
    };

}(Laser.logic, Laser.graphics));

