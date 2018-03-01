const Common = {}

/**
 * @implements {Common.EventTarget}
 * @unrestricted
 */
Common.Object = class {
  constructor() {
    /** @type {(!Map<symbol, !Array<!Common.Object._listenerCallbackTuple>>|undefined)} */
    this._listeners;
  }

  /**
   * @override
   * @param {symbol} eventType
   * @param {function(!Common.Event)} listener
   * @param {!Object=} thisObject
   * @return {!Common.EventTarget.EventDescriptor}
   */
  addEventListener(eventType, listener, thisObject) {
    if (!listener)
      console.assert(false);

    if (!this._listeners)
      this._listeners = new Map();

    if (!this._listeners.has(eventType))
      this._listeners.set(eventType, []);
    this._listeners.get(eventType).push({thisObject: thisObject, listener: listener});
    return {eventTarget: this, eventType: eventType, thisObject: thisObject, listener: listener};
  }

  /**
   * @override
   * @param {symbol} eventType
   * @return {!Promise<*>}
   */
  once(eventType) {
    return new Promise(resolve => {
      var descriptor = this.addEventListener(eventType, event => {
        this.removeEventListener(eventType, descriptor.listener);
        resolve(event.data);
      });
    });
  }

  /**
   * @override
   * @param {symbol} eventType
   * @param {function(!Common.Event)} listener
   * @param {!Object=} thisObject
   */
  removeEventListener(eventType, listener, thisObject) {
    console.assert(listener);

    if (!this._listeners || !this._listeners.has(eventType))
      return;
    var listeners = this._listeners.get(eventType);
    for (var i = 0; i < listeners.length; ++i) {
      if (listeners[i].listener === listener && listeners[i].thisObject === thisObject)
        listeners.splice(i--, 1);
    }

    if (!listeners.length)
      this._listeners.delete(eventType);
  }

  /**
   * @override
   * @param {symbol} eventType
   * @return {boolean}
   */
  hasEventListeners(eventType) {
    return !!(this._listeners && this._listeners.has(eventType));
  }

  /**
   * @override
   * @param {symbol} eventType
   * @param {*=} eventData
   */
  dispatchEventToListeners(eventType, eventData) {
    if (!this._listeners || !this._listeners.has(eventType))
      return;

    var event = /** @type {!Common.Event} */ ({data: eventData});
    var listeners = this._listeners.get(eventType).slice(0);
    for (var i = 0; i < listeners.length; ++i)
      listeners[i].listener.call(listeners[i].thisObject, event);
  }
};

/**
 * @interface
 */
Common.EventTarget = function() {};

/**
 * @param {!Array<!Common.EventTarget.EventDescriptor>} eventList
 */
Common.EventTarget.removeEventListeners = function(eventList) {
  for (var i = 0; i < eventList.length; ++i) {
    var eventInfo = eventList[i];
    eventInfo.eventTarget.removeEventListener(eventInfo.eventType, eventInfo.listener, eventInfo.thisObject);
  }
  // Do not hold references on unused event descriptors.
  eventList.splice(0, eventList.length);
};

Common.EventTarget.prototype = {
  /**
   * @param {symbol} eventType
   * @param {function(!Common.Event)} listener
   * @param {!Object=} thisObject
   * @return {!Common.EventTarget.EventDescriptor}
   */
  addEventListener(eventType, listener, thisObject) {},

  /**
   * @param {symbol} eventType
   * @return {!Promise<*>}
   */
  once(eventType) {},

  /**
   * @param {symbol} eventType
   * @param {function(!Common.Event)} listener
   * @param {!Object=} thisObject
   */
  removeEventListener(eventType, listener, thisObject) {},

  /**
   * @param {symbol} eventType
   * @return {boolean}
   */
  hasEventListeners(eventType) {},

  /**
   * @param {symbol} eventType
   * @param {*=} eventData
   */
  dispatchEventToListeners(eventType, eventData) {},
};

// export const CommonObject = Common.Object
// export const EventTarget = Common.EventTarget

export default {
  Object: Common.Object,
  EventTarget: Common.EventTarget
}
