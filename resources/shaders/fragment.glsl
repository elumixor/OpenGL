#version 400 core

const int MAX_POINT_LIGHTS = 2;
const int MAX_DIRECTIONAL_LIGHTS = 2;

in vec2 UV;
in vec3 model_normal;
in vec3 model_position;

out vec4 color;

struct Attenuation {
        float constant;
        float linear;
        float exponential;
    };

struct BaseLight {
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
};

struct Directional {
    BaseLight base;

    vec3 direction;
};

struct Point {
    BaseLight base;
    Attenuation attenuation;
    vec3 position;

};

uniform int point_lights_count;
uniform Point[MAX_POINT_LIGHTS] point_lights;

uniform int directional_lights_count;
uniform Directional[MAX_DIRECTIONAL_LIGHTS] directional_lights;

uniform sampler2D texture_sampler;

uniform vec3 camera_position;
uniform float specular_intensity;
uniform float specular_power;
uniform bool grid_enabled;

vec3 calculateDirectionalLight(BaseLight light, vec3 direction, vec3 normal) {
    // calculate ambient property
    vec3 ambientColor = vec3(light.color * light.ambient_intensity);

    float diffuseFactor = dot(normal, -direction);

    vec3 diffuseColor, specularColor = diffuseColor = vec3(0f);

    if (diffuseFactor > 0) {
        // calculate diffuse
        diffuseColor = light.color * light.diffuse_intensity * diffuseFactor;

        // calculate specular
        vec3 vertextToEye = normalize(camera_position - model_position);
        vec3 lightReflect = normalize(reflect(direction, normal));
        float specularFactor = dot(vertextToEye, lightReflect);

        if (specularFactor > 0) {
            specularFactor = pow(specularFactor, specular_power);
            specularColor = light.color * specular_intensity * specularFactor;
        }
    }

    return (ambientColor + diffuseColor + specularColor);
}

vec3 calculatePointLight(Point light, vec3 normal) {
    vec3 direction = model_position - light.position;
    float distance = length(direction);

    direction = normalize(direction);
    float attenuation = light.attenuation.constant
                     + light.attenuation.linear * distance
                     + light.attenuation.exponential * distance * distance;

    return calculateDirectionalLight(light.base, direction, normal) / attenuation;
}

void main(){
    if (grid_enabled) {
        color = vec4(1);
    } else {
        vec3 normal = normalize(model_normal);

        vec3 totalLight = vec3(0);

        int count;
        count = min(directional_lights_count, MAX_DIRECTIONAL_LIGHTS);

        for (int i = 0; i < count; i++) {
            totalLight += calculateDirectionalLight(directional_lights[i].base, directional_lights[i].direction, normal);
        }

        count = min(point_lights_count, MAX_POINT_LIGHTS);

        for (int i = 0; i < count; i++) {
            totalLight += calculatePointLight(point_lights[i], normal);
        }

        color = texture2D(texture_sampler, UV.xy) * vec4(totalLight, 1.f);
    }
}
