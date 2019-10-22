import * as d3 from "d3";
import _ from "lodash";
import Base from "./base";

export default {
  draw: function (data, svgTag, tag, opts) {
    console.warn("ump_mask.js draw");
    console.warn("ump_mask.js data", data);

    let options = _.assignIn({}, Base.options, opts);

    var xScale = d3.scaleLinear()
      .domain([0, data.length])
      .range([0, options.width]); 

    var yScale = d3.scaleLinear()
      .domain([options.min, options.max])
      .range([0, options.height]);

    var svg = d3.select(svgTag)
      .call(Base.responsivefy)
      .append("g")
      .attr("class", tag)
      .attr("transform", `translate(${options.marginLeft},${options.marginTop})`)
      .selectAll("rect")
      .data(data.data)
      .enter()
        .append("rect")
        .attr("x", function (d) { return xScale(d.position); })
        .attr("y", function (d) { return yScale(0); })
        .attr("width", function (d) { return xScale(d.length); })
        .attr("height", function (d) { return yScale(1); })
        .attr("fill-opacity", options.fillOpacity)
        .attr("fill", options.fill)
      .select(function() { return this.parentNode; })
      .selectAll("line")
      .data(data.data)
      .enter()
        .append("line")
        .attr("x1", function (d) { return xScale(d.position); })
        .attr("y1", function (d) { return 0; })
        .attr("x2", function (d) { return xScale(d.position); })
        .attr("y2", function (d) { return options.height; })
        .attr("stroke", options.stroke)
        .attr("stroke-width", options.strokeWidth);
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
