import type { Parameter } from "@/types";
import type { Model } from "@/types";
export type { Model as Policy };

export const getPolicies = (): Policy[] => [
  {
    name: "epsilonGreedy",
    label: "Epsilon Greedy",
    parameters: [
      {
        name: "epsilon",
        label: "Epsilon",
        value: 0.1,
        min: 0.0,
        max: 1.0,
        sliderStep: 0.05,
      },
    ] as Parameter[],
  },
  {
    name: "ucb",
    label: "Upper Confidence Bound",
    parameters: [
      {
        name: "exploration",
        label: "Exploration Constant",
        value: 0.7,
        min: 0.0,
        sliderStep: 0.05,
      },
    ],
  },
];
