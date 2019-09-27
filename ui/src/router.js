import Vue from 'vue';
import Router from 'vue-router';
import Home from './views/Home.vue';
import Catalog from './views/Catalog.vue';
import Records from './views/Records.vue';
import Settings from './views/Settings.vue';
import TrainingRecord from './views/TrainingRecord.vue';

Vue.use(Router);

export default new Router({
  routes: [
    {
      path: '/home',
      name: 'home',
      component: Home,
    },
    { path: '/', redirect: '/home' },
    {
      path: '/catalog',
      name: 'catalog',
      component: Catalog,
    },
    {
      path: '/records',
      name: 'records',
      component: Records,
    },
    {
      path: '/record/:uuid',
      name: 'training-record',
      component: TrainingRecord,
      props: true,
    },
    {
      path: '/settings',
      name: 'settings',
      component: Settings,
    },
    {
      path: '/about',
      name: 'about',
      component: () => import('./views/About.vue'),
    },
  ],
});
