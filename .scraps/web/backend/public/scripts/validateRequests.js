export function validTopFields(jreq) {
    return ['model', 'policy', 'parameters'].every(fld => Object.hasAttribute(jreq, fld));
}

export function validPolicy(jreqPolicy) {
    result = ['epsilonGreedy', 'ucb'].every(fld => Object.hasAttribute(jreqPolicy, fld));
}

export function validParameters(jreqParameters) {
    return ['numberOfSteps'].every(fld => Object.hasAttribute(jreqParameters, fld));
}
