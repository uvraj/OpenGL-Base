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
        abs(coord.y - yValue) < 0.01 ? 
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

float sineTaylorApproxmation(float x) {
    float y;
    for(int i = 0; i < 10; i++) {
        y += (pow(-1.0, i) / float(fact(2 * i + 1))) * pow(x, 2.0 * i + 1.0);
    }
    return y;
} 

float derivative(float x) {
    const float dx = 0.1;
    return (sin(x + dx) + sin(x)) / dx; 
}

void main() {
    vec2 graphCoord = (texcoord - 0.5) * graphScale;
    graphCoord.x *= aspectRatio;

    drawGraph(fragColor, graphCoord, sineTaylorApproxmation(graphCoord.x), vec4(1.0, 0.0, 0.0, 0.0));
    drawGraph(fragColor, graphCoord, sin(graphCoord.x), vec4(1.0, 0.0, 0.0, 0.0));
}