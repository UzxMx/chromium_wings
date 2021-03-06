// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import '@/inspector/utilities'

import Vue from 'vue'
import App from './App'
import router from './router'
import SDK from '@/inspector/sdk'

SDK.targetManager.connectToMainTarget(() => {
  console.log('websocket connection lost')
})

Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  components: { App },
  template: '<App/>'
})
