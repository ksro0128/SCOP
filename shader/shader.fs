#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 RGB;
in vec2 TexCoords;

uniform sampler2D mainTexture;
uniform bool textureEnabled;
uniform float textureMix;

void main() {
    vec3 tColor = texture(mainTexture, TexCoords).rgb;
    // vec3 color = textureEnabled ? tColor : vec3(0.0, 0.0, 0.0);
    // vec3 color = textureEnabled ? tColor : RGB;
    vec3 color = mix(RGB, tColor, textureMix);
    
    FragColor = vec4(color, 1.0);
}
