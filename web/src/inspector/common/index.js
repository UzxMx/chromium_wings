import CommonObject from './Object'
import * as UIString from './UIString'
import * as Settings from './Settings'
import * as Color from './Color'
import * as ParsedURL from './ParsedURL'
import * as ResourceType from './ResourceType'

export default {
  ...CommonObject,
  ...UIString,
  ...Settings,
  ...Color,
  ...ParsedURL,
  ...ResourceType
}
