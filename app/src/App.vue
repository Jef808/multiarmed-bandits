<template>
  <div class="message-string">
    <p v-if="isLoading">Loading...</p>
    <p v-if="failedFetch">Failed to fetch data!</p>
    <p v-else></p>
  </div>
  <div id="app">
    <div class="input-header">
      <label for="input-dir">Directory: </label>
      <input type="text" id="input-dir" placeholder="data/" v-model.lazy.trim="dataSourcePath" />
    </div>
    <div class="input-header">
      <label for="input-file">File Prefix: </label>
      <input type="text" id="input-file" placeholder="ucb_" v-model.lazy.trim="dataSourceFile" />
      <button @click="loadData" :disabled="isLoading">Load data</button>
    </div>
    <hr />
    <div id="div-selectors-container">
      <div class="div-selector" id="div-env-select">
    <label for="env-select">Environment: </label>
    <option-selector :id="env-select" :options="envsList" @change="onEnvChange"></option-selector>
    </div>
      <div class="div-selector" id="div-agent-select">
    <label for="agent-select">Agent: </label>
    <option-selector :id="agent-select" :options="agentsList" @change="onAgentChange"></option-selector>
    </div>
    </div>
    <hr />
    <div>
      <file-list id="file-list" :dir="dataDir"></file-list>
    </div>
    <div class="main-tile">
      <div class="view-body">
        <button @click="onToggleRewardsLosses">{{ toggleRewardsLossesText }}</button>
        <vue-apex-chart
          width="800"
          type="line"
          :height="chartOptions.chart.height"
          :options="chartOptions"
          :series="currentSeries"
        ></vue-apex-chart>
      </div>
    </div>
  </div>
</template>

<script>
import axios from "axios";
import VueApexChart from "vue3-apexcharts";
import FileList from "./components/FileList.vue"
import OptionSelector from "./components/OptionSelector.vue"
import environments from "./js/environments"
import agents from "./js/agents"


export default {
    name: "App",
    components: {
        FileList,
        VueApexChart,
        OptionSelector
    },
    data() {
        return {
            dataDir: "data/",
            envsList: [],
            agentsList: [],
            selectedEnv: {},
            selectedAgent: {},
            currentData: {},
            failedFetch: false,
            isLoading: false,
            isInitialized: false,
            flagRewardsLosses: false,
            btnToggleText: "Show Losses",
            dataSourceUrl: "data/ucb_1.json",
            policyName: "",
            chartOptions: {
                chart: {
                    height: 640,
                    type: "line",
                    zoom: {
                        type: "x",
                        enabled: true,
                        autoScaleYaxis: false,
                    },
                    toolbar: {
                        autoSelected: "zoom",
                    },
                },
                noData: {
                    text: "No data available!",
                },
                xaxis: {
                    tickAmount: 12,
                },
                dataLabels: {
                    enabled: false,
                },
                stroke: {
                    curve: "straight",
                },
                title: {
                    text: "UCB run",
                    align: "center",
                },
                grid: {
                    row: {
                        colors: ["#f3f3f3", "transparent"],
                        opacity: 0.5,
                    },
                },
            },
        };
    },
    methods: {
        async loadData() {
            this.isLoading = true;

            const okay = await this.collectData(this.dataSourceUrl);
            this.isLoading = false;

            this.isInitialized = okay;
        },
        async collectData(loc) {
            this.isLoading = true;
            let data;
            let success;
            try {
                data = await this.fetchData(loc);
                this.currentData = data;
                success = true;
            } catch (e) {
                success = false;
                await this.onFetchFailure(e, loc);
            }
            return success;
        },
        async fetchData(loc) {
            return await axios
                .get(loc)
                .then((response) => (this.currentData = response.data));
        },
        async onFetchFailure(e, loc) {
            this.fetchFailted = true;
            console.log("Failure while fetching data at ", loc, ": ", e);
      setTimeout(function () {
        this.failedFetch = true;
      }, 3000);
    },
    onToggleRewardsLosses() {
        this.flagRewardsLosses = !this.flagRewardsLosses;
    },
    onEnvChange(event) {
        this.selectedEnv = event.target.value;
    },
    onAgentChange(event) {
        this.selectedAgent = event.target.value;
    }
  },
  computed: {
    rewardsData() {
      return [{ name: this.policyName, data: this.currentData.rewards }];
    },
    lossesData() {
      return [{ name: this.policyName, data: this.currentData.losses }];
    },
    currentSeries() {
      return this.flagRewardsLosses ? this.lossesData : this.rewardsData;
    },
    btnLoadText() {
      return this.isLoading ? "" : "Load Data";
    },
    toggleRewardsLossesText() {
      return this.flagRewardsLosses? "Show Rewards": "Show Losses";
    },
  },
  mounted() {
    this.agentsList = agents();
    this.envsList = environments();
    this.loadData();
  }
};
</script>

<style>
#app {
  /* background: #fff; */
  background: #f5f5f5;
  margin: 2rem 0 4rem 0;
  padding: 1rem;
  padding-top: 0;
  position: relative;
  height: 640;
  width: 480;
  box-shadow: 0 2px 4px 0 rgba(0, 0, 0, 0.2), 0 2.5rem 5rem 0 rgba(0, 0, 0, 0.1);
}
.message-string {
  height: 32px;
  max-height: 32px;
}
.div-selectors-container {
  display: flex;
  flex-direction: row;
  justify-content: center;
  align-items: center;
}
.div-selector {
  display: flex;
  flex-direction: column;
  justify-content: left;
  align-items: left;
}
.input-header {
  display: flex;
  flex-direction: row;
  /* justify-content: left; */
  /* align-items: center; */
  align-items: space-around;
}
.main-tile {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
}
button {
  padding: 10px;
  /* background-color: #cce6ff; */
  background-color: #e1e1e1;
  color: #4d4d4d;
}
</style>
