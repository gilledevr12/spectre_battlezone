let userId = Math.random();

let socket;

let Laser = {
    logic: {},
    main: {},
    main_hub: {},
    graphics: {},
};

socket = io();

socket.on('connect', function(){
    socket.emit('join', {name: userId});
});

socket.on('name player', function (msg) {
    document.getElementById('log-bar').hidden = true;
    // var res = msg.split(" ");
    // if (res[res.length - 1] === userName) userName = res[0];
    userId = msg.userName;
    Laser.main.init(socket, userId, msg.color, msg.pickups);
});

socket.on('ready', function (msg) {
    Laser.main_hub.init(socket, msg.pickups);
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

socket.on('log message', function(msg){
    var node = document.createElement("li");
    var br = document.createElement("br");
    var textnode = document.createTextNode(msg);
    node.appendChild(textnode);
    node.appendChild(br);
    node.className = "list-group-item justify-content-between align-items-center";
    document.getElementById("logmessages").appendChild(node);
    document.getElementById("log-bar").scrollTop = document.getElementById("log-bar").scrollHeight;
});

var time = new Date().getTime();

function refresh(){
	if(new Date().getTime() - time >= 360000){
		window.location.reload(true);
	} else {
		setTimeout(refresh, 180000);
	}
}

setTimeout(refresh, 180000);
