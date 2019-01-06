import {
  getChildNodes,
  getDOMModel,
  getXPath,
  evaluateXPath
} from './dom'

export async function findPage (node) {
  let pageXPath = await getXPath(node)
  let pageLinkXPath = `${pageXPath}//a[@href]`
  let { object } = await evaluateXPath(`//${pageLinkXPath}`)
  let result = await object.getPropertyAsync(['length'])
  if (result.value <= 0) {
    return false
  }
  return {
    node, pageXPath, pageLinkXPath
  }
}
