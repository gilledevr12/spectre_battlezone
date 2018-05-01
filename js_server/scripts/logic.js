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

    Object.defineProperty(that, 'color', {
        get: () => color,
        set: value => { color = value; }
    });

    let stats = {
        id: '',
        alive: true,
        health: 100
    };

    let color = '';

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

Laser.logic.p1 = {
    font: '48px serif',
    text: "|-Player1-|",
    position: {
        x: .18,
        y: .51
    },
    width: 1,
    fill: 'black'
}

Laser.logic.p2 = {
    font: '48px serif',
    text: "-Player2-",
    position: {
        x: .44,
        y: .51
    },
    width: 1,
    fill: 'black'
}

Laser.logic.p3 = {
    font: '48px serif',
    text: "|-Player3-|",
    position: {
        x: .67,
        y: .51
    },
    width: 1,
    fill: 'black'
}

Laser.logic.health1 = {
    font: '35px serif',
    text: "Health: ",
    position: {
        x: .021,
        y: .60
    },
    width: 1,
    fill: 'black'
}

Laser.logic.armor = {
    font: '35px serif',
    text: "Armor: ",
    position: {
        x: .021,
        y: .65
    },
    width: 1,
    fill: 'black'
}
Laser.logic.ammo = {
    font: '35px serif',
    text: "Ammo: ",
    position: {
        x: .021,
        y: .70
    },
    width: 1,
    fill: 'black'
}

Laser.logic.healthText = {
    font: '130px serif',
    text: "100",
    position: {
        x: .5,
        y: .0001
    },
    width: 4,
    fill: 'chocolate'
}

Laser.logic.armorText = {
    font: '130px serif',
    text: "100",
    position: {
        x: .5,
        y: .25
    },
    width: 4,
    fill: 'chocolate'
}

Laser.logic.ammoText = {
    font: '130px serif',
    text: "20",
    position: {
        x: .5,
        y: .50
    },
    width: 4,
    fill: 'chocolate'
}
Laser.logic.p1HealthText = {
    font: '35px serif',
    text: "100",
    position: {
        x: .25,
        y: .60
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p2HealthText = {
    font: '35px serif',
    text: "100",
    position: {
        x: .51,
        y: .60
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p3HealthText = {
    font: '35px serif',
    text: "100",
    position: {
        x: .74,
        y: .60
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p1ArmorText = {
    font: '35px serif',
    text: "0",
    position: {
        x: .25,
        y: .65
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p2ArmorText = {
    font: '35px serif',
    text: "0",
    position: {
        x: .51,
        y: .65
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p3ArmorText = {
    font: '35px serif',
    text: "0",
    position: {
        x: .74,
        y: .65
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p1AmmoText = {
    font: '35px serif',
    text: "20",
    position: {
        x: .25,
        y: .70
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p2AmmoText = {
    font: '35px serif',
    text: "20",
    position: {
        x: .51,
        y: .70
    },
    width: 1,
    fill: 'black'
}
Laser.logic.p3AmmoText = {
    font: '35px serif',
    text: "20",
    position: {
        x: .74,
        y: .70
    },
    width: 1,
    fill: 'black'
}


Laser.logic.cross = {
    texture: 'cross.png',
    position: {
        x: 0.25,
        y: 0.10,
    },
    size: {
        width: 0.22,
        height: 0.19
    }
};

Laser.logic.shield = {
    texture: 'shield.png',
    position: {
        x: 0.25,
        y: 0.35,
    },
    size: {
        width: 0.22,
        height: 0.19
    }
};

Laser.logic.shell = {
    texture: 'shell.png',
    position: {
        x: 0.25,
        y: 0.60,
    },
    size: {
        width: 0.09,
        height: 0.12
    }
};

Laser.logic.shotgun = {
    texture: 'shotgun.png',
    position: {
        x: 0.50,
        y: 0.87,
    },
    size: {
        width: 0.35,
        height: 0.08
    }
};

Laser.logic.player_red = {
    texture: 'player_red.png',
    position: {
        x: 0.15,
        y: 0.55,
    },
    size: {
        width: 0.05,
        height: 0.05
    }
};

Laser.logic.player_blue = {
    texture: 'player_blue.png',
    position: {
        x: 0.15,
        y: 0.60,
    },
    size: {
        width: 0.05,
        height: 0.05
    }
};

Laser.logic.player_green = {
    texture: 'player_green.png',
    position: {
        x: 0.15,
        y: 0.65,
    },
    size: {
        width: 0.05,
        height: 0.05
    }
};

Laser.logic.time = function () {

    let that = {};

    that.getTime = function () {
        var date = new Date();
        return date.toTimeString();
    };

    Object.defineProperty(that, 'fill', {
        get: () => fill,
    });

    Object.defineProperty(that, 'position', {
        get: () => position,
    });

    Object.defineProperty(that, 'width', {
        get: () => width,
    });

    Object.defineProperty(that, 'font', {
        get: () => font,
    });


    Object.defineProperty(that, 'text', {
        get: () => text,
        set: value => { text = value; }
    });

    let font = '60px serif';
    let text = '';
    let position = {
        x: .05,
        y: .87
    };
    let width = 2;
    let fill = 'black';

    return that;
};
