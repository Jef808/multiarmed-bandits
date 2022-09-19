<script setup lang="ts">
import { watchEffect, onMounted } from "vue";
import * as d3 from "d3";
import { withGaussianNoise } from "./../scripts/utils";

interface Arm {
  id: number;
  value: number;
  visits: number;
  total: number;
  sample: () => number;
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
type ArmId = Arm["id"];
interface Sample {
  id: ArmId;
  reward: number;
}

const { arms } = defineProps<Props>();

let maxValue: number = 0;

const margin: Margin = $ref({ top: 30, right: 30, bottom: 70, left: 60 });
const width: number = $ref(680 - margin.left - margin.right);
const height: number = $ref(400 - margin.top - margin.bottom);

let samples: Array<Sample> = $ref([]);
let totalVisits: number = $ref(0);
let totalRewards: number = $ref(0);
let regret: number = $ref(0);
const onSample: (a: Arm) => void = function (a) {
  const reward = a.sample();
  samples.push({ id: a.id, reward: reward });
  totalVisits += 1;
  totalRewards += reward;
  regret += maxValue - reward;
};
const format: (s: Sample) => string = (s) =>
  `id: ${s.id}, reward: ${s.reward.toFixed(2)}`;

onMounted(() => {
  maxValue = Math.max(...arms.map((a) => a.value));

  const svg = d3
    .select("#d3-chart")
    .append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
    .append("g")
    .attr("transform", `translate(${margin.left},${margin.top})`);

  const x = d3.scaleBand().range([0, width]).domain([0, d.length]).padding(0.2);
  svg
    .append("g")
    .attr("transform", `translate(0,${height})`)
    .call(d3.axisBottom(x))
    .selectAll("text");

  const y = d3.scaleLinear().domain([-3, 3]).range([height, 0]);

  svg.append("g").call(d3.axisLeft(y));

  svg
    .selectAll("mybar")
    .data(arms)
    .enter()
    .append("rect")
    .attr("x", (d) => x(d.id))
    .attr("y", (d) => y(0))
    .attr("width", x.bandwidth())
    .attr("height", (d) => height - y(0))
    .attr("fill", "#69b3a2")
    .attr("visits", (d) => d.visits)
    .on("click", (e, d) => onSample(d));

  svg
    .selectAll("rect")
    .transition()
    .duration(600)
    .call((d) => {
      if (!!d.value) {
        this.attr("y", (d) => y(d.value));
        this.attr("height", (d) => height - y(d.value));
      }
      return this;
    })
    .delay((d, i) => i * 100);

  const circles = d3
    .select("#circles")
    .append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", 1.5 * x.bandwidth())
    .attr("transform", `translate(${margin.left + 0.5 * x.bandwidth()},0.0)`);

  circles
    .selectAll("mycircle")
    .data(arms)
    .enter()
    .append("circle")
    .attr("cx", (d, i) => x(i))
    .attr("cy", 0.4 * x.bandwidth())
    .attr("r", 0.4 * x.bandwidth())
    .attr("stroke", "black")
    .attr("fill", "#69a3b2");
});
</script>

<template>
  <div id="d3-chart"></div>
  <div id="circles"></div>
  <div id="stats">
    <br />Number of samples: {{ totalVisits }} <br />Total rewards:
    {{ totalRewards.toFixed(2) }} <br />Average Rewards:
    {{ (totalRewards / totalVisits).toFixed(2) }} <br />Average Regret:
    {{ (regret / totalVisits).toFixed(2) }}
  </div>
</template>
