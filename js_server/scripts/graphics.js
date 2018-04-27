Laser.graphics = (function() {
    let canvas = document.getElementById('canvas-main');
    let canvas_stats = document.getElementById('canvas-stats');

    let context = canvas.getContext('2d');
    let context_stats = canvas_stats.getContext('2d');

    let images = {};

    function resizeCanvas() {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;

        //
        // Have to figure out where the upper left corner of the unit world is
        // based on whether the width or height is the largest dimension.
        if (canvas.width < canvas.height) {
            canvas.height = canvas.width;
        } else {
            canvas.width = canvas.height;
        }

        canvas_stats.width = window.innerWidth - canvas.width;
        canvas_stats.height = canvas.height;
        canvas_stats.style.left = (canvas.width).toString() + "px";

    }

    function initGraphics() {
        window.addEventListener('resize', function() {
            resizeCanvas();
        }, false);
        resizeCanvas();
    }

    //------------------------------------------------------------------
    //
    // Place a 'clear' function on the Canvas prototype, this makes it a part
    // of the canvas, rather than making a function that calls and does it.
    //
    //------------------------------------------------------------------
    CanvasRenderingContext2D.prototype.clear = function() {
        this.save();
        this.setTransform(1, 0, 0, 1, 0, 0);
        this.clearRect(0, 0, canvas.width, canvas.height);
        this.restore();
    };

    //------------------------------------------------------------------
    //
    // Public function that allows the client code to clear the canvas.
    //
    //------------------------------------------------------------------
    function clear() {
        context.clear();
    }

    //------------------------------------------------------------------
    //
    // Rotate the canvas to prepare it for rendering of a rotated object.
    //
    //------------------------------------------------------------------
    function rotateCanvas(center, rotation) {
        context.translate(center.x * canvas.width, center.y * canvas.height);
        context.rotate(rotation);
        context.translate(-(center.x * canvas.width), -(center.y * canvas.height));
    }

    function drawImage(texture, center, size) {
        context.save();
        context.drawImage(images[texture],
            Math.floor((center.x - (size.width / 2)) * canvas.width),
            Math.floor((center.y - (size.height / 2)) * canvas.height),
            Math.ceil(size.width * canvas.width), Math.ceil(size.height * canvas.height));

        context.restore();
    }

    function drawBorder() {
        context.save()
        context.beginPath();
        context.moveTo(0,0);
        context.lineTo(canvas.width, 0);
        context.lineTo(canvas.width, canvas.height);
        context.lineTo(0, canvas.height);
        context.closePath();
        context.strokeStyle = 'blue';

        context.lineWidth = 3;
        context.stroke();
        context.restore();
    }

    function drawText(spec) {
        context_stats.save();
        context_stats.font = spec.font;
        context_stats.fillStyle = 'chocolate';
        context_stats.textBaseline = 'top';

        context_stats.fillText(
            spec.text, spec.position.x * canvas_stats.width, spec.position.y * canvas_stats.height);

        context_stats.strokeStyle = 'black';
        context_stats.strokeText(
            spec.text, spec.position.x * canvas_stats.width, spec.position.y * canvas_stats.height);
        context_stats.restore();
    }

    function drawCircle(color, position, radius) {
        context.save();
        context.beginPath();
        context.arc((position.x*canvas.width), (position.y*canvas.height),
            radius, 0, 2 * Math.PI);
        context.fillStyle = color;
        context.fill();
        context.lineWidth = 2;

        context.stroke();

        context.restore();
    }

    function drawTriangle(color, center, direction, size) {
        context.save();
        rotateCanvas(center, direction);

        context.beginPath();

        context.moveTo(center.x*canvas.width, center.y*canvas.height - (size.height*canvas.height/2));
        context.lineTo(center.x*canvas.width + (size.width*canvas.width/3), center.y*canvas.height + (size.height*canvas.height/3));
        context.lineTo(center.x*canvas.width - (size.width*canvas.width/3), center.y*canvas.height + (size.height*canvas.height/3));
        context.closePath();
        context.fillStyle = color;

        context.fill();
        context.strokeStyle = 'grey';

        context.lineWidth = 1;
        context.stroke();
        context.restore();
    }

    function createImage(location) {
        images[location] = new Image();
        images[location].src = 'images/' + location;
    }

    function drawRectangle(position, size, rotation, fill, stroke) {

        context.save();

        // context.fillStyle = fill;
        // context.strokeStyle = stroke;
        // context.fillRect(Math.floor((position.x)* world.size + world.left),
        //     Math.floor((position.y) * world.size + world.top),
        //     size.width*world.size, size.height*world.size);
        // context.strokeRect(Math.floor((position.x)* world.size + world.left),
        //     Math.floor((position.y) * world.size + world.top),
        //     size.width*world.size, size.height*world.size);

        context.restore();
    }

    function drawLaser(center, orientation) {
        context.save();
        rotateCanvas(center, orientation);
        context.beginPath();
        context.moveTo(center.x*canvas.width, center.y*canvas.height);
        context.lineTo(center.x*canvas.width, center.y*canvas.height - canvas.height);

        context.closePath();
        context.strokeStyle = 'red';

        context.lineWidth = 1;
        context.stroke();
        context.restore();
    }

    return {
        clear: clear,
        createImage: createImage,
        drawImage: drawImage,
        drawTriangle: drawTriangle,
        drawCircle: drawCircle,
        initGraphics: initGraphics,
        drawRectangle: drawRectangle,
        drawLaser: drawLaser,
        drawBorder: drawBorder,
        drawText: drawText
    };
}());
