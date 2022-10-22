import { createApp } from "vue";
import "./assets/reset.css";
import "./style.css";
import App from "./App.vue";

//import "vue-select/dist/vue-select.css";
//import VueNumberInput from "@chenfengyuan/vue-number-input";

const app = createApp(App);

//app.component('vue-number-input', VueNumberInput);

app.mount("#app");
