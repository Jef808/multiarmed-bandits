

export function onError(message) {
    return this({ satus: "error", message: message });
}

export function validTopFields(jreq) {
    return ['model', 'policy', 'parameters'].every(fld => Object.hasAttribute(jreq, fld));
}

export function validModel(jreqModel) {
    result = ['name', 'parameters'].every(fld => Object.hasAttribute(jreqModel, fld));
    result &= ['mab'].includes(jreq);
    return result && ['numberOfArms'].includes(jreqModel);
}

export function validPolicy(jreqPolicy) {
    result = ['epsilonGreedy', 'ucb'].every(fld => Object.hasAttribute(jreqPolicy, fld));
}

export function validParameters(jreqParameters) {
    return ['numberOfSteps'].every(val => Object.hasAttribute(val, jreqParameters));
}


// function depthOf(obj) {
//     // For validating objects passsed as requests.2
//     let level = 1;
//     for (const key in obj) {
//         if (!obj.hasOwnProperty(key)) continue;

//         if (typeof obj[key] == 'object') {
//             let depth = depthOf(obj[key]) + 1;
//             level = Math.max(depth, level);
//         }
//     }
//     return level;
// }


// function serve() {
//     let sock = zmq.socket('req');
//     sock.connect('tcp://127.0.0.1:5555');
//     console.log('Client bound to port 5555')

//     setInterval(function () {
//         console.log('sending request');
//         sock.send('request');
//     }, 500);
// }
