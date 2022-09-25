import { scaleLinear, scaleBand, scalePow, scaleDiverging, scaleDivergingPow, scaleSequential, scaleSequentialPow } from "d3-scale";
import { bin, extent, rollup } from "d3-array";
import { area, curveCatmullRom } from "d3-shape";
import { interpolateRdBu, interpolateBrBG } from "d3-scale-chromatic"
import type { Arm, Sample } from "./arm";
import type { Bin, ScaleBand, ScaleLinear, ScalePower } from "d3";

const d3 = {
    scaleLinear,
    scaleBand,
    scalePow,
    scaleDiverging,
    scaleDivergingPow,
    scaleSequential,
    scaleSequentialPow,
    bin,
    rollup,
    extent,
    area,
    curveCatmullRom,
    interpolateRdBu,
    interpolateBrBG
};

function getYExtent(data: Arm[]) {
    const yExtents: Array<[number, number] | [undefined, undefined]> = data.map(
        (d) => d3.extent(d.samples.map((s) => s.value))
    );
    const defYExtents: Array<[number, number]> = yExtents
        .filter(d => d[0] != undefined) as [number, number][];
    const yMin = Math.min(...defYExtents.map((d) => d[0]));
    const yMax = Math.max(...defYExtents.map((d) => d[1]));
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
    return d3.scaleLinear().range([height, 0]).domain(getYExtent(data));
}



function getBinBuilder(numberOfBins: number, yScale: ScaleLinear<number, number, never>) {
    return d3
        .bin<Sample, number>()
        .domain(() => d3.extent(yScale.domain()))
        .thresholds(yScale.ticks(numberOfBins))
        .value(d => d.value);
}

function yBinData(
    data: Arm[],
    numberOfBins: number,
    yScale: ScaleLinear<number, number, never>
): Map<Arm["id"], Bin<Sample, number>[]> {
    const bins = getBinBuilder(numberOfBins, yScale);

    const valueBins = new Map(data.map((d) => [d.id, bins(d.samples)]));

    return valueBins;
}

export function recentStepsColorMap(data: Arm[], recentRatio: number = 0.25) {
    const totalNumberOfSteps = Math.max(...data.map(d => Math.max(...d.samples.map(s => s.step))));
    const numberRecentSteps = recentRatio * totalNumberOfSteps;
    const uniformMeanExp = numberRecentSteps / data.length;
    const differenceUpperBound = numberRecentSteps * (1 - 1 / data.length)

    const scaleD = d3.scaleDiverging(t => d3.interpolateBrBG(t)).domain([-uniformMeanExp, 0, differenceUpperBound]);
    const scaleDB = d3.scaleDiverging(t => d3.interpolateRdBu(1 - t)).domain([-uniformMeanExp, 0, differenceUpperBound]);

    return new Map(data.map(d => {
        // Collect recent samples
        return d.samples.filter(s => s.step >= totalNumberOfSteps - numberRecentSteps);
    }).map((s, id) => {
        // Look at difference between relative frequency of recent samples and value of
        // uniform random sampling expectation, scaled so that zero difference corresponds
        // to the neutral color.
        return [id, scaleDB(s.length - uniformMeanExp)];
    }));
};

export function violinSvgPath(
    data: Arm[],
    numberOfBins: number,
    xScale: ScaleBand<string>,
    yScale: ScaleLinear<number, number, never>
): Map<Arm["id"], string> {
    const bins = yBinData(data, numberOfBins, yScale);

    const binsId: Array<Arm["id"]> = Array.from(bins.keys());
    const binsArr: Array<Bin<Sample, number>[]> = Array.from(bins.values());

    const maxBinNum: number = binsArr.reduce(
        (prev: number, cur: Bin<Sample, number>[]) =>
            Math.max(prev, ...cur.map((b) => b.length)), 0);

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
    const rectToViolin = d3
        .area<Bin<Sample, number>>()
        .x0((d) => xNum(-d.length))
        .x1((d) => xNum(d.length))
        .y((d) => yScale(d.x0 as number))
        .curve(d3.curveCatmullRom);

    return new Map(binsId.map((id) => [id, rectToViolin(binsArr[id]) ?? ""]));
}
