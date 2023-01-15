import { useRandomNormal } from "./randomNormal";

export class Sample {
  constructor(
    public value: number,
    public step: number
  ) { }
}

export function ValueOf(sample: Sample) {
  return sample.value;
}

export class Arm {
  constructor(
    public id: number,
    public value: number,
    public samples: Sample[] = [],
    public noiseStdDev: number = 1
  ) { }

  sample(step: number) {
    const value = this.value + useRandomNormal(0, this.noiseStdDev);
    this.samples.push(new Sample(value, step));
    return value;
  }
}
