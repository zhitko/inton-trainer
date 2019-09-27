<template>
  <md-field>
    <label for="locale">{{ $t("locale-changer.language") }}</label>
    <md-select v-model="$i18n.locale" name="locale" id="locale">
      <md-option v-for="(lang, i) in langs" :key="`Lang${i}`" :value="lang">{{ lang }}</md-option>
    </md-select>
  </md-field>
</template>

<script>
export default {
  name: 'locale-changer',
  data() {
    return { langs: ['ru', 'en'] };
  },
};
</script>
