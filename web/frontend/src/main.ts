import { createApp } from "vue";
import { createPinia } from "pinia";
import { registerPlugins } from "@/plugins";

import vuetify from "./plugins/vuetify";
import App from "./App.vue";


const pinia = createPinia();
const app = createApp(App);

app.use(pinia).use(vuetify).mount("#app");
