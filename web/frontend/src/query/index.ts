import { reactive } from 'vue'
import { useDefaults, useUniqueIds } from '@/utils'

export const queryOptions = reactive(
    useUniqueIds(
        useDefaults([
            {
                name: "numberOfSteps",
                label: "Number of Steps",
                modelValue: 1,
                min: 1,
            },
        ]), "option-"
    )
)
