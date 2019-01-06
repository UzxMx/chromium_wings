function foo () {
  return new Promise(resolve => {
    resolve(30)
  })
}

async function resolveAfter2Seconds (x) {
  var b = await foo()
  return new Promise(resolve => {
    setTimeout(() => {
      resolve(b)
    }, 2000)
  })
}

async function bar () {
  var x = await resolveAfter2Seconds(10);
  console.log(x)
}

export default {
  bar
}
