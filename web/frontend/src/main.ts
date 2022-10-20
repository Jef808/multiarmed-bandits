import { createApp } from 'vue'
import './style.css'
import App from './App.vue'

import vSelect from "vue-select";
import "vue-select/dist/vue-select.css";
import VueNumberInput from "@chenfengyuan/vue-number-input";

const app = createApp(App);

app.component('v-select', vSelect);
app.component('vue-number-input', VueNumberInput);

app.mount('#app')
