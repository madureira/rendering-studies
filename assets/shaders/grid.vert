// ============================================================================
// INFINITE GRID - VERTEX SHADER
// ============================================================================
//
// This shader is part of an infinite grid rendering technique. Instead of
// drawing actual grid geometry, we render a fullscreen quad and use ray-casting
// in the fragment shader to determine where each pixel intersects the ground
// plane (Y=0).
//
// The vertex shader's job is to:
// 1. Pass through the fullscreen quad vertices
// 2. "Unproject" the screen corners back to world space, creating rays that
//    the fragment shader will use for intersection testing
//
// ============================================================================

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

// Input: Fullscreen quad vertices in NDC space (range -1 to +1)
// The quad covers the entire screen: (-1,-1), (-1,+1), (+1,+1), (+1,-1)
layout(location = 0) in vec3 a_Vertex;

// ============================================================================
// UNIFORMS
// ============================================================================

uniform mat4 u_View;          // View matrix (camera transform)
uniform mat4 u_Projection;    // Projection matrix
uniform mat4 u_ViewInv;       // Inverse view matrix (world from camera)
uniform mat4 u_ProjectionInv; // Inverse projection matrix (view from clip)
uniform vec3 u_CameraPos;     // Camera position in relative coordinates

// ============================================================================
// OUTPUTS TO FRAGMENT SHADER
// ============================================================================

// IMPORTANT: We output homogeneous coordinates (vec4) instead of dividing by w
// here. This is crucial for correct perspective interpolation.
//
// If we divided by w in the vertex shader:
//   vec3 near = (ViewInv * ProjInv * clipPos).xyz / w;  // WRONG
//
// The hardware would then linearly interpolate these vec3 values across the
// triangle. But perspective projection is NON-LINEAR, so this linear
// interpolation produces incorrect ray directions, causing visual artifacts
// (shimmer, jitter) especially when the camera moves.
//
// By passing vec4 (homogeneous coordinates), the hardware interpolates all
// four components, and we perform the w-divide in the fragment shader where
// we have the correctly interpolated values for each pixel.
out vec4 v_NearH;     // Near plane point in world space (homogeneous)
out vec4 v_FarH;      // Far plane point in world space (homogeneous)
out vec3 v_CameraPos; // Camera position (passed through to fragment)

// ============================================================================
// UNPROJECT FUNCTION
// ============================================================================

// Transforms a point from Normalized Device Coordinates (NDC) back to
// world space. Returns homogeneous coordinates (don't divide by w yet).
//
// The transformation chain is:
//   World -> View -> Clip -> NDC (normal rendering)
//   NDC -> Clip -> View -> World (unprojection, what we do here)
//
// Parameters:
//   x, y: Screen position in NDC (-1 to +1)
//   z: Depth in NDC (-1 = near plane, +1 = far plane in OpenGL)
//
vec4 unprojectPointH(float x, float y, float z)
{
    // Note: OpenGL uses z range [-1, +1] for NDC
    // DirectX uses [0, 1] - using wrong range causes incorrect ray directions!
    vec4 clipPos = vec4(x, y, z, 1.0);

    // Transform from clip space back to world space
    // ProjInv: clip -> view space
    // ViewInv: view -> world space
    return u_ViewInv * u_ProjectionInv * clipPos;
}

// ============================================================================
// MAIN
// ============================================================================

void main()
{
    // For each vertex of the fullscreen quad, calculate where a ray from
    // that screen position would be at the near and far planes.
    //
    // These two points define a ray: nearP -> farP
    // The fragment shader will interpolate these for each pixel and find
    // where the ray intersects the ground plane.

    // Near plane point (z = -1 in OpenGL NDC)
    v_NearH = unprojectPointH(a_Vertex.x, a_Vertex.y, -1.0);

    // Far plane point (z = +1 in OpenGL NDC)
    v_FarH = unprojectPointH(a_Vertex.x, a_Vertex.y, 1.0);

    // Pass camera position to fragment shader
    v_CameraPos = u_CameraPos;

    // Output vertex position directly (fullscreen quad in NDC, no transformation needed)
    gl_Position = vec4(a_Vertex, 1.0);
}
