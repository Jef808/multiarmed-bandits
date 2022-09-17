export function withGaussianNoise(mean: number, stdDev: number) {
    return (num?: number) => (num ?? 0) + mean + stdDev * boxMullerTransform();
}

const boxMullerTransform = () => {
    let u = 0;
    while (u < Number.EPSILON) {
        u = Math.random();
    }
    const v = Math.random();
    return Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v);
};
