Laser.graphics = (function() {
    let canvas = document.getElementById('canvas-main');
    let canvas_shield = document.getElementById('canvas-main-shield');
    let canvas_mini = document.getElementById('canvas-mini');
    let canvas_mini_shield = document.getElementById('canvas-mini-shield');
    let canvas_right = document.getElementById('canvas-right');
    let canvas_pregame = document.getElementById('canvas-pregame');
    let user_name = document.getElementById('h1-id-username');
    let timer = document.getElementById('field-clock');
    let ammo_disp = document.getElementById('ammo-display');
    let health_disp = document.getElementById('health-display');
    let log_bar = document.getElementById('log-bar');

    let context = canvas.getContext('2d');
    let context_shield = canvas_shield.getContext('2d');
    let context_mini = canvas_mini.getContext('2d');
    let context_mini_shield = canvas_mini_shield.getContext('2d');
    let context_right = canvas_right.getContext('2d');
    let context_pregame = canvas_pregame.getContext('2d');

    let images = {};
    let world = {
        size: 0,
        top: 0,
        left: 0
    }

    function resizeCanvas() {
        var smallestSize = 0;

        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        canvas_shield.width = canvas.width;
        canvas_shield.height = canvas.height;

        //
        // Have to figure out where the upper left corner of the unit world is
        // based on whether the width or height is the largest dimension.
        if (canvas.width < canvas.height) {
            smallestSize = canvas.width;
            world.size = smallestSize;
            world.left = Math.floor(canvas.width * 0.05);
            world.top = 0;
        } else {
            smallestSize = canvas.height;
            world.size = smallestSize;
            world.top = 0;
            world.left = (canvas.width - world.size) / 2;
        }

        canvas_pregame.width = canvas.width;
        canvas_pregame.height = window.innerHeight;
        canvas_mini.width = world.left;
        canvas_mini.height = canvas.height;
        canvas_mini_shield.width = world.left;
        canvas_mini_shield.height = canvas.height;
        canvas_right.width = world.left;
        canvas_right.height = canvas.height;
        canvas_right.style.left = (world.size + world.left).toString() + "px";

        user_name.style.left = (canvas_mini.width/100).toString() + "px";
        user_name.style.top = (canvas_mini.width).toString() + "px";
        user_name.style.fontSize = (canvas_mini.width/6).toString() + "px";

        timer.style.left = (canvas_mini.width/100).toString() + "px";
        timer.style.top = (canvas_mini.width + (canvas_mini.width/6)).toString() + "px";
        timer.style.fontSize = (canvas_mini.width/6).toString() + "px";

        health_disp.style.left = (canvas_mini.width/100).toString() + "px";
        health_disp.style.top = (canvas_mini.width + (canvas_mini.width/6) + (canvas_mini.width/6)).toString() + "px";
        health_disp.style.fontSize = (canvas_mini.width/6).toString() + "px";

        ammo_disp.style.left = (canvas_mini.width/100).toString() + "px";
        ammo_disp.style.top = (canvas_mini.width + (canvas_mini.width/6) + (canvas_mini.width/6) + (canvas_mini.width/6)).toString() + "px";
        ammo_disp.style.fontSize = (canvas_mini.width/6).toString() + "px";

        log_bar.width = world.left;
        log_bar.height = canvas.height;
        log_bar.style.left = (world.size + world.left).toString() + "px";

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
        this.clearRect(0, 0, canvas_mini.width, canvas_mini.height);
        this.restore();
    };

    //------------------------------------------------------------------
    //
    // Public function that allows the client code to clear the canvas.
    //
    //------------------------------------------------------------------
    function clear() {
        context.clear();
        context_shield.clear();
        context_mini.clear();
        context_mini_shield.clear();
        context_right.clear();
        context_pregame.clear();
    }

    //------------------------------------------------------------------
    //
    // Simple pass-through to save the canvas context.
    //
    //------------------------------------------------------------------
    function saveContext() {
        context.save();
        context_shield.save();
        context_mini.save();
        context_mini_shield.save();
        context_right.save();
    }

    //------------------------------------------------------------------
    //
    // Simple pass-through the restore the canvas context.
    //
    //------------------------------------------------------------------
    function restoreContext() {
        context.restore();
        context_shield.restore();
        context_mini.restore();
        context_mini_shield.restore();
        context_right.restore();
    }

    //------------------------------------------------------------------
    //
    // Rotate the canvas to prepare it for rendering of a rotated object.
    //
    //------------------------------------------------------------------
    function rotateCanvas(center, rotation) {
        context.translate(center.x * world.size + world.left, center.y * world.size + world.top);
        context.rotate(rotation);
        context.translate(-(center.x * world.size + world.left), -(center.y * world.size + world.top));
    }

    //------------------------------------------------------------------
    //
    // Draw an image into the local canvas coordinate system.
    //
    //------------------------------------------------------------------
    function draw(texture, center, size, orientation, me, px_spot, sprite_cnt) {
        context.save();

        if (px_spot === undefined) {
            px_spot = 0;
        }

        if (sprite_cnt === undefined) {
            sprite_cnt = 1;
        }

        if (me){
            orientation -= (Math.PI/2);
        }

        rotateCanvas(center, orientation);

        //fov
        if (me){
            context.globalAlpha = .15;
            context.beginPath();
            context.arc(Math.floor((center.x - size.width / 2) * world.size + world.left) + (size.width * world.size/2),
                Math.floor((center.y - size.height / 2) * world.size + world.top) + (size.width * world.size/2),
                world.size*(.943), Math.PI*(1/8), Math.PI*(7/8), true);
            context.lineTo(Math.floor((center.x - size.width / 2) * world.size + world.left) + (size.width * world.size/2),
                Math.floor((center.y - size.height / 2) * world.size + world.top) + (size.width * world.size/2));
            context.fillStyle = 'gray';
            context.fill();
            context.stroke();
        }

        context.globalAlpha = 1;
        context.drawImage(images[texture], px_spot*(images[texture].width/sprite_cnt), 0,
            images[texture].width/sprite_cnt, images[texture].height,
            Math.floor((center.x - size.width / 2) * world.size + world.left),
            Math.floor((center.y - size.height / 2) * world.size + world.top),
            Math.ceil(size.width * world.size), Math.ceil(size.height * world.size));

        context.restore();

    }

    function drawPeople(center) {
        context_pregame.save();
        context_pregame.drawImage(images['bunny.png'], 0, 0,
            images['bunny.png'].width, images['bunny.png'].height,
            Math.floor((center.x - (.03 / 2)) * canvas_pregame.width),
            Math.floor((center.y - (.03 / 2)) * canvas_pregame.height),
            Math.ceil(.03 * world.size), Math.ceil(.03 * world.size));

        context_pregame.restore();
    }

    function drawGame() {
        context_pregame.save();

        context_pregame.drawImage(
            images['2000x2000map.png'], 0, 0,
            canvas_pregame.width,
            canvas_pregame.height);

        context_pregame.restore();
    }

    function drawShield(center, view) {

        context_shield.save();

        context_shield.beginPath();
        context_shield.fillStyle = 'rgba(0,0,255,0.5)';
        context_shield.fillRect(0,0,canvas.width, canvas.height);

        context_shield.beginPath();
        context_shield.globalCompositeOperation = 'destination-out';
        context_shield.arc(world.left + (center.x - view.left)*world.size,
            world.top + (center.y - view.top) * world.size,
            center.radius*world.size, 0, 2 * Math.PI);
        context_shield.fillStyle = 'white';
        context_shield.fill();

        context_shield.restore();
    }

    function miniMap() {
        var that = {},
            ready = false,
            image_map = new Image();

        image_map.onload = function () {
            ready = true;
        };
        image_map.src = 'images/background/2000x2000map.png';

        that.drawMini = function () {
            if (ready) {
                context_mini.save();

                context_mini.drawImage(
                    image_map,
                    canvas_mini.width/100,
                    canvas_mini.width/100,
                    canvas_mini.width - (canvas_mini.width/50), canvas_mini.width - (canvas_mini.width/50));

                context_mini.restore();
            }
        };

        that.drawPosition = function (position, view, size) {
            context_mini.save();
            let mini_size = canvas_mini.width - (canvas_mini.width/50);
            context_mini.beginPath();
            context_mini.arc(((position.x + view.left)*mini_size/size.width) + canvas_mini.width/100,
                ((position.y + view.top)*mini_size/size.height) + canvas_mini.width/100,
                1.5, 0, 2 * Math.PI);
            context_mini.fillStyle = 'red';
            context_mini.fill();
            context_mini.stroke();

            context_mini.restore();
        };

        that.drawShield = function (position, view, size) {
            context_mini_shield.save();
            let mini_size = canvas_mini.width - (canvas_mini.width/50);

            context_mini_shield.beginPath();
            context_mini_shield.fillStyle = 'rgba(0,0,255,0.5)';
            context_mini_shield.rect(canvas_mini.width/100,
                canvas_mini.width/100,
                canvas_mini.width - (canvas_mini.width/50),
                canvas_mini.width - (canvas_mini.width/50));
            context_mini_shield.fill();


            context_mini_shield.beginPath();
            context_mini_shield.globalCompositeOperation = "destination-out";
            context_mini_shield.arc(((position.x)*mini_size/size.width) + canvas_mini.width/100,
                ((position.y)*mini_size/size.height) + canvas_mini.width/100,
                position.radius*mini_size/size.height, 0, 2 * Math.PI);
            context_mini_shield.fillStyle = 'white';

            context_mini_shield.fill();

            context_mini_shield.restore();
        };

        return that;
    };

    function createImage(location) {
        images[location] = new Image();
        images[location].src = 'images/' + location;
    }

    function drawMissile(center, direction, color) {

        context.save();
        rotateCanvas(center, direction);

        context.beginPath();

        context.moveTo(Math.floor((center.x - .007)* world.size + world.left),
            Math.floor((center.y - .007) * world.size + world.top) - .02);
        context.lineTo(Math.floor((center.x + .02)* world.size + world.left),
            Math.floor((center.y)  * world.size + world.top));
        context.lineTo(Math.floor((center.x - .007)* world.size + world.left),
            Math.floor((center.y + .007) * world.size + world.top));
        context.closePath();
        context.fillStyle = color;

        context.fill();
        context.strokeStyle = 'black';

        context.lineWidth = 2;
        context.stroke();
        context.restore();
    }

    function drawRectangle(position, size, rotation, fill, stroke) {

        context.save();

        context.fillStyle = fill;
        context.strokeStyle = stroke;
        context.fillRect(Math.floor((position.x)* world.size + world.left),
            Math.floor((position.y) * world.size + world.top),
            size.width*world.size, size.height*world.size);
        context.strokeRect(Math.floor((position.x)* world.size + world.left),
            Math.floor((position.y) * world.size + world.top),
            size.width*world.size, size.height*world.size);

        context.restore();
    }

    function TiledImage(spec) {
        var RENDER_POS_EPISILON = 0.00001;

        var viewport = {
                left: 0,
                top: 0
            },
            that = {
                get viewport() { return viewport; },
                get tileSize() { return spec.tileSize; },
                get size() { return spec.size; },
                get pixel() { return spec.pixel; },
                get assetKey() { return spec.assetKey; },
                get tilesX() { return spec.pixel.width / spec.tileSize; },
                get tilesY() { return spec.pixel.height / spec.tileSize; }
            };

        //------------------------------------------------------------------
        //
        // Set the top/left corner of the image viewport.  By definition the
        // size of the viewport is square and of size 1,1 in world coordinates.
        //
        //------------------------------------------------------------------
        that.setViewport = function(left, top) {
            viewport.left = left;
            viewport.top = top;
        };

        //------------------------------------------------------------------
        //
        // Move the viewport by the distance vector.
        //
        //------------------------------------------------------------------
        that.move = function(vector) {
            viewport.left += vector.x;
            viewport.top += vector.y;

            //
            // Make sure we don't move beyond the viewport bounds
            viewport.left = Math.max(viewport.left, 0);
            viewport.top = Math.max(viewport.top, 0);

            viewport.left = Math.min(viewport.left, spec.size.width - 1);
            viewport.top = Math.min(viewport.top, spec.size.height - 1);
        }

        that.render = function() {
            var tileSizeWorldCoords = spec.size.width * (spec.tileSize / spec.pixel.width),
                oneOverTileSizeWorld = 1 / tileSizeWorldCoords,	// Combination of DRY and eliminating a bunch of divisions
                imageWorldXPos = viewport.left,
                imageWorldYPos = viewport.top,
                worldXRemain = 1.0,
                worldYRemain = 1.0,
                renderPosX = 0.0,
                renderPosY = 0.0,
                tileLeft,
                tileTop,
                tileAssetName,
                tileRenderXStart,
                tileRenderYStart,
                tileRenderXDist,
                tileRenderYDist,
                tileRenderWorldWidth,
                tileRenderWorldHeight;

            while (worldYRemain > RENDER_POS_EPISILON) {
                tileLeft = Math.floor(imageWorldXPos * oneOverTileSizeWorld);
                tileTop = Math.floor(imageWorldYPos * oneOverTileSizeWorld);

                if (worldXRemain === 1.0) {
                    tileRenderXStart = imageWorldXPos * oneOverTileSizeWorld - tileLeft;
                } else {
                    tileRenderXStart = 0.0;
                }
                if (worldYRemain === 1.0) {
                    tileRenderYStart = imageWorldYPos * oneOverTileSizeWorld - tileTop;
                } else {
                    tileRenderYStart = 0.0;
                }
                tileRenderXDist = 1.0 - tileRenderXStart;
                tileRenderYDist = 1.0 - tileRenderYStart;
                tileRenderWorldWidth = tileRenderXDist / oneOverTileSizeWorld;
                tileRenderWorldHeight = tileRenderYDist / oneOverTileSizeWorld;
                if (renderPosX + tileRenderWorldWidth > 1.0) {
                    tileRenderWorldWidth = 1.0 - renderPosX;
                    tileRenderXDist = tileRenderWorldWidth * oneOverTileSizeWorld;
                }
                if (renderPosY + tileRenderWorldHeight > 1.0) {
                    tileRenderWorldHeight = 1.0 - renderPosY;
                    tileRenderYDist = tileRenderWorldHeight * oneOverTileSizeWorld;
                }

                tileAssetName = spec.assetKey + '-' + Rocket.assets.numberPad(tileTop * that.tilesX + tileLeft, 3);

                context.drawImage(
                    Rocket.assets[tileAssetName].image,
                    tileRenderXStart * spec.tileSize, tileRenderYStart * spec.tileSize,
                    tileRenderXDist * spec.tileSize, tileRenderYDist * spec.tileSize,
                    Math.floor(renderPosX * world.size + world.left), Math.floor(renderPosY * world.size + world.top),
                    Math.ceil(tileRenderWorldWidth * world.size), Math.ceil(tileRenderWorldHeight * world.size));

                imageWorldXPos += tileRenderWorldWidth;
                renderPosX += tileRenderWorldWidth;

                //
                // Subtract off how much of the current tile we used, if there isn't any
                // X distance to render, then move down to the next row of tiles.
                worldXRemain -= tileRenderWorldWidth;
                if (worldXRemain <= RENDER_POS_EPISILON) {
                    imageWorldYPos += tileRenderWorldHeight;
                    renderPosY += tileRenderWorldHeight;
                    worldYRemain -= tileRenderWorldHeight;

                    imageWorldXPos = viewport.left;
                    renderPosX = 0.0;
                    worldXRemain = 1.0;
                }
            }
        };

        return that;
    };

    return {
        clear: clear,
        saveContext: saveContext,
        restoreContext: restoreContext,
        rotateCanvas: rotateCanvas,
        draw: draw,
        drawShield: drawShield,
        createImage: createImage,
        TiledImage: TiledImage,
        initGraphics: initGraphics,
        miniMap: miniMap,
        drawMissile: drawMissile,
        drawGame: drawGame,
        drawPeople: drawPeople,
        drawRectangle: drawRectangle
    };
}());
