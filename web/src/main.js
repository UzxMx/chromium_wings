// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import '@/inspector/utilities'

import Vue from 'vue'
import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
import Icon from 'vue-awesome/components/Icon'
import 'vue-awesome/icons'

import '@/mixins/keep-scroll-position'
import store from '@/store'
import router from './router'
import App from './App'
import SDK from '@/inspector/sdk'

SDK.targetManager.connectToMainTarget(() => {
  console.error('websocket connection lost')
})

Vue.config.productionTip = false

Vue.use(ElementUI)
Vue.component('icon', Icon)

/* eslint-disable no-new */
document.addEventListener('DOMContentLoaded', () => {
  const node = document.createElement('div')
  document.body.appendChild(node)
  new Vue({
    store,
    router,
    el: node,
    render: h => h(App)
  })
})
