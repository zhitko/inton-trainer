import * as d3 from "d3";
import _ from "lodash";
import Base from "./base";

export default {
  draw: function (data, svgTag, tag, opts) {
    console.warn("octaves.js draw");
    console.warn("octaves.js data", data);

    let options = _.assignIn({}, Base.options, opts);

    var yScale = d3.scaleLinear()
      .domain([0, data.range])
      .range([0, options.height]);

    var svg = d3.select(svgTag)
      .call(Base.responsivefy)
      .append("g")
      .attr("class", tag)
      .attr("transform", `translate(${options.marginLeft},${options.marginTop})`)
        .append("rect")
        .attr("x", function (d) { return 0; })
        .attr("y", function (d) { return yScale(data.range - data.value); })
        .attr("width", function (d) { return options.width; })
        .attr("height", function (d) { return yScale(data.value); })
        .attr("fill-opacity", options.fillOpacity)
        .attr("fill", options.fill);
  },
  legend: function (svgTag, tag, opts) {
    let options = _.assignIn({}, Base.legendOptions, opts);
    let svg = d3.select(svgTag)
      .call(Base.responsivefy)
      .append("g")
      .attr("class", `${tag}-legend`)
      .attr("transform", `translate(${options.marginLeft},${options.marginTop})`)
      .append("rect")
      .attr("x", function (d) { return 0; })
      .attr("y", function (d) { return 0; })
      .attr("width", function (d) { return options.width; })
      .attr("height", function (d) { return options.height; })
      .attr("fill-opacity", options.fillOpacity)
      .attr("fill", options.fill)
      .attr("stroke", options.stroke)
      .attr("stroke-width", options.strokeWidth)
      .select(function() { return this.parentNode; })
      .append("text")
      .attr("dx", function(d){return options.width + 5})
      .attr("dy", function(d){return 12})
      .text(function(d){return (options.text || tag)})
  },
  clean: Base.clean
}
