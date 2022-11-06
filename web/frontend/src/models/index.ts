import { reactive } from 'vue'
import { useDefaultParameters, useUniqueParamsIds } from '@/utils'

export var models = reactive(
    useUniqueParamsIds(
        useDefaultParameters([
            {
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
            },
        ]), "model-param-"
    )
)
