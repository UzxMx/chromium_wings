function dumpAllProperties (o) {
  var objectToInspect
  var result = []
  
  for (objectToInspect = o; objectToInspect !== null; objectToInspect = Object.getPrototypeOf(objectToInspect)) {
    result = result.concat(Object.getOwnPropertyNames(objectToInspect))
  }

  console.log(result)
}

export default {
  dumpAllProperties
}
