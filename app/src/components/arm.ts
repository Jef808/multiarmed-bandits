import { useRandomNormal } from "./randomNormal";

export class Arm {
  constructor(
    public id: number,
    public value: number,
    public samples: number[] = [],
    public noiseStdDev: number = 1
  ) {}

  sample() {
    const value = this.value + useRandomNormal(0, this.noiseStdDev);
    this.samples.push(value);
    return value;
  }
}
