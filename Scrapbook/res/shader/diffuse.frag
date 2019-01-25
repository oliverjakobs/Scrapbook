#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  

uniform sampler2D tex;

uniform vec3 lightPos; 
uniform vec4 lightColor;

uniform vec3 viewPos;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec4 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.5f;
	int shininess = 32;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
	vec4 specular = specularStrength * spec * lightColor;

    FragColor = (ambient + diffuse + specular) * texture(tex, TexCoord);
} 