import SDK from '@/inspector/sdk'

describe('SDK', () => {
  it('should be ok', () => {
    expect(SDK.TargetManager).to.be.a('function')
    expect(SDK.targetManager).to.be.a('object')
  })

  it('can connect to main target', () => {
    SDK.targetManager.connectToMainTarget()
    expect(SDK.targetManager._mainConnection).to.be.instanceof(SDK.StubConnection)
  })
})
