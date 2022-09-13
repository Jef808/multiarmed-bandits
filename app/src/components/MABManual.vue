<template>
  <div>
    <JSCarting
      id="chart-manual"
      class="flex chart"
      ref="chartManual"
      v-if="dataLoaded"
      :options="chartOptions"
    ></JSCarting>
  </div>
</template>

<script>
import JSCharting, { JSC } from "jscharting-vue";
import { assert, sampleGaussianNoise } from "./../js/utils";

const chartConfig = () => {
  return {
    debug: true,
    type: "vertical column",
    title_position: "full",
    defaultAnnotation: {
      label_style: {
        fontWeight: "bold",
        fontSize: 15,
      },
    },
    // ptitle_label_text: , // [Best: %maxPointName %max, Worst: %minPointName %min]",
    // legend: {
    //   template: "%icon %value",
    //   header: ",Expl Cst",
    //   margin: 0,
    //   position: "right",
    //   title: {
    //     label_text: "UCB RUNS",
    //   },
    //   shadow: true,
    //   width: "100px",
    //   height: "500px",
    //   boxVisible: true,
    //   corners: "round",
    //   radius: 2,
    //   layout: "vertical",
    //   outline: { color: "#a5c6ee", width: 3 },
    //   defaultEntry: {
    //     padding: 4,
    //     style: {
    //       fontSize: "12pt",
    //       fontFamily: "Arial",
    //       fontWeight: "normal",
    //     },
    //     visible: true,
    //     attributes: {
    //       value: "%exploration",
    //       name: "%metric",
    //     },
    //   },
    // },
    xAxis: {
      label: {
        text: "Actions",
        position: "right",
      },
    },
    yAxis: {
      label_text: "Average Reward",
      scale: {
        range: [-1, 2.5],
      },
    },
    axisToZoom: "y",
    defaultSeries: {
      type: "column",
      line: {
        opacity: 0.5,
        color: "lightenMore",
        width: 2,
      },
      states: {
        select: {
          // Selected line is solid, and thicker.
          line: {
            opacity: 1,
            color: "currentColor",
            width: 3,
          },
        },
        // Series hover and mute states are not necessary
        hover: { enabled: false },
        mute: { enabled: false },
      },
      // events: {
      //   events_click: () => {
      //     const series = this;
      //     me.selected = series.options("name");
      //     me.selectSeries();
      //     return false;
      //   },
      // },
    },
  };
};

export default {
  components: {
    JSCharting,
  },
  props: {
    numberOfArms: { default: 10, type: Number },
    valueMean: { default: 0.0, type: Number },
    valueStdDev: { default: 1.0, type: Number },
  },
  data() {
    return {
      series: {
        expectedValues: {
          name: "expectedValues",
          id: "expectedValues",
          points: [],
        },
        results: {
          name: "results",
          id: "results",
          points: [],
        },
      },
      chartOptions: chartConfig(),
      dataLoaded: false,
      expectedRewards: [],
      sampleNoiseMean: 0.0,
      sampleNoiseStdDev: 1,
      rewards: [],
      visits: [],
    };
  },
  mounted() {
    const me = this;

    me.series.expectedValues.points = Array.from(
      { length: me.numberOfArms },
      (i) => {
        const expectedValue = sampleGaussianNoise(
          me.sampleNoiseMean,
          me.sampleNoiseStdDev
        );
        return {
          x: i + 1,
          y: expectedValue,
          name: `${i + 1}`,
          id: `${i + 1}`,
        };
      }
    );

    me.chartOptions = JSC.merge(me.chartOptions, {
      series: me.series.expectedValues,
    });

    me.series.results = {
      points: Array.from({ length: me.numberOfArms }, (i) => {
        return {
          x: i + 1,
          y: 0,
          regret: 0,
          visits: 0,
          name: `${i + 1}`,
          id: `${i + 1}`,
        };
      }),
    };
    me.dataLoaded = true;

    console.log("After mounted():", me.series.expectedValues);
  },
  methods: {
    makeSeries(expectedValues) {
      const me = this;
      return {
        ...expectedValues,
        xAxis: {
          scale: { range: [0, me.numberOfArms] },
          label_text: "Arms",
        },
        yAxis: {
          label_text: "Expected Values",
        },
        defaultPoints: {
          legendEntry: {
            name: `${x}`,
          },
        },
      };
    },
    sample(armNumber) {
      assert(
        armNumber >= 1 && armNumber <= this.numberOfArms,
        "sample argument out of bounds."
      );
      const noise = sampleGaussianNoise(0.0, 1.0);
      return this.series.expectedValues.points[actionNumber] + noise;
    },
  },
};
</script>
