import { useRandomNormal } from "./randomNormal";

export type Sample = {
  readonly value: number;
  readonly step: number;
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
    this.samples.push({ value, step });
    return value;
  }
}
