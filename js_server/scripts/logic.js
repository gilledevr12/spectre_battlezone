Laser.logic.Player = function () {
    let that = {};

    Object.defineProperty(that, 'position', {
        get: () => position,
        set: value => { position = value; }
    });

    Object.defineProperty(that, 'direction', {
        get: () => direction,
        set: value => { direction = value; }
    });

    Object.defineProperty(that, 'stats', {
        get: () => stats,
        set: value => { stats = value; }
    });

    Object.defineProperty(that, 'size', {
        get: () => size,
        set: value => { size = value; }
    });

    Object.defineProperty(that, 'inventory', {
        get: () => inventory,
        set: value => { inventory = value; }
    });

    Object.defineProperty(that, 'shotFired', {
        get: () => shotFired,
        set: value => { shotFired = value; }
    });

    let stats = {
        id: '',
        alive: true,
        health: 100
    };

    let inventory = {
        armor: 0,
        ammo: 0,
        weapon: "pea_shooter"
    };

    let position = {
        x: .2,
        y: .2
    };

    let size = {
        width: .04, height: .04
    };

    let direction = 0;
    let shotFired = 0;

    return that;
}

Laser.logic.createQueue = function () {

    let that = [];

    that.enqueue = function(value) {
        that.push(value);
    }

    that.dequeue = function() {
        return that.shift();
    }

    Object.defineProperty(that, 'front', {
        get: () => that[0]
    });

    Object.defineProperty(that, 'empty', {
        get: () => { return that.length === 0; }
    });

    return that;

};