import useRandomNormal from "./randomNormal";

export type Sample = {
  id: number | Date,
  value: number,
}

declare function ArmFactory (id: number | string, mean: number, stdDev: number): {
  sample(nbSamples: number): void;
  pushSample({id, value}: {id: number | Date, value: number}): void;
  id: number | Date;
  samples: Sample[];
  mean: number;
  stdDev: number;
};

export type Arm = ReturnType<typeof ArmFactory>

export default function(id: number | Date, mean: number, stdDev: number): Arm {
  let _samples = [] as Sample[];
  let sample = (nbSamples: number = 1) => {
    var sampleCount = 0;
    for (let i = 0; i < nbSamples; ++i) {
      _samples.push({id: sampleCount++, value: useRandomNormal(mean, stdDev)});
    };
  }
  let pushSample = ({id, value}: {id: number | Date; value: number}) => {
    _samples.push({id, value});
  };
  return {
    id,
    mean,
    stdDev,
    sample,
    pushSample
  };
}
