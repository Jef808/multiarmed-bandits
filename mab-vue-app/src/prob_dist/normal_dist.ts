import construct_distribution from "./prob-dist";

type NormalData = { mean: number; stdDev: number };
const normalDefs: NormalData = { mean: 0.0, stdDev: 1.0 };

export default function normal_distribution([mean, stdDev]: [
    number,
    number
]): () => number {
    return construct_distribution<NormalData>(
        normalDefs,
        normalDefs,
        () => mean + stdDev * boxMullerTransform()
    );
}

/// Sample a Gaussian distribution with mean `0` and standard deviation `1`.
function boxMullerTransform(): number {
    let u = 0;
    while (u < Number.EPSILON) {
        u = Math.random();
    }
    const v = Math.random();
    return -2.0 * Math.log(u) * Math.cos(2.0 * Math.PI * v);
}

/// Sample a Gaussian distribution with mean `mean` and standard deviation `stdDev`.
