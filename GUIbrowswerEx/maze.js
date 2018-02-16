MyMaze.mazeLayout = (function() {

    let allCells = [];
    let possibleCells = [];
    let mazeCells = [];
    let whichContact = [];
    let dim = 15;
    var is = true;

    function getSize() {
        return dim;
    }

    for (let i = 0; i < dim; i++){
        allCells.push([])
        for (j = 0; j < dim; j++){
            allCells[i].push({
                ref: {
                    up: null,
                    down: null,
                    left: null,
                    right: null
                },
                row: i, col: j, maze: false
            });
        }
    }

    function init(){
        var rowStart = Math.floor(Math.random()*(dim - 2)) + 1;
        var colStart = Math.floor(Math.random()*(dim - 2)) + 1;
        mazeCells.push({
            ref: {
                up: null,
                down: null,
                left: null,
                right: null
            },
            row: rowStart, col: colStart, maze: true
        });
        allCells[rowStart][colStart].maze = true;
        for (i = -1; i < 2; i++){
            possibleCells.push({
                ref: {
                    up: null,
                    down: null,
                    left: null,
                    right: null
                },
                row: rowStart + i, col: colStart, maze: false
            });
            i++;
        }
        for (i = -1; i < 2; i++) {
            possibleCells.push({
                ref: {
                    up: null,
                    down: null,
                    left: null,
                    right: null
                },
                row: rowStart, col: colStart + i, maze: false
            });
            i++;
        }
    }

    function insert(target, row, col) {
        target.push({
            ref: {
                up: null,
                down: null,
                left: null,
                right: null
            },
            row: row,
            col: col
        });
    }

    function notInList(target, row, col) {
        for (let things in target){
            if (target[things].row === row && target[things].col === col ){
                return false;
            }
        }
        return true;
    }

    function passesTests(cell, direction, target) {
        var inMaze = cell.maze;
        switch(direction){
            case "up":
                if (cell.row - 1 >= 0){
                    if (inMaze) {
                        if (!allCells[cell.row - 1][cell.col].maze && notInList(target, cell.row - 1, cell.col)){
                            insert(target, cell.row - 1, cell.col);
                        }
                    } else {
                        insert(target, cell.row - 1, cell.col);
                    }
                }
                break;
            case "left":
                if (cell.col - 1 >= 0){
                    if (inMaze) {
                        if (!allCells[cell.row][cell.col-1].maze && notInList(target, cell.row, cell.col - 1)) {
                            insert(target, cell.row, cell.col - 1);
                        }
                    } else {
                        insert(target, cell.row, cell.col - 1);
                    }
                }
                break;
            case "down":
                if (cell.row + 1 < dim) {
                    if (inMaze) {
                        if (!allCells[cell.row + 1][cell.col].maze && notInList(target, cell.row + 1, cell.col)) {
                            insert(target, cell.row + 1, cell.col);
                        }
                    } else {
                        insert(target, cell.row + 1, cell.col);
                    }
                }
                break;
            case "right":
                if (cell.col + 1 < dim) {
                    if (inMaze) {
                        if (!allCells[cell.row][cell.col+1].maze && notInList(target, cell.row, cell.col + 1)){
                            insert(target, cell.row, cell.col + 1);
                        }
                    } else {
                        insert(target, cell.row, cell.col + 1);
                    }
                }
                break;
            default:
                return false;
        }
    }

    function addToFrontier() {
        var newestCell = mazeCells[mazeCells.length - 1];
        for (let dir in newestCell.ref){
            if (newestCell.ref[dir] === null) {
                passesTests(newestCell, dir, possibleCells);
                possibleCells[possibleCells.length - 1].maze = false
            }
        }
    }

    function foundCellInMaze(cell) {
        if (cell === null) return false;
        var dir = null;
        for (let index in mazeCells){
            if (mazeCells[index].row + 1 === cell.row && mazeCells[index].col === cell.col) dir = "up";
            if (mazeCells[index].row - 1 === cell.row && mazeCells[index].col === cell.col) dir = "down";
            if (mazeCells[index].row  === cell.row && mazeCells[index].col + 1 === cell.col) dir = "left";
            if (mazeCells[index].row  === cell.row && mazeCells[index].col - 1 === cell.col) dir = "right";
            if (dir != null) {
                passesTests(cell, dir, whichContact);
                dir = null;
            }
        }
        return false;
    }

    function findDir(whichCell, possibleCell) {
        //returns relative to the cell to be added
        if (whichCell.row === possibleCell.row){
            if (whichCell.col === possibleCell.col - 1){
                allCells[whichCell.row][whichCell.col].ref.right = possibleCell;
                allCells[possibleCell.row][possibleCell.col].ref.left = allCells[whichCell.row][whichCell.col];
            } else { //col + 1
                allCells[whichCell.row][whichCell.col].ref.left = possibleCell;
                allCells[possibleCell.row][possibleCell.col].ref.right = allCells[whichCell.row][whichCell.col];
            }
        } else { //then columns are equal
            if (whichCell.row === possibleCell.row - 1){
                allCells[whichCell.row][whichCell.col].ref.down = possibleCell;
                allCells[possibleCell.row][possibleCell.col].ref.up = allCells[whichCell.row][whichCell.col];
            } else {
                allCells[whichCell.row][whichCell.col].ref.up = possibleCell;
                allCells[possibleCell.row][possibleCell.col].ref.down = allCells[whichCell.row][whichCell.col];
            }
        }
        possibleCell.maze = true;
        allCells[possibleCell.row][possibleCell.col].maze = true;
        mazeCells.push(possibleCell);
    }

    function removeWall() {
        var index = Math.floor(Math.random()*(possibleCells.length - 1));

        foundCellInMaze(possibleCells[index]);

        var newIndex = Math.floor(Math.random()*(whichContact.length - 1));

        findDir(whichContact[newIndex], possibleCells[index]);
        possibleCells.splice(index,1);
        whichContact.length = 0;
    }

    function createMaze(){
        init();
        while(possibleCells.length){
            console.log("hello")
            removeWall();
            if (possibleCells.length) addToFrontier();
        }
        // for (i = 0; i < dim; i++){
        //     for(j = 0; j < dim; j++){
        //         console.log(allCells[i][j])
        //     }
        // }
        return allCells;
    }

    function getMaze() {
        return allCells;
    }

    return {
        createMaze : createMaze,
        getSize : getSize,
        getMaze : getMaze
    };

}());

