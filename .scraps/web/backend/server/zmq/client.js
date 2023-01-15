import * as zmq from 'zeromq'


function depthOf(obj) {
    // For validating objects passsed as requests.2
    let level = 1;
    for (const key in obj) {
        if (!obj.hasOwnProperty(key)) continue;

        if (typeof obj[key] == 'object') {
            let depth = depthOf(obj[key]) + 1;
            level = Math.max(depth, level);
        }
    }
    return level;
}

function serve() {
    let sock = zmq.socket('req');
    sock.connect('tcp://127.0.0.1:5555');
    console.log('Client bound to port 5555')

    setInterval(function () {
        console.log('sending request');
        sock.send('request');
    }, 500);
}
