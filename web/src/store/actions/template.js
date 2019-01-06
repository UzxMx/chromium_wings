/* eslint no-empty-pattern: 0 */

import request from '@/request'

export const getTemplates = ({}, params) => {
  return new Promise((resolve, reject) => {
    request.get('/template', { params }).catch(err => reject(err)).then(resp => resolve(resp))
  })
}

export const getTemplate = ({}, id) => {
  return new Promise((resolve, reject) => {
    request.get(`/template/${id}`).catch(err => reject(err)).then(resp => resolve(resp))
  })
}

export const updateTemplate = ({}, params) => {
  return new Promise((resolve, reject) => {
    request.put(`/template/${params.id}`, params).catch(err => reject(err)).then(resp => resolve(resp))
  })
}
