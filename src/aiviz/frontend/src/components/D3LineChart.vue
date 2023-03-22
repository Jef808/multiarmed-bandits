<script setup lang="ts">
 import * as d3 from 'd3';
 import { computed, onMounted, onBeforeUpdate } from "vue";

 export interface SeriesProps {
     name: string;
     values: {action: number | string, step: number, value: number}[];
 }

 export interface Props {
     id: string;
     name: string;
     values: {x: number, y: number}[];
     width: number;
     height: number;
     xPadding: number;
     yPadding: number;
 }

 const props = defineProps<Props>();

 onMounted(() => {
     renderAxis();
 })

 onBeforeUpdate(() => {
     renderAxis();
 });

 const viewBox = computed(() => {
     return `0 0 ${props.width} ${props.height}`;
 });

 const chartTransformAttr = computed(() => {
     return `translate(${props.xPadding},${props.yPadding})`;
 });

 const xAxisTransformAttr = computed(() => {
     return `translate(0,${props.height-2*props.yPadding})`;
 });

 const xRange = computed(() => {
     return [0, props.width - 2 * props.xPadding];
});

 const yRange = computed(() => {
     return [props.height - 2 * props.yPadding, 0];
});

 const scales = computed(() => {
     if (props.values.length === 0) {
         throw "values is empty";
     }
     const X = d3.scaleLinear()
                 .domain([
                     d3.min(props.values.map(({x}) => x)) as number,
                     d3.max(props.values.map(({x}) => x)) as number
                 ]).rangeRound(xRange.value);
     const Y = d3.scaleLinear()
                 .domain(
                     d3.extent(props.values.map(({y}) => y)) as [number, number]
                 ).nice()
                 .range(yRange.value);
     return { X, Y };
 });

function renderAxis() {
     const { X, Y } = scales.value;
     d3.select<SVGGElement, number>("g.axes-x").call(d3.axisBottom(X));
     d3.select<SVGGElement, number>("g.axes-y").call(d3.axisLeft(Y));
 }

 const path = computed(() => {
     const { X, Y } = scales.value;
     return d3.line()
              .x(d => X(d[0]))
              .y(d => Y(d[1]));
 });

 const line = computed(() => {
     if (line !== null) {
         return path.value(props.values.map(({x, y}) => ([x, y])));
     }
 });

 function onDebug() {
     console.log("values", props.values);
     console.log("path.d", line);
     renderAxis();
 }
</script>

<template>
  <div>
    <svg
        class="chart"
        :width="width"
        :height="height"
        :viewBox="viewBox"
    >
        <g :transform="chartTransformAttr" class="chart">
            <g class="axes-x"
             :transform="xAxisTransformAttr"
            ></g>
            <g class="axes-y"
            >
            </g>
            <path class="chart-line"
                  :d="line" />
        </g>
    </svg>
  </div>
</template>

<style>
 .chart {
     margin: 25px;
 }
 .chart-line {
     fill: none;
     stroke: #76BF8A;
     stroke-width: 3px;
 }
</style>
