MyMaze.main = (function(play) {

    let highScore = 0;
    let tempScore = 0;

    // document.getElementById("UI_layout").addEventListener("load", wasClicked);

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

    wasClicked();

    return {
        wasClicked : wasClicked,
        stopClicked : stopClicked
    }

}(MyMaze.play));