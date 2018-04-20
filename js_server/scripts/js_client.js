let userId = Math.random();

let socket;

document.getElementById('id-chat').hidden = false;
document.getElementById('id-game').hidden = true;
document.getElementById('min-players').hidden = true;
socket = io();
socket.on('connect', function(){
    socket.emit('join', {name: userId});
});
socket.on('tcp', function(msg){
    var args = msg.split(" ");
    if (args[args.length - 1] === userId) userId = args[0];
    var node = document.createElement("li");
    var br = document.createElement("br");
    var textnode = document.createTextNode(msg);
    node.appendChild(textnode);
    node.appendChild(br);
    node.className = "list-group-item justify-content-between align-items-center";
    document.getElementById("messages").appendChild(node);
    document.getElementById("chat-bar").scrollTop = document.getElementById("chat-bar").scrollHeight;
});
socket.on('start game', function (msg) {
    if (msg === "player reconnect") {
        document.getElementById('id-game').hidden = false;
        document.getElementById('id-chat').hidden = true;
        window.addEventListener('keydown', function(event) {
            socket.emit('input', event.keyCode);
        });
    } else {
        document.getElementById('min-players').hidden = false;
        var countdown = document.getElementById('countdown');

        // format countdown string + set tag value
        countdown.innerHTML = "Ready to Start in: " + msg;
    }

    if (msg === "countdown finished"){
        document.getElementById('id-game').hidden = false;
        document.getElementById('id-chat').hidden = true;
        window.addEventListener('keydown', function(event) {
            socket.emit('input', event.keyCode);
        });
    }
});
socket.on('chat message', function(msg){
    var res = msg.split(" ");
    if (res[res.length - 1] === userId) userId = res[0];
    var node = document.createElement("li");
    var br = document.createElement("br");
    var textnode = document.createTextNode(msg);
    node.appendChild(textnode);
    node.appendChild(br);
    node.className = "list-group-item justify-content-between align-items-center";
    document.getElementById("messages").appendChild(node);
    document.getElementById("chat-bar").scrollTop = document.getElementById("chat-bar").scrollHeight;
});

document.getElementById('button-chat').addEventListener('click', function(){
    socket.emit('chat message', document.getElementById('input-send').value);
    document.getElementById('input-send').value = "";
});

document.getElementById('input-send').addEventListener('keyup', function(event) {
    event.preventDefault();
    if (event.keyCode === 13) { //13 = enter
        document.getElementById('button-chat').click();
    }
});

var time = new Date().getTime();

function refresh(){
	if(new Date().getTime() - time >= 10000){
		window.location.reload(true);
	} else {
		setTimeout(refresh, 5000);
	}
}

setTimeout(refresh, 5000);
