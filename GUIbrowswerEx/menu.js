MyMaze.main = (function(play) {

    let highScore = 0;
    let tempScore = 0;

    function wasClicked() {
        tempScore = play.getScore();
        if (tempScore > highScore) highScore = tempScore;
        play.run({
            highScore : highScore
        });
    }

    function stopClicked() {
        play.stop({});
    }

    return {
        wasClicked : wasClicked,
        stopClicked : stopClicked
    }

}(MyMaze.play));