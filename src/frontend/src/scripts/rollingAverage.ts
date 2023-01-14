import _, {chunk, reduce, map} from 'underscore';

export function rollingAverage<V extends object>(series: Series<V>, windowSize: number): Series<{}> {
    const _valueSum = (arr) => _.reduce(arr, (s, v) => s + v.value, 0);
    return _.chain(series)
        .chunk(windowSize)
        .map((win, idx) => ({
            step: idx * windowSize,
            value: _valueSum(win) / windowSize
        }))
        .value()
        .slice(0, series.length / windowSize);
}
