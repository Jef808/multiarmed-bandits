import * as zmq from 'zeromq'

function request_receiver(req) {

}

function serve() {
    let sock = zmq.socket('req');
    sock.connect('tcp://127.0.0.1:5555');
    console.log('Client bound to port 5555')

    setInterval(function() {
        console.log('sending request');
        sock.send('request');
    }, 500);
}
