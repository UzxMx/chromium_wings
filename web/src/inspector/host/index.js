import * as InspectorFrontendHost from './InspectorFrontendHost'
import * as DevtoolsCompatibility from './devtools_compatibility'
import InspectorFrontendHostAPI from './InspectorFrontendHostAPI'

export default {
  ...InspectorFrontendHost,
  ...DevtoolsCompatibility,
  InspectorFrontendHostAPI
}
