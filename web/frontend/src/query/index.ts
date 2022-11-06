import { withDefaultParams } from '@/utils'

export var queryOptions = [
    withDefaultParams({
        name: "numberOfSteps",
        label: "Number of Steps",
        modelValue: 1,
        min: 1,
    }),
];
