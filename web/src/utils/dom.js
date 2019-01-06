import SDK from '@/inspector/sdk'
import { dumpAllProperties } from './debug'

function getRuntimeModel () {
  return SDK.targetManager.models(SDK.RuntimeModel)[0]
}

function getExecutionContext () {
  return getRuntimeModel().executionContexts()[0]
}

function evaluate (expression) {
  return getExecutionContext().evaluate({
    expression,
    objectGroup: 'console',
    includeCommandLineAPI: true
  }, true)
  // .then(resp => {
  //   // dumpAllProperties(resp)
  //   let obj = resp.object
  //   console.log('obj.objectId: ' + obj.objectId)
  //   console.log('obj.type: ' + obj.type)
  //   console.log('obj.subtype: ' + obj.subtype)
  //   console.log('obj.value: ' + obj.value)
  //   console.log('obj.description: ' + obj.description)
  //   console.log('obj.hasChildren: ' + obj.hasChildren)
  //   console.log('obj.preview: ' + obj.preview)
  //   console.log('obj.customPreview: ' + obj.customPreview())
  //   // obj.getAllProperties(false, true, (properties, internalProperties) => {
  //   //   for (let i = 0; i < properties.length; ++i) {
  //   //     let property = properties[i]
  //   //     if (property.name === '__proto__' && !property.isAccessorProperty()) {
  //   //       continue
  //   //     }
  //   //     var canShowProperty = property.getter || !property.isAccessorProperty()
  //   //     if (canShowProperty && property.name !== '__proto__') {
  //   //       console.log(`property ${property.name}: ${property.value}`)        
  //   //     }
  //   //   }
  //   // })
  //   obj.getProperty([0], result => { 
  //     if (result.isNode()) {
  //       console.log('node id: ' + result.objectId)
  //       getDOMModel().pushNodeToFrontend(result.objectId).then(resp => {
  //         console.log(resp)
  //         console.log(resp.id)
  //       })
  //     }
  //   })
  //   // function foo (args) {
  //   //   console.log('length: ' + this.length)
  //   //   return this.length
  //   // }
  //   // obj.callFunction(foo, [{value: 'test'}], result => {
  //   //   console.log('type: ' + result.type + " " + result.subtype + " " + result.value)
  //   // })
  // })
}

export function evaluateXPath (xpath) {
  console.log(`evaluateXPath: ${xpath}`)
  return evaluate(`$x("${xpath}")`)
}

async function matchNodeAgainstXPath (targetNode, xpath) {
  console.log(`test xpath: ${xpath}`)
  let { object } = await evaluateXPath(xpath)
  // In general, the resp is the remote object with array type.
  let result = await object.getPropertyAsync(['length'])
  if (result.value !== 1) {
    return false
  }
  result = await object.getPropertyAsync([0])
  if (!result.isNode()) {
    return false
  }
  let node = await getDOMModel().pushNodeToFrontend(result.objectId)
  if (node.id !== targetNode.id) {
    console.log(`node.id: ${node.id} targetNode.id: ${targetNode.id}`)
    return false
  }
  return true
}

export async function getChildNodes (node) {
  let children = await node.getChildNodesAsync()
  return children || []
}

async function getChildNodeIndex (node) {
  let children = await getChildNodes(node.parentNode)
  return children.findIndex(child => child.id === node.id)
}

async function getChildNodeIndexWithSameType (node) {
  let children = await getChildNodes(node.parentNode)
  let index = 0
  for (let child of children) {
    if (child.nodeName() === node.nodeName()) {
      index += 1
    }
    if (child.id === node.id) {
      return index
    }
  }
}

export function getDOMModel () {
  return SDK.targetManager.models(SDK.DOMModel)[0]
}

export async function getXPath (node) {
  let xpath = '', targetNode = node
  let nodeName = node.nodeName().toLowerCase()
  let found = false

  while (nodeName) {
    let id = node.getAttribute('id')
    if (id) {
      xpath = `${nodeName}[@id='${id}']${xpath ? '/' : ''}${xpath}`
      found = await matchNodeAgainstXPath(targetNode, `//${xpath}`)
      if (found) {
        break
      }
    } else {
      let index = await getChildNodeIndexWithSameType(node)
      xpath = `${nodeName}[${index}]${xpath ? '/' : ''}${xpath}`
    }

    if (nodeName === 'body') {
      break
    }

    node = node.parentNode
    nodeName = node.nodeName().toLowerCase()
  }

  if (!found) {
    found = await matchNodeAgainstXPath(targetNode, `//${xpath}`)
    if (!found) {
      console.log(`Target node ${targetNode.nodeName()} not found. XPath //${xpath} tried.`)
    }
  }
  return xpath
}

/**
  * Get the first node matched against the specified xpath
 */
export async function getNodeByXPath (xpath) {
  let { object } = await evaluateXPath(xpath)
  let result = await object.getPropertyAsync(['length'])
  if (result.value < 1) {
    return false
  }
  result = await object.getPropertyAsync([0])
  if (!result.isNode()) {
    return false
  }
  let node = await getDOMModel().pushNodeToFrontend(result.objectId)
  return node
}
