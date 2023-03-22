<script setup lang="ts">
import { ref, onMounted, computed } from "vue";
import type { Arm } from "../scripts/arm";
import {
  getXScale,
  getYScale,
  recentStepsColorMap,
  violinSvgPath,
} from "../scripts/yBins";
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
const emit = defineEmits<{
  (e: "sample", id: number): void;
}>();
const width = ref(props.outerWidth - props.marginRight - props.marginLeft);
const height = ref(props.outerHeight - props.marginTop - props.marginBottom);
const numberOfBins = ref(20);
const recentRatio = ref(0.25);
const repeatCount = ref(1);
const xAxisContainer = ref(d3.select<SVGGElement, any>("g[id=xAxisContainer]"));
const yAxisContainer = ref(d3.select<SVGGElement, any>("g[id=yAxisContainer]"));
onMounted(() => {
  updateAxis();
});
function chartClick(id: number) {
  for (let i = 0; i < repeatCount.value; i++) {
    console.log("emitting event 'sample' with 'id' = ", id);
    emit("sample", id);
  }
}
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
function updateAxis() {
  xAxisContainer.value = d3.select<SVGGElement, any>("g[id=xAxisContainer]");
  xAxisContainer.value.selectChildren("g").remove();
  xAxisContainer.value.call(d3.axisBottom(xScale.value));
  yAxisContainer.value = d3.select<SVGGElement, any>("g[id=yAxisContainer]");
  yAxisContainer.value.selectChildren("g").remove();
  yAxisContainer.value.call(d3.axisLeft(yScale.value));
}
const colorMap = computed(() => {
  return recentStepsColorMap(props.data, recentRatio.value);
});
const violinPath = computed(() => {
  return violinSvgPath(
    props.data,
    numberOfBins.value,
    xScale.value,
    yScale.value
  );
});
</script>

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
          @click="chartClick(d.id)"
        >
          <path
            id="d.id"
            strokestyle="none"
            :fill="colorMap.get(d.id)"
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
        <label>Recent Ratio: </label>
        <input
          id="recentRatio"
          v-model="recentRatio"
          min="0.1"
          max="0.9"
          step="0.1"
          type="number"
        />
      </p>
      <p>
        <label>Repeat: </label>
        <input id="input-repeat" v-model="repeatCount" type="number" />
      </p>
    </div>
  </div>
</template>
