//initialize
MyMaze.play = (function(graphics, mazeLayout, input) {

    let done = false;
    let stopped = false;
    let score = 0;

    function getScore() {
        return score;
    }

    function run(spec) {

        done = false;
        stopped = false;
        let scoreToggleButton = false;
        let highScore = spec.highScore;
        let pastTime = performance.now();
        let stopwatch = {};
        let halfsecond = false;
        stopwatch.time = 0;
        var options = document.getElementById("dimension");
        let dimension = options.value;

        let theMaze = graphics.Grid({
            size: dimension,
            layout: mazeLayout.createMaze({
                size : dimension
            })
        });

        let avatar = graphics.createCharacter({
            imageSource: 'falcon.png',
            cell: mazeLayout.getMaze()[0][0],
            size: dimension
        });

        let pastPath = graphics.crumbs({
            imageSource: 'rebel.png',
            trail: avatar.getTrail(),
            size: dimension,
            fade: false,
            id: "bread"
        });

        let pathToEnd = graphics.crumbs({
            imageSource: 'rebel.png',
            trail: mazeLayout.getTrail(),
            size: dimension,
            fade: true,
            id: "path"
        });

        let hint = graphics.crumbs({
            imageSource: 'yoda.png',
            trail: mazeLayout.getTrail(),
            size: dimension,
            fade: false,
            id: "yoda"
        });

        let goal = graphics.createCharacter({
            imageSource: 'death.png',
            cell: mazeLayout.getMaze()[dimension - 1][dimension - 1],
            size: dimension
        });

        function findPathRecursive() {
            mazeLayout.getTrail().length = 0;
            findPath(mazeLayout.getMaze()[mazeLayout.getSize() - 1][mazeLayout.getSize() - 1]);
            if (mazeLayout.getTrail().length === 0) done = true;
        }

        function findPath(cell) {
            if (cell.row === avatar.getLocation().row && cell.col === avatar.getLocation().col) {
                //mazeLayout.getTrail().push(cell);
                cell.visited = false;
                return true;
            }
            cell.visited = true;
            if (cell.ref.up && (typeof cell.ref.up.visited === 'undefined' || !cell.ref.up.visited)) {
                if (findPath(cell.ref.up)) {
                    mazeLayout.getTrail().push(cell);
                    cell.visited = false;
                    return true;
                }
            }
            if (cell.ref.down && (typeof cell.ref.down.visited === 'undefined' || !cell.ref.down.visited)) {
                if (findPath(cell.ref.down)) {
                    mazeLayout.getTrail().push(cell);
                    cell.visited = false;
                    return true
                }
            }
            if (cell.ref.left && (typeof cell.ref.left.visited === 'undefined' || !cell.ref.left.visited)) {
                if (findPath(cell.ref.left)) {
                    mazeLayout.getTrail().push(cell);
                    cell.visited = false;
                    return true
                }
            }
            if (cell.ref.right && (typeof cell.ref.right.visited === 'undefined' || !cell.ref.right.visited)) {
                if (findPath(cell.ref.right)) {
                    mazeLayout.getTrail().push(cell);
                    cell.visited = false;
                    return true
                }
            }
            cell.visited = false;
            return false;
        }

        function scoreToggle(elapsedTime) {
            scoreToggleButton = !scoreToggleButton;
        }

        let inputs = input.Keyboard();

        function gameLoop() {
            let now = performance.now();
            let elapsedTime = now - pastTime;
            pastTime = now;
            update(elapsedTime);
            render();
            if (done) {
                graphics.cleanUp();
                mazeLayout.cleanUp();
                score = avatar.getScore();
                return;
            }
            requestAnimationFrame(gameLoop);
        }

        function acceptInput(elapsedTime) {
            inputs.processInput(elapsedTime);
        }

        function update(elapsedTime) {
            acceptInput(elapsedTime);
            findPathRecursive();
            if (stopped) done = true;
            stopwatch.time += elapsedTime;

            stopwatch.minutes = parseInt(Math.floor(stopwatch.time / 60000));
            stopwatch.minutes = (stopwatch.minutes < 10) ? "0" + stopwatch.minutes : stopwatch.minutes;
            stopwatch.seconds = parseInt(Math.floor(stopwatch.time / 1000) % 60);
            stopwatch.seconds = (stopwatch.seconds < 10) ? "0" + stopwatch.seconds : stopwatch.seconds;

            ((stopwatch.time / 100) % 10 > 4) ? halfsecond = true : halfsecond = false;
        }

        function render() {
            var node1 = document.getElementById('message');
            if (done){
                node1.innerHTML = ("Congrats!!! Your score was " + avatar.getScore() + ". Click Start to play again.");
            } else {
                node1.innerHTML = ("");
                graphics.clear();
                theMaze.draw();
                pastPath.renderCrumbs();
                goal.renderAvatar();
                if (halfsecond) {
                    pathToEnd.renderCrumbs();
                }
                hint.renderCrumbs();
                avatar.renderAvatar();
                var node = document.getElementById('time');
                node.innerHTML = ("Time: " + stopwatch.minutes + ":" + stopwatch.seconds);
                var node2 = document.getElementById('score');
                (scoreToggleButton) ? node2.innerHTML = ("Score: " + avatar.getScore()) : node2.innerHTML = ("");
                var node3 = document.getElementById('highScore');
                node3.innerHTML = ("High Score: " + highScore);
            }
        }

        console.log('game initializing...');

        inputs.registerCommand(KeyEvent.DOM_VK_UP, avatar.moveUp);
        inputs.registerCommand(KeyEvent.DOM_VK_DOWN, avatar.moveDown);
        inputs.registerCommand(KeyEvent.DOM_VK_LEFT, avatar.moveLeft);
        inputs.registerCommand(KeyEvent.DOM_VK_RIGHT, avatar.moveRight);
        inputs.registerCommand(KeyEvent.DOM_VK_W, avatar.moveUp);
        inputs.registerCommand(KeyEvent.DOM_VK_S, avatar.moveDown);
        inputs.registerCommand(KeyEvent.DOM_VK_A, avatar.moveLeft);
        inputs.registerCommand(KeyEvent.DOM_VK_D, avatar.moveRight);
        inputs.registerCommand(KeyEvent.DOM_VK_I, avatar.moveUp);
        inputs.registerCommand(KeyEvent.DOM_VK_K, avatar.moveDown);
        inputs.registerCommand(KeyEvent.DOM_VK_J, avatar.moveLeft);
        inputs.registerCommand(KeyEvent.DOM_VK_L, avatar.moveRight);
        inputs.registerCommand(KeyEvent.DOM_VK_H, graphics.hintToggle);
        inputs.registerCommand(KeyEvent.DOM_VK_B, graphics.crumbsToggle);
        inputs.registerCommand(KeyEvent.DOM_VK_P, graphics.finishPath);
        inputs.registerCommand(KeyEvent.DOM_VK_Y, scoreToggle);

        requestAnimationFrame(gameLoop);
    }

    function stop() {
        stopped = true;
    }

    return {
        run : run,
        stop : stop,
        getScore : getScore
    }

}(MyMaze.graphics, MyMaze.mazeLayout, MyMaze.input));