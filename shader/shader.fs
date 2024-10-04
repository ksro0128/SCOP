#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 RGB;
in vec2 TexCoords;

uniform sampler2D mainTexture;
uniform float textureMix;

void main() {
    vec3 tColor = texture(mainTexture, TexCoords).rgb;
    vec3 color = mix(RGB, tColor, textureMix);
    
    FragColor = vec4(color, 1.0);
}
