#version 330 core
 out vec4 FragColor;
 uniform vec3 fColor;
 void main() {
            FragColor = vec4(fColor, 1.0); // Red color
        }
    