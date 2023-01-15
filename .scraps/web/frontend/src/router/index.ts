import { createRouter, createWebHistory } from "vue-router";
import ModelOrPolicyView from "../views/ModelOrPolicyView.vue";
import ParametersView from "../views/ParametersView.vue";

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: "/",
      name: "home",
      component: () => {},
    },
    {
      path: "/model/:name",
      name: "models",
      // route level code-splitting
      // this generates a separate chunk (About.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      component: ModelOrPolicyView,
    },
    {
      path: "/policy/:name",
      name: "post",
      component: ModelOrPolicyView,
    },
  ],
});

export default router;
