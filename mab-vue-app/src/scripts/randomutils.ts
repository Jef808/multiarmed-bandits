export interface distribution<D> {
    data: D = {};
    sample?(): number;
}

export const distribution<D>{
    return Object.create({
        data: D,
        sample: (d: D) => number
    });


}(s: (d: D) => number, d: D) {
    let data: D = d;
    let sample: (d: D) => number = s;
    return () => {
        sample(data);
    };
}

export type GaussianDistData = { mean: number; stdDev: number };

export function gaussianDist(
    args: { mean: number; stdDev: number } = { mean: 0, stdDev: 1 }
) {
    let data: GaussianDistData = args;
    let sample: (d: GaussianDistData) => number = function(d) {
        return d.mean + d.stdDev * boxMullerTransform();
    };
    return () => sample(data);
}

const boxMullerTransform = () => {
    let u = 0;
    while (u < Number.EPSILON) {
        u = Math.random();
    }
    const v = Math.random();
    return Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v);
};
