#version 330 core
out vec4 FragColor;

in vec3 ourColor;

void main() {
    FragColor = vec3(ourColor, 1.0f);
}
