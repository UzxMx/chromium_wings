/**
 * This directive won't work with keep-alive, for the inserted hook is only fired once.
 */
function parseNode (el, binding) {
  if (binding.value && binding.value.selector) {
    let node = el.querySelector(binding.value.selector)
    if (node) {
      return node
    }
  }
  return el
}

export default function (Vue) {
  Vue.directive('keep-scroll-position', {
    bind (el, binding) {
      el = parseNode(el, binding)
      el.addEventListener('scroll', function (event) {
        event.target.setAttribute('data-vue-keep-scroll-position', event.target.scrollLeft + '-' + event.target.scrollTop)
      })
    },

    inserted (el, binding) {
      var i, len, pos, target, targets
      el = parseNode(el, binding)
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
  })
}
