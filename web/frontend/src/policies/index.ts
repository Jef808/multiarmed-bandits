import { withDefaultParameters } from '@/utils'

export var policies = [
    withDefaultParameters({
        name: "epsilonGreedy",
        label: "Epsilon Greedy",
        parameters: [
            {
                name: "epsilon",
                label: "Epsilon",
                modelValue: 0.1,
                min: 0.0,
                max: 1.0,
                sliderStep: 0.05,
            },
        ],
    }),
    withDefaultParameters({
        name: "ucb",
        label: "Upper Confidence Bound",
        parameters: [
            {
                name: "exploration",
                label: "Exploration Constant",
                modelValue: 0.7,
                min: 0.0,
                sliderStep: 0.05,
            },
        ],
    }),
];
