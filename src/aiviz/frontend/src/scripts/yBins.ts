import { scaleLinear, scaleBand, scalePow, scaleDiverging, scaleDivergingPow, scaleSequential, scaleSequentialPow } from "d3-scale";
import { bin, extent, rollup } from "d3-array";
import { area, curveCatmullRom } from "d3-shape";
import { interpolateRdBu, interpolateBrBG } from "d3-scale-chromatic"
import { type Arm, type Sample } from "./arm";
import type { Bin, ScaleBand, ScaleLinear } from "d3";

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

type ArmIdType = number | Date;
type NumberBounds = [number, number];
type SampleBin = Bin<Sample, number>;
type CMValue = string;
type SVGPathType = string;

function getYExtent(data: Arm[]): NumberBounds {
    return data.map((arm) => d3.extent(arm.samples().map((s: Sample) => s. value)) as NumberBounds)
               .reduce((accMinMax, currentMinMax) => {
                   let [[dMin, dMax], [m, M]] = [accMinMax, currentMinMax];
                   return [Math.min(dMin, m), Math.max(dMax, M)];
               },
               [0, 0]);
}

export function getXScale(data: Arm[], width: number) {
    return d3.scaleBand()
             .domain(data.map((d) => `${d.id}`))
             .range([0, width])
             .padding(0.05);
}

export function getYScale(data: Arm[], height: number) {
    return d3.scaleLinear().range([height, 0]).domain(getYExtent(data));
}

function binBuilder(numberOfBins: number, yScale: ScaleLinear<number, number, never>) {
    return d3.bin<Sample, number>()
             .domain(d3.extent(yScale.domain()) as [number, number])
             .thresholds(yScale.ticks(numberOfBins))
             .value(d => d.value);
}

function yBinData(
    data: Arm[], numberOfBins: number, yScale: ScaleLinear<number, number, never>) {
    const bins = binBuilder(numberOfBins, yScale);
    return new Map<ArmIdType, SampleBin[]>(data.map(a => ([a.id(), bins(a.samples())])));
}

export function recentStepsColorMap(data: Arm[], recentRatio: number = 0.25) {
    const totalSamples = data.reduce((total, arm) => total + arm.samples().length, 0);
    const recentSamples = recentRatio * totalSamples;

    const uniformMeanExp = recentSamples / data.length;
    const differenceUpperBound = recentSamples * (1 - 1 / data.length)

    // const scaleD = d3.scaleDiverging(t => d3.interpolateBrBG(t)).domain([-uniformMeanExp, 0, differenceUpperBound]);
    const scaleDiv = d3.scaleDiverging(t => d3.interpolateRdBu(1 - t))
                      .domain([-uniformMeanExp, 0, differenceUpperBound]);

    const entries: [ArmIdType, CMValue][] = data.map(arm => {
        return [
            arm.id(),
            scaleDiv(arm.samples().filter(s => s.id >= totalSamples - recentSamples).length - uniformMeanExp)
        ];
    });

    return new Map<ArmIdType, CMValue>(entries);
};

export function violinSvgPath(data: Arm[], numberOfBins: number, xScale: ScaleBand<string>,
    yScale: ScaleLinear<number, number, never>) {
    const bins = yBinData(data, numberOfBins, yScale);

    const binValues: Array<SampleBin[]> = [...bins.values()];

    const maxBinSize: number = Math.max(...binValues.map((bv => bv.length)));

    // Maps a bin's length and id to its (non-translated) x-position in the chart.
    const xNum = d3.scaleLinear()
                   .domain([-maxBinSize, maxBinSize])
                   .range([0, xScale.bandwidth()]);

    // Takes the sequence of rectangular bin shape boundaries to
    // a sequence of areas scaled to proportions and smoothed with a curve
    // type.
    // Note: The translation over the correct tick is taken cared of in the
    // `transform` CSS attribute.
    const rectToViolin = d3.area<SampleBin>()
                           .x0((d) => xNum(-d.length))
                           .x1((d) => xNum(d.length))
                           .y((d) => yScale(d.x0 as number))
                           .curve(d3.curveCatmullRom);

    const binIds: Array<ArmIdType> = [...bins.keys()];

    return new Map<ArmIdType, SVGPathType>(binIds.map((armId, idx) => [armId, rectToViolin(binValues[idx]) ?? ""]));
}
