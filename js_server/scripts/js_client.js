let userId = Math.random();

let socket;

let Laser = {
    logic: {},
    main: {},
    graphics: {},
};

socket = io();

socket.on('connect', function(){
    socket.emit('join', {name: userId});
});

socket.on('name player', function (msg) {
    var res = msg.split(" ");
    if (res[res.length - 1] === userId) userId = res[0];
    Laser.main.init(socket, userId);
});

// socket.on('chat message', function(msg){
//     var res = msg.split(" ");
//     if (res[res.length - 1] === userId) userId = res[0];
//     var node = document.createElement("li");
//     var br = document.createElement("br");
//     var textnode = document.createTextNode(msg);
//     node.appendChild(textnode);
//     node.appendChild(br);
//     node.className = "list-group-item justify-content-between align-items-center";
//     document.getElementById("messages").appendChild(node);
//     document.getElementById("chat-bar").scrollTop = document.getElementById("chat-bar").scrollHeight;
// });

var time = new Date().getTime();

function refresh(){
	if(new Date().getTime() - time >= 120000){
		window.location.reload(true);
	} else {
		setTimeout(refresh, 5000);
	}
}

setTimeout(refresh, 5000);
