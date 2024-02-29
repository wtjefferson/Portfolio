/*CS435
  Project #7
  William Jefferson
  Description: This program is based on the introduction to a concert tape for the band, "TOOL",
  for their "Lateralus" tour that I watched, and decided to recreate in WebGl with my own touches.
  The album, "Lateralus" is about the mathematical connection to the natural laws of the universe.
  The flaming eye represents the search for introspection and knowledge of self, and the heptagram's
  seven points represent completeness in Biblical numerology. I used custom vertex and fragment shaders
  in order to capture the heatwave effect on the eye's animation. The eye is a .png mapped to a very
  thin plane, which gives it a 2D effect. The heptagram is rendered with cylinder geometry with
  its material adjusted for reflections. A series of sliders allows the user to adjust the intensity
  and color of two spotlights (outer and inner) respectively. Ultimately, the animation of the 3D
  heptagram rotating and tilting back and forth around the 2D flaming eye, and the interpolation of
  light and color across the heptagram's geometry provides for an aesthetically pleasing, and
  interesting scene.*/
// =======================
// Shaders and Textures
// =======================

// Custom shaders for heatwave effect
// Vertex Shader
var vertexShader = `
    varying vec2 vUv;
    void main() {
        vUv = uv;
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
    }
`;

// Fragment Shader
var fragmentShader = `
    uniform sampler2D map;
    uniform float time;
    varying vec2 vUv;

    void main() {
        vec2 uv = vUv;
        uv.x += sin(uv.y * 10.0 + time) * 0.1;
        gl_FragColor = texture2D(map, uv);
    }
`;

// Load eye texture
var loader = new THREE.TextureLoader();
var eyeTexture = loader.load('eye.png');

// Create ShaderMaterial with the shaders and the texture
var shaderMaterial = new THREE.ShaderMaterial({
    uniforms: {
        time: { value: 0 },
        map: { value: eyeTexture }
    },
    vertexShader: vertexShader,
    fragmentShader: fragmentShader
});

// =======================
// Initialize Scene
// =======================
var scene = new THREE.Scene();
var camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

// Create a CubeCamera
var cubeRenderTarget = new THREE.WebGLCubeRenderTarget(128); // Adjust size for performance/quality
var cubeCamera = new THREE.CubeCamera(0.1, 1000, cubeRenderTarget);
scene.add(cubeCamera);

// Adjust renderer to the canvas size
var canvas = document.getElementById('webgl-canvas');
var renderer = new THREE.WebGLRenderer({ antialias: true, canvas: canvas });
renderer.setSize(canvas.clientWidth, canvas.clientHeight);

// =======================
// Creating Geometry
// =======================

// Create Line Segments with Thicker and Reflective Material
function createLineSegment(vertex1, vertex2) {
    const material = new THREE.MeshStandardMaterial({
        color: 0xFFFFFF, // White
        envMap: cubeRenderTarget.texture,
        roughness: -1, // Lower roughness makes the material more reflective
        metalness: 1  // Higher metalness for a metallic look
    });

    const direction = new THREE.Vector3().subVectors(vertex2, vertex1);
    const length = direction.length();
    const geometry = new THREE.BoxGeometry(length, 0.5, 0.5);
    const lineSegment = new THREE.Mesh(geometry, material);
    lineSegment.position.copy(vertex1.clone().add(vertex2).divideScalar(2));
    lineSegment.lookAt(vertex2);
    return lineSegment;
}

// Initialize the edges for the heptagram
function createHeptagramEdge(vertex1, vertex2) {
    // Calculate the distance and midpoint
    const distance = vertex1.distanceTo(vertex2);
    const midpoint = new THREE.Vector3().addVectors(vertex1, vertex2).multiplyScalar(0.5);

    // Create a cylinder to represent the edge
    const edgeGeometry = new THREE.CylinderGeometry(0.4, 0.4, distance, 8);
    const edgeMaterial = new THREE.MeshStandardMaterial({ 
        color: 0xffffff,
        envMap: cubeRenderTarget.texture,
        roughness: ~1,
        metalness: 1
    });
    const edge = new THREE.Mesh(edgeGeometry, edgeMaterial);

    // Position the cylinder
    edge.position.set(midpoint.x, midpoint.y, midpoint.z);

    // Rotate the cylinder to align with the edge vertices
    edge.lookAt(vertex2);
    edge.rotateX(Math.PI / 2); // Cylinders are aligned along the Y-axis by default, so we rotate it

    return edge;
}

// Create a 3D heptagram
function createHeptagram(radius) {
    const vertices = [];
    const heptagramGroup = new THREE.Group();

    // Calculate vertices of a regular heptagon
    for (let i = 0; i < 7; i++) {
        const angle = (2 * Math.PI / 7) * i;
        const x = radius * Math.cos(angle);
        const y = radius * Math.sin(angle);
        vertices.push(new THREE.Vector3(x, y, 0));
    }

    // Indices for the {7/2} heptagram edges
    const indices = [0, 2, 4, 6, 1, 3, 5, 0]; // Loop back to the start

    // Create 3D edges
    for (let i = 0; i < indices.length - 1; i++) {
        const edge = createHeptagramEdge(vertices[indices[i]], vertices[indices[i + 1]]);
        heptagramGroup.add(edge);
    }

    return heptagramGroup;
}

// Create plane geometry to map eye to
var imageAspectRatio = 1066 / 2827;
var planeHeight = 5;
var planeWidth = planeHeight * imageAspectRatio;
var planeGeometry = new THREE.PlaneGeometry(planeWidth, planeHeight);

// Create material with the texture
var planeMaterial = new THREE.MeshBasicMaterial({ map: eyeTexture });

// Create and add the mesh to the scene
var planeMeshWithShader = new THREE.Mesh(planeGeometry, shaderMaterial);
scene.add(planeMeshWithShader);

// Add the heptagram to the scene
const heptagram = createHeptagram(10); // Radius 10 units
scene.add(heptagram);

// ===============================
// Lighting and Camera Adjustment
// ===============================

// Add a bright spotlight outside the heptagram
var spotlightAbove = new THREE.SpotLight(0xFFFFFF, 5); // White color, high intensity
spotlightAbove.position.set(0, 0, -15); // Position outside the heptagram
spotlightAbove.target = heptagram; // Target the heptagram
scene.add(spotlightAbove);

// Add a bright spotlight inside the heptagram
var spotlightBelow = new THREE.SpotLight(0xFFFFFF, 5); // White color, high intensity
spotlightBelow.position.set(0, 0, -20); // Position inside the heptagram
spotlightBelow.target = heptagram; // Target the heptagram
scene.add(spotlightBelow);

// Main lighting
const spotlight = new THREE.SpotLight(0xFFFFFF, 1); // White
spotlight.position.set(0, -10, 10);
spotlight.target = heptagram;
scene.add(spotlight);

// Add an ambient light for overall illumination
var ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
scene.add(ambientLight);
ambientLight.intensity = 20.0;

// Adjust the camera, spotlight, and render the scene
camera.position.z = 20;
spotlight.position.set(0, -10, 20);
spotlight.target = heptagram;

// Adjust the spotlightAbove
spotlightAbove.intensity = 15; // Increase intensity
spotlightAbove.angle = Math.PI / 2; // Wider angle
spotlightAbove.penumbra = 0.5; // Soft edge
spotlightAbove.distance = 100;

// Adjust the spotlightBelow
spotlightBelow.intensity = 15; // Increase intensity
spotlightBelow.angle = Math.PI / 4; // Wider angle
spotlightBelow.penumbra = 0.5; // Soft edge

// Position the CubeCamera at the center of the heptagram
cubeCamera.position.copy(heptagram.position);

// Window resize listener
window.addEventListener('resize', onWindowResize, false);
function onWindowResize(){
    camera.aspect = canvas.clientWidth / canvas.clientHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(canvas.clientWidth, canvas.clientHeight);
}

// =======================
// Animation variables
// =======================
// Flags
var rotateHeptagram = false;
var animationActive = false;
var lastTime = 0;
var tiltSpeed = 1;
var isTopLightColorChanging = false;
var isBottomLightColorChanging = false;
var isIntensityChanging = false;

// Define target intensity and color for the top light
var targetIntensityTop = spotlightAbove.intensity;
var targetColorTop = spotlightAbove.color.clone();

// Define target intensity and color for the bottom light
var targetIntensityBottom = spotlightBelow.intensity;
var targetColorBottom = spotlightBelow.color.clone();

// Function to update the color of the top light
function updateTopLightColor() {
    var r = topLightR.value / 255;
    var g = topLightG.value / 255;
    var b = topLightB.value / 255;
    targetColorTop.setRGB(r, g, b);
}

// Function to update the color of the bottom light
function updateBottomLightColor() {
    var r = bottomLightR.value / 255;
    var g = bottomLightG.value / 255;
    var b = bottomLightB.value / 255;
    targetColorBottom.setRGB(r, g, b);
}

// =======================
// Animation loop
// =======================
function animate(time) {
    requestAnimationFrame(animate);
    // Smoothly interpolate towards slider target values
    // Top light sliders
    if (isTopLightColorChanging) {
        spotlightAbove.color.lerp(targetColorTop, 0.1);
        if (spotlightAbove.color.equals(targetColorTop)) {
            isTopLightColorChanging = false;
        }
    }
    // Bottom light sliders
    if (isBottomLightColorChanging) {
        spotlightBelow.color.lerp(targetColorBottom, 0.1);
        if (spotlightBelow.color.equals(targetColorBottom)) {
            isBottomLightColorChanging = false;
        }
    }
    // Light intensity
    if (isIntensityChanging) {
        spotlightAbove.intesnity += (targetIntensityTop - spotlightAbove.intensity) * 0.1;
        spotlightBelow.intensity += (targetIntensityBottom - spotlightBelow.intensity) * 0.1;
        if (Math.abs(spotlightAbove.intensity - targetIntensityTop) < 0.01 && Math.abs(spotlightBelow.intensity - targetIntensityBottom) < 0.01) {
            isIntensityChanging = false;
        }
    }
    // Update the CubeCamera
    cubeCamera.update(renderer, scene);
    renderer.render(scene, camera);
    // Timing variables
    var deltaTime = (time - lastTime) / 1000; // Time in seconds since the last frame
    lastTime = time;
    // Animate heptagram
    if (rotateHeptagram) {
        heptagram.rotation.z += deltaTime * Math.PI / 2;
    }
    // Animate eye
    if (animationActive) {
        shaderMaterial.uniforms.time.value += deltaTime;
        var tilt = Math.sin(time * 0.001 * tiltSpeed);
        var tiltAngle = THREE.MathUtils.degToRad(tilt * 35);
        heptagram.rotation.x = tiltAngle;
    }
    cubeCamera.update(renderer, scene);
    renderer.render(scene, camera);
}

animate(0);

// =======================
// Event Listeners
// =======================

// Get references to the RGB sliders for the top light
var topLightR = document.getElementById('top-light-r');
var topLightG = document.getElementById('top-light-g');
var topLightB = document.getElementById('top-light-b');

// Get references to the RGB sliders for the bottom light
var bottomLightR = document.getElementById('bottom-light-r');
var bottomLightG = document.getElementById('bottom-light-g');
var bottomLightB = document.getElementById('bottom-light-b');

// Function to update spotlight brightness
function updateSpotlightIntensity(intensity) {
    spotlightAbove.intensity = intensity;
    spotlightBelow.intensity = intensity;
}

// Outer spotlight RGB sliders
topLightR.addEventListener('input', function() {
    updateTopLightColor();
    isTopLightColorChanging = true;
});
topLightG.addEventListener('input', function() {
    updateTopLightColor();
    isTopLightColorChanging = true;
});
topLightB.addEventListener('input', function() {
    updateTopLightColor();
    isTopLightColorChanging = true;
});

// Inner spotlight RGB sliders
bottomLightR.addEventListener('input', function() {
    updateBottomLightColor();
    isBottomLightColorChanging = true;
});
bottomLightG.addEventListener('input', function() {
    updateBottomLightColor();
    isBottomLightColorChanging = true;
});
bottomLightB.addEventListener('input', function() {
    updateBottomLightColor();
    isBottomLightColorChanging = true;
});

// Start Button
document.getElementById('toggle-button').addEventListener('click', function() {
    animationActive = !animationActive;
    rotateHeptagram = !rotateHeptagram; // Toggle roation on and off
    shimmerEffect = animationActive; // Toggle heatwave effect
    this.textContent = animationActive ? 'Stop' : 'Start'; // Change button text
});

// Brightness Slider
document.getElementById('slider').addEventListener('input', function(event){
    var intensity = parseFloat(event.target.value);
    updateSpotlightIntensity(intensity * 20);
    isIntensityChanging = true;
});

// Initialize slider by position and intensity
document.getElementById('slider').value = 50; // Set initial slider position (50%)
updateSpotlightIntensity(50); // Set initial intensity

// Call the function once to set the initial color
updateTopLightColor();
updateBottomLightColor();