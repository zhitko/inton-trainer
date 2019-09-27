// #006E82
// #8214A0
// #005AC8
// #00A0FA
// #FA78FA
// #14D2DC
// #AA0A3C
// #FA7850
// #0AB45A
// #F0F032
// #A0FA82
// #FAE6BE

<template>
  <div class="page-container">
    <md-app md-waterfall md-mode="overlap">
      <md-app-toolbar class="md-primary md-large">
        <div class="md-toolbar-row">
          <md-button class="md-icon-button" @click="menuVisible = !menuVisible">
            <md-icon>menu</md-icon>
          </md-button>

          <span class="md-title">{{ $t("app-title") }}</span>

          <div class="md-toolbar-section-end">
            <md-button class="md-icon-button" @click="goBack" v-if="hasBack()">
              <md-icon>keyboard_backspace</md-icon>
            </md-button>
          </div>
        </div>
      </md-app-toolbar>

      <md-app-drawer :md-active.sync="menuVisible">
        <SideMenu/>
      </md-app-drawer>

      <md-app-content>
        <router-view></router-view>
      </md-app-content>
    </md-app>
  </div>
</template>

<style lang="scss" scoped>
  .md-app {
    min-height: 600px;
    max-height: 100vh;
    border: 1px solid rgba(#000, .12);
  }
  .md-app-content {
    padding-bottom: 70px;
  }
</style>

<script>
import SideMenu from '@/components/SideMenu.vue';

export default {
  name: 'App',
  data: () => ({
    menuVisible: false,
  }),
  components: {
    SideMenu,
  },
  methods: {
    hasBack() {
      return this.$router.currentRoute.name !== 'home';
    },
    goBack() {
      return window.history.length > 1
        ? this.$router.go(-1)
        : this.$router.push('/');
    },
  },
};
</script>
