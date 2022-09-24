/// Sample a normal distribution with mean `0` and standard deviation `1`.
function boxMullerTransform() {
  return (
    Math.sqrt(-2.0 * Math.log(1 - Math.random())) *
    Math.cos(2.0 * Math.PI * Math.random())
  );
}

export function useRandomNormal(mean: number, stdDev: number) {
  return mean + stdDev * boxMullerTransform();
}
