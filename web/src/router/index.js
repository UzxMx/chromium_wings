import Vue from 'vue'
import Router from 'vue-router'

import users from '@/components/users/all'
import templates from '@/components/templates/all'
import data from '@/components/data/all'

Vue.use(Router)

export default new Router({
  mode: 'history',
  routes: [
    {
      path: '/users/sign_in',
      name: 'Users.SignIn',
      component: users.SignIn
    },
    {
      path: '/users/index',
      name: 'Users.Index',
      component: users.Index
    },
    {
      path: '/users',
      redirect: '/users/index'
    },
    {
      path: '/templates/index',
      name: 'Templates.Index',
      component: templates.Index
    },
    {
      path: '/templates',
      redirect: '/templates/index'
    },
    {
      path: '/templates/:id',
      name: 'Templates.Detail',
      component: templates.Form
    },
    {
      path: '/templates/new',
      name: 'Templates.New',
      component: templates.Form
    },
    {
      path: '/data/index',
      name: 'Data.Index',
      component: data.Index
    },
    {
      path: '/data/:id',
      name: 'Data.Detail',
      component: data.Detail
    },
    {
      path: '/',
      redirect: '/templates'
    },
    {
      path: '/wings/index.html',
      redirect: '/'
    }
  ],
  // scrollBehavior (to, from, savedPosition) {
  //   console.log(savedPosition)
  //   if (savedPosition) {

  //     return savedPosition
  //   } else {
  //     return { x: 0, y: 0 }
  //   }
  // }
})
