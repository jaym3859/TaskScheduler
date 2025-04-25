#shader vertex
#version 450 core

layout(location=0) in vec3 a_Position;  // Vertex position
layout(location=1) in vec4 a_Color;     // Vertex color

uniform mat4 u_ViewProj;  // View-Projection matrix
uniform mat4 u_Transform; // Transformation matrix

out vec4 v_Color;  // Output color to pass to the geometry shader

void main() {
    v_Color = a_Color;  // Pass vertex color to the geometry shader
    gl_Position = u_ViewProj * u_Transform * vec4(a_Position, 1.0);
};

#shader geometry
#version 450 core

layout(triangles) in;  // Input is triangles
layout(line_strip, max_vertices = 6) out;  // Output will be lines (for outline)

in vec4 v_Color[];  // Color input from vertex shader (3 vertices per triangle)

out vec4 g_Color;  // Output color to fragment shader

void main() {
    g_Color = v_Color[0];  // Set the outline color to the color of the first vertex

    // Generate lines for each edge of the triangle
    for (int i = 0; i < 3; ++i) {
        int next = (i + 1) % 3;
        
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();  // Emit the current vertex
        gl_Position = gl_in[next].gl_Position;
        EmitVertex();  // Emit the next vertex
        EndPrimitive();  // Finish the primitive (the line)
    }
}

#shader fragment
#version 450 core

layout(location=0) out vec4 o_Color;  // Output color

in vec4 g_Color;  // Color input from geometry shader

void main() {
    o_Color = g_Color;  // Set the output color to the passed color from the geometry shader
};

