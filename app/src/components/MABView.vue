<template>
  <div>
    <JSCharting ref="myChart" v-if="dataLoaded" :options="chartOptions" />
  </div>
</template>

<script>
import JSCharting, { JSC } from "jscharting-vue";
// import { assert } from "./../js/utils";

const chartConfig = () => {
  return {
    debug: true,
    type: "marker",
    xAxis: {
      label_text: "Actions",
    },
    yAxis: {
      label_text: "Expected Value",
      defaultTick_label_text: "%value",
    },
    defaultPoint: {
      tooltip: "<b>Action %name</b> EV: %yValue<br/>Std Dev: <b>%yOffset%</b>",
      subvalue_line_width: 2,
    },
    legend_visible: false,
    title: {
      label_text: "<b>Actions expected values</b>",
    },
  };
};

const makeSeries = async (actions, noiseStdDev) => {
  return {
    points: await actions.map((a, i) => {
      return {
        name: `${i}`,
        x: i,
        y: a,
        subvalue: {
          offset: noiseStdDev,
          line: { width: 1 },
        },
      };
    }),
  };
};

export default {
  components: { JSCharting },

  props: {
    actionExpectations: { type: Array, required: true },
    gaussianNoiseStdDev: { default: 1 },
  },

  data() {
    return {
      series: [],
      dataLoaded: false,
      chartOptions: chartConfig(),
    };
  },

  async mounted() {
    const me = this;

    console.log(
      "Mounted.\nactionExpectations:",
      me.actionExpectations,
      "gaussianNoiseStdDev:",
      me.gaussianNoiseStdDev
    );

    const series = await makeSeries(
      me.actionExpectations,
      me.gaussianNoiseStdDev
    );
    console.log("Series:", series);

    me.series.push(series);

    me.chartOptions = JSC.merge(me.chartOptions, {
      series: me.series,
    });

    me.dataLoaded = true;
  },
};
</script>
