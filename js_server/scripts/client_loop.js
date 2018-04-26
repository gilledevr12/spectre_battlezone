Laser.main = (function(logic, graphics, assets) {

    let socketIO = null;
    let beenDead = false;

    let lastTimeStamp, messageId = 1,
        myPlayer = {
            model: logic.Player(),
            sprite: logic.Sprite({
                spriteSheet: 'bunnysheet.png',
                spriteCount: 8,
                me: true,
                spriteSize: .05,			// Maintain the size on the sprite
            }, graphics)},
        background = null,
        initialized = false,
        mini = graphics.miniMap(),
        jobQueue = logic.createQueue(),
        otherUsers = [],
        missiles = {},
        hits = [],
        pregame = true,
        gameCount = 0,
        gameTime = 10 * 60, //seconds
        shield = {x:0,y:0,radius:0,particles:[]},
        pickups = [],
        worldParams = {
            height: 5,
            width: 5
        },
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
            hits.push({
                particle: logic.ParticleSystem({
                    position: {
                        x: data.position.x,
                        y: data.position.y
                    },
                    size: .005,
                    speed: .2,
                    lifetime: 200,
                    fill: 'rgba(255, 0, 0, 0.5)',
                    direction: data.direction - Math.PI,
                    theta: Math.PI/4
                }, graphics),
                life: 200
            });
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
        if (pregame) {
            if (isNaN(gameCount)){
                gameCount = 0;
            }
            gameCount += elapsedTime;
            if (gameCount > 10000 && !initialized){
                document.addEventListener("click", printMousePos);
            }
            if (gameCount > 20000) {
                pregame = false;
                document.removeEventListener("click", printMousePos);
                update(elapsedTime);
            }
        }
        shiftView(myPlayer.model.position, elapsedTime);
        myPlayer.model.position = obstacle();
        myPlayer.model.projected = myPlayer.model.position;
        for (let index in otherUsers){
            otherUsers[index].model.update(elapsedTime);
            otherUsers[index].sprite.update(elapsedTime);
        }
        myPlayer.sprite.update(elapsedTime);

        for (let index in hits){
            hits[index].life -= elapsedTime;
            hits[index].particle.update(elapsedTime);
            if (hits[index].life < 0) {
                hits.splice(index, 1);
            }
        }

        let removeMissiles = [];
        for (let missile in missiles) {
            if (!missiles[missile].update(elapsedTime)) {
                removeMissiles.push(missiles[missile]);
            } else if (missiles[missile].particle) {
                missiles[missile].particle.setPosition(missiles[missile].position.x, missiles[missile].position.y);
                missiles[missile].particle.update(elapsedTime);
            }
        }

        for(let a = 0; a < shield.particles.length; a++) {
            let angle = a*(2*Math.PI/360);
            shield.particles[a].setPosition(shield.x+ Math.cos(angle)*shield.radius,shield.y+ Math.sin(angle)*shield.radius);
            shield.particles[a].update(elapsedTime);
        }

        for (let missile = 0; missile < removeMissiles.length; missile++) {
            delete missiles[removeMissiles[missile].id];
        }
    }

    function render(){
        graphics.clear();
        if (pregame){
            graphics.drawGame();
            let myPosition = {
                x: (myPlayer.model.position.x + background.viewport.left)/5,
                y: (myPlayer.model.position.y + background.viewport.top)/5,
            }
            graphics.drawPeople(myPosition);
            for (let index in otherUsers){
                let position = {
                    x: otherUsers[index].model.map.x/5,
                    y: otherUsers[index].model.map.y/5
                }
                graphics.drawPeople(position);
            }
            return;
        }
        background.render();

        for (let index in otherUsers){
            let object = otherUsers[index].model.state.position;
            if (!object.hasOwnProperty('x')) continue;
            let position = drawObjects(object);
            if(!otherUsers[index].model.state.dead){
                if (position.hasOwnProperty('x')){
                    otherUsers[index].sprite.render(position, otherUsers[index].model.state.orientation - (Math.PI/2));
                }
            }
        }
        for (let missile in missiles){
            let position = drawObjects(missiles[missile].position);
            if (position.hasOwnProperty('x')){
                graphics.drawMissile(position, missiles[missile].direction, 'orange');
                if (missiles[missile].particle){
                    missiles[missile].particle.render(background.viewport);
                }
            }
        }
        for (let pickup in pickups){
            let position = drawObjects(pickups[pickup].position);
            if (position.hasOwnProperty('x')){
                graphics.draw(pickups[pickup].texture, position, {width: pickups[pickup].width,height: pickups[pickup].height},0,false);
            }
        }

        for (let building in buildingArray){
            let position = drawObjects(buildingArray[building].model.position, true);
            if (position.hasOwnProperty('x')){
                graphics.draw(buildingArray[building].texture, position,
                    buildingArray[building].model.size, buildingArray[building].model.orientation, false)
            }
        }

        // draw self
        if(myPlayer.model.dead){
            if(!beenDead){
                beenDead = true;
                deadSound.play();
            }

            graphics.draw('tombstone.png', myPlayer.model.position, myPlayer.model.size, myPlayer.model.orientation, false);
        }else{
            myPlayer.sprite.render(myPlayer.model.position, myPlayer.model.orientation);
            if (myPlayer.model.weapon >= 0) {
                let vectorX = Math.cos(myPlayer.model.orientation) * (myPlayer.model.radius*1.75);
                let vectorY = Math.sin(myPlayer.model.orientation) * (myPlayer.model.radius*1.75);
                let position = {
                    x: myPlayer.model.position.x + vectorX,
                    y: myPlayer.model.position.y + vectorY
                }
                if (myPlayer.model.weapon){
                    let size = {
                        width: .04,
                        height: .04
                    }
                    graphics.draw('bazooka2.png', position, size, myPlayer.model.orientation, false);
                } else {
                    let size = {
                        width: .05,
                        height: .01
                    }
                    graphics.draw('bazooka1.png', position, size, myPlayer.model.orientation, false);
                }
            }
            let rectSize = {
                width: .05,
                height: .01
            }
            let rectPosition = {
                x: myPlayer.model.position.x - .02,
                y: myPlayer.model.position.y - .04
            }

            graphics.drawRectangle(rectPosition, rectSize, 0, 'red', 'black');
            rectSize.width = (myPlayer.model.health/100)*rectSize.width;
            graphics.drawRectangle(rectPosition, rectSize, 0, 'green', 'black');
        }

        for (let tree in treeArray){
            let position = drawObjects(treeArray[tree].model.position, true);
            if (position.hasOwnProperty('x')){
                graphics.draw(treeArray[tree].texture, position,
                    treeArray[tree].model.size, treeArray[tree].model.orientation, false)
            }
        }

        for (let index in hits){
            hits[index].particle.render(background.viewport);
        }
        graphics.drawShield(shield, background.viewport);

        for (let particle = 0; particle<shield.particles.length; particle += (5-Math.ceil(shield.radius))) {
            let position = drawObjects(shield.particles[particle].position, true);
            if (position.hasOwnProperty('x')){
                shield.particles[particle].render(background.viewport)
            }
        }

        mini.drawMini();
        mini.drawPosition(myPlayer.model.position, background.viewport, background.size);
        mini.drawShield(shield, background.viewport, background.size);

        document.getElementById('field-clock').innerHTML = gameClock(gameTime);
        document.getElementById('health-display').innerHTML = "Health: " + myPlayer.model.health;
        document.getElementById('ammo-display').innerHTML = "Ammo: " + myPlayer.model.ammo;
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

        network();
        requestAnimationFrame(gameLoop);
    }

    return {
        init : init
    };

}(Laser.logic, Laser.graphics, Laser.assets));

