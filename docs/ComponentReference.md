# Component / JSON Parameter Reference

🌍 *Read this in other languages: [English](ComponentReference.md) | [日本語 (Japanese)](ComponentReference_ja.md)* *(Note: The English documentation is AI-translated from the original Japanese).*

In this engine, you can data-drive the components attached to entities and their parameters using Prefab or Timeline JSON files.

### 📌 Basic Data Type Formatting
When specifying types with multiple values like `Vector3` or `Color` in JSON, you can write them as an **Array**.
* `Vector3` example: `[0.0, 1.0, 0.0]` (X, Y, Z), or object format `{"x": 0.0, "y": 1.0, "z": 0.0}` is also valid.
* `Color` example: `[255.0, 255.0, 255.0]` (R, G, B) *Note: specify in the range of 0 to 255.*

---

## 🎨 Animation Components
Controls the visual appearance, color, and rotational animations of the drone.

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **SpinAnimationRequestComponent** | `axis`<br>`duration`<br>`minSpeed`<br>`maxSpeed`<br>`isRandomAxis`<br>`resetRotation` | Vector3<br>Float<br>Float<br>Float<br>Bool<br>Bool | `[0, 0, 0]`<br>`0.0`<br>`0.0`<br>`0.0`<br>`false`<br>`false` | Executes a spin animation based on the specified conditions upon attachment. Rotation automatically stops and the component is removed after `duration` elapses. If `isRandomAxis` is `true`, it rotates on a random axis. Setting `resetRotation` to `true` resets the current orientation (aligns facing forward) when rotation starts. If `minSpeed > maxSpeed`, it is automatically corrected to `maxSpeed = minSpeed`. |
| **ColorFadeRequestComponent** | `targetColor`<br>`duration` | Color<br>Float | `[255,255,255]`<br>`0.0` | Smoothly fades the color (linear interpolation) from the current color to the `targetColor` over `duration` seconds. |
| **ColorBlinkComponent** | `color1`<br>`color2`<br>`speed`<br>`delayTime` | Color<br>Color<br>Float<br>Float | `[255,255,255]`<br>`[255,255,255]`<br>`1.0`<br>`0.0` | Blinks between two colors at a frequency of `speed` (Hz). Specifying `delayTime` (seconds) can intentionally delay the start timing of the blink. *`speed` must be greater than 0.* |

---

## 🧠 Behavior Components
Controls movement towards target locations and special orbital trajectories.

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **OrbitComponent** | `speed`<br>`duration`<br>`axis` | Float<br>Float<br>Vector3 | `1.0`<br>`0.0`<br>`[0,1,0]`(Up) | Causes the entity to orbit around the target coordinates on the `axis` at a given `speed`. The specified `axis` is automatically normalized. |
| **SteeringComponent** | `stiffness`<br>`dampingRatio` | Float<br>Float | `15.0`<br>`1.0` | Calculates steering force (spring-damper model) towards the target location. `stiffness` represents the strength of attraction, and `dampingRatio` represents the effectiveness of the brakes (1.0 is critical damping). |

---

## 🌪️ Force Components (Physics / Crowd Simulation)
Applies physical forces to the entity, creating complex behaviors like swarms or noise.

### ForceProfile (Sub-object)
`LiftComponent`, `AttractionComponent`, and `SwirlComponent` have an internal `profile` object.

| Parameter | Type | Default Value | Description |
|---|---|---|---|
| `falloff` | String | `"Constant"` | Force attenuation method based on distance. Choose from `"Constant"` (no attenuation), `"Linear"`, or `"Quadratic"`. |
| `distance` | String | `"Sphere"` | Distance calculation method. Choose from `"Sphere"` (3D sphere), `"Cylinder"` (XZ plane distance from Y axis), `"AxisX"`, `"AxisY"`, `"AxisZ"` (each axis direction). |
| `radius` | Float | `100.0` | Maximum radius the force affects. Must be greater than 0. |
| `innerScale` | Float | `1.0` | Force strength scale at the center (t=0). |
| `outerScale` | Float | `0.0` | Force strength scale at the boundary (t=1). |

### Component List

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **BoidsComponent** | `separationWeight`<br>`alignmentWeight`<br>`cohesionWeight`<br>`visionRadius`<br>`maxForce`<br>`updateInterval` | Float<br>Float<br>Float<br>Float<br>Float<br>Float | `1.0`<br>`1.0`<br>`1.0`<br>`100.0`<br>`5.0`<br>`0.2` | Applies crowd simulation (Boids). Specifies weights for separation (avoidance), alignment (facing same direction), cohesion (moving to center), along with vision radius and maximum turning force. Adjust neighborhood search frequency via `updateInterval` (seconds). (*Requires `SpatialPartitionComponent`*) |
| **JitterMovementComponent** | `strength` | Float | `100.0` | Applies random jitter force every frame. |
| **LiftComponent** | `center`<br>`axis`<br>`profile`<br>`strength`<br>`maxForce` | Vector3<br>Vector3<br>ForceProfile<br>Float<br>Float | `[0,0,0]`<br>`[0,1,0]`(Up)<br>(See above)<br>`100.0`<br>`500.0` | Applies lift force in the specified `axis` direction. The force can be profile-controlled based on distance from the `center` using `profile`. The `axis` is automatically normalized. |
| **AttractionComponent** | `center`<br>`profile`<br>`plane`<br>`strength`<br>`maxForce` | Vector3<br>ForceProfile<br>String<br>Float<br>Float | `[0,0,0]`<br>(See above)<br>`"XYZ"`<br>`100.0`<br>`500.0` | Applies attraction force towards the specified `center`. You can restrict the force to specific planes/axes using `plane` (`"XYZ"`, `"XY"`, `"XZ"`, `"YZ"`, `"X"`, `"Y"`, `"Z"`). |
| **SwirlComponent** | `center`<br>`axis`<br>`profile`<br>`strength`<br>`maxForce` | Vector3<br>Vector3<br>ForceProfile<br>Float<br>Float | `[0,0,0]`<br>`[0,1,0]`(Up)<br>(See above)<br>`100.0`<br>`500.0` | Applies swirling force around the `center` on the specified `axis`. The `axis` is automatically normalized. |
| **RandomSwirlAxisRequestComponent** | `axisCount`<br>`noiseStrength` | Int<br>Float | `2`<br>`0.15` | Randomly assigns the rotation axis of `SwirlComponent` from the specified number of axes and adds noise. Used when you want to create complex vortices. |
| **GravityComponent** | `direction`<br>`strength` | Vector3<br>Float | `[0,-1,0]`(Down)<br>`100.0` | Applies gravity force in the specified `direction`. The `direction` is automatically normalized. |
| **WanderComponent** | `strength`<br>`frequency`<br>`updateInterval` | Float<br>Float<br>Float | `100.0`<br>`0.5`<br>`0.2` | Applies wandering force in random directions using time-varying noise. `frequency` adjusts noise change speed, and `updateInterval` (seconds) adjusts update frequency. |
| **DartComponent** | `intervalMin`<br>`intervalMax`<br>`strength` | Float<br>Float<br>Float | `2.0`<br>`5.0`<br>`100.0` | Applies instantaneous strong force in random directions at regular intervals. Wait time until next trigger is randomly determined within `intervalMin` to `intervalMax` (seconds). |
| **BoundaryComponent** | `radius`<br>`force`<br>`center` | Float<br>Float<br>Vector3 | `100.0`<br>`10.0`<br>`[0,0,0]` | Applies pushing force (`force`) inward for entities trying to cross the distance (`radius`) from the `center`, preventing them from flying away into space. |

---

## ⚙️ Physics Components (Velocity Control)
Controls direct constraints, modification, and damping of Velocity.

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **RandomVelocityRequestComponent** | `minSpeed`<br>`maxSpeed` | Float<br>Float | `100.0`<br>`200.0` | Gives an initial velocity in a completely random direction/speed within the specified range upon attachment (for effects like explosions). |
| **SpeedConstraintComponent** | `max`<br>`min` | Float<br>Float | `100.0`<br>`0.0` | Forcibly clamps (restricts) the entity's movement speed within the specified `min` to `max` range. |
| **PlaneConstraintComponent** | `normal`<br>`distance` | Vector3<br>Float | `[0,1,0]`(Up)<br>`0.0` | Constrains the entity's movement to a specified plane. `normal` specifies the normal direction of the plane, and `distance` specifies the offset from the origin. The `normal` is automatically normalized. |
| **DampingComponent** | `factor` | Float | `0.95` | Damping rate (air resistance) multiplied by velocity every frame. 1.0 means no damping, 0.0 means immediate stop. |
| **VelocityRotationComponent** | `rotateSpeed` | Float | `5.0` | Smoothly rotates the entity's orientation (Transform Rotation) toward the current movement direction (Velocity). `rotateSpeed` adjusts the turning speed. |

---

## 📦 Render Components
Specifies the shape and material of entities. Use the data-driven `MaterialRequestComponent` for material settings.

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **MaterialRequestComponent** | `color`<br>`shaderType` | Color<br>String | (Optional)<br>(Optional) | Request component to set materials. Only specified parameters are applied, omitted ones are unchanged. `shaderType` is specified as a string (e.g., `"Lit"`). |
| **PrimitiveMeshComponent** | `type`<br>`scale` | String<br>Vector3 | `"Cube"`<br>`[1.0, 1.0, 1.0]` | Specifies the shape and scale of the primitive to render. `type` can be `"Cube"`, `"Sphere"`, etc. `scale` changes the rendered size (each axis). |

---

## 🌐 Spatial Components
Components related to optimization foundations like spatial partitioning and neighborhood search.

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **SpatialPartitionComponent** | (None in JSON) | - | - | **[IMPORTANT]** When using `BoidsComponent`, this must be attached to optimize neighborhood searches. Since internal parameters are calculated automatically at runtime, leave it empty in JSON like `{"SpatialPartitionComponent": {}}`. |

---

## 🌅 Scene Components
Configures environmental settings like the overall scene lighting.

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **SceneLightingComponent** | `ambient`<br>`direction`<br>`lightColor`<br>`fillColor` | Float<br>Vector3<br>Color<br>Color | `0.1`<br>`[-1,-1,1]`<br>`[255,255,255]`<br>`[0,0,0]` | Configures scene lighting. `ambient` is ambient light strength, `direction` is directional light direction, `lightColor` is main light color, and `fillColor` is fill light color. |

---

## 🛸 Formation Components
Controls drone identification, and the overall shape/placement mode of figures (formations) drawn in the air.

| Component Name | Parameters | Type | Default Value | Description |
|---|---|---|---|---|
| **DroneComponent** | (None) | - | - | Tag component to make the system recognize the target entity as a "drone". Has no data (parameters). |
| **FormationComponentWireframe** | `scale`<br>`file` | Float<br>String | `300.0`<br>`""` | Connects vertices of the specified JSON (`file`) with line segments and places drones on those lines. Suitable for continuous-line 3D representations like cubes or torus knots. |
| **FormationComponentPointCloud** | `scale`<br>`file` | Float<br>String | `300.0`<br>`""` | Treats the specified JSON (`file`) vertices as a point cloud and places drones directly (while culling) at each point. Suitable for dense expressions like paintings or logos. |
| **FormationRotationComponent** | `rotateSpeed` | Float | `0.0` | Rotates the entire formation at the specified speed. If `rotateSpeed` is `0.0`, it does not rotate. |
