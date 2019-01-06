<template>
  <div class="root">
    <with-menus v-if="isAuthenticated"></with-menus>
    <router-view v-else></router-view>
  </div>
</template>

<script>
import { mapGetters } from 'vuex'
import WithMenus from '@/components/containers/WithMenus'

export default {
  computed: mapGetters(['isAuthenticated']),

  components: { WithMenus },

  created () {
    if (!this.isAuthenticated && this.$route.name !== 'Users.SignIn') {
      this.$router.replace({ name: 'Users.SignIn' })
    }
  }
}
</script>

<!-- TODO only show right border when previewer is shown. -->
<style lang="scss">
  * {
    box-sizing: border-box;
  }

  html, body {
    margin: 0;
    padding: 0;
    width: 100%;
    height: 100%;
  }

  body {
    border-right: 1px solid #ccc;
  }

  .fa-icon {
    width: auto;
    height: 1.5em; /* or any other relative font sizes */

    /* You would have to include the following two lines to make this work in Safari */
    max-width: 100%;
    max-height: 100%;

    color: #409EFF;
  }  
</style>

<style lang="scss" scoped>
  .root {
    height: 100%;
  }
</style>
