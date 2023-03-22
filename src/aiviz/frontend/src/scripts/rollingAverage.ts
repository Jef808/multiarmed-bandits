import type { Series } from "@/data/types";
import _ from "underscore";

export function rollingAverage(series: Series, windowSize: number): Series {
  function _valueSum(arr: Series) {
    return _.reduce(arr, (s, v) => s + v.value, 0);
  }
  return _.chain(series)
    .chunk(windowSize)
    .map((win, idx) => ({
      step: idx * windowSize,
      value: _valueSum(win) / windowSize,
    }))
    .value()
    .slice(0, series.length / windowSize);
}
