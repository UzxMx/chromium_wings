import Vue from 'vue'

import * as types from './mutation-types'

export const state = {}

function loadAuth () {
  let key = 'access_token'
  Vue.set(state, key, localStorage.getItem(key))
}

function saveAuth (auth) {
  let key = 'access_token'
  if (auth) {
    localStorage.setItem(key, auth.idToken)
  } else {
    localStorage.removeItem(key)
  }
}

loadAuth()

export const mutations = {
  [types.SET_AUTH] (state, auth) {
    saveAuth(auth)
    loadAuth()
  }
}
