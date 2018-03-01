<template>
  <div id="app">
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
import utils from '@/utils'

export default {
  data () {
    return {
      inspecting: false,
      node: null
    }
  },

  methods: {
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

<style>
</style>
