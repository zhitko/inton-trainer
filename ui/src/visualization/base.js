import * as d3 from "d3";

export default {
  options: {
    marginTop: 0, 
    marginRight: 0, 
    marginBottom: 0, 
    marginLeft: 0,
    width: window.innerWidth,
    height: window.innerHeight,
    min: 0,
    max: 1,
    stroke: "black",
    strokeWidth: "1",
    fill: "none"
  },
  legendOptions: {
    marginTop: 0, 
    marginRight: 0, 
    marginBottom: 0, 
    marginLeft: 0,
    width: 15,
    height: 15,
    stroke: "black",
    strokeWidth: "1",
    fill: "none",
    text: ''
  },
  responsivefy: function(svg) {
    var container = d3.select(svg.node().parentNode),
        width = parseInt(svg.style("width")),
        height = parseInt(svg.style("height")),
        aspect = width / height;

    svg.attr("viewBox", "0 0 " + width + " " + height)
        .attr("perserveAspectRatio", "xMinYMid")
        .call(resize);

    d3.select(window).on("resize." + container.attr("id"), resize);

    function resize() {
        var targetWidth = parseInt(container.style("width"));
        svg.attr("width", targetWidth);
        svg.attr("height", Math.round(targetWidth / aspect));
    }
  },
  clean: function (svgTag, tag) {
    d3.select(svgTag)
      .select(`.${tag}`)
      .remove();
    d3.select(svgTag)
      .select(`.${tag}-legend`)
      .remove();
  }
}
