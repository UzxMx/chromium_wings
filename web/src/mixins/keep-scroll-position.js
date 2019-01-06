import Vue from 'vue'

function needToKeepScrollPosition (comp) {
  let name = comp.constructor.options.name
  return name === 'ElTable'
}

Vue.mixin({
  mounted () {
    if (needToKeepScrollPosition(this)) {
      let el = this.$el.querySelector('.el-table__body-wrapper')
      el.addEventListener('scroll', function (event) {
        event.target.setAttribute('data-vue-keep-scroll-position', event.target.scrollLeft + '-' + event.target.scrollTop)
      })
    }
  },

  activated () {
    if (needToKeepScrollPosition(this)) {
      let i, len, pos, target, targets

      let el = this.$el.querySelector('.el-table__body-wrapper')
      window.el = el

      targets = el.querySelectorAll('[data-vue-keep-scroll-position]')
      if (targets.length > 0) {
        for (i = 0, len = targets.length; i < len; i++) {
          target = targets[i]
          pos = target.getAttribute('data-vue-keep-scroll-position').split('-')
          target.scrollLeft = pos[0]
          target.scrollTop = pos[1]
        }
      } else {
        if (el.hasAttribute('data-vue-keep-scroll-position')) {
          pos = el.getAttribute('data-vue-keep-scroll-position').split('-')
          el.scrollLeft = pos[0]
          el.scrollTop = pos[1]
        }
      }
    }
  }
})
