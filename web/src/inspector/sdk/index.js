import * as Target from './Target'
import * as Connections from './Connections'
import * as DOMModel from './DOMModel'
import * as ResourceTreeModel from './ResourceTreeModel'
import * as SecurityOriginManager from './SecurityOriginManager'
import * as NetworkManager from './NetworkManager'
import * as RuntimeModel from './RuntimeModel'
import * as DebuggerModel from './DebuggerModel'
import * as OverlayModel from './OverlayModel'
import * as SourceMapManager from './SourceMapManager'

export default {
  ...Target,
  ...Connections,
  ...DOMModel,
  ...ResourceTreeModel,
  ...SecurityOriginManager,
  ...NetworkManager,
  ...RuntimeModel,
  ...DebuggerModel,
  ...OverlayModel,
  ...SourceMapManager
}
