<template>
  <div>
    <p>Two way selection binding. Use dropdown or hover a series line.</p>
    <label>
      Selected:
      <select v-model="selected" @change="selectSeries">
        <option v-for="ser in series" :value="ser.name" :key="ser.name">
          {{ ser.name }}
        </option>
      </select>
    </label>
    <JSCharting
      ref="myChart"
      v-if="dataLoaded"
      :options="chartOptions"
      @rendered="selectSeries"
      style="height: 640px"
    />
  </div>
</template>

<script>
import JSCharting, { JSC } from "jscharting-vue";
import axios from "axios";
import { assert, smoothen } from "./../js/utils";

const chartConfig = () => {
  return {
    debug: true,
    type: "spline",
    title_label_text: "My Chart",
    defaultTooltip_enabled: false,
    yAxis: {
      // defaultTick_label_text: "Rewards",
      scale_range: [-1, 3],
    },
    // xAxis: {
    //   scale_type: "time",
    //   formatString: "yyyy",
    // },
    defaultPoint: {
      marker: {
        visible: false,
        size: 12,
      },
      states: {
        // Point hover and mute states are not necessary
        hover: { enabled: false },
        mute: { enabled: false },
      },
      focusGlow_width: 0,
    },
    legend_visible: true,
    defaultSeries: {
      // Default line is translucent and desaturated
      line: {
        opacity: 0.2,
        color: "desaturate",
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
      line_width: 1,
    },
  };
};

export default {
  components: { JSCharting },

  async mounted() {
    const me = this;

    for await (const d of me.ucbData()) {
      console.log("ucbData:", d);

      const rewards = me.makeFullSeries(d, "rewards");
      console.log("Rewards series:", rewards);

      const smoothRewards = me.makeSmoothSeries(rewards);
      console.log("Smoothened rewards:", smoothRewards);

      me.series.push(smoothRewards);

      console.log("Selected:", me.selected);
      console.log("me.series:", me.series);
      console.log("Now have", me.series.length, " series loaded");

      me.chartOptions = JSC.merge(me.chartOptions, {
        defaultPoint: {
          events: {
            mouseOver: function () {
              var point = this;
              me.selected = point.series.options("name");
              me.selectSeries();
              return false;
            },
          },
        },
        series: me.series,
      });

      me.dataLoaded = true;
    }
  },
  data() {
    return {
      series: [
        {
          name: "",
          points: [],
        },
      ],
      seriesData: [],
      dataLoaded: false,
      smoothRatio: 0.1,
      chartOptions: chartConfig(),
      selected: "",
    };
  },
  methods: {
    selectionChanged: () => {
      selectSeries();
    },
    selectSeries: function (argChart) {
      const me = this;
      const chart = me.chartInstance() || argChart;
      chart?.series().each(function (s) {
        s.options({ selected: me.selected === s.name });
      });
    },
    chartInstance: function () {
      return this.$refs.myChart.instance;
    },
    makeFullSeries: function (jsonData, name) {
      return {
        name: jsonData.name,
        points: jsonData[name].map((val, ind) => {
          return { x: ind, y: val };
        }),
      };
    },
    makeSmoothSeries: function (series) {
      return {
        name: series.name,
        points: smoothen(series.points, this.smoothRatio),
      };
    },
    ucbData: async function* () {
      let ucbFiles = await axios
        .get("./data/")
        .then((res) =>
          res.data.filter((d) => d.startsWith("ucb") && d.endsWith(".json"))
        );
      for await (const ucbFile of ucbFiles) {
        let fileData = await axios
          .get(`./data/${ucbFile}`)
          .then((res) => res.data)
          .then((d) => {
            Object.defineProperty(d, "name", {
              value: `${ucbFile}`,
              writable: false,
            });
            return d;
          });
        yield fileData;
      }
    },
  },
};
</script>

<style>
JSCharting {
  height: "600px";
  width: "480px";
}
</style>
