<script setup lang="ts">
 import * as d3 from 'd3';
 import { withDefaults, ref, computed, type PropType, watch, onMounted, onBeforeMount, onBeforeUpdate } from "vue";

 export interface SeriesProps {
     name: string;
     values: {action: number | string, step: number, value: number}[];
 }

 export interface Props {
     id: string;
     name: string;
     values: {action: string, step: number, value: number}[];
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
         console.warn("values is empty");
         return;
     }
     const X = d3.scaleLinear()
       .domain([
           d3.min(props.values, ({step}) => step),
           d3.max(props.values, ({step}) => step)
       ])
       .rangeRound(xRange.value);
     const Y = d3.scaleLinear()
                 .domain(
                     d3.extent(props.values, ({value}) => value)
                 ).nice()
                 .range(yRange.value);
     return { X, Y };
 });
 /* */
 function renderAxis() {
     const { X, Y } = scales.value;
     d3.select("g.axes-x").call(d3.axisBottom(X));
     d3.select("g.axes-y").call(d3.axisLeft(Y));
 }
 /* */
 const path = computed(() => {
     const { X, Y } = scales.value;
     return d3.line()
              .x(d => X(d.step))
              .y(d => Y(d.value));
 });
 /* */
 const line = computed(() => {
     return path.value(props.values);
 });
/* */
function onDebug() {
  console.log("values", props.values);
  console.log("path.d", line);
  renderAxis();
}
 </script>

<template>
  <div>
    <div>
        <v-btn @click="onDebug">
            Debug
        </v-btn>
    </div>
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
            <g class="axes-y"></g>
            <path class="chart-line" :d="line" />
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
