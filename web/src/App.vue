<template>
  <div id="app">
    <button @click="openPreviewer">Open Previewer</button>
    <button @click="toggleInspect">{{ !inspecting ? 'Inspect' : 'Stop inspect' }}</button>
    <div v-if="node">
      <div>node type: {{ node.nodeType() }}</div>
      <div>node name: {{ node.nodeName() }}</div>
      <button @click="highlightNode">Highlight node</button>
      <button @click="scrollIntoView">Scroll into view</button>
      <button @click="highlight">Highlight</button>
    </div>
  </div>
</template>

<script>
import Protocol from '@/inspector/protocol'
import SDK from '@/inspector/sdk'
import Host from '@/inspector/host'
import utils from '@/utils'

export default {
  data () {
    return {
      inspecting: false,
      node: null
    }
  },

  methods: {
    openPreviewer () {
      Host.InspectorFrontendHost.openPreviewer()
    },

    toggleInspect () {
      this.inspecting = !this.inspecting
      let mode = this.inspecting ? Protocol.Overlay.InspectMode.SearchForNode : Protocol.Overlay.InspectMode.None

      for (var overlayModel of SDK.targetManager.models(SDK.OverlayModel)) {
        overlayModel.setInspectMode(mode)
      }
    },

    inspectNodeRequested (event) {
      /** @type {!SDK.DeferredDOMNode} */
      var deferredNode = event.data
      deferredNode.resolve(node => {
        this.node = node
      })
      this.toggleInspect()
    },

    highlightNode () {
      this.node.highlightForTwoSeconds()
    },

    scrollIntoView () {
      this.node.scrollIntoView()
    },

    highlight () {
      this.node.highlight()
    }
  },

  mounted () {
    SDK.targetManager.addModelListener(SDK.OverlayModel, SDK.OverlayModel.Events.InspectNodeRequested, this.inspectNodeRequested, this)
  }
}
</script>

<style lang="scss">
  html, body {
    width: 100%;
    height: 100%;
    border-right: 1px solid red;
  }
</style>

<style lang="scss" scoped>
  #app {
    width: 100%;
    height: 100%;
    border-right: 1px solid #ddd;
  }
</style>
