
MyMaze.graphics = (function() {
    let canvas = document.getElementById('canvas-main');
    let context = canvas.getContext('2d');
    let crumbsToggleButton = false;
    let hintToggleButton = false;
    let pathToggleButton = false;
    let nextPath = {};

    function cleanUp() {
        crumbsToggleButton = false;
        hintToggleButton = false;
        pathToggleButton = false;
        nextPath = 0;
        clear();
    }

    function clear() {
        context.save();
        context.setTransform(1, 0, 0, 1, 0, 0);
        context.clearRect(0, 0, canvas.width, canvas.height);
        context.restore();
    }

    function Grid(spec) {
        let that = {};

        that.draw = function() {
            let width = (canvas.width/spec.size);
            let height = (canvas.height/spec.size);

            context.beginPath();
            for (let i = 0; i < spec.size; i++){
                for (let j = 0; j < spec.size; j++){
                    context.moveTo(j*width, height*i);
                    if (spec.layout[i][j].ref.up === null) context.lineTo((j+1)*width, height*i)
                    context.moveTo((j+1)*width, height*i);
                    if (spec.layout[i][j].ref.right === null) context.lineTo((j+1)*width, (i+1)*height);
                    context.moveTo((j+1)*width, height*(i+1));
                    if (spec.layout[i][j].ref.down === null) context.lineTo(j*width, (i+1)*height);
                    context.moveTo(j*width, height*(i+1));
                    if (spec.layout[i][j].ref.left === null) context.lineTo(j*width, height*i);
                    context.lineWidth = 2;
                }
            }
            context.strokeStyle = 'rgb(255,255,255)';
            context.closePath();
            context.stroke();
        };

        return that;
    }

    function crumbs(spec) {
        let that = {};

        let image = new Image();
        image.onload = function() {};
        image.src = spec.imageSource;

        that.renderCrumbs = function(){
            nextPath = spec.trail[0];
            if ((crumbsToggleButton && spec.id === "bread") ||
                (pathToggleButton && spec.id === "path")) {
                for (let crumbs in spec.trail) {
                    context.save();
                    if (spec.fade){
                        context.globalAlpha = .25;
                    }
                    context.drawImage(image, spec.trail[crumbs].col * (canvas.width / spec.size) + (canvas.width / spec.size / 4),
                        spec.trail[crumbs].row * (canvas.height / spec.size) + (canvas.height / spec.size / 4),
                        canvas.width / spec.size / 2, canvas.height / spec.size / 2);
                    context.restore();
                }
            } else if ((hintToggleButton && spec.id === "yoda")){
                context.save();
                context.drawImage(image, spec.trail[0].col * (canvas.width / spec.size) + (canvas.width / spec.size / 4),
                    spec.trail[0].row * (canvas.height / spec.size) + (canvas.height / spec.size / 4),
                    canvas.width / spec.size / 2, canvas.height / spec.size / 2);
                context.restore();
            }
        };

        return that;
    }

    function createCharacter(spec){
        let that = {};
        let isReady = false;

        let image = new Image();
        image.onload = function() {
            isReady = true;
        };
        image.src = spec.imageSource;

        let trail = [];
        let score = 0;

        that.getTrail = function () {
            return trail;
        };

        that.getLocation = function () {
            return spec.cell;
        };

        that.renderAvatar = function() {
            if (isReady) {
                context.save();
                context.drawImage(image, spec.cell.col*(canvas.width/spec.size),
                    spec.cell.row*(canvas.height/spec.size), canvas.width/spec.size, canvas.height/spec.size);
                context.restore();
            }
        };

        function points() {
            if (nextPath.row === spec.cell.row && nextPath.col === spec.cell.col){
                score += 3;
            } else {
                score -= 5;
            }
        }

        that.getScore = function(){
            return score;
        };

        that.moveUp = function(elapsedTime) {
            trail.push({row: spec.cell.row, col: spec.cell.col});
            if (spec.cell.ref.up != null) {
                spec.cell = spec.cell.ref.up;
                points();
            }
        };

        that.moveDown = function(elapsedTime) {
            trail.push({row: spec.cell.row, col: spec.cell.col});
            if (spec.cell.ref.down != null) {
                spec.cell = spec.cell.ref.down;
                points();
            }
        };

        that.moveLeft = function(elapsedTime) {
            trail.push({row: spec.cell.row, col: spec.cell.col});
            if (spec.cell.ref.left != null) {
                spec.cell = spec.cell.ref.left;
                points();
            }
        };

        that.moveRight = function(elapsedTime) {
            trail.push({row: spec.cell.row, col: spec.cell.col});
            if (spec.cell.ref.right != null) {
                spec.cell = spec.cell.ref.right;
                points();
            }
        };

        return that;
    }

    function crumbsToggle(elapsedTime) {
        crumbsToggleButton = !crumbsToggleButton;
    };

    function finishPath(elapsedTime) {
        pathToggleButton = !pathToggleButton;
    };

    function hintToggle(elapsedTime) {
        hintToggleButton = !hintToggleButton;
    };

    return {
        clear : clear,
        Grid : Grid,
        crumbs : crumbs,
        createCharacter : createCharacter,
        crumbsToggle : crumbsToggle,
        hintToggle : hintToggle,
        finishPath : finishPath,
        cleanUp : cleanUp,
    };
}());

