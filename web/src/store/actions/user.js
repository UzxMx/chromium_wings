import request from '@/request'
import * as types from '@/store/mutation-types'

export const signIn = ({ state, commit }, { username, password }) => {
  return new Promise((resolve, reject) => {
    request.post({
      url: '/users/login',
      withAccessToken: false
    }, { username, password }).catch(err => reject(err)).then(resp => {
      if (resp.data && resp.data.idToken) {
        commit(types.SET_AUTH, resp.data)
      }
      resolve(resp)
    })
  })
}
