<template>
  <div class="root">
    <el-form :model="formData" label-width="120px">
      <el-form-item label="URL">
        <el-input v-model="formData.url" @keyup.enter.native="handleURLEnter"></el-input>
      </el-form-item>

      <el-form-item label="名称">
        <el-input v-model="formData.name"></el-input>
      </el-form-item>

      <el-form-item label="状态">
        <el-select v-model="formData.status">
          <el-option label="停止" value="0"></el-option>
          <el-option label="待审" value="2"></el-option>
          <el-option label="运行中" value="1"></el-option>
        </el-select>
      </el-form-item>      

      <el-form-item label="列表区域">
        <div class="selection">
          <span class="text" v-if="listSelected">已选择</span>
          <div class="buttons">
            <el-button size="small" @click="select('list')">{{ listSelecting ? '取消选择' : '选择' }}</el-button>
            <el-button v-if="listSelected" size="small" @click="highlightForTwoSeconds('list')">高亮显示</el-button>
          </div>
        </div>
      </el-form-item>

      <el-form-item label="页码区域">
        <div class="selection">
          <span class="text" v-if="pageSelected">已选择</span>
          <div class="buttons">
            <el-button size="small" @click="select('page')">{{ pageSelecting ? '取消选择' : '选择' }}</el-button>
            <el-button v-if="pageSelected" size="small" @click="highlightForTwoSeconds('page')">高亮显示</el-button>
          </div>
        </div>
      </el-form-item>

      <el-form-item label="列表标题">
        <div class="selection">
          <span class="text" v-if="nodeInListSelected('title')">已选择</span>
          <div class="buttons">
            <el-button size="small" @click="select('titleInList')">{{ titleInListSelecting ? '取消选择' : '选择' }}</el-button>
          </div>
        </div>
      </el-form-item>

      <el-form-item label="列表日期">
        <div class="selection">
          <span class="text" v-if="nodeInListSelected('date')">已选择</span>
          <div class="buttons">
            <el-button size="small" @click="select('dateInList')">{{ titleInListSelecting ? '取消选择' : '选择' }}</el-button>
          </div>
        </div>
      </el-form-item>

      <el-form-item label="标题">
        <div class="selection">
          <span class="text" v-if="nodeSelected('title')">已选择</span>
          <div class="buttons">
            <el-button size="small" @click="select('title')">{{ titleSelecting ? '取消选择' : '选择' }}</el-button>
            <el-button v-if="nodeSelected('title')" size="small" @click="highlightForTwoSeconds('title')">高亮显示</el-button>
          </div>
        </div>
      </el-form-item>

      <el-form-item label="正文">
        <div class="selection">
          <span class="text" v-if="nodeSelected('content')">已选择</span>
          <div class="buttons">
            <el-button size="small" @click="select('content')">{{ contentSelecting ? '取消选择' : '选择' }}</el-button>
            <el-button v-if="nodeSelected('content')" size="small" @click="highlightForTwoSeconds('content')">高亮显示</el-button>
          </div>
        </div>
      </el-form-item>

      <el-form-item>
        <el-button type="primary" @click="submit">保存</el-button>
      </el-form-item>      
    </el-form>  
  </div>
</template>

<script>
import { mapActions } from 'vuex'
import { Loading, Message } from 'element-ui'

import Protocol from '@/inspector/protocol'
import SDK from '@/inspector/sdk'
import Host from '@/inspector/host'
import utils from '@/utils'

export default {
  data () {
    return {
      formData: {
        url: null,
        name: null,
        status: '0'
      },
      templateId: null,

      previewerOpened: false,
      selecting: false,
      selectionType: null,
      listSelection: null,
      pageSelection: null,
      nodeSelectionInList: {
        title: null,
        date: null
      },
      nodeSelection: {
        title: null,
        content: null
      }
    }
  },

  computed: {
    listSelecting () {
      return this.selecting && this.selectionType === 'list'
    },

    listSelected () {
      return this.listSelection && this.listSelection.listXPath && this.listSelection.listItemXPath && this.listSelection.listItemLinkXPath
    },

    pageSelecting () {
      return this.selecting && this.selectionType === 'page'
    },

    pageSelected () {
      return this.pageSelection && this.pageSelection.pageXPath && this.pageSelection.pageLinkXPath
    },

    titleInListSelecting () {
      return this.selecting && this.selectionType === 'titleInList'
    },

    dateInListSelecting () {
      return this.selecting && this.selectionType === 'dateInList'
    },

    titleSelecting () {
      return this.selecting && this.selectionType === 'title'
    },

    contentSelecting () {
      return this.selecting && this.selectionType === 'content'
    }    
  },

  methods: {
    ...mapActions(['getTemplate', 'updateTemplate']),

    nodeInListSelected (type) {
      return this.nodeSelectionInList[type] && this.nodeSelectionInList[type].nodeXPath
    },

    nodeSelected (type) {
      return this.nodeSelection[type] && this.nodeSelection[type].nodeXPath
    },

    openPreviewer (url) {
      this.previewerOpened = true
      Host.InspectorFrontendHost.openPreviewer(url)
    },

    closePreviewer () {
      this.previewerOpened = false
      Host.InspectorFrontendHost.closePreviewer()
    },

    select (type) {
      if (!this.selectionType || this.selectionType === type) {
        this.selecting = !this.selecting
        let mode = this.selecting ? Protocol.Overlay.InspectMode.SearchForNode : Protocol.Overlay.InspectMode.None

        for (var overlayModel of SDK.targetManager.models(SDK.OverlayModel)) {
          overlayModel.setInspectMode(mode)
        }

        if (this.selecting) {
          this.selectionType = type
        } else {
          this.selectionType = null
        }
      } else {
        this.selectionType = type
      }
    },

    inspectNodeRequested (event) {
      /** @type {!SDK.DeferredDOMNode} */
      var deferredNode = event.data
      let selectionType = this.selectionType
      deferredNode.resolve(node => {
        if (selectionType === 'list') {
          utils.findListNode(node).then(result => {
            if (result) {
              this.listSelection = result
            }
          })
        } else if (selectionType === 'page') {
          utils.findPage(node).then(result => {
            if (result) {
              this.pageSelection = result
            }
          })
        } else if (selectionType.endsWith('InList')) {
          let type = selectionType.substring(0, selectionType.indexOf('InList'))
          utils.findNodeInList(node, this.listSelection).then(result => {
            if (result) {
              this.nodeSelectionInList[type] = result
            }
          })
        } else {
          utils.getXPath(node).then(result => {
            if (result) {
              this.nodeSelection[selectionType] = {
                node: node,
                nodeXPath: result
              }
            }
          })
        }
      })
      // deselect
      this.select(selectionType)
    },

    highlightForTwoSeconds (type) {
      this.getNodeByType(type).then(node => {
        if (node) {
          node.highlightForTwoSeconds()
        }
      })
    },

    scrollIntoView (type) {
      this.getNodeByType(type).then(node => {
        if (node) {
          node.scrollIntoView()
        }
      })
    },

    highlight (type) {
      this.getNodeByType(type).then(node => {
        if (node) {
          node.highlight()
        }
      })
    },

    getNodeByType (type) {
      let xpath = this.getNodeXPathByType(type)
      return utils.getNodeByXPath(`//${xpath}`)
    },

    getNodeXPathByType (type) {
      switch (type) {
        case 'list':
          return this.listSelection.listXPath
        case 'page':
          return this.pageSelection.pageXPath
        default:
          return this.nodeSelection[type].nodeXPath
      }
    },

    handleURLEnter () {
      this.openPreviewer(this.formData.url)
    },

    splitByLastNode (xpath) {
      let ary = []
      let index = xpath.lastIndexOf('/')
      if (index === -1) {
        ary[1] = xpath
      } else {
        ary[1] = xpath.substring(index)
      }
      while (index >= 0 && xpath[index] === '/') {
        index -= 1
      }
      if (index !== -1) {
        ary[0] = xpath.substring(0, index + 1)
      }
      return ary
    },

    findTrunkXPath (xpath) {
      let path = this.splitByLastNode(xpath)[0]
      let trunkXPath = path // Use this as the fallback
      let hasIndex = fragment => /\[\d+\]/.test(fragment)
      while (path) {
        let ary = this.splitByLastNode(path)
        if (!ary[0]) {
          break
        }
        if (!hasIndex(ary[1]) && hasIndex(this.splitByLastNode(ary[0])[1])) {
          trunkXPath = ary[0]
          break
        }
        path = ary[0]
      }
      return trunkXPath
    },

    parseListSelection (listItemLinkXPath) {
      if (listItemLinkXPath) {
        let listXPath = this.findTrunkXPath(listItemLinkXPath)
        let index = listXPath.length
        let length = listItemLinkXPath.length
        while (index < length && listItemLinkXPath[index] === '/') {
          index += 1
        }
        while (index < length && listItemLinkXPath[index] !== '/') {
          index += 1
        }
        let listItemXPath = listItemLinkXPath.substring(0, index)
        this.listSelection = {
          listXPath, listItemXPath, listItemLinkXPath
        }
      }
    },

    parsePageSelection (pageLinkXPath) {
      if (pageLinkXPath) {
        let pageXPath = this.findTrunkXPath(pageLinkXPath)
        this.pageSelection = {
          pageXPath, pageLinkXPath
        }
      }
    },

    stripSlashFromXPathEnd (xpath) {
      let index = xpath.length - 1
      while (index >= 0 && xpath[index] === '/') {
        index -= 1
      }
      if (index === -1) {
        return ''
      } else {
        return xpath.substring(0, index + 1)
      }
    },

    // body/div[1]/div[2]/div[2]/div[1]/h2[1]//allText()
    stripAllTextFromXPath (xpath) {
      xpath = xpath.substring(0, xpath.lastIndexOf('allText()'))
      return this.stripSlashFromXPathEnd(xpath)
    },

    submit () {
      let childXpath = {}
      Array.prototype.forEach.call(['title', 'content'], key => {
        if (this.nodeSelection[key]) {
          let xpath = this.nodeSelection[key].nodeXPath
          if (key === 'title') {
            childXpath[key] = `${xpath}//allText()`
          } else {
            childXpath[key] = xpath
          }
        }
      })

      let template = {
        url: this.formData.url,
        name: this.formData.name,
        available: parseInt(this.formData.status),
        xpath: this.listSelection.listItemLinkXPath,
        pageXpath: this.pageSelection.pageLinkXPath,
        childXpath: JSON.stringify(childXpath)
      }
      if (this.templateId) {
        template.id = this.templateId
        let loading = Loading.service()
        this.updateTemplate(template).then(resp => {
          if (resp.data.template) {
            Message('保存成功')
          }
        }).finally(() => loading.close())
      }
    }
  },

  mounted () {
    SDK.targetManager.addModelListener(SDK.OverlayModel, SDK.OverlayModel.Events.InspectNodeRequested, this.inspectNodeRequested, this)
  },

  beforeRouteUpdate (to, from, next) {
    // TODO
  },

  activated () {
    this.openPreviewer()

    if (this.$route.params.id) {
      let templateId = this.$route.params.id

      let loading = Loading.service()
      this.getTemplate(templateId).then(resp => {
        let { template, user } = resp.data
        this.templateId = template.id
        this.formData.url = template.url
        this.formData.name = template.name
        this.formData.status = template.available.toString()

        this.parseListSelection(template.xpath)
        this.parsePageSelection(template.pageXpath)
        // TODO parse other xpath
        let childXpath = JSON.parse(template.childXpath)
        if (childXpath.title) {
          this.nodeSelection.title = {
            nodeXPath: this.stripAllTextFromXPath(childXpath.title)
          }
        }
        if (childXpath.content) {
          this.nodeSelection.content = {
            nodeXPath: childXpath.content
          }
        }

        let listChildXpath = JSON.parse(template.listChildXpath)

        this.openPreviewer(this.formData.url)
      }).finally(() => loading.close())
    }
  },

  deactivated () {
    this.closePreviewer()
  }
}
</script>

<style lang="scss" scoped>
  .root {
    padding: 10px;
  }

  .selection {
    display: flex;

    .text {
      color: #409EFF;
    }

    .buttons {
      flex: 1;
      text-align: right;
    }
  }
</style>
