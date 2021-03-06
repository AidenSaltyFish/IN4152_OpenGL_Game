#version 450

// uniform sampler2D colorMap;
uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

// uniform bool tintOn;
// uniform bool isSun;
uniform bool hasTexCoords;
// uniform bool turboModeOn;

uniform vec3 viewPos;

uniform struct Light {
    vec3 position;
    vec3 diffuseColor;
    vec3 ambientColor;
    vec3 specularColor;
    mat4 projectionMatrix;
    mat4 viewMatrix;
} light;

in struct Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
} material;

in vec3 passPosition;
in vec3 passNormal;
in vec2 passTexCoord;
in vec4 passShadowCoord;
in vec3 passColor;

out vec4 fragColor;

float getShadowMultiplier(in vec4 fragLightCoord, in bool spotlight) {

    float percentageShadow = 0.f;

    fragLightCoord.xyz /= fragLightCoord.w; // In NDC (-1, 1)
    fragLightCoord.xyz = fragLightCoord.xyz * 0.5 + 0.5; // In texture coordinates
    float fragLightDepth = fragLightCoord.z;
    vec2 shadowMapCoord = fragLightCoord.xy;
    float shadowMapDepth = texture(shadowMap, shadowMapCoord).x;

    // Data structure for the percentage close filtering
    float offset = 0.001; // adjust better
    vec2 ul = vec2(shadowMapCoord.x - offset, shadowMapCoord.y + offset);
    vec2 ml = vec2(shadowMapCoord.x - offset, shadowMapCoord.y);
    vec2 bl = vec2(shadowMapCoord.x - offset, shadowMapCoord.y - offset);
    vec2 u = vec2(shadowMapCoord.x, shadowMapCoord.y + offset);
    vec2 m = vec2(shadowMapCoord.x, shadowMapCoord.y);
    vec2 b = vec2(shadowMapCoord.x, shadowMapCoord.y - offset);
    vec2 ur = vec2(shadowMapCoord.x + offset, shadowMapCoord.y + offset);
    vec2 mr = vec2(shadowMapCoord.x + offset, shadowMapCoord.y);
    vec2 br = vec2(shadowMapCoord.x + offset, shadowMapCoord.y - offset);
    mat3 values = mat3(texture(shadowMap, ul).x, texture(shadowMap, ml).x, texture(shadowMap, bl).x,
        texture(shadowMap, u).x, texture(shadowMap, m).x, texture(shadowMap, b).x,
        texture(shadowMap, ur).x, texture(shadowMap, mr).x, texture(shadowMap, br).x);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (values[i][j] < fragLightDepth - 0.00001) { // is in shadow
                percentageShadow++;
            }
        }
    }

    percentageShadow /= 9.0f;

    return percentageShadow;
}

// Implementation of ColorBlinnPhong and Toon shading
vec3 getShading(Light light, vec3 lightDir, vec3 normal, vec3 diffuseToUse) {
    float distToLight = length(lightDir);
    distToLight = distToLight * distToLight;
    lightDir = normalize(lightDir);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.ambientColor;
    float diffuse = max(dot(lightDir, normal), 0.0);
    vec3 specular = vec3(0.f);

    if (diffuse > 0.0) {
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - passPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular = spec * specularStrength * light.specularColor;
    }

    // Toon shading
    //if (turboModeOn) {
    //    if (diffuse > 0 && diffuse < 0.25) {
    //        diffuse = 0.25;
    //    }
    //    else if (diffuse >= 0.25 && diffuse < 0.5) {
    //        diffuse = 0.5;
    //    }
    //    else if (diffuse >= 0.5 && diffuse < 0.75) {
    //        diffuse = 0.75;
    //    }
    //    else if (diffuse >= 0.75) {
    //        diffuse = 1;
    //    }
    //}

    vec3 result = material.ambientColor * ambient + diffuseToUse * diffuse + material.specularColor * specular;

    return result;
}


void main()
{
    // For shadows
    vec4 fragLightCoord = light.projectionMatrix * light.viewMatrix * vec4(passPosition, 1.0);
    float percentageShadow = getShadowMultiplier(fragLightCoord, false);

    vec3 normal = normalize(passNormal);

    vec3 lightDir = light.position - passPosition;
    vec3 texDiffuse, finalColor;

    if (hasTexCoords) {
        //if (isSun) {
        //    percentageShadow = 0;
        //    normal = -normal;
        //}

        texDiffuse = texture(texture_diffuse1, passTexCoord).rgb;
        finalColor = getShading(light, lightDir, normal, texDiffuse);

    }
    else {
        finalColor = getShading(light, lightDir, normal, material.diffuseColor);
    }

    //if(tintOn) fragColor = vec4(1.f, 0.f, 0.f, 1.0);
    //else
    fragColor = vec4(finalColor * (1 - percentageShadow), 1.0);
}