
MyMaze.graphics = (function() {
    //'use strict';

    let canvas = document.getElementById('canvas-main');
    let context = canvas.getContext('2d');

    function clear() {
        //context.save();
        context.setTransform(1, 0, 0, 1, 0, 0);
        context.clearRect(0, 0, canvas.width, canvas.height);
        //context.restore();
    }

    function Grid(spec) {
        let that = {};

        that.draw = function() {
            let width = (canvas.width/spec.size);
            let height = (canvas.height/spec.size);

            //console.log("start")
            context.beginPath();
            for (let i = 0; i < spec.size; i++){
                for (let j = 0; j < spec.size; j++){
                    var row = spec.layout[i][j].row;
                    var col = spec.layout[i][j].col;
                    context.moveTo(i*width, height*j);
                    if (spec.layout[j][i].ref.up === null) {context.lineTo((i+1)*width, height*j)}//; console.log("up", row, col)}
                    context.moveTo((i+1)*width, height*j);
                    if (spec.layout[j][i].ref.right === null) {context.lineTo((i+1)*width, (j+1)*height)}//; console.log("right", row, col);}
                    context.moveTo((i+1)*width, height*(j+1));
                    if (spec.layout[j][i].ref.down === null) {context.lineTo(i*width, (j+1)*height)}//; console.log("down", row, col);}
                    context.moveTo(i*width, height*(j+1));
                    if (spec.layout[j][i].ref.left === null) {context.lineTo(i*width, height*j)}//; console.log("left", row, col);}
                    context.lineWidth = 2;
                }
            }
            context.closePath();
            context.stroke();
        };

        return that;
    }

    function renderAvatar(spec) {
        if (spec.image.isReady) {
            context.drawImage(spec.image, spec.row*(canvas.width/spec.size), spec.col*(canvas.height/spec.size),
                canvas.width/spec.size, canvas.height/spec.size);
        }
    }



    return {
        clear : clear,
        Grid : Grid,
        renderAvatar : renderAvatar
    };
}());

