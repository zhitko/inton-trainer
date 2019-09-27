import * as d3 from "d3";
import _ from "lodash";
import Base from "./base";

export default {
  draw: function (data, svgTag, tag, opts) {
    console.warn("wave.js draw");

    let options = _.assignIn({}, Base.options, opts);

    var xScale = d3.scaleLinear()
      .domain([0, data.length])
      .range([0, options.width]); 

    var yScale = d3.scaleLinear()
      .domain([options.min, options.max])
      .range([0, options.height]);

    var line = d3.line()
      .x((d, i) => xScale(i))
      .y((d) => yScale(d)) 
      .curve(d3.curveMonotoneX)

    var svg = d3.select(svgTag)
      .call(Base.responsivefy)
      .append("g")
      .attr("class", tag)
      .attr("transform", `translate(${options.marginLeft},${options.marginTop})`)
      .append("path")
      .datum(data)
      .attr("class", "line")
      .attr("stroke", options.stroke)
      .attr("stroke-width", options.strokeWidth)
      .attr("d", line);
  },
  clean: Base.clean
}
