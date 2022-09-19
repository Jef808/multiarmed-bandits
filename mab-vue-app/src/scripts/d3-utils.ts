import * as d3 from "d3";

export interface binParams {
    size?: number;
    ratio?: number;
    number?: number;
}

export interface Point {
    x: number;
    y: number;
}

export function getNumberOfBins(p: binParams, d: Array<Point>): number {
    const params = Object.entries(p).filter((k, v) => Boolean(v));
    const keys = Object.keys(params);

    let result = 0;

    console.log("params:", params, "\nkeys:", keys);

    if (keys.length != 1) return -1;

    const key: string = keys[0];
    switch (key) {
        case "number": {
            result = <number>p.number;
            break;
        }
        case "size": {
            result = d.length / <number>p.size;
            break;
        }
        case "ratio": {
            result = d.length * <number>p.ratio;
            break;
        }
    }

    return result;
}

export function getBins(
    p: binParams,
    x: Array<number>,
    y: Array<number>
) {
    const numberOfBins = getNumberOfBins(
        p,
        x.map(function(d: number, i: number): Point {
            return {
                x: d,
                y: y[i],
            };
        })
    );

    const xScale = d3.scaleLinear().domain([x[0], x[x.length - 1]]);

    const thresholdArray = [...Array(numberOfBins)].map(
        (_, ind) => (xScale.domain()[1] / numberOfBins) * ind
    );

    const histogram = d3
        .bin()
        .domain(function(values: Iterable<number>): [number, number] {
            return [Math.min(...values), Math.max(...values)];
        })
        .value((_: number, i: number) => y[i])
        .thresholds(thresholdArray);

    return histogram(y);
}

export function myMovingAverage(
    bins: Array<d3.Bin<number, number>>,
    values: Array<number>
) {
    return bins
        .map((b) => {
            let result: number = NaN;

            const [l, r] = [b.x0, b.x1];
            if (!!l && !!r) {
                const [left, right] = [l, r] as [number, number];

                const size = right - left;
                result =
                    size > 0
                        ? values.slice(left, size).reduce((a, e) => a + e) / size
                        : 0;
            }

            return result;
        })
        .filter((v) => !!v);
}
