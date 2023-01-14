/**
 * main.ts
 *
 * Bootstraps Vuetify and other plugins then mounts the App`
 */

// Components
import App from './App.vue'

// Composables
import { createApp } from 'vue'

import { createPinia } from 'pinia'

// Plugins
import { registerPlugins } from '@/plugins'

import LogRocket from 'logrocket'
LogRocket.init('4lylln/mab_viz-dev')

const app = createApp(App)
const pinia = createPinia()

registerPlugins(app)

app.use(pinia).mount('#app')
