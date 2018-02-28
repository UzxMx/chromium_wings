import Protocol from '@/inspector/protocol'

describe('Protocol', () => {
  it('should be ok', () => {
    expect(typeof Protocol.InspectorBackend).to.equal('function')
    expect(typeof Protocol.inspectorBackend).to.equal('object')
  })

  it('can register command', () => {
    Protocol.inspectorBackend.registerCommand(
      'Overlay.setInspectMode',
      [{ name: 'mode', type: 'string', optional: false }, { name: 'highlightConfig', type: 'object', optional: true }],
      [],
      false
    )
  })
})
