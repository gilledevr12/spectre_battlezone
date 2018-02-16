//initialize

MyMaze.main = (function(graphics, mazeLayout) {

    let pastTime = performance.now();

    let theMaze = graphics.Grid({
        size: mazeLayout.getSize({}),
        layout: mazeLayout.createMaze({})
    });

    function createCharacter(imageSource, cellLocation) {
        let image = new Image();
        image.isReady = false;
        image.onload = function() {
            this.isReady = true;
        };
        image.src = imageSource;
        return {
            size : mazeLayout.getSize(),
            row: cellLocation.row,
            col: cellLocation.col,
            image: image
        };
    }

    function gameLoop() {
        let now = performance.now();
        let elapsedTime = now - pastTime;
        pastTime = now;
        acceptInput();
        update(elapsedTime);
        render();
        requestAnimationFrame(gameLoop);
    }

    function acceptInput() {
//        console.log("goo")
    }

    function update(elapsedTime) {

    }

    function render() {
        graphics.clear();
        theMaze.draw();
        graphics.renderAvatar(goal);
        graphics.renderAvatar(avatar);
    }

    console.log('game initializing...');
    var avatar = createCharacter('falcon.png', mazeLayout.getMaze()[0][0]);
    var goal = createCharacter('death.png', mazeLayout.getMaze()[14][14]);
    requestAnimationFrame(gameLoop);

}(MyMaze.graphics, MyMaze.mazeLayout));
