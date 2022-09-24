import { scaleLinear, scaleBand } from "d3-scale";
import { bin, group, rollup, extent, min, max } from "d3-array";
import { area, curveCatmullRom } from "d3-shape";
import type { Arm } from "./arm";
import type { Bin, ScaleBand, ScaleLinear } from "d3";
const d3 = {
    scaleLinear, scaleBand, bin, group, rollup, extent, min, max, area, curveCatmullRom
};


function getYExtent(data: Arm[]) {
    const yExtents: Array<[number, number] | [undefined, undefined]> = data.map(d => d3.extent(d.samples));
    const defYExtents: Array<[number, number]> = yExtents.filter(d => d != [undefined, undefined]) as [number, number][];
    const yMin = Math.min(...(defYExtents.map(d => d[0])));
    const yMax = Math.max(...(defYExtents.map(d => d[1])));
    return [yMin, yMax];
}

export function getXScale(data: Arm[], width: number) {
    return d3
        .scaleBand()
        .domain(data.map((d) => `${d.id}`))
        .range([0, width])
        .padding(0.05);
}
export function getYScale(data: Arm[], height: number) {
    return d3
        .scaleLinear()
        .range([height, 0])
        .domain(getYExtent(data));
}

export function yBinData(data: Arm[], numberOfBins: number, yScale: ScaleLinear<number, number, never>): Map<Arm['id'], Bin<number, number>[]> {
    const bins = d3
        .bin()
        .domain(_ => d3.extent(yScale.domain()))
        .thresholds(yScale.ticks(numberOfBins))
        .value(d => d);

    return new Map(data.map(d => [d.id, bins(d.samples)]));
}


export function violinSvgPath(bins: yBinDataResult, xScale: ScaleBand<string>, yScale: ScaleLinear<number, number, never>): Map<Arm['id'], string> {
    const binsId: Array<Arm['id']> = Array.from(bins.keys());
    const binsArr: Array<Bin<number, number>[]> = Array.from(bins.values());

    const maxBinNum: number = binsArr.reduce(
        (prev: number, cur: Bin<number, number>[]) => Math.max(prev, ...cur.map(b => b.length)), 0
    );

    // Maps a bin's length and id to its (non-translated) x-position in the chart.
    const xNum = d3
            .scaleLinear()
            .domain([-maxBinNum, maxBinNum])
            .range([0, xScale.bandwidth()]);

    // Takes the sequence of rectangular bin shape boundaries to
    // a sequence of areas scaled to proportions and smoothed with a curve
    // type.
    // Note: The translation over the correct tick is taken cared of in the
    // `transform` CSS attribute.
    const rectToViolin = (id: number) => {
        return d3
            .area()
            .x0(d => xNum(-d.length))
            .x1(d => xNum(d.length))
            .y(d => yScale(d.x0))
            .curve(d3.curveCatmullRom);
    }

    return new Map(binsId.map(id => [id, rectToViolin(id)(binsArr[id]) ?? ""]));
}
