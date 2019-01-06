import axios from 'axios'
import _ from 'lodash'
import Uri from 'jsuri'
import { Message } from 'element-ui'

import { state } from '@/store/mutations'

const request = {}
const DEFAULTS = {
  withUrlPrefix: true,
  withLocale: false,
  withAccessToken: true
}; // must add colon, otherwise it will show `Cannot read property 'forEach' of undefined`
['delete', 'get', 'head', 'options', 'post', 'put', 'patch'].forEach(method => {
  request[method] = function () {
    var args = Array.from(arguments)
    var options = DEFAULTS
    if (typeof args[0] === 'object') {
      options = _.extend({}, options, args[0])
      args[0] = args[0].url
    }

    if (options.withUrlPrefix) {
      var url = args[0][0] === '/' ? args[0] : `/${args[0]}`
      args[0] = `${process.env.API_HOST}${url}`
    }

    var configIdx
    if (method === 'post' || method === 'put' || method === 'patch') {
      configIdx = 2
    } else {
      configIdx = 1
    }
    while (args.length < configIdx) {
      args.push(null)
    }
    if (args.length === configIdx) {
      args.push({})
    }
    var config = args[configIdx]

    if (options.withLocale) {
      if (configIdx === 1) {
        var uri = new Uri(args[0])
        if (!uri.hasQueryParam('locale')) {
          uri.addQueryParam('locale', state.locale)
          args[0] = uri.toString()
        }
      } else {
        var data = args[1] || {}
        if (!data.locale) {
          data.locale = state.locale
          args[1] = data
        }
      }
    }

    if (options.withAccessToken) {
      config.headers = _.extend(config.headers || {}, {
        'Authorization': `Bearer ${state.access_token}`
      })
    }

    return axios[method].apply(null, args).then(resp => {
      if (resp.data.err_msg) {
        Message(resp.data.err_msg)
      }
      return resp
    }).catch(err => {
      const response = err.response
      if (response.status === 401) {
        // TODO
      } else if (response.data && response.data.error) {
        console.log(response)
        Message(response.data.error)
      }
      throw err
    })
  }
})

export default request
