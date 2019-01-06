import {
  getChildNodes,
  getDOMModel,
  getXPath,
  evaluateXPath
} from './dom'

export function getMostNodes (nodes) {
  if (!Array.isArray(nodes) || !nodes.length) {
    return
  }

  let map = {}
  nodes.forEach((node, index) => {
    let name = node.nodeName()
    let nodes = map[name] ? map[name] : (map[name] = [])
    nodes.push({
      node, index
    })
  })
  let max = -1, maxKey
  Object.keys(map).forEach(key => {
    if (map[key].length > max) {
      max = map[key].length
      maxKey = key
    }
  })
  return map[maxKey]
}

function groupNodesByIndex (nodes) {
  let index = -2, group = [], currentBucket
  nodes.forEach(node => {
    if (index + 1 !== node.index) {
      currentBucket = []
      group.push(currentBucket)
    }
    index = node.index
    currentBucket.push(node)
  })
  return group
}

async function isListNode (node) {
  let children = await getChildNodes(node)

  let mostNodes = getMostNodes(children)
  if (!mostNodes || mostNodes.length < 5 || mostNodes.length / children.length < 0.5) {
    return false
  }

  // check if node is close to each other
  if (groupNodesByIndex(mostNodes).length !== 1) {
    return false
  }

  for (let n of mostNodes) {
    let node = n.node
    let result = await getDOMModel().querySelectorAll(node.id, 'a[href]')
    if (result.length !== 1) {
      return false
    }
  }

  let listXPath = await getXPath(node)
  let listItemNodeName = mostNodes[0].node.nodeName().toLowerCase()
  let listItemXPath = `${listXPath}/${listItemNodeName}`
  let listItemLinkXPath = `${listItemXPath}//a[@href]`
  let { object } = await evaluateXPath(`//${listItemLinkXPath}`)
  let result = await object.getPropertyAsync(['length'])
  if (result.value !== mostNodes.length) {
    return false
  }
  let idx = 0
  for (let n of mostNodes) {
    let node = n.node
    let nodeId = await getDOMModel().querySelector(node.id, 'a[href]')

    let result = await object.getPropertyAsync([idx])
    if (!result.isNode()) {
      return false
    }
    let anchorNode2 = await getDOMModel().pushNodeToFrontend(result.objectId)
    if (nodeId !== anchorNode2.id) {
      return false
    }
    idx += 1
  }

  return {
    listXPath, listItemXPath, listItemLinkXPath
  }
}

export async function findListNode (node) {
  let result = await isListNode(node)
  if (result) {
    return {
      node,
      ...result
    }
  }

  let children = await getChildNodes(node)
  for (let child of children) {
    let result = await findListNode(child)
    if (result) {
      return result
    }
  }
}

export async function findNodeInList (node, list) {
  let nodeXPath = await getXPath(node)
  let { listItemXPath } = list
  if (nodeXPath.indexOf(listItemXPath) !== 0) {
    return false
  }
  nodeXPath = nodeXPath.substring(listItemXPath.length)
  let idx = nodeXPath.indexOf('/')
  nodeXPath = `${listItemXPath}${nodeXPath.substring(idx)}`

  let { object } = await evaluateXPath(`//${listItemXPath}`)
  let result = await object.getPropertyAsync(['length'])
  let listItemLength = result.value

  result = await evaluateXPath(`//${nodeXPath}`)
  result = await result.object.getPropertyAsync(['length'])
  let nodeLength = result.value
  if (nodeLength !== listItemLength) {
    return false
  }
  return {
    nodeXPath
  }
}
