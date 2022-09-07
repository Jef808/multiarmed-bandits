<template>
  <div>
    <div>
      <label>
        Selected:
        <select v-model="selected" @change="selectSeries">
          <option v-for="ser in series" :value="ser.name" :key="ser.name">
            {{ ser.name }}
          </option>
        </select>
      </label>
    </div>
    <div>
      <label>
        Metric:
        <select v-model="currentMetric" @change="selectMetric">
          <option
            v-for="metric in metrics"
            :value="metric.name"
            :key="metric.name"
          >
            {{ metric.name }}
          </option>
        </select>
      </label>
    </div>
    <div id="chart-container" class="flex container">
      <JSCharting
        id="chart-metric"
        class="flex chart"
        ref="myChart"
        v-if="dataLoaded"
        :options="chartOptions"
        @rendered="selectSeries"
        style="height: 800px; width: 1000px"
      />
      <div id="chart-actions-container" class="flex container">
        <MABView
          id="chart-actions"
          class="flex chart"
          v-if="modelDataLoaded"
          :actionExpectations="actionExpectations"
          :gaussianNoiseStdDev="samplingNoiseStdDev"
          style="height: 350px"
        ></MABView>
      </div>
    </div>
  </div>
</template>

<script>
import JSCharting, { JSC } from "jscharting-vue";
import axios from "axios";
import MABView from "./MABView.vue";
import { assert, smoothen } from "./../js/utils";

const chartConfig = () => {
  return {
    debug: true,
    type: "line",
    title_label_text: "<b>Multi-armed bandits</b>", // [Best: %maxPointName %max, Worst: %minPointName %min]",
    defaultTooltip_enabled: true,
    yAxis: {
      label: { text: "Metric Value" },
      scale_range: [-0.5, 2.5],
    },
    xAxis: {
      label: { text: "Steps" },
    },
    axisToZoom: "y",
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
    // legend_visible: true,
    legend: {
      title: "UCB by exploration constant",
      shadow: false,
      template: "%name %icon %value",
      header: "<b>File</b>,,<b>Exploration Cst</b>",
      position: "left",
      width: "300px",
      height: "500px",
      boxVisible: true,
      corners: "round",
      radius: 5,
      layout: "vertical",
      // margin_left: 30,
      outline: { color: "#a5c6ee", width: 3 },
      defaultEntry: {
        padding: 4,
        style: {
          // color: "currentColor",
          fontSize: "12pt",
          // fontStyle: "italic",
          fontFamily: "Arial",
          fontWeight: "normal",
        },
      },
    },
    defaultSeries: {
      line: {
        opacity: 0.5,
        color: "lightenMore",
        width: 2,
      },
      events: {
        events_click: function () {
          const series = this;
          me.selected = series.options("name");
          me.selectSeries();
          return false;
        },
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
      icon: {
        visible: true,
        outerShape: "square",
        width: 25,
        fill: "currentColor",
      },
    },
  };
};

export default {
  components: {
    JSCharting,
    MABView,
  },

  async mounted() {
    const me = this;

    for await (const d of me.ucbData()) {
      // console.log("ucbData:", d);
      me.currentData.push(d);

      if (!me.modelDataLoaded) {
        me.extractModelData(d);

        console.log(
          "Extracted model data:",
          me.expectedActions,
          me.samplingNoiseStdDev
        );

        me.modelDataLoaded = true;
      }

      const rewards = me.makeFullSeries(d, "rewards");
      const smoothRewards = me.makeSmoothSeries(rewards);
      me.series.push(smoothRewards);
      // console.log("Rewards series:", rewards);

      // console.log("Smoothened rewards:", smoothRewards);

      const losses = me.makeFullSeries(d, "losses");
      const smoothLosses = me.makeSmoothSeries(losses);
      me.seriesLosses.push(smoothLosses);

      // console.log("Selected:", me.selected);
      // console.log("me.series:", me.series);
      // console.log("Now have", me.series.length, " series loaded");

      me.chartOptions = JSC.merge(me.chartOptions, {
        defaultPoint: {
          events: {
            mouseOver: function () {
              let point = this;
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
          name: "Name",
          points: [],
          legendEntry: { visible: false },
        },
      ],
      seriesLosses: [
        {
          name: "Name",
          points: [],
          legendEntry: { visible: false },
        },
      ],
      metrics: [{ name: "rewards" }, { name: "losses" }],
      currentData: [],
      dataLoaded: false,
      modelDataLoaded: false,
      smoothRatio: 0.1,
      chartOptions: chartConfig(),
      selected: "Name",
      currentMetric: "rewards",
      actionExpectations: [],
      samplingNoiseStdDev: 1,
    };
  },
  methods: {
    toggleMetric: () => {
      this.currentMetric =
        this.currentMetric == "rewards" ? "losses" : "rewards";
      this.selectMetric();
    },
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
    selectMetric: function (argChart) {
      const me = this;
      const chart = this.chartInstance() || argChart;
      chart?.options({
        series: me.currentMetric == "rewards" ? me.series : me.seriesLosses,
      }); //
      //       chart?.series().each(function (s) {
      //         const visible = me.currentMetric === s.attributes.metric;
      //         const selected = me.selected === s.name;
      //         s.options({
      //           mute: !visible,
      //           selected: selected && !visible ? false : selected,
      //         });
      //       });
    },
    chartInstance: function () {
      return this.$refs.myChart.instance;
    },
    makeFullSeries: function (jsonData, metricName) {
      const exp_cst = `${jsonData.exploration_constant.toFixed(2)}`;
      return {
        name: `ucb_${metricName}_${exp_cst}`,
        attributes: {
          exploration: exp_cst,
          metric: metricName,
        },
        legendEntry: {
          value: "%exploration",
          states: {},
        },
        label: { text: "%metric -- %exploration" },
        points: jsonData[metricName].map((val, ind) => {
          return { x: ind, y: val };
        }),
      };
    },
    makeSmoothSeries: function (series) {
      return {
        ...series,
        points: smoothen(series.points, this.smoothRatio),
      };
    },
    extractModelData: function (jsonData) {
      const me = this;

      me.actionExpectations = jsonData.action_qvalues;

      //TODO: Add the sampling noise data in the json logs.
      me.samplingNoiseStdDev = 1;
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
            return { ...d, name: `${ucbFile}` };
          });
        // .then((d) => {
        //   Object.defineProperty(d, "name", {
        //     value: `${ucbFile}`,
        //     writable: false,
        //   });
        //   return d;
        // });
        yield fileData;
      }
    },
  },
};
</script>

<style>
.flex {
  display: flex;
}
chart-container {
  flex-direction: row;
  justify-content: space-evenly;
}
chart-actions-container {
  flex-direction: column;
  justify-content: space-evenly;
}
chart-metric {
  flex-basis: max-content;
}
chart-actions {
  flex-basis: min-content;
}
</style>
