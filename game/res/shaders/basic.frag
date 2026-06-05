#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture0;

void main()
{
    vec3 lightDirection = normalize(vec3(0.4, 1.0, 0.2));
    vec3 faceDirection = normalize(Normal);

    float diffuse = max(dot(faceDirection, lightDirection), 0.0);

    float ambient = 0.25f;

    float lighting = min(ambient + (diffuse * 0.75), 1.0);

    vec4 textureSample = texture(texture0, TexCoord);

    FragColor = vec4(textureSample.rgb * lighting, textureSample.a);
}