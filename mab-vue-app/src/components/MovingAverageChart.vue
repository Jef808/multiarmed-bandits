<script setup lang="ts">
import * as d3 from "d3";
import { onMounted } from "vue";
import * as util from "../scripts/d3-utils";

interface Props {
  data?: Array<util.Point>;
  binSize?: number;
  binRatio?: number;
  binNumber?: number;
  width?: number;
}

const {
  data = [],
  binSize,
  binRatio,
  binNumber,
  width = 600,
} = defineProps<Props>();

onMounted(() => {
  const bin = {
    params: {
      size: binSize,
      ratio: binRatio,
      number: binNumber,
    },
  };

  const bins = util.getBins(
    bin.params,
    data.map((d) => d.x),
    data.map((d) => d.y),
    width
  );

  const avgValues = util.myMovingAverage(
    bins,
    data.map((p) => p.y)
  );

  //const histogram = d3.histogram()(bins);

  //d3.select("#d3-histogram").append("svg").append(histogram);
});
</script>

<template>
  <div id="d3-histogram"></div>
</template>
