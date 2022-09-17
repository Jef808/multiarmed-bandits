<script setup lang="ts">
import { watchEffect, onMounted } from "vue";
import * as d3 from "d3";

interface Arm {
  id: number;
  value: number;
  visits: number;
}
interface Props {
  arms: Array<Arm>;
}
interface Margin {
  top: number;
  right: number;
  bottom: number;
  left: number;
}

const { arms } = defineProps<Props>();

const margin = $ref({ top: 30, right: 30, bottom: 70, left: 60 }),
  width: number = $ref(460 - margin.left - margin.right),
  height: number = $ref(400 - margin.top - margin.bottom);

onMounted(() => {
  const svg = d3
    .select("#d3-chart")
    .append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
    .append("g")
    .attr("transform", `translate(${margin.left},${margin.top})`);

  const x = d3
    .scaleBand()
    .range([0, width])
    .domain(arms.map((d) => d.id))
    .padding(0.2);

  svg
    .append("g")
    .attr("transform", `translate(0,${height})`)
    .call(d3.axisBottom(x))
    .selectAll("text")
    .style("text-anchor", "end");

  const y = d3.scaleLinear().domain([-2, 2]).range([height, 0]);

  svg.append("g").call(d3.axisLeft(y));

  svg
    .selectAll("mybar")
    .data(arms)
    .enter()
    .append("rect")
    .attr("x", (d) => x(d.id))
    .attr("y", (d) => y(d.value))
    .attr("width", x.bandwidth())
    .attr("height", (d) => height - y(d.value))
    .attr("fill", "#69b3a2");
});
</script>

<template>
  <div id="d3-chart"></div>
</template>
