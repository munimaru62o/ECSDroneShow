# Component / JSON Parameter Reference

🌍 *他の言語で読む: [English](ComponentReference.md) | [日本語 (Japanese)](ComponentReference_ja.md)*

本エンジンでは、プレハブ（Prefab）やタイムライン（Timeline）の JSON ファイルにおいて、エンティティに付与するコンポーネントとパラメータをデータ駆動で指定することができます。

### 📌 基本的なデータ型の書き方
JSON上で `Vector3` や `Color` などの複数の値を持つ型を指定する場合は、以下のように **配列（Array）** として記述することができます。
* `Vector3` の例: `[0.0, 1.0, 0.0]` （X, Y, Z）、またはオブジェクト形式 `{"x": 0.0, "y": 1.0, "z": 0.0}` も使用可能
* `Color` の例: `[255.0, 255.0, 255.0]` （R, G, B）※0〜255の範囲で指定。

---

## 🎨 Animation Components (アニメーション・視覚効果)
機体の見た目や、色、回転などの視覚的なアニメーションを制御します。

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **SpinAnimationRequestComponent** | `axis`<br>`duration`<br>`minSpeed`<br>`maxSpeed`<br>`isRandomAxis`<br>`resetRotation` | Vector3<br>Float<br>Float<br>Float<br>Bool<br>Bool | `[0, 0, 0]`<br>`0.0`<br>`0.0`<br>`0.0`<br>`false`<br>`false` | アタッチ時に指定された条件に基づき自転アニメーションを実行します。`duration`経過後に自動で回転が停止・削除されます。`isRandomAxis`が`true`の場合はランダムな軸で回転し、`resetRotation`を`true`に設定すると、回転開始時に現在の向きをリセット（正面向きに整列）します。`minSpeed > maxSpeed`の場合は自動的に`maxSpeed = minSpeed`に補正されます。 |
| **ColorFadeRequestComponent** | `targetColor`<br>`duration` | Color<br>Float | `[255,255,255]`<br>`0.0` | 現在の色から `targetColor` へ、`duration`（秒）かけて滑らかに色をフェード（線形補間）させます。 |
| **ColorBlinkComponent** | `color1`<br>`color2`<br>`speed`<br>`delayTime` | Color<br>Color<br>Float<br>Float | `[255,255,255]`<br>`[255,255,255]`<br>`1.0`<br>`0.0` | 2つの色の間を `speed`（Hz）の周期で明滅させます。`delayTime`（秒）を指定すると、発光の開始タイミングを意図的に遅らせることができます。※`speed`は必ず0より大きい値を指定してください。 |

---

## 🧠 Behavior Components (振る舞い・目標追従)
目標地点（Target）に対する動き方や、特殊な軌道を制御します。

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **OrbitComponent** | `speed`<br>`duration`<br>`axis` | Float<br>Float<br>Vector3 | `1.0`<br>`0.0`<br>`[0,1,0]`(Up) | 目標座標の周囲を `axis` を軸として `speed` の速度で公転（オービット）させます。指定した `axis` は自動的に正規化されます。 |
| **SteeringComponent** | `stiffness`<br>`dampingRatio` | Float<br>Float | `15.0`<br>`1.0` | 目標地点へ向かう操舵力（バネ・ダンパーモデル）を計算します。`stiffness` は引力の強さ、`dampingRatio` はブレーキの効き具合（1.0で臨界減衰）を表します。 |

---

## 🌪️ Force Components (力学・群衆シミュレーション)
機体に物理的な力（Force）を与え、群れやノイズなどの複雑な挙動を生み出します。

### ForceProfile（サブオブジェクト）
`LiftComponent`, `AttractionComponent`, `SwirlComponent` は内部に `profile` オブジェクトを持ちます。

| パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|
| `falloff` | String | `"Constant"` | 距離に応じた力の減衰方式。`"Constant"`（減衰なし）, `"Linear"`（線形減衰）, `"Quadratic"`（二次減衰）から選択。 |
| `distance` | String | `"Sphere"` | 距離の計算方式。`"Sphere"`（3D球体）, `"Cylinder"`（Y軸からのXZ平面距離）, `"AxisX"`, `"AxisY"`, `"AxisZ"`（各軸方向）から選択。 |
| `radius` | Float | `100.0` | 力が及ぶ最大半径。0より大きい値を指定してください。 |
| `innerScale` | Float | `1.0` | 中心（t=0）での力の強さスケール。 |
| `outerScale` | Float | `0.0` | 境界（t=1）での力の強さスケール。 |

### コンポーネント一覧

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **BoidsComponent** | `separationWeight`<br>`alignmentWeight`<br>`cohesionWeight`<br>`visionRadius`<br>`maxForce`<br>`updateInterval` | Float<br>Float<br>Float<br>Float<br>Float<br>Float | `1.0`<br>`1.0`<br>`1.0`<br>`100.0`<br>`5.0`<br>`0.2` | 群集シミュレーション（Boids）を適用します。分離（ぶつからない）、整列（同じ方向を向く）、結合（中心へ向かう）の重みと、視野半径・最大旋回力を指定します。`updateInterval`（秒）で近傍探索の更新頻度を調整します。（※`SpatialPartitionComponent` が必須です） |
| **JitterMovementComponent** | `strength` | Float | `100.0` | フレームごとにランダムな揺らぎの力（ジッター）を与えます。 |
| **LiftComponent** | `center`<br>`axis`<br>`profile`<br>`strength`<br>`maxForce` | Vector3<br>Vector3<br>ForceProfile<br>Float<br>Float | `[0,0,0]`<br>`[0,1,0]`(Up)<br>（後述）<br>`100.0`<br>`500.0` | 指定した `axis` 方向への浮力（リフト）を与えます。`center` からの距離に応じて `profile` で力をプロファイル制御できます。`axis` は自動的に正規化されます。 |
| **AttractionComponent** | `center`<br>`profile`<br>`plane`<br>`strength`<br>`maxForce` | Vector3<br>ForceProfile<br>String<br>Float<br>Float | `[0,0,0]`<br>（後述）<br>`"XYZ"`<br>`100.0`<br>`500.0` | 指定した `center` へ向かう引力を与えます。`plane` で力を制限する平面・軸を指定できます（`"XYZ"`, `"XY"`, `"XZ"`, `"YZ"`, `"X"`, `"Y"`, `"Z"`）。 |
| **SwirlComponent** | `center`<br>`axis`<br>`profile`<br>`strength`<br>`maxForce` | Vector3<br>Vector3<br>ForceProfile<br>Float<br>Float | `[0,0,0]`<br>`[0,1,0]`(Up)<br>（後述）<br>`100.0`<br>`500.0` | `center` を中心に `axis` を軸とした渦巻き状の力（スワール）を与えます。`axis` は自動的に正規化されます。 |
| **RandomSwirlAxisRequestComponent** | `axisCount`<br>`noiseStrength` | Int<br>Float | `2`<br>`0.15` | `SwirlComponent` の回転軸（axis）を、指定した数の軸の中からランダムに割り当て、ノイズを加えます。複雑な渦を作りたい場合に使用します。 |
| **GravityComponent** | `direction`<br>`strength` | Vector3<br>Float | `[0,-1,0]`(Down)<br>`100.0` | 指定した `direction` 方向への重力を与えます。`direction` は自動的に正規化されます。 |
| **WanderComponent** | `strength`<br>`frequency`<br>`updateInterval` | Float<br>Float<br>Float | `100.0`<br>`0.5`<br>`0.2` | 時間変化するノイズを使ってランダムな方向へさまよう力を与えます。`frequency` でノイズの変化速度を、`updateInterval`（秒）で更新頻度を調整します。 |
| **DartComponent** | `intervalMin`<br>`intervalMax`<br>`strength` | Float<br>Float<br>Float | `2.0`<br>`5.0`<br>`100.0` | 一定間隔でランダムな方向に瞬発的な強い力を与えます。次の発動までの待機時間を `intervalMin` 〜 `intervalMax`（秒）の範囲でランダムに決定します。 |
| **BoundaryComponent** | `radius`<br>`force`<br>`center` | Float<br>Float<br>Vector3 | `100.0`<br>`10.0`<br>`[0,0,0]` | `center` から `radius` の距離（境界）を超えようとした機体に対し、内側へ押し戻す力（`force`）を与え、機体が宇宙の彼方へ飛んでいくのを防ぎます。 |

---

## ⚙️ Physics Components (物理・速度制御)
速度（Velocity）に対する直接的な制約や加工、減衰などを制御します。

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **RandomVelocityRequestComponent** | `minSpeed`<br>`maxSpeed` | Float<br>Float | `100.0`<br>`200.0` | アタッチ時に、指定された範囲内で完全にランダムな方向・速度の初速を与えます（爆発などの演出用）。 |
| **SpeedConstraintComponent** | `max`<br>`min` | Float<br>Float | `100.0`<br>`0.0` | 機体の移動速度を、指定した `min` 〜 `max` の範囲内に強制的にクランプ（制限）します。 |
| **PlaneConstraintComponent** | `normal`<br>`distance` | Vector3<br>Float | `[0,1,0]`(Up)<br>`0.0` | 機体の移動を指定した平面上に拘束します。`normal` で平面の法線方向を、`distance` で原点からのオフセットを指定します。`normal` は自動的に正規化されます。 |
| **DampingComponent** | `factor` | Float | `0.95` | 毎フレームの速度に乗算される減衰率（空気抵抗）です。1.0で減衰なし、0.0で即座に停止します。 |
| **VelocityRotationComponent** | `rotateSpeed` | Float | `5.0` | 機体の向き（TransformのRotation）を、現在の進行方向（Velocity）へ滑らかに向かせます。`rotateSpeed` で振り向く速度を調整します。 |

---

## 📦 Render Components (描画)
エンティティの形状・マテリアルを指定します。マテリアルの設定にはデータドリブン対応の `MaterialRequestComponent` を使用してください。

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **MaterialRequestComponent** | `color`<br>`shaderType` | Color<br>String | （省略可）<br>（省略可） | マテリアルを設定するリクエストコンポーネントです。指定したパラメータのみが適用され、省略したパラメータは変更されません。`shaderType` は文字列で指定します（例: `"Lit"`）。 |
| **PrimitiveMeshComponent** | `type`<br>`scale` | String<br>Vector3 | `"Cube"`<br>`[1.0, 1.0, 1.0]` | 描画するプリミティブの形状とスケールを指定します。`type` には `"Cube"`, `"Sphere"` などが選択可能です。`scale` で描画時のサイズ（各軸）を変更します。 |

---

## 🌐 Spatial Components (空間管理)
空間分割・近傍探索などの最適化基盤に関連するコンポーネントです。

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **SpatialPartitionComponent** | (JSON指定なし) | - | - | **【重要】** `BoidsComponent` を使用する際、近傍探索を最適化するために必ずアタッチする必要があります。内部パラメータはランタイムで自動計算されるため、JSONには `{"SpatialPartitionComponent": {}}` と空で記述してください。 |

---

## 🌅 Scene Components (シーン設定)
シーン全体の光源などの環境設定を行います。

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **SceneLightingComponent** | `ambient`<br>`direction`<br>`lightColor`<br>`fillColor` | Float<br>Vector3<br>Color<br>Color | `0.1`<br>`[-1,-1,1]`<br>`[255,255,255]`<br>`[0,0,0]` | シーンのライティングを設定します。`ambient` は環境光の強さ、`direction` はディレクショナルライトの方向、`lightColor` はメインライトの色、`fillColor` は補助光の色です。 |

---

## 🛸 Formation Components (フォーメーション)
ドローンの識別や、空中に描く図形（フォーメーション）の全体的な形状と配置モードを制御します。

| コンポーネント名 | パラメータ | 型 | デフォルト値 | 説明 |
|---|---|---|---|---|
| **DroneComponent** | (なし) | - | - | 対象のエンティティが「ドローン」であることをシステムに認識させるためのタグコンポーネントです。データ（パラメータ）は持ちません。 |
| **FormationComponentWireframe** | `scale`<br>`file` | Float<br>String | `300.0`<br>`""` | 指定したJSON（`file`）の頂点間を線分で繋ぎ、その線上にドローンを配置します。立方体やトーラス結び目など、一筆書きの軌跡を描くような立体表現に適しています。 |
| **FormationComponentPointCloud** | `scale`<br>`file` | Float<br>String | `300.0`<br>`""` | 指定したJSON（`file`）の頂点を点群として扱い、ドローンを各点へ直接（間引きながら）配置します。絵画やロゴなど、面として密度を出したい表現に適しています。 |
| **FormationRotationComponent** | `rotateSpeed` | Float | `0.0` | フォーメーション全体を指定した速度で回転させます。`rotateSpeed` が `0.0` の場合は回転しません。 |