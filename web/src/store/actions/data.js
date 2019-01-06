/* eslint no-empty-pattern: 0 */

import request from '@/request'

export const getData = ({}, params) => {
  return new Promise((resolve, reject) => {
    request.get('/data', { params }).catch(err => reject(err)).then(resp => resolve(resp))
  })
}

export const getDataItem = ({}, id) => {
  return new Promise((resolve, reject) => {
    request.get(`/data/${id}`).catch(err => reject(err)).then(resp => resolve(resp))
  })
}
