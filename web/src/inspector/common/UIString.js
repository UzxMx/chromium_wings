const Common = {}

/**
 * @param {string} string
 * @param {...*} vararg
 * @return {string}
 */
Common.UIString = function(string, vararg) {
  return string;
};

module.exports = {
  UIString: Common.UIString
}
