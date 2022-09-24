<template>
  <div>
    <div id="viz">
      <svg :width="outerWidth" :height="outerHeight">
        <g :transform="globalTranslationAttr" />
        <g id="xAxisContainer" :transform="xAxisTranslationAttr" />
        <g id="yAxisContainer" />
        <g
          v-for="d in data"
          :key="d.id"
          :transform="itemTranslationAttr.at(d.id)"
        >
          <path
            id="d.id"
            style="stroke: none; fill: #69b3a2"
            :d="violinPath.get(d.id)"
            @click="onSample(d.id)"
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
import { getXScale, getYScale, violinSvgPath } from "./yBins";
import { select } from "d3-selection";
import { axisBottom, axisLeft } from "d3-axis";

const d3 = {
  select,
  axisBottom,
  axisLeft,
};

interface Props {
  outerWidth?: number;
  outerHeight?: number;
  marginTop?: number;
  marginRight?: number;
  marginBottom?: number;
  marginLeft?: number;
  data: Arm[];
}

const props = withDefaults(defineProps<Props>(), {
  outerWidth: 800,
  outerHeight: 480,
  marginTop: 30,
  marginRight: 30,
  marginBottom: 70,
  marginLeft: 60,
});

const width = ref(props.outerWidth - props.marginRight - props.marginLeft);
const height = ref(props.outerHeight - props.marginTop - props.marginBottom);
const numberOfBins = ref(20);
const xAxisContainer = ref(d3.select<SVGGElement, any>("g[id=xAxisContainer]"));
const yAxisContainer = ref(d3.select<SVGGElement, any>("g[id=yAxisContainer]"));

onMounted(() => {
  updateAxis();
});

const xScale = computed(() => {
  return getXScale(props.data, width.value);
});
const yScale = computed(() => {
  return getYScale(props.data, height.value);
});

const globalTranslationAttr = computed(() => {
  return `translate(${props.marginLeft},${props.marginTop})`;
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

function onSample(id: number) {
  props.data[id].sample();
}

function updateAxis() {
  xAxisContainer.value = d3.select<SVGGElement, any>("g[id=xAxisContainer]");
  xAxisContainer.value.selectChildren("g").remove();
  xAxisContainer.value.call(d3.axisBottom(xScale.value));

  yAxisContainer.value = d3.select<SVGGElement, any>("g[id=yAxisContainer]");
  yAxisContainer.value.selectChildren("g").remove();
  yAxisContainer.value.call(d3.axisLeft(yScale.value));
}

const violinPath = computed(() => {
  return violinSvgPath(
    props.data,
    numberOfBins.value,
    xScale.value,
    yScale.value
  );
});
</script>
