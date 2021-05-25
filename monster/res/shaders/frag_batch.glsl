#version 420

in vec2 tex;
in vec4 col;
out vec4 o_color;

uniform sampler2D image;

void main()
{
    // TODO(ck): Ignoring set color for now
    vec4 color = texture(image, tex);
    o_color = color;
}