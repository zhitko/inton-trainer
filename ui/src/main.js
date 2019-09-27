import Vue from 'vue';
import VueMaterial from 'vue-material';

import App from './App.vue';
import router from './router';


import 'vue-material/dist/vue-material.min.css';
import 'vue-material/dist/theme/default.css';
import './assets/google_roboto_material.css';
import i18n from './i18n';

Vue.config.productionTip = false;

Vue.use(VueMaterial);

new Vue({
  router,
  i18n,
  render: h => h(App),
}).$mount('#app');
