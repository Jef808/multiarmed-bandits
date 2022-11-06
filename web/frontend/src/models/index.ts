import { withDefaultParameters } from '@/utils'

export var models = [
    withDefaultParameters({
        name: "mab",
        label: "Multiarmed Bandits",
        parameters: [
            {
                name: "numberOfArms",
                label: "Number of Arms",
                modelValue: 2,
                min: 2,
            },
        ],
    }),
];
