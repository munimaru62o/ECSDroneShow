# ECS Drone Show

![C++20](https://img.shields.io/badge/C++-20-blue.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)

🌍 *他の言語で読む: [English](README.md) | [日本語 (Japanese)](README_ja.md)*

C++ と DxLib を使用して開発された、**ドローンショーをモチーフにした大規模群衆シミュレーター**です。  
グラフィックスはシンプルな描画に留め、裏側で動く独自のECS（Entity Component System）コアアーキテクチャによる10万Entity規模のリアルタイム制御・パフォーマンス最適化に焦点を当てています。  
JSON で定義したタイムラインに従い、大量の Entity が様々なフォーメーションやBoids群衆シミュレーションへ自律的に移行します。  

本プロジェクトは、ECSアーキテクチャの仕組みを深く理解し学習することを目的に開発されました。
既存の外部ECSライブラリを使用せず、「データ指向設計（DOD）に基づく純粋な C++ ECS コアアーキテクチャ」をフルスクラッチで独自に構築しています。
データ指向設計の実装例・参考資料として活用していただければ幸いです。

GPUインスタンシングの導入により、描画込みで **100,000 Entity** をリアルタイムに動作させることができます。シンプルな処理では Vsync 上限（**144 FPS**）に到達し、Boids などの複雑なシミュレーション実行時でも **約120 FPS前後** を維持します。

> ⚠️ **注意事項**:  
> 本デモには群集シミュレーションによる高密度なオブジェクトの密集・明滅表現が含まれます。  
> 集合体恐怖症（トライポフォビア）や光過敏症等に不安のある方は、動画の視聴にご注意ください。


### 🎬 デモ映像

<details>
<summary><strong>🐟 1. 10万EntityのBoids群集シミュレーション（クリックして表示）</strong></summary>
<br>

https://github.com/user-attachments/assets/e68bd192-84fa-4676-a53e-ed358be8140a

> **100,000体のEntityによる、魚群のような群集シミュレーションです。**  
> `SpatialGrid` による空間分割とローカルキャッシュを活用し、大規模な近傍探索（分離・整列・結合）を行いつつも、GPUインスタンシングと並列処理により約120FPSの滑らかなパフォーマンスを実現しています。

</details>


<details>
<summary><strong>🧊🐝 2. 幾何学フォーメーションと有機的な群飛（クリックして表示）</strong></summary>
<br>

https://github.com/user-attachments/assets/4f89d097-1cb6-4274-8011-4bbf2c030510

> **ワイヤーフレーム形状から、個々の揺らぎ（Jitter/Wander）シーケンスへ移行するデモです。**  
> JSONから読み込んだマクロな頂点座標へ追従しつつ、ミクロなノイズをECSの各システムが処理することで、機械的ではない「昆虫の大群のような」有機的なうねりを表現しています。

</details>


<details>
<summary><strong>✨ 3. フルシーケンス 4Kデモ映像 (YouTube)</strong></summary>
<br>

[![Full Sequence Demo](https://img.youtube.com/vi/gynuv1Rhf6s/maxresdefault.jpg)](https://youtu.be/gynuv1Rhf6s)

> **画像をクリックすると YouTube で 4K/60FPS の高画質フルデモが再生されます。**  
> タイムライン駆動によって、Boids、フォーメーション展開、スピンアニメーション、空間力学（Swirl, Orbit）などの各Componentが動的にアタッチ・デタッチされ、シームレスに状態遷移していく全編を収録しています。

</details>

---

## 目次

1. [概要と操作方法](#1-概要と操作方法)
   - [1.1 デモについて](#11-デモについて)
   - [1.2 操作方法](#12-操作方法)
2. [環境要件・ビルド方法](#2-環境要件・ビルド方法)
   - [2.1 開発・動作環境](#21-開発・動作環境)
   - [2.2 依存ライブラリ](#22-依存ライブラリ)
   - [2.3 プロジェクト構成](#23-プロジェクト構成)
   - [2.4 ビルド手順](#24-ビルド手順)
3. [設計思想とアーキテクチャ](#3-設計思想とアーキテクチャ)
   - [3.1 ECSのコア設計](#31-ecsentity-component-systemのコア設計)
   - [3.2 アプリケーションの実行パイプライン](#32-アプリケーションの実行パイプライン)
   - [3.3 データ駆動設計（Data-Driven Design）](#33-データ駆動設計data-driven-design)
4. [パフォーマンス最適化とトレードオフ](#4-パフォーマンス最適化とトレードオフ)
   - [4.1 データ指向設計によるメモリアクセス最適化](#41-データ指向設計によるメモリアクセス最適化sparse-dense-setの活用)
   - [4.2 マルチスレッドと安全なECS操作](#42-マルチスレッドと安全なecs操作threadpool--parallelfor--deferredcommand)
   - [4.3 群衆シミュレーション最適化](#43-群衆シミュレーション最適化)
   - [4.4 描画最適化（Dynamic Batching）](#44-描画最適化dynamic-batching)
   - [4.5 メモリ使用量のトレードオフ](#45-メモリ使用量のトレードオフ)
   - [4.6 パフォーマンス検証結果とボトルネック分析](#46-パフォーマンス検証結果とボトルネック分析)
5. [ディレクトリ構成](#5-ディレクトリ構成)
6. [今後やりたいこと](#6-今後やりたいこと)
7. [コントリビューションについて](#7-コントリビューションについて)
8. [ライセンス](#8-ライセンス)

---

## 1. 概要と操作方法

| 項目 | 内容 |
|---|---|
| 言語 | C++20 |
| 外部ライブラリ | DxLib 3.24f, nlohmann/json 3.12.0, OpenSimplex Noise |
| テストフレームワーク | Google Test |
| 初期Entity数 | 50,000 |
| 最大Entity数 | 100,000（キー入力で動的に追加・削除可能） |

### 1.1 デモについて

このデモでは、JSON で定義されたタイムラインに従い、
大量のドローン Entity がリアルタイムに挙動を切り替えます。

各シーケンスでは、

- フォーメーション追従
- Boids 群衆シミュレーション
- 空間力学の適用（Orbit, Lift, Attraction, Swirl, Gravity 等）
- ランダムノイズ・揺らぎ（Jitter, Wander, Dart 等）
- 色のフェード・明滅アニメーション
- スピンアニメーションやフォーメーション全体の回転

などの Component が動的に付与・削除されます。  
シーケンスの内容はすべて外部 JSON で定義されており、コード変更なしで演出内容を変更できます。

**タイムラインの切り替えについて**  
`data/config/game_config.json` の `timeline.playName` を変更するだけで、再生するタイムラインを切り替えられます。

| `playName` | 内容 |
|---|---|
| `DroneShow`（デフォルト） | フォーメーション・Boids 群集・空間力学など多彩なシーケンスのフルデモ |
| `PictureShow` | 点群ドローンで画像を描く、シンプルな PointCloud デモ |

### 1.2 操作方法

| キー | 動作 |
|---|---|
| `1` | Entity 1,000 体を追加スポーン |
| `2` | Entity 1,000 体を削除 |
| `3` | オーバーレイデバッグ表示有効化切り替え |
| `4` | 3Dデバッグ表示有効化切り替え （Debug::Config::IsEnabled有効時）|
| `ESC` | 終了 |

---

## 2. 環境要件・ビルド方法

### 2.1 開発・動作環境
- **OS**: Windows 10 / 11 (64bit)
- **IDE**: Visual Studio 2022
- **プログラミング言語**: C++20
- **検証環境**: 
  - CPU: インテル® Core™ Ultra 7 プロセッサー 265K
  - メモリ: 64 GB

### 2.2 依存ライブラリ
本プロジェクトは以下の外部ライブラリを使用しています。
- **DxLib (ver 3.24f)**: 描画・ウィンドウ管理
- **nlohmann/json (ver 3.12.0)**: 外部データ（Prefab / Timeline）のパース
- **OpenSimplex Noise**: ノイズ生成（Wander / 揺らぎ表現）
- **Google Test**: ユニットテスト

※ 必要なライブラリのヘッダおよびライブラリファイルは `External/` ディレクトリに同梱し、相対パスで設定済みです。
環境構築の手間なく、すぐにビルド・実行いただけます。

### 2.3 プロジェクト構成

本ソリューション（`ECSEngine.sln`）は以下の4つのプロジェクトで構成されています。

| プロジェクト | 種別 | 役割 |
|---|---|---|
| `ECSEngine` | スタティックライブラリ | ゲーム非依存の汎用エンジン層（ECSコア・物理・描画基盤など） |
| `DroneShow_Game` | スタティックライブラリ | ドローンショー固有のゲームロジック（System・Component・Manager など） |
| `DroneShow_App` | 実行ファイル | エントリポイント（`main.cpp`） 上記2ライブラリをリンクして起動する |
| `DroneShow_Test` | 実行ファイル | ユニットテスト（Google Test） `ECSEngine` および `DroneShow_Game` のテストを含む |

依存関係は次のようになっています。

```
DroneShow_App
  ├── DroneShow_Game  (スタティックリンク)
  └── ECSEngine       (スタティックリンク)

DroneShow_Test
  ├── DroneShow_Game  (スタティックリンク)
  └── ECSEngine       (スタティックリンク)

※ DroneShow_Game は内部的に ECSEngine（エンジンコア）に依存しています。
```

### 2.4 ビルド手順

1. 本リポジトリを任意のディレクトリにクローン（またはダウンロードして解凍）します。
2. フォルダ内にある `ECSEngine.sln` を Visual Studio 2022 で開きます。
3. **スタートアッププロジェクトの確認**
   - ソリューションエクスプローラーで `DroneShow_App` がスタートアッププロジェクト（太字表示）になっていることを確認します。
4. **ビルド構成の選択**
   - 本デモの最大パフォーマンス（10,000機以上のシミュレーション）を確認するため、上部のビルド構成プルダウンから **`Release` / `x64`** を選択することを推奨します。  
   （`Debug` ビルドでは最適化が効かず、FPSが低下する場合があります）
5. **実行（F5）**
   - パスやC++20の設定はプロジェクトファイル（`.vcxproj`）に保存されているため、そのまま「ローカル Windows デバッガー」をクリック（または `F5` キー）するだけでビルドと実行が開始されます。
6. **ユニットテストの実行**
   - ソリューションエクスプローラーから `DroneShow_Test` をスタートアッププロジェクトに設定し、実行（F5）することで、ECSコアや各種Systemの自動テストを確認できます。

---

## 3. 設計思想とアーキテクチャ

本プロジェクトは、以下の特徴と設計思想に基づいて開発されています。

### 主な特徴
- ECS をフルスクラッチ実装
- 固定 timestep + phase/order パイプライン
- ThreadPool + ParallelFor による並列 System 実行
- DeferredCommand によるスレッドセーフな ECS 更新
- SpatialGrid + Cache による Boids 最適化
- JSON ベースのタイムライン / Prefab / フォーメーション制御
- 100,000 Entity 規模でのシミュレーション検証

### 拡張性とモジュール性の重視
- **疎結合で拡張に開かれた設計**:  
  各システムやコンポーネント間の依存関係を最小限に抑え、機能追加時に既存コードへの影響を最小限に抑えるアーキテクチャを構築しました。
- **汎用エンジン層とアプリケーション層の分離**:  
  「他のプロジェクトでも使い回せる汎用的なECS基盤」と、「今回のドローンショー固有のゲームロジック」のディレクトリを明確に分離し、高い再利用性を確保しています。
- **データ駆動によるイテレーションの高速化**:  
  タイムラインや各種パラメータをJSONによる外部データとして定義することで、C++の再コンパイルなしに演出を調整可能にしました。

### 3.1 ECS（Entity Component System）のコア設計

データとロジックを分離し、大量Entityの効率的な管理を実現するために ECS を採用しました。  
学習目的でもあるため、コア実装も外部ライブラリに依存せず自前で実装しています。

> **💡 アーキテクチャ図について**  
> コアアーキテクチャの全体像およびクラス間の依存関係については、同梱の **[クラス図 (diagram.mmd)](docs/asset/mermaid/diagram.mmd)** をあわせてご参照ください。

#### モジュラー設計

本プロジェクトでは、機能追加時に既存コードへの変更を最小化することを重視しています。  
例えば新しい挙動を追加する場合、

- 新規Component定義
- 対応System実装
- JSONデータ定義

これらを追加するだけで動作可能です。  
既存Systemへの分岐追加を避け、ECSによるデータ分離と Trait ベースの設計により、
機能を疎結合に保っています。


#### Signature によるEntityフィルタリング

各 System は必要なコンポーネントの組み合わせを `std::bitset` の Signature として登録します。
コンポーネントが追加・削除されるたびに Signature が照合され、各 System の処理対象が自動的に更新されます。
これにより、関係のないEntityを System が処理するコストをゼロにしています。

#### コンパイル時トレイト（`ComponentTraits` / `SequenceTraits`）

コンポーネントの性質を `constexpr` のテンプレート特殊化で一元的に宣言します。
実行時コストゼロで、コンポーネントごとの挙動を静的に切り替えられます。
責務を明確にするため、エンジン基盤の特性（`ComponentTraits`）とゲーム依存の特性（`SequenceTraits`）に分離しています。

**Engine 依存トレイト (`ComponentTraits`)**
| トレイト | 意味 |
|---|---|
| `CoreComponent` | `true` の場合、全Entity生成時に自動付与される |
| `HasCreationTime` | `true` の場合、アタッチ時に `creationTime` へ絶対時間が自動書き込まれる |
| `SeedPolicy` | シード値の生成ポリシー（Entity依存、ランダム等）を指定する |
| `HasInitializer` | `true` の場合、コンポーネントが独自の初期化ロジックを持つ |

**Game 依存トレイト (`SequenceTraits`)**
| トレイト | 意味 |
|---|---|
| `DataDriven` | `true` の場合、ファクトリレジストリに自動登録され、JSONから動的に生成可能になる |
| `SyncOnCatchUp` | `true` の場合、途中スポーン時に既存Entityの値をコピーして同期する |
| `Target` | コンポーネントのアタッチ対象（システム全体 `System`、またはシーン `Scene`） |
| `OneShot` | `true` の場合、一度アタッチ・適用された直後に削除される |

```cpp
// 新しいデータ駆動コンポーネントを追加する場合、特殊化を書くだけ
template<>
struct SequenceTraits<MyComponent> : DefaultSequenceTraits {
    static constexpr bool DataDriven = true;
};
```

`ComponentFactoryRegistry` や各種システムはこのトレイトを `if constexpr` で参照しており、
処理の分岐コード（switchやif文）を書かずに新しいコンポーネントの性質を安全に宣言・一括登録できます。


### 3.2 アプリケーションの実行パイプライン

#### アプリケーションの初期化とConfig読み込み

本プロジェクトは、ソースコード上にハードコードされた値に依存せず、起動時に `data/config/game_config.json` を起点として全体の初期化を行います。
`Game::Init()` における大まかなセットアップフローは以下の通りです。

1. **Configの読み込み**:  
   汎用的な `JsonLoader` を使用して `game_config.json` をパースし、ウィンドウサイズ、各種ディレクトリパス、初期スポーン数などの基本設定を取得します。
2. **基盤とマネージャの初期化**:  
   DxLibの初期化（`ConfigureDxLib`）、グラフィックス描画基盤のセットアップを行った後、`PrefabManager` や `TimelineManager` などを生成します。
3. **ECSの登録（Registration）**:  
   `GameRegistrations::RegisterAllComponents()` と `RegisterAllSystems()` により、全コンポーネントとシステムを登録します（ここで `SequenceTraits<T>::DataDriven` が有効なものは `ComponentFactoryRegistry` へ自動登録されます）。
4. **外部データのロード**:  
   コンポーネント登録の完了後、`DirectoryLoader` などのファイル走査機構を活用し、指定ディレクトリ（`data/prefab/` や `data/timeline/` など）から、プレハブとタイムラインのJSONデータを一括ロードします。
5. **システムのセットアップ**:  
   システム間の依存関係（空間分割グリッドの共有、タイムラインデータの設定など）を構築（`SetupSystems()`）し、各システムの初期化（`InitSystems()`）を発火させます。
6. **初期Entityのスポーン**:  
   Configで指定された初期スポーン数とプレハブ名に基づき、`PrefabManager::Instantiate()` を呼び出して初期Entity群を生成します。

ファイルロード責務を `Loaders/` ディレクトリ配下に切り出したことで、一元化された安全なデータロード機構を備え、エントリーポイントからデータ駆動を徹底することで、C++側は「データをどう処理するか」というロジックの実行に専念できるアーキテクチャになっています。

#### システム実行フェーズ（固定 timestep + phase/order）

フレームループは4つのフェーズに分割されており、**物理演算は固定タイムステップ**で動作します。

```
Input   → Logic   → Physics（固定 60Hz）→ Render
                        └─ fixedAccumulator で複数回実行されることがある
```

また、各フェーズ内では Order を定義し、System をデータ依存順に実行しています。

```cpp
enum class LogicOrder
{
    PreUpdate               = 0,    // 入力処理、前フレームのクリーンアップなど
    Update                  = 10,   // ユーザーのメインロジック
    ApplyRequest            = 20,   // Request → Runtime変換
    PostUpdate              = 30,   // 状態遷移（ステートマシンなど）
    EventProcess            = 40,   // イベントの処理
};

enum class PhysicsOrder
{
    PreSimulation           = 0,  // 前処理
    SpatialPartition        = 10, // 空間構築
    SpatialCache            = 20, // 空間キャッシュ
    ForceClear              = 30, // Force初期化
    TargetGenerator         = 40, // Target生成
    TargetModifier          = 45, // Target加工
    DirectionGenerator      = 50, // Direction生成
    ForceGenerator          = 60, // Force生成
    Steering                = 70, // Steering
    VelocityGenerator       = 80, // Force -> Velocity
    VelocityModifier        = 83, // 速度の加算加工 (Noiseによる揺らぎ、風など)
    VelocityDamping         = 86, // 速度の減衰 (Dampingによる空気抵抗など)
    Constraint              = 90, // 制約
    TransformIntegration    = 100, // Velocity -> Transform
    Rotation                = 110, // 回転処理
    PostTransform           = 120, // Transform後処理
    MaterialModifier        = 130, // 視覚同期
    Finalize                = 140, // Cleanupなどの後処理
};

enum class RenderOrder
{
    Background              = 0,    // 背景
    Opaque                  = 10,   // 不透明オブジェクト（PrimitiveMesh等）
    Transparent             = 20,   // 半透明・パーティクル
    UI                      = 30,   // 前面UI
};
```

例えば BoidsSystem は SpatialGrid の構築後に実行される必要があり、Velocity 系 System は Force 生成後でなければ正しく動作できません。  
このように、各 System を「何を生成するか」、「どのデータを加工するか」という責務単位で分離し、パイプラインとして順序制御しています。  

これにより、

- データ依存の明確化
- System 間結合の低減
- 並列化しやすい構造
- Update 順依存バグの抑制

を実現しています。

> **💡 実行パイプライン図について**  
> 上記の各フェーズにおける System の厳密な実行順序（Order）とデータの流れについては、同梱の **[実行フローチャート (flowchart.mmd)](docs/asset/mermaid/flowchart.mmd)** をあわせてご参照ください。

### 3.3 データ駆動設計（Data-Driven Design）

Componentの追加、パラメータの調整などを外部データとして定義することで、
イテレーション速度を向上させる設計にしています。

#### タイムライン駆動のシーケンス設計

演出の内容はすべて JSON ファイルで定義されており、エンジニア以外でも編集できる構造になっています。

```json
{
  "sequences": [
    {
      "name": "formation_torus_knot",
      "duration": 10.0,
      "components": {
        "FormationComponentWireframe": { 
          "file": "data/formation/json/torus_knot.json", 
          "scale": 400.0 
        },
        "OrbitComponent": {
          "axis": [0.0, 1.0, 0.0],
          "speed": 0.15
        }
      }
    }
  ]
}
```

#### TimelineDirectorSystem によるシーケンス管理とコンポーネントの付与

`TimelineDirectorSystem` はタイムラインの進行を管理し、シーケンスの切り替え時に、JSON で定義されたコンポーネント群を各 Entity へ直接アタッチ（または削除）します。

基本的にはコンポーネント自体が「状態の定義」を表しており、シーケンスに記述されたコンポーネントがそのまま付与されるだけで、対応する System がそれを読み取って処理を行います。  
例えば `BoidsComponent` が付与されれば `BoidsSystem` が群集シミュレーションを行い、`SpeedConstraintComponent` が付与されれば `SpeedConstraintSystem` が速度制限を適用します。

一方、以下のような特殊なケースでは **Request コンポーネント** を介した初期化パターンを採用しています。

- **ワンショット処理**: `RandomVelocityRequestComponent` など、アタッチ時に一度だけ処理を行えばよいもの。対応する RequestSystem が処理後にリクエストコンポーネントを自動削除します。
- **現在の状態を読んで初期化が必要なもの**: `ColorFadeRequestComponent` の場合、フェード開始色として「現在の `MaterialComponent` の色」を記録する必要があります。`ColorFadeRequestSystem` がこれを受け取った時点で現在の状態を取得し、実際の実行用 `ColorFadeComponent` へ変換します。これにより、初期化ロジックと実行ロジックの責務が ECS 内で明確に分離されています。

#### 途中参加（動的スポーン）エンティティの完全同期

本デモでは、実行中にキー入力でドローンを動的に追加・削除できます。
この際、後から追加されたドローンが「演出の最初から」動いてしまい、群衆全体の動きとずれてしまう問題を防ぐため、以下の2段構えで状態同期を行っています。

1. **CatchUpNewEntity によるコンポーネントの追従**  
   スポーン時に `TimelineDirectorSystem::CatchUpNewEntity` を呼び出し、現在のシーケンスで有効なコンポーネント群を新規 Entity へ即座にアタッチ（および既存ドローンからのパラメータ同期）を行います。
2. **絶対時間（タイムスタンプ）による進行同期**  
   各コンポーネントにローカルなタイマーを持たせるのではなく、「シーケンスが開始した絶対時間」をコンポーネントに記録し、`Time::GetTotalTime()` との差分で進行状態を算出しています。

これにより、いつ追加された Entity であっても、他のドローンと全く同じシーケンス進行状態へ違和感なくシームレスに合流できるアーキテクチャを実現しています。

#### JSONによるプレハブとコンポーネントの動的生成（ComponentFactoryRegistry）

スポーンするEntityの初期コンポーネント構成は JSON ファイルで定義されています。
`ComponentFactoryRegistry` が JSON をパースして型安全なコンポーネント生成（ApplyOperation）を行うため、
`Instantiate()` にプレハブ名を渡すだけで対応するコンポーネント群が自動でアタッチされます。
各システムやマネージャに分散しがちなファクトリ処理を Registry に集約することで、コードの重複を排除しています。

```json
// data/prefab/drone_cube.json
{
  "name": "DroneCube",
  "components": {
    "PrimitiveMeshComponent": { "type": "Cube", "scale": [5.0, 5.0, 5.0] },
    "MaterialRequestComponent": { "color": [255.0, 255.0, 255.0] },
    "DampingComponent": {},
    "DroneComponent": {}
  }
}
```

```jsonc
// data/config/game_config.json (抜粋)
{
  "prefab": {
    "folderPath": "data/prefab/",
    "spawnName": "DroneCube" // 設定ファイルのプレハブ名を指定するだけ
  }
}
```

新しい種類のドローンを追加するとき、**コードを変更せず JSON を追加するだけ**で対応できます。

#### 外部フォーメーションデータの活用（ツール連携）

C++ のコード内に頂点データをハードコードせず、外部ツールやスクリプトで生成したデータをシームレスに取り込めるパイプラインを構築しています。

| フォーメーション | データ生成手法 | 概要 |
|---|---|---|
| Torus Knot / Superformula 等 | 数式生成スクリプト | 結び目曲線や自然界の形状など、数学的な数式から頂点を算出しJSON化 |
| 3Dモデル（Wireframe） | `obj_to_json.py` | 手元の `.obj` ファイルをワイヤーフレーム（一筆書き）用のJSONに変換 |
| 画像データ（PointCloud） | `image_to_json.py` | 任意の画像ファイルを解析し、色と座標を持った3D点群（PointCloud）のJSONに変換 |

これにより、点群ドローンで絵を描くといった演出も、画像をスクリプトに通して JSON を配置するだけで即座に実行可能です。

#### コンポーネント / パラメータ リファレンス

プレハブやタイムラインの JSON で指定可能なすべてのコンポーネント一覧と、各パラメータの詳細については、以下の専用ドキュメントを参照してください。

**[Component Reference を開く](docs/ComponentReference_ja.md)**

---

## 4. パフォーマンス最適化とトレードオフ

本プロジェクトでは、大規模なシミュレーションを実現するために以下の最適化方針（Data-Oriented Design）を採用しています。

- **メモリレイアウトを意識した設計**:  
  実行時のメモリアロケーションを避けるためのバッファ事前確保や、連続したメモリアクセスを保証する Component 管理など、常に CPU キャッシュヒット率を最大化する設計を徹底しています。
- **高負荷演算の徹底排除**:  
  1万機以上のEntityが毎フレーム実行する Update ループ内では、`sqrt` などの重い数学演算を避け（距離の二乗比較への置換など）、CPU負荷を極限まで抑え込んでいます。
- **並列処理を前提としたスレッドセーフティ**:  
  大量の計算を `ParallelFor` でスレッドに分割しつつ、コンポーネントの動的追加・削除時のデータ競合を防ぐため、DeferredCommand パターンを用いた安全な並列処理基盤を構築しました。

### 4.1 データ指向設計によるメモリアクセス最適化（Sparse-Dense Setの活用）

コンポーネントデータは、**Sparse-Dense Set（疎密配列）** パターンを用いて管理されています。
`ComponentArray<T>` 内部には、実際のデータを隙間なく詰めて格納する `std::vector<T>`（Dense配列）と、Entity ID から Dense 配列のインデックスを O(1) で引くための `EntitySparseSet` が用意されています。

**1. Swap-and-Pop によるメモリの連続性保証**  
Entityからコンポーネントが削除された際、Dense 配列の末尾要素を削除された位置にスワップ（移動）して隙間を埋める仕組み（Swap-and-Pop）を実装しています。これにより、データは常に連続したメモリ領域にパックされ、CPU のプリフェッチ（キャッシュ先読み）が極めて有効に機能します。

**2. ループ外での参照取得**  
各 System はループ外で `GetComponentArray<T>()` を一度だけ呼び出し、配列全体の参照を取得します。
ループ内では `GetData(entity)` を通じて O(1) でデータにアクセスできるため、ポインタのポインタを辿るような間接参照や、重いハッシュマップのルックアップコストを排除しています。

```cpp
// ループ外で配列の参照を取得（キャッシュフレンドリー）
auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
auto& velocityArray  = coordinator.GetComponentArray<VelocityComponent>();

ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
    // 連続したメモリアクセスが保証された高速なループ処理
    for (int i = startIdx; i < endIdx; ++i) {
        auto& transform = transformArray.GetData(entities[i]);
        auto& velocity  = velocityArray.GetData(entities[i]);
        // ...
    }
});
```

### 4.2 マルチスレッドと安全なECS操作（ThreadPool + ParallelFor + DeferredCommand）

`ThreadPool` を自前実装し、`std::condition_variable` によるスリープ待機でアイドル時の CPU 消費を抑えています。
各 System の Update 処理は `ParallelFor` により CPU コア数に応じてチャンク分割され、Worker スレッドに投下されます。

当初はタスクごとにスレッドを生成していましたが、生成コストが問題になったため ThreadPool 方式に移行しました。
各種 System では Entity をスレッド毎に分割し、処理を行うことで最適化を図っています。

#### DeferredCommand によるスレッドセーフな ECS 操作

`ParallelFor` のワーカースレッド内から ECS のコンポーネント追加・削除を直接呼ぶと、
`ComponentArray` の内部構造（Dense 配列・Sparse 配列）への同時書き込みが発生し、データ競合となります。

これを防ぐため、**DeferredCommand パターン**を採用しています。

```
ワーカースレッド内
  └─ coordinator.DeferAssignComponent<T>(entity, component); や 
     coordinator.DeferRemoveComponent<T>(entity); を呼び出す
           ↓
メインスレッド（各 UpdatePhase() の完了時）
  └─ 内部の FlushCommands() が走り、全スレッドのコンテキストを順次実行
```

`Coordinator` に遅延処理用のユーティリティ関数を持たせており、これらを呼び出すだけで内部的にスレッド固有のバッファ（`ThreadCommandContext`）へ自動的にキューイングされるため、競合は発生しません。
これにより、並列ループ内からでも透過的かつ安全にコンポーネントの追加・削除などの構造的変更を遅延実行できます。

### 4.3 群衆シミュレーション最適化

Boids のような群衆シミュレーションでは、周囲の Entity の状態を参照しながら挙動を決定する必要があります。
そのため、通常の ECS System よりも参照コストが高くなりやすく、大量 Entity 環境では大きな負荷要因となります。  
本プロジェクトでは、この問題に対して空間分割とキャッシュ最適化を実装しています。

#### **SpatialGrid**

SpatialGrid では、3次元空間を複数のグリッドセルへ分割し、Entity を所属セルごとに管理しています。

BoidsSystem では、全 Entity を総当たりで探索するのではなく、

- 自身が所属するセル
- 隣接セル

のみを探索対象としています。
これにより、近傍探索の計算量を大幅に削減しています。

#### **BoidCacheSystem**

BoidsSystem では、他 Entity の Position や Velocity を頻繁に参照します。
しかし ECS の特性上、参照対象 Entity のメモリ位置は必ずしも連続しておらず、
キャッシュミスによるメモリアクセスコストが発生します。

そこで BoidCacheSystem では、Boids 計算に必要なデータを事前にフラットな連続メモリへ集約し、
同一セル内の Entity を連続アクセスできるキャッシュを構築しています。

これにより、

- メモリ局所性の向上
- キャッシュミスの削減
- BoidsSystem の走査効率改善

を実現しています。

#### **タイムスライス（更新分散）**

空間分割とキャッシュを行っても、全 Entity が毎フレーム厳密な近傍探索（距離計算など）を行うと、数万機レベルでは CPU に大きな負荷がかかります。
そこで `BoidsSystem` では**タイムスライス**の手法を取り入れ、各 Entity の近傍計算（Force の算出）を毎フレームではなく、一定のインターバル（`updateInterval`）ごとに行うよう最適化しています。

- 計算をスキップしたフレームでは、**前回の計算でキャッシュした方向ベクトル（`cachedDirection`）をそのまま Force として再適用**します。
- 更新タイミングが全 Entity で集中しないよう、インターバルにランダムな乱数スケール（`updateIntervalScale`）を掛けることで、毎フレームの計算負荷を均等に分散（Load Balancing）させています。

群衆全体の動きはフレーム間で緩やかに変化するため、数フレーム間計算を使い回しても視覚的な違和感はほとんど生じず、この手法により大規模環境でのフレームレート低下を劇的に防いでいます。

### 4.4 描画最適化（GPU Instancing）

当初はEntity個数分の描画リクエストを発行していましたが、全Entityの頂点データを1つのバッファにまとめて一括転送するダイナミックバッチングを経て、現在は **GPUインスタンシング**に移行しています。  
インスタンスごとのトランスフォームおよびカラーデータを単一の構造化バッファに格納し、シェーダー側でインスタンスIDから参照することで、Entity数によらずドローコールを最小限に抑えています。  
これにより、描画込みで 100,000 Entity が Vsync 上限で動作するスケーラビリティを実現しています。

#### スレッドローカルバッファによるロックフリーな描画データ収集（`PrimitiveMeshRenderSystem`）

描画処理においてもマルチスレッドの恩恵を最大限に引き出すため、`PrimitiveMeshRenderSystem` では **スレッドローカルバッファ** パターンを採用しています。  
描画データの収集から GPU への送信まで、以下の6ステップで構成されます。

```
[1] 並列収集フェーズ（ParallelFor / ロックフリー）
    各ワーカースレッドが自スレッド専用のバッファ（ThreadLocalBuffer）に
    InstanceData（ワールド行列・カラー）を書き込む
    ※ ワールド行列は TRS(position, rotation, mesh.scale) として構築
          ↓
[2] サイズ事前計算
    バッチ（[Shader × Mesh]の組み合わせ）ごとに全スレッドバッファの
    エントリ数を合算し、最終バッファの容量を予約
          ↓
[3] 最終バッファのリセット（Clear + Reserve）
    フレームループ内での realloc を回避するため、前フレームのバッファを
    クリアしてから必要数だけ容量確保
          ↓
[4] マージフェーズ（シングルスレッド / Gather）
    全スレッドのローカルバッファを、バッチ別の最終バッファへ連結
          ↓
[5] 送信フェーズ（DrawInstanced × バッチ数）
    [Shader × Mesh]の組み合わせごとに RenderManager::DrawInstanced を
    1回呼び出すだけで全Entityの描画が完了
          ↓
[6] クリーンアップ
    スレッドローカルバッファをクリア（メモリは解放せず再利用）
```

**ロックフリーを実現する仕組み**: 収集フェーズでは各ワーカースレッドが `ThreadPool::GetCurrentWorkerIndex()` で自スレッドのバッファを特定し、排他制御なしに書き込みます。マージはすべてのワーカーが完了したあとにメインスレッドが実行するため、ミューテックスが不要です。

**フレーム内アロケーションの排除**: `Init()` 時点でスレッド数に応じた容量を事前確保（`reserve`）しているため、フレームループ内での `realloc` が発生しません。マージ先の最終バッファも毎フレームの `clear()` + `reserve()` で済み、OS からのメモリ確保を伴うアロケーションを回避しています。

**[Shader × Mesh] バッチインデックスによる描画の最適化**: `ThreadLocalBuffer` は `NUM_SHADERS × NUM_MESH_TYPES` 個のフラットな配列を持ちます（`GetBatchIndex(shader, mesh)` で1次元インデックスに変換）。各 Entity の `MaterialComponent::shaderType` と `PrimitiveMeshComponent::type` の組み合わせを Key としてバッファに振り分けることで、**同一シェーダー・同一メッシュ種別の全 Entity を 1 回の `DrawInstanced` で描画** できます。これにより State Change（シェーダー切り替えやメッシュ切り替え）のオーバーヘッドを最小限に抑えています。

### 4.5 メモリ使用量のトレードオフ

本実装では、ECS のキャッシュ効率と実行時パフォーマンスを優先するため、
各 ComponentArray が最大 Entity 数分の容量を事前確保しています。

これにより、

- 実行時の realloc
- ポインタ無効化
- メモリ断片化
- ランダムアクセス増加

を抑制し、高負荷シミュレーション環境でも安定したメモリアクセス性能を維持しています。

一方で、パフォーマンスを優先して各ComponentArrayを最大Entity数分事前確保しているため、メモリ使用量は比較的大きくなります。
これは Data-Oriented Design（DOD）によるキャッシュ効率最適化を優先した設計上のトレードオフです。  
物理メモリが不足した環境では、OS のページング（スワップ）が発生し、大幅なパフォーマンス低下を引き起こす可能性があります。

### 4.6 パフォーマンス検証結果とボトルネック分析

Boids、Steering、Force Accumulation、Animation などを含む、大規模シミュレーション環境で検証を行っています。

#### 計測結果（※検証環境：Intel Core Ultra 7 265K / RAM 64GB）

| 条件 | シングルスレッド | マルチスレッド |
|---|---|---|
| 100,000 Entity（Rendering ON / GPU Instancing・シンプルな処理） | 約 100 FPS | **144 FPS**（V-Sync 上限） |
| 100,000 Entity（Rendering ON / GPU Instancing・Boids などの複雑なシミュレーション） | 約 10 FPS | **約 120 FPS** |

#### 計測結果から確認できること

- GPU Instancing 導入により、描画込みで 100,000 Entity をシンプルな処理では Vsync 上限（144 FPS）で動作可能
- Boids などの近傍探索を伴う複雑なシミュレーションでは CPU 負荷が支配的となり、マルチスレッド時は約 120 FPS で動作
- シングルスレッド時、Boids 実行中は 10 FPS 前後まで急落し、ParallelFor による負荷分散の効果が特に大きい処理であることを確認
- ECS / Physics / Boids 系処理が並列実行によってスケールしていることを確認

Boids 実行中のシングル / マルチ比較（10 FPS vs 120 FPS）から、近傍探索を伴う高負荷処理ほど並列化の効果が顕著であることを確認しています。

#### シーケンス切り替え時のスパイクについて

マルチスレッド実行時は安定かつ高パフォーマンスで動作しますが、シーケンスの切り替わり時（Boids や Formation の状態遷移時）に一時的な処理落ち（スパイク）が発生することがあります。

このスパイクの主な原因は、`TimelineDirectorSystem` による ECSの構造的な再編成コストです。
シーケンス移行時、すべてのEntityに対して一斉にコンポーネントの追加・削除（Signature の変更）が行われます。この変更が `SystemManager` を介して全 System に同期的・直列的に通知され、各 System 内部の対象 Entity キャッシュリスト（`EntitySparseSet`）への追加・削除処理が数百万回規模で一気に走るため、これが瞬間的な CPU 負荷のスパイクとして現れています。現状はシングルスレッドで動作していますが、将来的には、構造的変更を Deferred Buffer に積み、複数スレッドから安全にキューイングしてからメインスレッドで一括処理するアーキテクチャへの改修を検討しています。

#### プロファイリングによるボトルネック分析

GPU Instancing 導入後、Visual Studio Profiler で Boids 実行中（高負荷時）のホットスポットを計測しました。  
結果として、**顕著な単一ボトルネックは存在せず、CPU 時間がロジック演算全体に分散していること**が確認できています。

| 関数名 | 合計 CPU % |
|---|---|
| `BoidsSystem::Update` (lambda) | 18.96 % |
| `Quaternion::Slerp` | 5.53 % |
| `Quaternion::LookRotation` | 5.06 % |
| `PrimitiveMeshRenderSystem::Update` (lambda) | 9.17 % |
| `Quaternion::Normalize` | 4.83 % |
| `acosf_fma` | 4.12 % |
| `OpenSimplex2S::noise3_BCC` | 4.06 % |
| `cosf` / `sinf` | 各 3.9 % 前後 |
| `memcpy_repmovs` | 3.05 % |
| `SpatialBoidCacheSystem::Update` | 2.68 % |
| `JitterMovementSystem::Update` (lambda) | 2.21 % |
| `WanderSystem::Update` (lambda) | 1.85 % |

各システム・数学演算・ノイズ計算が細かく分散しており、一部を取り出して最適化してもフレーム全体への影響が限定的なフラットな分布になっています。  
これは、前段の最適化（GPU Instancing・ParallelFor・Boids タイムスライス分散・`sqrt` 回避など）が各所でバランスよく効いている結果です。

`BoidsSystem` が最大ホットスポットとなっているのは、近傍探索・3体ルール演算・クォータニオン演算（Slerp / LookRotation）のコストが組み合わさるためです。`Quaternion::Slerp` や `acosf` の処理コストはその内訳の一部として現れています。  
`PrimitiveMeshRenderSystem` の並列収集処理（9.17 %）は描画系の負荷を示しますが、GPU Instancing 導入によって旧来の Dynamic Batching 時代（DxLib 内部の `memcpy` が支配的だった）に比べ描画コストは大幅に抑制されており、ボトルネックはほぼ完全にシミュレーション側に移っています。

---

## 5. ディレクトリ構成
```
ECSEngine/                                    ... リポジトリルート
├── ECSEngine/                                ... [Static Lib] 汎用エンジン層
│   └── src/Engine/
│       ├── ECS/                            ... ECSコア実装（外部依存なし）
│       │   ├── Coordinator.h / .cpp        ... ECS全体の統括クラス
│       │   ├── ComponentManager.h          ... コンポーネント管理
│       │   ├── SystemManager.h             ... システム管理・フェーズ制御
│       │   ├── EntityManager.h / .cpp      ... Entity生成・破棄
│       │   ├── EntitySparseSet.h / .cpp    ... Sparse-Dense Set実装
│       │   ├── ComponentArray.h            ... コンポーネントデータ配列
│       │   ├── SystemPhases.h              ... フェーズ・Order定義
│       │   └── ...
│       ├── Components/                     ... 汎用コンポーネント定義
│       │   ├── CoreComponents.h
│       │   ├── PhysicsComponents.h
│       │   ├── ForceComponents.h
│       │   ├── AnimationComponents.h
│       │   └── ...
│       ├── Systems/                        ... 汎用システム実装
│       │   ├── Animation/
│       │   ├── Behavior/
│       │   ├── Force/
│       │   ├── Physics/
│       │   ├── Render/
│       │   ├── Scene/
│       │   └── Spatial/
│       ├── Math/                           ... Vector3, Quaternion, Color, Constants
│       ├── Physics/                        ... 物理演算補助
│       ├── Platform/                       ... プラットフォーム依存の薄いラッパー
│       ├── Render/                         ... GPU Instancing / 描画基盤
│       ├── Spatial/                        ... SpatialGrid（空間分割）
│       ├── Traits/                         ... ComponentTraits.h（コンパイル時トレイト）
│       ├── Debug/                          ... デバッグ表示
│       └── Utils/                          ... ThreadPool, Time
│
├── DroneShow_Game/                           ... [Static Lib] ゲーム固有ロジック
│   ├── src/Game/
│   │   ├── Game.h / Game.cpp               ... ゲームクラス（初期化・メインループ）
│   │   ├── Components/                     ... ゲーム固有のComponent定義
│   │   │   └── GameComponents.h
│   │   ├── Systems/                        ... ゲーム固有のSystem実装
│   │   │   ├── TimelineDirectorSystem.h / .cpp
│   │   │   ├── FormationSystemWireframe.h
│   │   │   └── FormationSystemPointCloud.h
│   │   ├── Managers/                       ... 各種Managerクラス
│   │   │   ├── InputManager.h
│   │   │   ├── PrefabManager.h / .cpp
│   │   │   └── TimelineManager.h / .cpp
│   │   ├── Loaders/                        ... 汎用データローダー
│   │   │   ├── DirectoryLoader.h
│   │   │   └── JsonLoader.h
│   │   ├── Registry/                       ... コンポーネント・システム登録
│   │   │   ├── ComponentRegistry.h
│   │   │   ├── SystemRegistry.h
│   │   │   ├── Components/
│   │   │   └── Systems/
│   │   ├── Parsers/                        ... JSONパーサー
│   │   ├── Traits/                         ... ゲーム固有のComponentTraits特殊化
│   │   └── Registration/                   ... 各種ファクトリ登録
│   └── data/                               ... 外部データ（コード変更なしに演出を編集可能）
│       ├── config/                         ... game_config.json（起動設定）
│       ├── formation/
│       │   ├── json/                       ... フォーメーション頂点データ（JSON）
│       │   └── obj/                        ... OBJファイル（obj_to_json.py で変換）
│       ├── prefab/                         ... Entityのプレハブ定義（JSON）
│       ├── shader/                         ... GPU Instancingシェーダー
│       └── timeline/                       ... タイムライン定義（JSON）
│
├── DroneShow_App/                            ... [Executable] エントリポイント
│   └── src/
│       └── main.cpp                        ... WinMain / アプリ起動
│
├── DroneShow_Test/                           ... [Executable] ユニットテスト（Google Test）
│   ├── Engine/
│   │   ├── ECS/                            ... ECSコアのテスト
│   │   └── Systems/                        ... Engineシステムのテスト
│   └── Game/
│       └── Systems/                        ... Gameシステムのテスト
│
├── External/                               ... 外部ライブラリ
├── docs/                                   ... クラス図・フローチャートなどのドキュメント
└── script/                                 ... フォーメーション生成スクリプト（Python）
```

---

## 6. 今後やりたいこと

**[描画最適化]**
- インスタンシングシェーダーのさらなる最適化（LOD / カリング対応など）

**[ECS最適化]**
- シーケンス切り替え時のスパイク処理軽減（構造的変更の遅延評価化）
- SIMD命令を用いた演算のさらなる最適化

**[機能拡張]**
- カメラアニメーション機能を実装し、シーケンスデータと連動した演出強化を行う

**[開発環境改善]**
- 各種Systemのユニットテスト網羅率を向上させる
- 設定ファイルのホットリロード機能を実装し、パラメータ調整のイテレーション速度を向上させる
- エラー処理のLoggerを実装する

---

## 7. コントリビューションについて

本リポジトリは主に学習および実装例の提示を目的として公開しているため、**原則としてPull Request（PR）の受け付けは行っておりません**
バグの報告や動作・実装に関するご質問等がありましたら、Issue にてお知らせいただけますと幸いです。

---

## 8. ライセンス

本プロジェクトは [MIT License](LICENSE) のもとで公開されています。

### サードパーティライブラリ

本プロジェクトでは以下の外部ライブラリを使用しています。各ライブラリの詳細なライセンス情報については、[THIRD_PARTY_LICENSES](THIRD_PARTY_LICENSES) ファイルをご参照ください。

- **[DxLib](https://dxlib.xsrv.jp/)**: フリーソフトライセンス（内部で zlib, libpng, Ogg Vorbis/Theora 等を使用）
- **[nlohmann/json](https://github.com/nlohmann/json)**: MIT License
- **[OpenSimplex Noise](https://github.com/KdotJPG/OpenSimplex2)**: The Unlicense / CC0 1.0 Universal（パブリックドメイン）
- **[Google Test](https://github.com/google/googletest)**: BSD-3-Clause License

### アセット・データ

本プロジェクトのデモで点群（PointCloud）データの生成元として使用している一部の画像は、外部機関が提供するパブリックドメインのオープンアクセス画像を利用しています。

- **[The Metropolitan Museum of Art (The Met)](https://www.metmuseum.org/)**: CC0 1.0 Universal (Public Domain)
