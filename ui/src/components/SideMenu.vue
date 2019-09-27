<template>
    <span>
        <md-toolbar class="md-transparent" md-elevation="0">
            {{ $t("side-menu.title") }}
        </md-toolbar>

        <md-list>
            <md-list-item to="/home">
                <md-icon>home</md-icon>
                <span class="md-list-item-text">
                    {{ $t("side-menu.home") }}
                </span>
            </md-list-item>

            <md-list-item to="/catalog">
                <md-icon>playlist_play</md-icon>
                <span class="md-list-item-text">
                    {{ $t("side-menu.catalog") }}
                </span>
            </md-list-item>

            <md-list-item to="/settings">
                <md-icon>settings</md-icon>
                <span class="md-list-item-text">
                    {{ $t("side-menu.settings") }}
                </span>
            </md-list-item>

            <md-list-item to="/about">
                <md-icon>help</md-icon>
                <span class="md-list-item-text">
                    {{ $t("side-menu.about") }}
                </span>
            </md-list-item>

            <md-list-item>
            <LocaleChanger/>
            </md-list-item>
        </md-list>
    </span>
</template>

<style lang="scss" scoped>
  .md-drawer {
    width: 230px;
    max-width: calc(100vw - 125px);
  }
</style>

<script>
import LocaleChanger from '@/components/LocaleChanger.vue';

export default {
  components: {
    LocaleChanger,
  },
};
</script>
