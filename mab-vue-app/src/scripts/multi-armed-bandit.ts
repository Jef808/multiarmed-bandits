import * as utils from "./utils";
import { normal_dist } from "./prob_dist/normal-dist";

export default function(
    numberOfArms: number,
    valueMean?: number,
    valueStdDev?: number,
    noiseStdDev?: number
) {
    const makeValue = () =>
        valueMean ?? 0 + gaussianDist({ mean: 0, stdDev: valueStdDev ?? 1 })();
    const sample = (armValue: number) =>
        armValue + gaussianDist({ mean: 0, stdDev: noiseStdDev ?? 1 })();

    return utils.makeObject({
        data: {
            arms: [
                ...Array.from({ length: numberOfArms }, (_, i) =>
                    Object.create({
                        id: i,
                        value: makeValue(),
                        total: 0,
                        visits: 0,
                    })
                ),
            ],
        },
        methods: {
            sample(arm: number) {
                if (arm < 0 || arm >= this.arms.length) throw "arm out of bound";
                return sample(this.arms[arm].value);
            },
        },
    });
}
