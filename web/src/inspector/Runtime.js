var Runtime = class {
  /**
   * @param {!Array.<!Runtime.ModuleDescriptor>} descriptors
   */
  constructor(descriptors) {
    /** @type {!Array<!Runtime.Module>} */
    this._modules = [];
    /** @type {!Object<string, !Runtime.Module>} */
    this._modulesMap = {};
    /** @type {!Array<!Runtime.Extension>} */
    this._extensions = [];
    /** @type {!Object<string, !function(new:Object)>} */
    this._cachedTypeClasses = {};
    /** @type {!Object<string, !Runtime.ModuleDescriptor>} */
    this._descriptorsMap = {};

    for (var i = 0; i < descriptors.length; ++i)
      this._registerModule(descriptors[i]);
  }

  /**
   * @param {string} url
   * @return {!Promise.<string>}
   */
  static loadResourcePromise(url) {
    return new Promise(load);

    /**
     * @param {function(?)} fulfill
     * @param {function(*)} reject
     */
    function load(fulfill, reject) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', url, true);
      xhr.onreadystatechange = onreadystatechange;

      /**
       * @param {Event} e
       */
      function onreadystatechange(e) {
        if (xhr.readyState !== XMLHttpRequest.DONE)
          return;

        if ([0, 200, 304].indexOf(xhr.status) === -1)  // Testing harness file:/// results in 0.
          reject(new Error('While loading from url ' + url + ' server responded with a status of ' + xhr.status));
        else
          fulfill(e.target.response);
      }
      xhr.send(null);
    }
  }

  /**
   * http://tools.ietf.org/html/rfc3986#section-5.2.4
   * @param {string} path
   * @return {string}
   */
  static normalizePath(path) {
    if (path.indexOf('..') === -1 && path.indexOf('.') === -1)
      return path;

    var normalizedSegments = [];
    var segments = path.split('/');
    for (var i = 0; i < segments.length; i++) {
      var segment = segments[i];
      if (segment === '.')
        continue;
      else if (segment === '..')
        normalizedSegments.pop();
      else if (segment)
        normalizedSegments.push(segment);
    }
    var normalizedPath = normalizedSegments.join('/');
    if (normalizedPath[normalizedPath.length - 1] === '/')
      return normalizedPath;
    if (path[0] === '/' && normalizedPath)
      normalizedPath = '/' + normalizedPath;
    if ((path[path.length - 1] === '/') || (segments[segments.length - 1] === '.') ||
        (segments[segments.length - 1] === '..'))
      normalizedPath = normalizedPath + '/';

    return normalizedPath;
  }

  /**
   * @param {!Array.<string>} scriptNames
   * @param {string=} base
   * @return {!Promise.<undefined>}
   */
  static _loadScriptsPromise(scriptNames, base) {
    /** @type {!Array<!Promise<undefined>>} */
    var promises = [];
    /** @type {!Array<string>} */
    var urls = [];
    var sources = new Array(scriptNames.length);
    var scriptToEval = 0;
    for (var i = 0; i < scriptNames.length; ++i) {
      var scriptName = scriptNames[i];
      var sourceURL = (base || self._importScriptPathPrefix) + scriptName;

      var schemaIndex = sourceURL.indexOf('://') + 3;
      var pathIndex = sourceURL.indexOf('/', schemaIndex);
      if (pathIndex === -1)
        pathIndex = sourceURL.length;
      sourceURL = sourceURL.substring(0, pathIndex) + Runtime.normalizePath(sourceURL.substring(pathIndex));

      if (_loadedScripts[sourceURL])
        continue;
      urls.push(sourceURL);
      promises.push(Runtime.loadResourcePromise(sourceURL).then(
          scriptSourceLoaded.bind(null, i), scriptSourceLoaded.bind(null, i, undefined)));
    }
    return Promise.all(promises).then(undefined);

    /**
     * @param {number} scriptNumber
     * @param {string=} scriptSource
     */
    function scriptSourceLoaded(scriptNumber, scriptSource) {
      sources[scriptNumber] = scriptSource || '';
      // Eval scripts as fast as possible.
      while (typeof sources[scriptToEval] !== 'undefined') {
        evaluateScript(urls[scriptToEval], sources[scriptToEval]);
        ++scriptToEval;
      }
    }

    /**
     * @param {string} sourceURL
     * @param {string=} scriptSource
     */
    function evaluateScript(sourceURL, scriptSource) {
      _loadedScripts[sourceURL] = true;
      if (!scriptSource) {
        // Do not reject, as this is normal in the hosted mode.
        console.error('Empty response arrived for script \'' + sourceURL + '\'');
        return;
      }
      self.eval(scriptSource + '\n//# sourceURL=' + sourceURL);
    }
  }

  /**
   * @param {string} url
   * @param {boolean} appendSourceURL
   * @return {!Promise<undefined>}
   */
  static _loadResourceIntoCache(url, appendSourceURL) {
    return Runtime.loadResourcePromise(url).then(
        cacheResource.bind(this, url), cacheResource.bind(this, url, undefined));

    /**
     * @param {string} path
     * @param {string=} content
     */
    function cacheResource(path, content) {
      if (!content) {
        console.error('Failed to load resource: ' + path);
        return;
      }
      var sourceURL = appendSourceURL ? Runtime.resolveSourceURL(path) : '';
      Runtime.cachedResources[path] = content + sourceURL;
    }
  }

  /**
   * @param {string} appName
   * @return {!Promise.<undefined>}
   */
  static startApplication(appName) {
    console.timeStamp('Runtime.startApplication');

    var allDescriptorsByName = {};
    for (var i = 0; i < allDescriptors.length; ++i) {
      var d = allDescriptors[i];
      allDescriptorsByName[d['name']] = d;
    }

    var applicationPromise;
    if (applicationDescriptor)
      applicationPromise = Promise.resolve(applicationDescriptor);
    else
      applicationPromise = Runtime.loadResourcePromise(appName + '.json').then(JSON.parse.bind(JSON));

    return applicationPromise.then(parseModuleDescriptors);

    /**
     * @param {!{modules: !Array.<!Object>, has_html: boolean}} appDescriptor
     * @return {!Promise.<undefined>}
     */
    function parseModuleDescriptors(appDescriptor) {
      var configuration = appDescriptor.modules;
      var moduleJSONPromises = [];
      var coreModuleNames = [];
      for (var i = 0; i < configuration.length; ++i) {
        var descriptor = configuration[i];
        var name = descriptor['name'];
        var moduleJSON = allDescriptorsByName[name];
        if (moduleJSON)
          moduleJSONPromises.push(Promise.resolve(moduleJSON));
        else
          moduleJSONPromises.push(Runtime.loadResourcePromise(name + '/module.json').then(JSON.parse.bind(JSON)));
        if (descriptor['type'] === 'autostart')
          coreModuleNames.push(name);
      }

      return Promise.all(moduleJSONPromises).then(instantiateRuntime);

      /**
       * @param {!Array.<!Object>} moduleDescriptors
       * @return {!Promise.<undefined>}
       */
      function instantiateRuntime(moduleDescriptors) {
        for (var i = 0; i < moduleDescriptors.length; ++i) {
          moduleDescriptors[i].name = configuration[i]['name'];
          moduleDescriptors[i].condition = configuration[i]['condition'];
          moduleDescriptors[i].remote = configuration[i]['type'] === 'remote';
        }
        self.runtime = new Runtime(moduleDescriptors);
        if (coreModuleNames)
          return /** @type {!Promise<undefined>} */ (self.runtime._loadAutoStartModules(coreModuleNames));
        return Promise.resolve();
      }
    }
  }

  /**
   * @param {string} appName
   * @return {!Promise.<undefined>}
   */
  static startWorker(appName) {
    return Runtime.startApplication(appName).then(sendWorkerReady);

    function sendWorkerReady() {
      self.postMessage('workerReady');
    }
  }

  /**
   * @param {string} name
   * @return {?string}
   */
  static queryParam(name) {
    return Runtime._queryParamsObject[name] || null;
  }

  /**
   * @return {string}
   */
  static queryParamsString() {
    return location.search;
  }

  /**
   * @return {!Object}
   */
  static _experimentsSetting() {
    try {
      return /** @type {!Object} */ (
          JSON.parse(self.localStorage && self.localStorage['experiments'] ? self.localStorage['experiments'] : '{}'));
    } catch (e) {
      console.error('Failed to parse localStorage[\'experiments\']');
      return {};
    }
  }

  static _assert(value, message) {
    if (value)
      return;
    Runtime._originalAssert.call(Runtime._console, value, message + ' ' + new Error().stack);
  }

  /**
   * @param {string} platform
   */
  static setPlatform(platform) {
    Runtime._platform = platform;
  }

  /**
   * @param {!Object} descriptor
   * @return {boolean}
   */
  static _isDescriptorEnabled(descriptor) {
    var activatorExperiment = descriptor['experiment'];
    if (activatorExperiment === '*')
      return Runtime.experiments.supportEnabled();
    if (activatorExperiment && activatorExperiment.startsWith('!') &&
        Runtime.experiments.isEnabled(activatorExperiment.substring(1)))
      return false;
    if (activatorExperiment && !activatorExperiment.startsWith('!') &&
        !Runtime.experiments.isEnabled(activatorExperiment))
      return false;
    var condition = descriptor['condition'];
    if (condition && !condition.startsWith('!') && !Runtime.queryParam(condition))
      return false;
    if (condition && condition.startsWith('!') && Runtime.queryParam(condition.substring(1)))
      return false;
    return true;
  }

  /**
   * @param {string} path
   * @return {string}
   */
  static resolveSourceURL(path) {
    var sourceURL = self.location.href;
    if (self.location.search)
      sourceURL = sourceURL.replace(self.location.search, '');
    sourceURL = sourceURL.substring(0, sourceURL.lastIndexOf('/') + 1) + path;
    return '\n/*# sourceURL=' + sourceURL + ' */';
  }

  useTestBase() {
    Runtime._remoteBase = 'http://localhost:8000/inspector-sources/';
    if (Runtime.queryParam('debugFrontend'))
      Runtime._remoteBase += 'debug/';
  }

  /**
   * @param {!Runtime.ModuleDescriptor} descriptor
   */
  _registerModule(descriptor) {
    var module = new Runtime.Module(this, descriptor);
    this._modules.push(module);
    this._modulesMap[descriptor['name']] = module;
  }

  /**
   * @param {string} moduleName
   * @return {!Promise.<undefined>}
   */
  loadModulePromise(moduleName) {
    return this._modulesMap[moduleName]._loadPromise();
  }

  /**
   * @param {!Array.<string>} moduleNames
   * @return {!Promise.<!Array.<*>>}
   */
  _loadAutoStartModules(moduleNames) {
    var promises = [];
    for (var i = 0; i < moduleNames.length; ++i)
      promises.push(this.loadModulePromise(moduleNames[i]));
    return Promise.all(promises);
  }

  /**
   * @param {!Runtime.Extension} extension
   * @param {?function(function(new:Object)):boolean} predicate
   * @return {boolean}
   */
  _checkExtensionApplicability(extension, predicate) {
    if (!predicate)
      return false;
    var contextTypes = extension.descriptor().contextTypes;
    if (!contextTypes)
      return true;
    for (var i = 0; i < contextTypes.length; ++i) {
      var contextType = this._resolve(contextTypes[i]);
      var isMatching = !!contextType && predicate(contextType);
      if (isMatching)
        return true;
    }
    return false;
  }

  /**
   * @param {!Runtime.Extension} extension
   * @param {?Object} context
   * @return {boolean}
   */
  isExtensionApplicableToContext(extension, context) {
    if (!context)
      return true;
    return this._checkExtensionApplicability(extension, isInstanceOf);

    /**
     * @param {!Function} targetType
     * @return {boolean}
     */
    function isInstanceOf(targetType) {
      return context instanceof targetType;
    }
  }

  /**
   * @param {!Runtime.Extension} extension
   * @param {!Set.<!Function>=} currentContextTypes
   * @return {boolean}
   */
  isExtensionApplicableToContextTypes(extension, currentContextTypes) {
    if (!extension.descriptor().contextTypes)
      return true;

    return this._checkExtensionApplicability(extension, currentContextTypes ? isContextTypeKnown : null);

    /**
     * @param {!Function} targetType
     * @return {boolean}
     */
    function isContextTypeKnown(targetType) {
      return currentContextTypes.has(targetType);
    }
  }

  /**
   * @param {*} type
   * @param {?Object=} context
   * @param {boolean=} sortByTitle
   * @return {!Array.<!Runtime.Extension>}
   */
  extensions(type, context, sortByTitle) {
    return this._extensions.filter(filter).sort(sortByTitle ? titleComparator : orderComparator);

    /**
     * @param {!Runtime.Extension} extension
     * @return {boolean}
     */
    function filter(extension) {
      if (extension._type !== type && extension._typeClass() !== type)
        return false;
      if (!extension.enabled())
        return false;
      return !context || extension.isApplicable(context);
    }

    /**
     * @param {!Runtime.Extension} extension1
     * @param {!Runtime.Extension} extension2
     * @return {number}
     */
    function orderComparator(extension1, extension2) {
      var order1 = extension1.descriptor()['order'] || 0;
      var order2 = extension2.descriptor()['order'] || 0;
      return order1 - order2;
    }

    /**
     * @param {!Runtime.Extension} extension1
     * @param {!Runtime.Extension} extension2
     * @return {number}
     */
    function titleComparator(extension1, extension2) {
      var title1 = extension1.title() || '';
      var title2 = extension2.title() || '';
      return title1.localeCompare(title2);
    }
  }

  /**
   * @param {*} type
   * @param {?Object=} context
   * @return {?Runtime.Extension}
   */
  extension(type, context) {
    return this.extensions(type, context)[0] || null;
  }

  /**
   * @param {*} type
   * @param {?Object=} context
   * @return {!Promise.<!Array.<!Object>>}
   */
  allInstances(type, context) {
    return Promise.all(this.extensions(type, context).map(extension => extension.instance()));
  }

  /**
   * @return {?function(new:Object)}
   */
  _resolve(typeName) {
    if (!this._cachedTypeClasses[typeName]) {
      var path = typeName.split('.');
      var object = self;
      for (var i = 0; object && (i < path.length); ++i)
        object = object[path[i]];
      if (object)
        this._cachedTypeClasses[typeName] = /** @type function(new:Object) */ (object);
    }
    return this._cachedTypeClasses[typeName] || null;
  }

  /**
   * @param {!Function} constructorFunction
   * @return {!Object}
   */
  sharedInstance(constructorFunction) {
    if (Runtime._instanceSymbol in constructorFunction)
      return constructorFunction[Runtime._instanceSymbol];
    var instance = new constructorFunction();
    constructorFunction[Runtime._instanceSymbol] = instance;
    return instance;
  }
};

/**
 * @type {!Object.<string, string>}
 */
Runtime._queryParamsObject = {
  __proto__: null
};

{
  (function parseQueryParameters() {
    var queryParams = Runtime.queryParamsString();
    if (!queryParams)
      return;
    var params = queryParams.substring(1).split('&');
    for (var i = 0; i < params.length; ++i) {
      var pair = params[i].split('=');
      var name = pair.shift();
      Runtime._queryParamsObject[name] = pair.join('=');
    }
  })();
}

export default Runtime