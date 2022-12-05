#version 330 core 

uniform bool sineTaylorEnabled;
uniform bool expTaylorEnabled;

uniform int sineTaylorIterations;
uniform int expTaylorIterations;

uniform float graphScale;
uniform float aspectRatio;

in vec2 texcoord;

out vec4 fragColor;

void drawGraph(inout vec4 color, vec2 coord, float yValue, const vec4 graphColor) {
    color += 
        abs(coord.y - yValue) < 0.02 ? 
        graphColor : 
        vec4(0.0);
}

int fact(int x){
    int factOut = 1;

    for (int i = 1; i <= x; i++) {
        factOut = factOut * i;
    }

    return factOut;
}

float sineTaylorApproximation(float x) {
    float y = 0.0;
    int sign = 1;
    for(int i = 0; i < sineTaylorIterations; i++) {
        y += sign / float(fact(2 * i + 1)) * pow(x, 2.0 * i + 1.0);
        sign *= -1;
    }
    return x - pow(x, 3) / float(fact(3)) + pow(x, 5) / float(fact(5));
} 

float derivative(float x) {
    const float dx = 0.1;
    return (sin(x + dx) + sin(x)) / dx; 
}

void main() {
    vec2 graphCoord = (texcoord - 0.5) * graphScale;
    graphCoord.x *= aspectRatio;

    drawGraph(fragColor, graphCoord, sineTaylorApproximation(graphCoord.x), vec4(1.0, 0.0, 0.0, 0.0));
    drawGraph(fragColor, graphCoord, sin(graphCoord.x), vec4(0.0, 0.4039, 0.8627, 0.0));
}