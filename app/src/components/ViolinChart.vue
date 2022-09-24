<template>
  <div>
    <div id="viz">
      <svg :width="outerWidth" :height="outerHeight">
        <g :transform="globalTranslationAttr" />
        <g id="xAxis" :transform="xAxisTranslationAttr" />
        <g id="yAxis" />
        <g
          v-for="d in data"
          :key="d.id"
          :transform="itemTranslationAttr.at(d.id)"
        >
          <path
            :id="d.id"
            style="stroke: none; fill: #69b3a2"
            :d="violinPath.get(d.id)"
          />
        </g>
      </svg>
    </div>
    <div>
      <p>
        <label># Bins: </label>
        <input
          id="nBin"
          v-model="numberOfBins"
          min="1"
          max="100"
          step="5"
          type="number"
        />
      </p>
    </div>
  </div>
</template>
<script setup lang="ts">
import { defineProps, ref, onMounted, computed } from "vue";
import type { Arm } from "./arm";
import { getXScale, getYScale, yBinData, violinSvgPath } from "./yBins";

import { select, selectAll } from "d3-selection";
import { scaleBand, scaleLinear } from "d3-scale";
import { axisBottom, axisLeft } from "d3-axis";
import { extent, bin } from "d3-array";
import { area, curveCatmullRom } from "d3-shape";

const d3 = {
  select,
  selectAll,
  scaleBand,
  scaleLinear,
  axisBottom,
  axisLeft,
  extent,
  bin,
};

export interface Margin {
  top: number;
  right: number;
  bottom: number;
  left: number;
}

export interface Props {
  margin?: Margin;
  outerWidth?: number;
  outerHeight?: number;
  data: Arm[];
}

const props = withDefaults(defineProps<Props>(), {
  margin: {
    top: 10,
    right: 30,
    bottom: 30,
    left: 70,
  },
  outerWidth: 800,
  outerHeight: 480,
});

const width = ref(props.outerWidth - props.margin.right - props.margin.left);
const height = ref(props.outerHeight - props.margin.top - props.margin.bottom);
const numberOfBins = ref(20);

onMounted(() => {
  d3.select("#xAxis").call(d3.axisBottom(xScale.value));
  d3.select("#yAxis").call(d3.axisLeft(yScale.value));
});

const xScale = computed(() => {
  return getXScale(props.data, width.value);
});
const yScale = computed(() => {
  return getYScale(props.data, height.value);
});

const globalTranslationAttr = computed(() => {
  return `translate(${props.margin.left},${props.margin.top})`;
});

const xAxisTranslationAttr = computed(() => {
  return `translate(0,${height.value})`;
});

const itemTranslationAttr = computed(() => {
  return props.data.map((d) => {
    const ids = `${d.id}`;
    return `translate(${xScale.value(ids)},0)`;
  });
});

const bins = computed(() => {
  return yBinData(props.data, numberOfBins.value, yScale.value);
});

const violinPath = computed(() => {
  return violinSvgPath(bins.value, xScale.value, yScale.value);
});
</script>
