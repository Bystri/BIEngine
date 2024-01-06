#version 330 core
uniform vec2 resolution;
uniform float antialias;

attribute float thickness;
attribute vec2 p0, p1, uv;

varying float v_alpha, v_thickness;
varying vec2 v_p0, v_p1, v_p;

void main() {

  if( abs(thickness) < 1.0 ) {
	 v_thickness = 1.0;
	 v_alpha = abs(thickness);
  } else {
	 v_thickness = abs(thickness);
	 v_alpha = 1.0;
  } 

  float t = v_thickness/2.0 + antialias;
  float l = length(p1-p0);
  float u = 2.0*uv.x - 1.0;
  float v = 2.0*uv.y - 1.0;

  // Screen space
  vec2 T = normalize(p1-p0);
  vec2 O = vec2(-T.y , T.x);
  vec2 p = p0 + vec2(0.5,0.5) + uv.x*T*l + u*T*t + v*O*t;
  gl_Position = vec4(2.0*p/resolution-1.0, 0.0, 1.0);

  // Local space
  T = vec2(1.0, 0.0);
  O = vec2(0.0, 1.0);
  p = uv.x*T*l + u*T*t + v*O*t;

  v_p0 = vec2(0.0, 0.0);
  v_p1 = vec2(  l, 0.0);
  v_p  = p;
}