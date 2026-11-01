import { AmbientLight } from 'three';
import { AnimationAction } from 'three';
import { AnimationClip } from 'three';
import { AnimationMixer } from 'three';
import { Audio as Audio_3 } from 'three';
import { AudioListener as AudioListener_2 } from 'three';
import { BufferGeometry } from 'three';
import { Camera as Camera_2 } from 'three';
import type { clone } from 'three/examples/jsm/utils/SkeletonUtils.js';
import { Color } from 'three';
import { ColorSpace } from 'three';
import type { DeepReadonly } from 'ts-essentials';
import { DirectionalLight } from 'three';
import type { DRACOLoader } from 'three/examples/jsm/loaders/DRACOLoader.js';
import { Euler } from 'three';
import { Event as Event_2 } from 'three';
import type { GLTF } from 'three/examples/jsm/loaders/GLTFLoader.js';
import type { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';
import { Group } from 'three';
import { Intersection } from 'three';
import { MagnificationTextureFilter } from 'three';
import { Material as Material_2 } from 'three';
import { Matrix4 } from 'three';
import { Mesh } from 'three';
import { MeshBasicMaterial } from 'three';
import { MeshPhysicalMaterial } from 'three';
import { MeshStandardMaterial } from 'three';
import { MinificationTextureFilter } from 'three';
import type { Object3D as Object3D_2 } from 'three';
import { Object3DEventMap } from 'three';
import { OrthographicCamera } from 'three';
import { PerspectiveCamera } from 'three';
import { PointLight } from 'three';
import { PositionalAudio } from 'three';
import { Quaternion as Quaternion_2 } from 'three';
import { Raycaster } from 'three';
import { RectAreaLight } from 'three';
import type { RGBELoader } from 'three/examples/jsm/loaders/RGBELoader.js';
import { Scene } from 'three';
import { ShaderMaterial } from 'three';
import { ShadowMaterial as ShadowMaterial_2 } from 'three';
import { SpotLight } from 'three';
import { Texture } from 'three';
import { TextureLoader } from 'three';
import { Vector3 } from 'three';
import { Vector4 } from 'three';
import { VideoTexture } from 'three';
import { WebGLRenderer } from 'three';
import { WebGLRenderList } from 'three';
import { WebGLRenderTarget } from 'three';

declare global {
    export interface EcsEventTypes {
    }
}

declare global {
    export interface EcsEventTypes {
        [events.RECORDER_VIDEO_ERROR]: Error;
        [events.RECORDER_VIDEO_READY]: {
            videoBlob: Blob;
        };
        [events.RECORDER_SCREENSHOT_READY]: Blob;
        [events.RECORDER_FINALIZE_PROGRESS]: ProgressInfo;
        [events.RECORDER_PREVIEW_READY]: {
            videoBlob: Blob;
        };
        [events.RECORDER_PROCESS_FRAME]: FrameInfo;
        [UiEvents.UI_CLICK]: {
            x: number;
            y: number;
        };
        [UiEvents.UI_PRESSED]: {
            x: number;
            y: number;
        };
        [UiEvents.UI_RELEASED]: {
            x: number;
            y: number;
        };
        [UiEvents.UI_HOVER_START]: UiEvents.UiHoverEvent;
        [UiEvents.UI_HOVER_END]: UiEvents.UiHoverEvent;
        [PointerEvents.SCREEN_TOUCH_START]: PointerEvents.ScreenTouchStartEvent;
        [PointerEvents.SCREEN_TOUCH_MOVE]: PointerEvents.ScreenTouchMoveEvent;
        [PointerEvents.SCREEN_TOUCH_END]: PointerEvents.ScreenTouchEndEvent;
        [PointerEvents.GESTURE_START]: PointerEvents.GestureStartEvent;
        [PointerEvents.GESTURE_MOVE]: PointerEvents.GestureMoveEvent;
        [PointerEvents.GESTURE_END]: PointerEvents.GestureEndEvent;
        [InputEvents.GAMEPAD_CONNECTED]: InputEvents.GamepadConnectedEvent;
        [InputEvents.GAMEPAD_DISCONNECTED]: InputEvents.GamepadDisconnectedEvent;
        [CameraEvents.ACTIVE_CAMERA_CHANGE]: {
            camera: CameraObject;
        };
        [CameraEvents.ACTIVE_CAMERA_EID_CHANGE]: {
            eid: Eid;
        };
        [CameraEvents.XR_CAMERA_STOP]: {};
        [CameraEvents.XR_CAMERA_EDIT]: {
            camera: CameraObject;
        };
        [events.SPLAT_MODEL_LOADED]: {
            model: THREE_TYPES.Object3D;
        };
        [events.GLTF_MODEL_LOADED]: {
            model: THREE_TYPES.Group;
        };
        [events.GLTF_ANIMATION_FINISHED]: {
            name: string;
        };
        [events.GLTF_ANIMATION_LOOP]: {
            name: string;
        };
        [events.AUDIO_END]: undefined;
        [events.POSITION_ANIMATION_COMPLETE]: undefined;
        [events.SCALE_ANIMATION_COMPLETE]: undefined;
        [events.ROTATE_ANIMATION_COMPLETE]: undefined;
        [events.CUSTOM_VEC3_ANIMATION_COMPLETE]: undefined;
        [events.CUSTOM_PROPERTY_ANIMATION_COMPLETE]: undefined;
        'audio-error': {
            error: Error;
        };
        [events.VIDEO_CAN_PLAY_THROUGH]: {
            src: string;
        };
        [events.VIDEO_END]: {
            src: string;
        };
        'video-error': {
            error: Error;
        };
        [PhysicsEvents.COLLISION_START_EVENT]: {
            other: Eid;
        };
        [PhysicsEvents.COLLISION_END_EVENT]: {
            other: Eid;
        };
        [PhysicsEvents.UPDATE_EVENT]: {};
        [events.LOCATION_SPAWNED]: LocationSpawnedEvent;
    }
}

declare type Action = {
    name: string;
    bindings: Binding[];
};

export declare interface ActiveCameraChangeEvent {
    camera: CameraObject;
}

export declare interface ActiveCameraEidChangeEvent {
    eid: Eid;
}

declare type AlignContent = 'flex-start' | 'flex-end' | 'center' | 'stretch' | 'space-between' | 'space-around' | 'space-evenly';

declare type AlignItems = 'flex-start' | 'flex-end' | 'center' | 'stretch' | 'baseline';

declare namespace api {
    export {
        ready,
        isReady,
        WorldAttribute,
        RootAttribute,
        SchemaOf,
        Entity,
        LocationSpawnedEvent,
        World,
        createWorld,
        registerBehavior,
        unregisterBehavior,
        registerComponent,
        getBehaviors,
        getAttribute,
        listAttributes,
        Eid,
        Schema,
        ReadData,
        WriteData,
        math,
        createStateMachine,
        deleteStateMachine,
        tickStateMachine,
        defineState,
        defineStateGroup,
        defineTrigger,
        State,
        StateGroup,
        MachineId,
        StateMachineDefinition,
        StateMachineDefiner,
        BaseMachineDefProps,
        Audio_2 as Audio,
        Camera,
        Face,
        ImageTarget,
        SphereGeometry,
        BoxGeometry,
        PlaneGeometry,
        CapsuleGeometry,
        ConeGeometry,
        CylinderGeometry,
        TetrahedronGeometry,
        PolyhedronGeometry,
        CircleGeometry,
        RingGeometry,
        TorusGeometry,
        FaceGeometry,
        GltfModel,
        Hidden,
        Light,
        Material,
        HiderMaterial,
        ShadowMaterial,
        UnlitMaterial,
        VideoMaterial,
        Persistent,
        Shadow,
        Splat,
        Position,
        Scale,
        Quaternion,
        ThreeObject,
        Ui,
        VideoControls,
        input,
        ScreenTouchStartEvent,
        ScreenTouchMoveEvent,
        ScreenTouchEndEvent,
        GestureStartEvent,
        GestureMoveEvent,
        GestureEndEvent,
        GamepadConnectedEvent,
        GamepadDisconnectedEvent,
        UiClickEvent,
        UiHoverEvent,
        eid,
        f32,
        f64,
        i32,
        ui8,
        ui32,
        string,
        boolean,
        defineSystemQuery,
        defineSystem,
        defineQuery,
        enterQuery,
        changedQuery,
        exitQuery,
        lifecycleQueries,
        physics,
        ColliderType,
        ColliderShape,
        Collider,
        assets,
        audio,
        CustomPropertyAnimation,
        PositionAnimation,
        ScaleAnimation,
        RotateAnimation,
        CustomVec3Animation,
        FollowAnimation,
        LookAtAnimation,
        ParticleEmitter,
        ParticlesSchema,
        CameraEvents,
        XR_FACE_FOUND,
        XR_FACE_UPDATED,
        XR_FACE_LOST,
        ActiveCameraChangeEvent,
        ActiveCameraEidChangeEvent,
        OrbitControls,
        FlyController,
        FaceAnchor,
        FaceMeshAnchor,
        FaceAttachment,
        Disabled,
        events,
        video
    }
}
export default api;

declare type Asset = {
    type: 'asset';
    asset: string;
};

declare type Asset_2 = {
    data: Blob;
    remoteUrl?: string;
    localUrl: string;
};

declare type AssetManager = {
    load: (request: AssetRequest) => Promise<Asset_2>;
    clear: (request: AssetRequest) => void;
    loadSync: (request: AssetRequest) => Asset_2;
    setAssetManifest: (newManifest: AssetManifest) => void;
    resolveAsset: (assetPath: string) => string | null;
    getStatistics: () => AssetStatistics;
};

declare type AssetManifest = StoredAssetManifest | AssetManifestMappings;

declare type AssetManifestMappings = {
    [filePath: string]: string;
} & {
    assets?: never;
};

declare type AssetRequest = {
    id?: Eid;
    url: string;
};

export declare const assets: AssetManager;

declare type AssetStatistics = {
    pending: number;
    complete: number;
    total: number;
};

declare type Attributes = RootAttribute<{}>[];

export declare const audio: {
    getCurrentTime: (world: World, eid: Eid) => number;
    setCurrentTime: (world: World, eid: Eid, time: number) => void;
};

declare const Audio_2: RootAttribute<    {
url: "string";
volume: "f32";
loop: "boolean";
paused: "boolean";
pitch: "f32";
positional: "boolean";
refDistance: "f32";
rolloffFactor: "f32";
distanceModel: "string";
maxDistance: "f32";
}>;
export { Audio_2 as Audio }

declare type AudioControls = {
    mute: () => void;
    unmute: () => void;
    pause: () => void;
    play: () => void;
    setVolume: (newVolume: number) => void;
};

declare type AudioSettings = {
    src?: Resource;
    volume?: number;
    loop?: boolean;
    paused?: boolean;
    pitch?: number;
    positional?: boolean;
    refDistance?: number;
    rolloffFactor?: number;
    distanceModel?: DistanceModel;
    maxDistance?: number;
};

declare type BackgroundSize = 'contain' | 'cover' | 'stretch' | 'nineslice';

declare type BaseGraphObject = {
    id: ObjectId;
    name?: string;
    parentId?: string;
    prefab?: true;
    position: Vec3Tuple;
    rotation: Vec4Tuple;
    scale: Vec3Tuple;
    geometry: Geometry;
    material: Material_3;
    gltfModel?: GltfModel_2 | null | undefined;
    splat?: Splat_2 | null | undefined;
    collider?: Collider_2 | null | undefined;
    audio?: AudioSettings | null | undefined;
    videoControls?: VideoControlsGraphSettings | null | undefined;
    ui?: UiGraphSettings | null | undefined;
    hidden?: boolean;
    shadow?: Shadow_2;
    light?: Light_2;
    camera?: Camera_3;
    face?: Face_2;
    imageTarget?: ImageTarget_2;
    map?: Map_2;
    mapTheme?: MapTheme;
    mapPoint?: MapPoint;
    components: Record<GraphComponent['id'], GraphComponent>;
    ephemeral?: boolean;
    disabled?: true;
    persistent?: true;
    order?: number;
};

export declare interface BaseMachineDefProps {
    world: World;
    eid: Eid;
    entity: Entity;
}

declare type BaseSchema<S extends ExtendedSchema<Schema>> = {
    [K in keyof S]: S[K] extends ExtendedSchemaValue<infer T> ? T : never;
};

declare interface BaseWorld {
    time: Time;
    allEntities: Set<Eid>;
    eidToEntity: Map<Eid, Entity>;
    three: ThreeState;
    insertRaycastStage: (stage: RaycastStage, idx: number) => void;
    /** @deprecated */
    scene: Scene;
}

declare type BasicMaterial = {
    type: 'basic';
    color: string;
    textureSrc?: string | Resource;
    roughness?: number;
    metalness?: number;
    opacity?: number;
    normalScale?: number;
    emissiveIntensity?: number;
    roughnessMap?: string | Resource;
    metalnessMap?: string | Resource;
    opacityMap?: string | Resource;
    normalMap?: string | Resource;
    emissiveMap?: string | Resource;
    emissiveColor?: string;
    side?: Side;
    blending?: MaterialBlending;
    repeatX?: number;
    repeatY?: number;
    offsetX?: number;
    offsetY?: number;
    wrap?: TextureWrap;
    depthTest?: boolean;
    depthWrite?: boolean;
    wireframe?: boolean;
    forceTransparent?: boolean;
    textureFiltering?: TextureFiltering;
    mipmaps?: boolean;
};

declare const behaviors: WorldBehavior[];

declare type Binding = {
    input: string;
    modifiers: string[];
};

export declare const boolean = "boolean";

export declare const BoxGeometry: RootAttribute<    {
width: "f32";
height: "f32";
depth: "f32";
}>;

declare type BoxGeometry_2 = {
    type: 'box';
    width: number;
    height: number;
    depth: number;
};

declare type Callback = () => void;

declare type Callback_2 = () => void;

export declare const Camera: RootAttribute<    {
type: "string";
fov: "f32";
zoom: "f32";
left: "i32";
right: "i32";
top: "i32";
bottom: "i32";
xrCameraType: "string";
phone: "string";
desktop: "string";
headset: "string";
nearClip: "f32";
farClip: "f32";
leftHandedAxes: "boolean";
uvType: "string";
direction: "string";
disableWorldTracking: "boolean";
enableLighting: "boolean";
enableWorldPoints: "boolean";
scale: "string";
enableVps: "boolean";
mirroredDisplay: "boolean";
meshGeometryFace: "boolean";
meshGeometryEyes: "boolean";
meshGeometryIris: "boolean";
meshGeometryMouth: "boolean";
enableEars: "boolean";
maxDetections: "i32";
}>;

declare type Camera_3 = {
    type?: CameraType;
    left?: number;
    right?: number;
    top?: number;
    bottom?: number;
    fov?: number;
    zoom?: number;
    nearClip?: number;
    farClip?: number;
    xr?: XrConfig;
};

declare type CameraDirectionType = 'front' | 'back';

export declare const CameraEvents: {
    ACTIVE_CAMERA_CHANGE: "active-camera-change";
    ACTIVE_CAMERA_EID_CHANGE: "active-camera-entity-change";
    XR_CAMERA_EDIT: "xr-camera-edit";
    XR_CAMERA_STOP: "xr.stop";
    CAMERA_TRANSFORM_UPDATE: "cameraupdate";
};

declare type CameraManager = {
    getActiveEid: () => Eid;
    setActiveEid: (eid: Eid) => void;
    notifyCameraAdded: (eid: Eid) => void;
    notifyCameraRemoved: (eid: Eid) => void;
    attach: () => void;
    detach: () => void;
};

declare type CameraObject = OrthographicCamera | PerspectiveCamera;

declare type CameraType = 'perspective' | 'orthographic';

export declare const CapsuleGeometry: RootAttribute<    {
radius: "f32";
height: "f32";
}>;

declare type CapsuleGeometry_2 = {
    type: 'capsule';
    radius: number;
    height: number;
};

export declare const changedQuery: (t: RootQuery) => Query;

export declare const CircleGeometry: RootAttribute<    {
radius: "f32";
}>;

declare type CircleGeometry_2 = {
    type: 'circle';
    radius: number;
};

export declare const Collider: RootAttribute<ColliderSchema>;

declare type Collider_2 = {
    type?: ColliderType_2;
    geometry?: BoxGeometry_2 | SphereGeometry_2 | PlaneGeometry_2 | CapsuleGeometry_2 | ConeGeometry_2 | CylinderGeometry_2 | {
        type: 'auto';
    };
    mass?: number;
    linearDamping?: number;
    angularDamping?: number;
    friction?: number;
    rollingFriction?: number;
    spinningFriction?: number;
    restitution?: number;
    eventOnly?: boolean;
    lockXPosition?: boolean;
    lockYPosition?: boolean;
    lockZPosition?: boolean;
    lockXAxis?: boolean;
    lockYAxis?: boolean;
    lockZAxis?: boolean;
    gravityFactor?: number;
    highPrecision?: boolean;
    offsetX?: number;
    offsetY?: number;
    offsetZ?: number;
    offsetQuaternionX?: number;
    offsetQuaternionY?: number;
    offsetQuaternionZ?: number;
    offsetQuaternionW?: number;
    simplificationMode?: SimplificationMode;
};

declare type ColliderSchema = typeof colliderSchema;

declare const colliderSchema: {
    readonly width: "f32";
    readonly height: "f32";
    readonly depth: "f32";
    readonly radius: "f32";
    readonly mass: "f32";
    readonly linearDamping: "f32";
    readonly angularDamping: "f32";
    readonly friction: "f32";
    readonly restitution: "f32";
    readonly gravityFactor: "f32";
    readonly offsetX: "f32";
    readonly offsetY: "f32";
    readonly offsetZ: "f32";
    readonly offsetQuaternionX: "f32";
    readonly offsetQuaternionY: "f32";
    readonly offsetQuaternionZ: "f32";
    readonly offsetQuaternionW: "f32";
    readonly shape: "ui32";
    readonly type: "ui8";
    readonly eventOnly: "boolean";
    readonly lockXPosition: "boolean";
    readonly lockYPosition: "boolean";
    readonly lockZPosition: "boolean";
    readonly lockXAxis: "boolean";
    readonly lockYAxis: "boolean";
    readonly lockZAxis: "boolean";
    readonly highPrecision: "boolean";
    readonly simplificationMode: "string";
};

export declare const ColliderShape: {
    readonly Box: 0;
    readonly Sphere: 1;
    readonly Plane: 2;
    readonly Capsule: 3;
    readonly Cone: 4;
    readonly Cylinder: 5;
    readonly Circle: 6;
};

export declare const ColliderType: {
    readonly Static: 0;
    readonly Dynamic: 1;
    readonly Kinematic: 2;
};

declare type ColliderType_2 = 'static' | 'dynamic' | 'kinematic';

declare const COLLISION_END_EVENT: "physics-collision-end";

declare const COLLISION_START_EVENT: "physics-collision-start";

declare type Color_2 = {
    type: 'color';
    color?: string;
};

declare type ComponentCallbackArgs<S extends Schema, D extends Schema> = {
    schema: WriteData<S>;
    data: WriteData<D>;
};

declare type ComponentCursor<S extends Schema, D extends Schema> = {
    eid: Eid;
    schema: WriteData<S>;
    data: WriteData<D>;
    schemaAttribute: WorldAttribute<S>;
    dataAttribute: WorldAttribute<D>;
};

declare type ComponentRegistration<ES extends ExtendedSchema<Schema>, ED extends ExtendedSchema<Schema>> = {
    name: string;
    /**
     * Add data that can be configured on the component.
     */
    schema?: ES;
    /**
     * Add defaults for the schema fields.
     */
    schemaDefaults?: Partial<ReadData<BaseSchema<ES>>>;
    /**
     * Add data that cannot be configured outside of the component.
     */
    data?: ED;
    /**
     * Runs when the component is added to an entity.
     */
    add?: (w: World, cursor: ComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void;
    /**
     * Runs every frame for each entity.
     */
    tick?: (w: World, cursor: ComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void;
    /**
     * Runs when the component is removed from an entity.
     */
    remove?: (w: World, cursor: RemovedComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void;
    /**
     * Define stateful behaviors such as event handling and transitions.
     */
    stateMachine?: ComponentStateMachineDefinition<BaseSchema<ES>, BaseSchema<ED>> | ComponentStateMachineDefiner<BaseSchema<ES>, BaseSchema<ED>>;
};

declare type ComponentStateMachineDefiner<S extends Schema, D extends Schema> = (props: ComponentStateMachineDefProps<S, D>) => void;

declare type ComponentStateMachineDefinition<S extends Schema, D extends Schema> = Omit<StateMachineDefinition<ComponentCallbackArgs<S, D>>, 'prepareCallback'>;

declare interface ComponentStateMachineDefProps<S extends Schema, D extends Schema> extends BaseMachineDefProps {
    schemaAttribute: WorldAttribute<S>;
    dataAttribute: WorldAttribute<D>;
    defineState: (name: string) => IStateDefiner<ComponentCallbackArgs<S, D>>;
    defineStateGroup: (substates?: Array<StateId | IStateGroupDefiner<unknown>>) => IStateGroupDefiner<ComponentCallbackArgs<S, D>>;
}

export declare const ConeGeometry: RootAttribute<    {
radius: "f32";
height: "f32";
}>;

declare type ConeGeometry_2 = {
    type: 'cone';
    radius: number;
    height: number;
};

declare const createInputListener: (events: Events, element: HTMLElement) => {
    api: InputListenerApi;
    handleGamepadLoop: (forceGamepadUpdate?: boolean) => void;
    attach: () => void;
    detach: () => void;
};

declare const createPointerListener: (stages: RaycastStage[], events: Events, element: HTMLElement) => PointerApi;

/**
 * Create a state machine
 * @param world the world to create the state machine in
 * @param eid the entity that owns the state machine
 * @param definition the state machine definition. This can be either an object or a function that
 *                   generate the definition object
 * @returns the id of the created state machine
 */
export declare const createStateMachine: <CallbackArgument = void>(world: World, eid: Eid, definition: StateMachineDefinition<CallbackArgument> | StateMachineDefiner) => MachineId;

export declare const createWorld: (scene: Scene, renderer: WebGLRenderer, camera: CameraObject) => World;

export declare const CustomPropertyAnimation: RootAttribute<BaseSchema<    {
attribute: "string";
property: "string";
autoFrom: "boolean";
from: "f32";
to: "f32";
duration: "f32";
loop: "boolean";
reverse: "boolean";
easeIn: "boolean";
easeOut: "boolean";
easingFunction: "string";
target: "eid";
}>>;

/**
 * a trigger that transitions instantly when activated.
 * @param caller the function to trigger the transition
 */
declare type CustomTrigger = {
    type: 'custom';
    handle: TriggerHandle;
};

export declare const CustomVec3Animation: RootAttribute<BaseSchema<    {
attribute: "string";
autoFrom: "boolean";
fromX: "f32";
fromY: "f32";
fromZ: "f32";
toX: "f32";
toY: "f32";
toZ: "f32";
duration: "f32";
loop: "boolean";
reverse: "boolean";
easeIn: "boolean";
easeOut: "boolean";
easingFunction: "string";
target: "eid";
}>>;

export declare const CylinderGeometry: RootAttribute<    {
radius: "f32";
height: "f32";
}>;

declare type CylinderGeometry_2 = {
    type: 'cylinder';
    radius: number;
    height: number;
};

declare type DataForEvent<EVENT> = EVENT extends keyof EcsEventTypes_2 ? EcsEventTypes_2[EVENT] : unknown;

export declare const defineQuery: (terms: RootAttribute<any>[]) => RootQuery;

/**
 * Function to define a new state
 * @param name the name of the state
 * @returns a new state
 */
export declare const defineState: <CallbackArgument = void>(name: string) => IStateDefiner<CallbackArgument>;

/**
 * Function to define a new group
 * @param substates the substates of the group (leaving blank will capture all states)
 * @returns a new group
 */
export declare const defineStateGroup: <CallbackArgument = void>(substates?: Array<StateId | IStateGroupDefiner<unknown>>) => IStateGroupDefiner<CallbackArgument>;

export declare const defineSystem: <T extends Attributes>(terms: T, callback: SystemCallback<T>) => (world: World) => void;

export declare const defineSystemQuery: <T extends Attributes>(terms: T) => SystemQuery<T>;

/**
 * define a custom trigger that can be called to cause a transition
 * @returns a new custom trigger definition
 */
export declare const defineTrigger: () => TriggerHandle;

export declare const deleteStateMachine: (world: World, machineId: MachineId) => void;

declare type DeviceSupportType = 'AR' | 'VR' | '3D' | 'disabled';

declare type Direction = 'ltr' | 'rtl';

export declare const Disabled: RootAttribute<{}>;

declare const dispatchPhysicsEvents: (world: World, events: InternalEvents) => void;

declare type Display = 'none' | 'flex';

declare type DistanceModel = 'exponential' | 'inverse' | 'linear';

export declare type Ecs = typeof api;

declare type EcsEventTypes_2 = globalThis.EcsEventTypes;

declare interface EcsRenderOverride {
    engage(): void;
    disengage(): void;
    render(dt: number): void;
}

declare type EcsTextureKey = 'textureSrc' | 'roughnessMap' | 'metalnessMap' | 'normalMap' | 'opacityMap' | 'emissiveMap';

declare type EffectsManager = {
    setFog: (fog: DeepReadonly<Fog> | undefined) => void;
    getFog: () => DeepReadonly<Fog> | undefined;
    setSky: (sky: DeepReadonly<EffectsManagerSky> | undefined) => void;
    getSky: () => DeepReadonly<EffectsManagerSky> | undefined;
    attach: () => void;
    detach: () => void;
};

declare type EffectsManagerSky = Sky<string>;

export declare type Eid = bigint;

export declare const eid = "eid";

declare type EidGetter = Eid | (() => Eid);

declare type ElementOf<T extends Type> = TypeToValue[T];

export declare const enterQuery: (t: RootQuery) => Query;

export declare type Entity = EntityTransformManager & {
    eid: Eid;
    get: <S extends Schema>(component: RootAttribute<S>) => ReadData<S>;
    has: <S extends Schema>(component: RootAttribute<S>) => boolean;
    set: <S extends Schema>(component: RootAttribute<S>, data: Partial<ReadData<S>>) => void;
    remove: <S extends Schema>(component: RootAttribute<S>) => void;
    reset: <S extends Schema>(component: RootAttribute<S>) => void;
    hide(): void;
    show(): void;
    isHidden(): boolean;
    disable(): void;
    enable(): void;
    isDisabled(): boolean;
    delete(): void;
    isDeleted(): boolean;
    setParent(parent: Eid | Entity | undefined | null): void;
    getChildren(): Entity[];
    getParent(): Entity | null;
    addChild(child: Eid | Entity): void;
};

declare type EntityReference = {
    type: 'entity';
    id: ObjectId;
};

declare type EntityTransformManager = {
    [K in keyof TransformManager]: FunctionWithoutEid<TransformManager[K]>;
} & {
    lookAt: (other: Eid | Entity) => void;
};

declare type EventListener_2<D = unknown> = (event: QueuedEvent<D>) => void;

declare type EventListenerForEvent<EVENT> = EventListener_2<DataForEvent<EVENT>>;

declare interface Events {
    globalId: Eid;
    addListener: <T extends string>(target: Eid, name: T, listener: EventListener_2<DataForEvent<T>>) => void;
    removeListener: (target: Eid, name: string, listener: EventListener_2) => void;
    dispatch: (target: Eid, name: string, data?: unknown) => void;
}

export declare const events: {
    readonly ACTIVE_SPACE_CHANGE: "active-space-change";
    readonly AUDIO_CAN_PLAY_THROUGH: "audio-can-play-through";
    readonly AUDIO_END: "audio-end";
    readonly VIDEO_CAN_PLAY_THROUGH: "video-can-play-through";
    readonly VIDEO_END: "video-end";
    readonly GLTF_MODEL_LOADED: "gltf-model-loaded";
    readonly GLTF_ANIMATION_FINISHED: "gltf-animation-finished";
    readonly GLTF_ANIMATION_LOOP: "gltf-animation-loop";
    readonly SPLAT_MODEL_LOADED: "splat-model-loaded";
    readonly POSITION_ANIMATION_COMPLETE: "position-animation-complete";
    readonly SCALE_ANIMATION_COMPLETE: "scale-animation-complete";
    readonly ROTATE_ANIMATION_COMPLETE: "rotate-animation-complete";
    readonly CUSTOM_VEC3_ANIMATION_COMPLETE: "vector3-animation-complete";
    readonly CUSTOM_PROPERTY_ANIMATION_COMPLETE: "animation-complete";
    readonly LOCATION_SPAWNED: "locationSpawned";
    readonly RECORDER_VIDEO_STARTED: "recorder-video-started";
    readonly RECORDER_VIDEO_STOPPED: "recorder-video-stopped";
    readonly RECORDER_VIDEO_ERROR: "recorder-video-error";
    readonly RECORDER_VIDEO_READY: "recorder-video-ready";
    readonly RECORDER_FINALIZE_PROGRESS: "recorder-finalize-progress";
    readonly RECORDER_PREVIEW_READY: "recorder-preview-ready";
    readonly RECORDER_PROCESS_FRAME: "recorder-process-frame";
    readonly RECORDER_SCREENSHOT_READY: "recorder-screenshot-ready";
    readonly REALITY_CAMERA_CONFIGURED: "reality.cameraconfigured";
    readonly REALITY_TRACKING_STATUS: "reality.trackingstatus";
    readonly REALITY_LOCATION_SCANNING: "reality.locationscanning";
    readonly REALITY_LOCATION_FOUND: "reality.locationfound";
    readonly REALITY_LOCATION_UPDATED: "reality.locationupdated";
    readonly REALITY_LOCATION_LOST: "reality.locationlost";
    readonly REALITY_MESH_FOUND: "reality.meshfound";
    readonly REALITY_MESH_LOST: "reality.meshlost";
    readonly REALITY_IMAGE_LOADING: "reality.imageloading";
    readonly REALITY_IMAGE_SCANNING: "reality.imagescanning";
    readonly REALITY_IMAGE_FOUND: "reality.imagefound";
    readonly REALITY_IMAGE_UPDATED: "reality.imageupdated";
    readonly REALITY_IMAGE_LOST: "reality.imagelost";
    readonly REALITY_READY: "realityready";
    readonly FACE_CAMERA_CONFIGURED: "facecontroller.cameraconfigured";
    readonly FACE_LOADING: "facecontroller.faceloading";
    readonly FACE_SCANNING: "facecontroller.facescanning";
    readonly FACE_FOUND: "facecontroller.facefound";
    readonly FACE_UPDATED: "facecontroller.faceupdated";
    readonly FACE_LOST: "facecontroller.facelost";
    readonly FACE_MOUTH_OPENED: "facecontroller.mouthopened";
    readonly FACE_MOUTH_CLOSED: "facecontroller.mouthclosed";
    readonly FACE_LEFT_EYE_OPENED: "facecontroller.lefteyeopened";
    readonly FACE_LEFT_EYE_CLOSED: "facecontroller.lefteyeclosed";
    readonly FACE_RIGHT_EYE_OPENED: "facecontroller.righteyeopened";
    readonly FACE_RIGHT_EYE_CLOSED: "facecontroller.righteyeclosed";
    readonly FACE_LEFT_EYEBROW_RAISED: "facecontroller.lefteyebrowraised";
    readonly FACE_LEFT_EYEBROW_LOWERED: "facecontroller.lefteyebrowlowered";
    readonly FACE_RIGHT_EYEBROW_RAISED: "facecontroller.righteyebrowraised";
    readonly FACE_RIGHT_EYEBROW_LOWERED: "facecontroller.righteyebrowlowered";
    readonly FACE_RIGHT_EYE_WINKED: "facecontroller.righteyewinked";
    readonly FACE_LEFT_EYE_WINKED: "facecontroller.lefteyewinked";
    readonly FACE_BLINKED: "facecontroller.blinked";
    readonly FACE_INTERPUPILLARY_DISTANCE: "facecontroller.interpupillarydistance";
    readonly FACE_EAR_POINT_FOUND: "facecontroller.earpointfound";
    readonly FACE_EAR_POINT_LOST: "facecontroller.earpointlost";
    readonly HAND_CAMERA_CONFIGURED: "handcontroller.cameraconfigured";
    readonly LAYERS_CAMERA_CONFIGURED: "layerscontroller.cameraconfigured";
};

/**
 * a trigger that transitions to the next state when an event is received
 * @param type a constant string to identify the trigger type
 * @param event the type of event that will trigger the transition
 * @param target optional entity id to listen for the event on
 * @param where optional condition to determine whether to transition
 * @param beforeTransition optional callback to run before transitioning (deprecated)
 */
declare type EventTrigger<T extends string = string> = {
    type: 'event';
    event: T;
    target?: Eid;
    where?: (event: QueuedEvent<DataForEvent<T>>) => boolean;
    /** @deprecated */
    beforeTransition?: (event: QueuedEvent<DataForEvent<T>>) => boolean;
};

export declare const exitQuery: (t: RootQuery) => Query;

declare type ExponentialFog = {
    type: 'exponential';
    density: number;
    color: string;
};

declare type ExtendedSchema<S extends Schema> = {
    [key in keyof S]: ExtendedSchemaValue<S[key]>;
};

declare type ExtendedSchemaValue<T extends Type> = T | [T] | [T, TypeToValue[T]];

export declare const f32 = "f32";

export declare const f64 = "f64";

export declare const Face: RootAttribute<    {
id: "i32";
addAttachmentState: "boolean";
}>;

declare type Face_2 = {
    id: number;
    addAttachmentState: boolean;
};

export declare const FaceAnchor: RootAttribute<BaseSchema<ExtendedSchema<Schema>>>;

export declare const FaceAttachment: RootAttribute<BaseSchema<    {
point: "string";
}>>;

declare interface FaceEffectCameraSchema {
    nearClip: number;
    farClip: number;
    direction: string;
    meshGeometryFace: boolean;
    meshGeometryEyes: boolean;
    meshGeometryIris: boolean;
    meshGeometryMouth: boolean;
    uvType: string;
    maxDetections: number;
    enableEars: boolean;
    mirroredDisplay: boolean;
    allowedDevices: string;
}

export declare const FaceGeometry: RootAttribute<    {}>;

declare type FaceGeometry_2 = {
    type: 'face';
    id: number;
};

export declare const FaceMeshAnchor: RootAttribute<BaseSchema<ExtendedSchema<Schema>>>;

declare type Faces = 4 | 8 | 12 | 20;

declare type FlexDirection = 'row' | 'column' | 'row-reverse' | 'column-reverse';

declare namespace FlexStyles {
    export {
        FlexWrap,
        AlignContent,
        AlignItems,
        JustifyContent,
        FlexDirection,
        Direction,
        Display,
        Overflow,
        PositionMode,
        TextAlignContent,
        VerticalTextAlignContent
    }
}

declare type FlexWrap = 'nowrap' | 'wrap' | 'wrap-reverse';

export declare const FlyController: RootAttribute<BaseSchema<    {
verticalSensitivity: "f32";
horizontalSensitivity: "f32";
moveSpeedX: "f32";
moveSpeedY: "f32";
moveSpeedZ: "f32";
invertedY: "boolean";
invertedX: "boolean";
}>>;

declare type Fog = NoFog | LinearFog | ExponentialFog;

export declare const FollowAnimation: RootAttribute<BaseSchema<    {
target: "eid";
minDistance: "f32";
maxDistance: "f32";
elasticity: "f32";
}>>;

declare type FontResource = {
    type: 'font';
    font: string;
} | Resource;

declare interface FrameInfo {
    elapsedTimeMs: number;
    maxRecordingMs: number;
    ctx: CanvasRenderingContext2D;
    canvas: HTMLCanvasElement;
}

declare type FunctionWithoutEid<Fn extends (eid: Eid, ...args: any[]) => any> = (Fn extends (eid: Eid, ...args: infer A) => infer R ? (...args: A) => R : never);

declare const GAMEPAD_CONNECTED: "input-gamepad-connected";

declare const GAMEPAD_DISCONNECTED: "input-gamepad-disconnected";

export declare interface GamepadConnectedEvent {
    gamepad: Gamepad;
}

export declare interface GamepadDisconnectedEvent {
    gamepad: Gamepad;
}

declare type Geometry = BoxGeometry_2 | SphereGeometry_2 | PlaneGeometry_2 | CapsuleGeometry_2 | ConeGeometry_2 | CylinderGeometry_2 | TetrahedronGeometry_2 | PolyhedronGeometry_2 | CircleGeometry_2 | RingGeometry_2 | TorusGeometry_2 | FaceGeometry_2 | null;

declare const GESTURE_END: "gesture-end";

declare const GESTURE_MOVE: "gesture-move";

declare const GESTURE_START: "gesture-start";

export declare interface GestureEndEvent {
    startPosition: ScreenPosition;
    position: ScreenPosition;
    startSpread: number;
    spread: number;
    touchCount: number;
    target: Eid | undefined;
    nextTouchCount: number | undefined;
}

export declare interface GestureMoveEvent {
    startPosition: ScreenPosition;
    position: ScreenPosition;
    positionChange: ScreenPosition;
    startSpread: number;
    spread: number;
    touchCount: number;
    spreadChange: number;
}

export declare interface GestureStartEvent {
    startPosition: ScreenPosition;
    position: ScreenPosition;
    startSpread: number;
    spread: number;
    touchCount: number;
}

export declare const getAttribute: (name: string) => RootAttribute<{}>;

export declare const getBehaviors: () => DeepReadonly<typeof behaviors>;

declare const getProportionalPosition: (element: HTMLElement, event: PointerEvent) => ScreenPosition;

export declare const GltfModel: RootAttribute<    {
url: "string";
animationClip: "string";
loop: "boolean";
paused: "boolean";
time: "f32";
timeScale: "f32";
collider: "boolean";
reverse: "boolean";
repetitions: "ui32";
crossFadeDuration: "f32";
}>;

declare type GltfModel_2 = {
    src: Resource;
    animationClip?: string;
    loop?: boolean;
    paused?: boolean;
    timeScale?: number;
    reverse?: boolean;
    repetitions?: number;
    crossFadeDuration?: number;
};

declare type Gradient = {
    type: 'gradient';
    style?: GradientStyle;
    colors?: string[];
};

declare type GradientStyle = 'linear' | 'radial';

declare type GraphComponent = {
    id: ObjectId;
    name: string;
    parameters: Record<string, string | number | boolean | EntityReference>;
};

declare type GraphObject = Partial<BaseGraphObject> & {
    id: ObjectId;
    components: Record<GraphComponent['id'], GraphComponent>;
    instanceData?: InstanceData;
};

export declare const Hidden: RootAttribute<Schema>;

export declare const HiderMaterial: RootAttribute<    {}>;

declare type HiderMaterial_2 = {
    type: 'hider';
};

export declare const i32 = "i32";

declare type Image_2<T = Resource> = {
    type: 'image';
    src?: T;
};

export declare const ImageTarget: RootAttribute<    {
name: "string";
}>;

declare type ImageTarget_2 = {
    name: string;
    staticOrientation?: StaticImageTargetOrientation;
};

export declare const input: {
    SCREEN_TOUCH_START: "screen-touch-start";
    SCREEN_TOUCH_MOVE: "screen-touch-move";
    SCREEN_TOUCH_END: "screen-touch-end";
    GESTURE_START: "gesture-start";
    GESTURE_MOVE: "gesture-move";
    GESTURE_END: "gesture-end";
    GAMEPAD_CONNECTED: "input-gamepad-connected";
    GAMEPAD_DISCONNECTED: "input-gamepad-disconnected";
    UI_CLICK: "click";
    UI_PRESSED: "ui-pressed";
    UI_RELEASED: "ui-released";
    UI_HOVER_START: "ui-hover-start";
    UI_HOVER_END: "ui-hover-end";
};

declare interface InputApi extends InputListenerApi, InputManagerApi {
    attach: () => void;
    detach: () => void;
}

declare namespace InputEvents {
    export {
        createInputListener,
        GAMEPAD_CONNECTED,
        GAMEPAD_DISCONNECTED,
        InputListenerApi,
        GamepadConnectedEvent,
        GamepadDisconnectedEvent
    }
}

declare interface InputListenerApi {
    getAxis: (gamepadIdx?: number) => DeepReadonly<number[]> | undefined;
    getGamepads: () => DeepReadonly<Gamepad[]>;
    getKey: (code: string) => boolean;
    getKeyDown: (code: string) => boolean;
    getKeyUp: (code: string) => boolean;
    getButton: (input: number, gamepadIdx?: number) => boolean;
    getButtonDown: (input: number, gamepadIdx?: number) => boolean;
    getButtonUp: (input: number, gamepadIdx?: number) => boolean;
    enablePointerLockRequest: () => void;
    disablePointerLockRequest: () => void;
    isPointerLockActive: () => boolean;
    exitPointerLock: () => void;
    getMouseButton: (value: number) => boolean;
    getMouseDown: (value: number) => boolean;
    getMouseUp: (value: number) => boolean;
    getMousePosition: () => DeepReadonly<[number, number]>;
    getMouseVelocity: () => DeepReadonly<[number, number]>;
    getMouseScroll: () => DeepReadonly<[number, number]>;
    getTouch: (identifier?: number) => boolean;
    getTouchDown: (identifier?: number) => boolean;
    getTouchUp: (identifier?: number) => boolean;
    getTouchIds: () => number[];
}

declare interface InputManagerApi {
    setActiveMap: (name: string) => void;
    getActiveMap: () => string;
    getAction: (action: string) => number;
    readInputMap: (inputMap: DeepReadonly<InputMap>) => void;
}

declare type InputMap = Record<string, Action[]>;

declare type InstanceData = {
    instanceOf: string;
    deletions: PrefabInstanceDeletions;
    children?: PrefabInstanceChildren;
};

declare type InternalEvents = Events & InternalState;

declare type InternalState = {
    globalId: Eid;
    _world: World;
    _listeners: ListenerMap;
    _queue: QueuedEvent[];
};

declare type IntersectionResult = {
    eid?: Eid;
    point: Vec3;
    distance: number;
    threeData: Intersection;
};

export declare const isReady: () => boolean;

declare interface IStateDefiner<CallbackArgument = void> {
    name: string;
    initial: () => this;
    onEnter: (cb: State<CallbackArgument>['onEnter']) => this;
    onTick: (cb: State<CallbackArgument>['onTick']) => this;
    onExit: (cb: State<CallbackArgument>['onExit']) => this;
    onEvent: <T extends string>(event: T, nextState: StateId, args?: Omit<EventTrigger<T>, 'type' | 'event'>) => this;
    wait: (timeout: number, nextState: StateId) => this;
    onTrigger: (trigger: TriggerHandle, nextState: StateId) => this;
    listen: <T extends string>(target: EidGetter, name: T, listener: EventListener_2<DataForEvent<T>>) => this;
}

declare interface IStateGroupDefiner<CallbackArgument = void> {
    onEnter: (cb: StateGroup<CallbackArgument>['onEnter']) => this;
    onTick: (cb: StateGroup<CallbackArgument>['onTick']) => this;
    onExit: (cb: StateGroup<CallbackArgument>['onExit']) => this;
    onEvent: <T extends string>(event: T, nextState: StateId, args?: Omit<EventTrigger<T>, 'type' | 'event'>) => this;
    wait: (timeout: number, nextState: StateId) => this;
    onTrigger: (trigger: TriggerHandle, nextState: StateId) => this;
    listen: <T extends string>(target: EidGetter, name: T, listener: EventListenerForEvent<T>) => this;
}

declare type JustifyContent = 'flex-start' | 'flex-end' | 'center' | 'space-between' | 'space-around' | 'space-evenly';

declare interface LateWorld {
    start: () => void;
    stop: () => void;
    tick: (dt?: number) => void;
    tock: () => void;
    getTickMode: () => TickMode;
    setTickMode: (tickMode: TickMode) => void;
    destroy: () => void;
    loadScene: (scene: DeepReadonly<SceneGraph>, callback?: (handle: SceneHandle) => void) => SceneHandle;
    createEntity: (prefabNameOrEid?: string | Eid) => Eid;
    deleteEntity: (eid: Eid) => void;
    getInstanceEntity: (instanceEid: Eid, prefabChildEid: Eid) => Eid;
    spawnIntoObject: (eid: Eid, object: DeepReadonly<BaseGraphObject>, graphIdToEid: Map<string, Eid>) => void;
    setScale: (eid: Eid, x: number, y: number, z: number) => void;
    setPosition: (eid: Eid, x: number, y: number, z: number) => void;
    setQuaternion: (eid: Eid, x: number, y: number, z: number, w: number) => void;
    setTransform: (eid: Eid, transform: Mat4) => void;
    getWorldTransform: (eid: Eid, transform: Mat4) => void;
    normalizeQuaternion: (eid: Eid) => void;
    setParent: (eid: Eid, parent: Eid) => void;
    getParent: (eid: Eid) => Eid;
    getChildren: (eid: Eid) => Generator<Eid>;
    raycast: (origin: Vec3Source, direction: Vec3Source, near?: number, far?: number) => IntersectionResult[];
    raycastFrom: (eid: Eid, near?: number, far?: number) => IntersectionResult[];
    audio: AudioControls;
    camera: CameraManager;
    pointer: PointerApi;
    events: Events;
    getEntity: (eid: Eid) => Entity;
    transform: TransformManager;
    input: InputApi;
    xr: XrManager;
    setSceneHook: (hook: SpacesHandle & PrefabsHandle) => void;
    spaces: SpacesHandle;
    effects: EffectsManager;
}

declare type LifecycleQueries = {
    init: (world: World) => void;
    enter: Query;
    changed: Query;
    exit: Query;
};

export declare const lifecycleQueries: (t: RootQuery) => LifecycleQueries;

export declare const Light: RootAttribute<    {
type: "string";
r: "ui8";
g: "ui8";
b: "ui8";
intensity: "f32";
castShadow: "boolean";
targetX: "f32";
targetY: "f32";
targetZ: "f32";
shadowNormalBias: "f32";
shadowBias: "f32";
shadowAutoUpdate: "boolean";
shadowBlurSamples: "ui32";
shadowRadius: "f32";
shadowMapSizeHeight: "i32";
shadowMapSizeWidth: "i32";
shadowCameraNear: "f32";
shadowCameraFar: "f32";
shadowCameraLeft: "f32";
shadowCameraRight: "f32";
shadowCameraTop: "f32";
shadowCameraBottom: "f32";
distance: "f32";
decay: "f32";
followCamera: "boolean";
angle: "f32";
penumbra: "f32";
colorMap: "string";
width: "f32";
height: "f32";
}>;

declare type Light_2 = {
    type: LightType;
    color?: string;
    intensity?: number;
    castShadow?: boolean;
    target?: Vec3Tuple;
    shadowNormalBias?: number;
    shadowBias?: number;
    shadowAutoUpdate?: boolean;
    shadowBlurSamples?: number;
    shadowRadius?: number;
    shadowMapSize?: Vec2Tuple;
    shadowCamera?: Vec6Tuple;
    distance?: number;
    decay?: number;
    followCamera?: boolean;
    angle?: number;
    penumbra?: number;
    colorMap?: Resource;
    width?: number;
    height?: number;
};

declare type Lights = ShadowLights | NoShadowLights;

declare type LightType = 'directional' | 'ambient' | 'point' | 'spot' | 'area';

declare type LinearFog = {
    type: 'linear';
    near: number;
    far: number;
    color: string;
};

export declare const listAttributes: () => string[];

declare type ListenerMap = Map<Eid, Map<string, Set<EventListener_2>>>;

declare type ListenerParams = {
    target: EidGetter;
    name: string;
    listener: EventListener_2<any>;
};



export declare type LocationSpawnedEvent = {
    id: string;
    imageUrl: string;
    title: string;
    lat: number;
    lng: number;
    mapPoint: Eid;
};

declare type LocationVisualization = 'mesh' | 'splat' | 'none';

export declare const LookAtAnimation: RootAttribute<BaseSchema<    {
target: "eid";
targetX: "f32";
targetY: "f32";
targetZ: "f32";
lockX: "boolean";
lockY: "boolean";
}>>;

export declare type MachineId = number;

declare type Major = {
    major: number;
};

declare type Map_2 = {
    latitude: number;
    longitude: number;
    targetEntity?: EntityReference;
    radius: number;
    spawnLocations: boolean;
    useGps: boolean;
};

declare type MapPoint = {
    latitude: number;
    longitude: number;
    targetEntity?: EntityReference;
    meters: number;
    minScale: number;
};

declare type MapTheme = {
    landColor?: string;
    buildingColor?: string;
    parkColor?: string;
    parkingColor?: string;
    roadColor?: string;
    sandColor?: string;
    transitColor?: string;
    waterColor?: string;
    landOpacity?: number;
    buildingOpacity?: number;
    parkOpacity?: number;
    parkingOpacity?: number;
    roadOpacity?: number;
    sandOpacity?: number;
    transitOpacity?: number;
    waterOpacity?: number;
    lod?: number;
    buildingBase?: number;
    parkBase?: number;
    parkingBase?: number;
    roadBase?: number;
    sandBase?: number;
    transitBase?: number;
    waterBase?: number;
    buildingMinMeters?: number;
    buildingMaxMeters?: number;
    roadLMeters?: number;
    roadMMeters?: number;
    roadSMeters?: number;
    roadXLMeters?: number;
    transitMeters?: number;
    waterMeters?: number;
    roadLMin?: number;
    roadMMin?: number;
    roadSMin?: number;
    roadXLMin?: number;
    transitMin?: number;
    waterMin?: number;
    landVisibility?: boolean;
    buildingVisibility?: boolean;
    parkVisibility?: boolean;
    parkingVisibility?: boolean;
    roadVisibility?: boolean;
    sandVisibility?: boolean;
    transitVisibility?: boolean;
    waterVisibility?: boolean;
};

/**
 * Interface representing a 4x4 matrix.  A 4x4 matrix is represented by a 16 dimensional array of
 * data, with elements stored in column major order. A special kind of matrix, known as a TRS matrix
 * (for Translation, Rotation, and Scale) is common in 3D geometry for representing the position,
 * orientation, and size of points in a 3D scene. Many special types of matrices have easily
 * specified inverses. By specifying these ahead of time, Mat4 allows for matrix inverse to be a
 * very fast O(1) operation. `Mat4` objects are created with the `ecs.math.mat4` `Mat4Factory`, or
 * through operations on other `Mat4` objects.
 */
declare interface Mat4 {
    /**
     * Create a new matrix with the same components as this matrix.
     *
     * API Type: Immutable API.
     *
     * @returns a new matrix with the same components as this matrix.
     */
    clone: () => Mat4;
    /**
     * Get the raw data of the matrix, in column-major order.
     *
     * API Type: Immutable API.
     *
     * @returns the 16-element raw data of the matrix, in column-major order.
     */
    data: () => number[];
    /**
     * Decompose the matrix into its translation, rotation, and scale components, assuming it was
     * formed by a translation, rotation, and scale in that order. If `target` is supplied, the result
     * will be stored in `target` and `target` will be returned. Otherwise, a new {t, r, s} object
     * will be created and returned.
     *
     * If you don't need the rotation in quaternion form, decomposeT and decomposeS are more efficient
     * ways to get just the translation or scale.
     *
     * API Type: Immutable API.
     *
     * @param target optional target object to store the result in.
     * @returns target if supplied, otherwise a new {t, r, s} object.
     */
    decomposeTrs: (target?: {
        t: Vec3;
        r: Quat;
        s: Vec3;
    }) => {
        t: Vec3;
        r: Quat;
        s: Vec3;
    };
    /**
     * Get the rotation component of the TRS matrix
     *
     * @param target optional target object to store the result in.
     * @returns target if supplied, otherwise a new quaternion.
     */
    decomposeR: (target?: Quat) => Quat;
    /**
     * Get the translation component of the TRS matrix.
     *
     * @param target optional target object to store the result in.
     * @returns target if supplied, otherwise a new vec3.
     */
    decomposeT: (target?: Vec3) => Vec3;
    /**
     * Get the scale component of the TRS matrix.
     *
     * @param target optional target object to store the result in.
     * @returns target if supplied, otherwise a new vec3.
     */
    decomposeS: (target?: Vec3) => Vec3;
    /**
     * Compute the determinant of the matrix.
     *
     * API Type: Immutable API.
     *
     * @returns the determinant of the matrix.
     */
    determinant: () => number;
    /**
     * Check whether two matrices are equal, with a specified floating point tolerance.
     *
     * API Type: Immutable API.
     *
     * @param m matrix to compare to.
     * @param tolerance used to judge near equality.
     * @returns true if all matrix elements are equal within the specified tolerance, false otherwise.
     */
    equals: (m: Mat4, tolerance: number) => boolean;
    /**
     * Invert the matrix, or throw if the matrix is not invertible. Because Mat4 stores a precomputed
     * inverse, this operation is very fast.
     *
     * API Type: Immutable API.
     *
     * @returns the inverse of the matrix.
     */
    inv: () => Mat4;
    /**
     * Get the raw data of the inverse matrix, in column-major order, or null if the matrix is not
     * invertible.
     *
     * API Type: Immutable API.
     *
     * @returns the 16-element raw data of the inverse matrix, in column-major order, or null if the
     *   matrix is not invertible.
     */
    inverseData: () => number[] | null;
    /**
     * Get a matrix with the same position and scale as this matrix, but with the rotation set to look
     * at the target.
     *
     * API Type: Immutable API.
     *
     * @param target vector where the target is located.
     * @param up vector representing the up direction from the mat4's perspective.
     * @returns a new matrix with the same position and scale as this matrix, but with the rotation
     *   set to look at the target.
     */
    lookAt: (target: Vec3Source, up: Vec3Source) => Mat4;
    /**
     * Multiply the matrix by a scalar. Scaling by 0 throws an error.
     *
     * API Type: Immutable API.
     *
     * @param s scalar to multiply the matrix by.
     * @returns the matrix multiplied by the scalar.
     */
    scale: (s: number) => Mat4;
    /**
     * Get the transpose of the matrix.
     *
     * API Type: Immutable API.
     *
     * @returns the transpose of the matrix.
     */
    transpose: () => Mat4;
    /**
     * Multiply the matrix by another matrix.
     *
     * API Type: Immutable API.
     *
     * @param m matrix to multiply by.
     * @returns the matrix multiplied by another matrix.
     */
    times: (m: Mat4) => Mat4;
    /**
     * Multiply the matrix by a vector using homogeneous coordinates.
     *
     * API Type: Immutable API.
     *
     * @param v vector to multiply by.
     * @param target optional target to store the result in.
     * @returns the transformed vector.
     */
    timesVec: (v: Vec3Source, target?: Vec3) => Vec3;
    /**
     * Invert the matrix, or throw if the matrix is not invertible. Because Mat4 stores a precomputed
     * inverse, this operation is very fast. Store the result in this Mat4 and return this Mat4 for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this matrix for chaining.
     */
    setInv: () => Mat4;
    /**
     * Set the matrix rotation to look at the target, keeping translation and scale unchanged. Store
     * the result in this Mat4 and return this Mat4 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param target vector where the target is located.
     * @param up vector representing the up direction from the mat4's perspective.
     * @returns this matrix for chaining.
     */
    setLookAt: (target: Vec3Source, up: Vec3Source) => Mat4;
    /**
     * Set the matrix to the result of m times this matrix. Store the result in this Mat4 and return
     * this Mat4 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param m matrix to premultiply by.
     * @returns this matrix for chaining.
     */
    setPremultiply: (m: Mat4) => Mat4;
    /**
     * Multiply each element of the matrix by a scaler. Scaling by 0 throws an error. Store the result
     * in this Mat4 and return this Mat4 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param s scalar to multiply the matrix by.
     * @returns this matrix for chaining.
     */
    setScale: (s: number) => Mat4;
    /**
     * Multiply the matrix by another matrix. Store the result in this Mat4 and return this Mat4 for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @param m matrix to multiply by.
     * @returns this matrix for chaining.
     */
    setTimes: (m: Mat4) => Mat4;
    /**
     * Set the matrix to its transpose. Store the result in this Mat4 and return this Mat4 for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this matrix for chaining.
     */
    setTranspose: () => Mat4;
    /**
     * Set the matrix to the identity matrix. Store the result in this Mat4 and return this Mat4 for
     * chaining.
     *
     * API Type: Set API.
     *
     * @returns this matrix for chaining.
     */
    makeI: () => Mat4;
    /**
     * Set this matrix to a rotation matrix from the specified quaternion. Store the result in this
     * Mat4 and return this Mat4 for chaining.
     *
     * API Type: Set API.
     *
     * @param r quaternion representing the desired rotation matrix.
     * @returns this matrix for chaining.
     */
    makeR: (r: QuatSource) => Mat4;
    /**
     * Create a matrix with specified row data, and optionally specified inverse row data. `dataRows`
     * and `inverseDataRows` should be four arrays, each with four numbers. If the inverse is not
     * specified, it will be computed if the matrix is invertible. If the matrix is not invertible,
     * calling inv() will throw an error.
     *
     * API Type: Set API.
     *
     * @param rowData for the matrix, 4 arrays of 4 elements each.
     * @param inverseRowData optional inverse row data for the matrix, 4 arrays of 4 elements each.
     * @returns this matrix for chaining.
     */
    makeRows: (rowData: number[][], inverseRowData?: number[][]) => Mat4;
    /**
     * Set this matrix to a scale matrix from the specified vector. No element of the vector should be
     * zero. Store the result in this Mat4 and return this Mat4 for chaining.
     *
     * API Type: Set API.
     *
     * @param s vector representing the desired scale in each of the x, y, and z dimensions.
     * @returns this matrix for chaining.
     */
    makeS: (s: Vec3Source) => Mat4;
    /**
     * Set this matrix to a translation matrix from the specified vector. Store the result in this
     * Mat4 and return this Mat4 for chaining.
     *
     * API Type: Set API.
     *
     * @param t vector representing the desired translation in each of the x, y, and z dimensions.
     * @returns this matrix for chaining.
     */
    makeT: (t: Vec3Source) => Mat4;
    /**
     * Set this matrix to a translation and rotation matrix from the specified vector and quaternion.
     * Store the result in this Mat4 and return this Mat4 for chaining.
     *
     * API Type: Set API.
     *
     * @param t vector representing the desired translation in each of the x, y, and z dimensions.
     * @param r quaternion representing the desired rotation matrix.
     * @returns this matrix for chaining.
     */
    makeTr: (t: Vec3Source, r: QuatSource) => Mat4;
    /**
     * Set this matrix to a translation, rotation, and scale matrix from the specified vectors and
     * quaternion. Store the result in this Mat4 and return this Mat4 for chaining.
     *
     * API Type: Set API.
     *
     * @param t vector representing the desired translation in each of the x, y, and z dimensions.
     * @param r quaternion representing the desired rotation matrix.
     * @param s vector representing the desired scale in each of the x, y, and z dimensions.
     * @returns this matrix for chaining.
     */
    makeTrs: (t: Vec3Source, r: QuatSource, s: Vec3Source) => Mat4;
    /**
     * Set the value of the matrix and inverse to the provided values. If no inverse is provided, one
     * will be computed if possible. If the matrix is not invertible, calling inv() will throw an
     * error. Store the result in this Mat4 and return this Mat4 for chaining.
     *
     * API Type: Set API.
     *
     * @param data for the matrix, 16 elements in column-major order.
     * @param inverseData optional inverse data for the matrix, 16 elements in column-major order.
     * @returns this matrix for chaining.
     */
    set: (data: number[], inverseData?: number[]) => Mat4;
}

declare const mat4: Mat4Factory;

/**
 * Factory for Mat4. Mat4 objects are created with the `ecs.math.mat4` Mat4Factory.
 */
declare interface Mat4Factory {
    /**
     * Identity matrix.
     *
     * API Type: Factory API.
     *
     * @returns the identity matrix.
     */
    i: () => Mat4;
    /**
     * Create the matrix with directly specified data, in column major order. An optional inverse can
     * be specified. If the inverse is not specified, it will be computed if the matrix is invertible.
     * If the matrix is not invertible, calling inv() will throw an error.
     *
     * API Type: Factory API.
     *
     * @param data for the matrix, 16 elements in column-major order.
     * @param inverseData optional inverse data for the matrix, 16 elements in column-major order.
     * @returns the matrix with the specified data.
     */
    of: (data: number[], inverseData?: number[]) => Mat4;
    /**
     * Create a rotation matrix from a quaternion.
     *
     * API Type: Factory API.
     *
     * @param q quaternion representing the rotation.
     * @returns the rotation matrix.
     */
    r: (q: QuatSource) => Mat4;
    /**
     * Create a matrix with specified row data, and optionally specified inverse row data. `dataRows`
     * and `inverseDataRows` should be four arrays, each with four numbers. If the inverse is not
     * specified, it will be computed if the matrix is invertible. If the matrix is not invertible,
     * calling inv() will throw an error.
     *
     * API Type: Factory API.
     *
     * @param dataRows for the matrix, 4 arrays of 4 elements each.
     * @param inverseDataRows optional inverse row data for the matrix, 4 arrays of 4 elements each.
     * @returns the matrix with the specified row data.
     */
    rows: (dataRows: number[][], inverseDataRows?: number[][]) => Mat4;
    /**
     * Create a scale matrix. No scale element should be zero.
     *
     * API Type: Factory API.
     *
     * @param v vector representing the scale in each of the x, y, and z dimensions.
     * @returns the scale matrix.
     */
    s: (v: Vec3Source) => Mat4;
    /**
     * Create a translation matrix.
     *
     * API Type: Factory API.
     *
     * @param v vector representing the translation in each of the x, y, and z dimensions.
     * @returns the translation matrix.
     */
    t: (v: Vec3Source) => Mat4;
    /**
     * Create a translation and rotation matrix.
     *
     * API Type: Factory API.
     *
     * @param t vector representing the translation in each of the x, y, and z dimensions.
     * @param r quaternion representing the rotation matrix.
     * @returns the translation and rotation matrix.
     */
    tr: (t: Vec3Source, r: QuatSource) => Mat4;
    /**
     * Create a translation, rotation, and scale matrix.
     *
     * API Type: Factory API.
     *
     * @param t vector representing the translation in each of the x, y, and z dimensions.
     * @param r quaternion representing the rotation matrix.
     * @param s vector representing the scale in each of the x, y, and z dimensions.
     * @returns the translation, rotation, and scale matrix.
     */
    trs: (t: Vec3Source, r: QuatSource, s: Vec3Source) => Mat4;
}

export declare const Material: RootAttribute<    {
r: "ui8";
g: "ui8";
b: "ui8";
textureSrc: "string";
roughness: "f32";
metalness: "f32";
opacity: "f32";
roughnessMap: "string";
metalnessMap: "string";
side: "string";
normalScale: "f32";
emissiveIntensity: "f32";
emissiveR: "ui8";
emissiveG: "ui8";
emissiveB: "ui8";
opacityMap: "string";
normalMap: "string";
emissiveMap: "string";
blending: "string";
repeatX: "f32";
repeatY: "f32";
offsetX: "f32";
offsetY: "f32";
wrap: "string";
depthTest: "boolean";
depthWrite: "boolean";
wireframe: "boolean";
forceTransparent: "boolean";
textureFiltering: "string";
mipmaps: "boolean";
}>;

declare type Material_3 = BasicMaterial | UnlitMaterial_2 | ShadowMaterial_3 | HiderMaterial_2 | VideoMaterial_2 | null;

declare type MaterialBlending = 'no' | 'normal' | 'additive' | 'subtractive' | 'multiply';

export declare namespace math {
    export {
        Mat4,
        Mat4Factory,
        Quat,
        QuatFactory,
        QuatSource,
        Vec2,
        Vec2Factory,
        Vec2Source,
        Vec3,
        Vec3Factory,
        Vec3Source,
        mat4,
        quat,
        vec2,
        vec3
    }
}

declare type MatrixUpdateMode = 'auto' | 'manual';

declare type Minor = {
    minor: number;
};

declare type NoFog = {
    type: 'none';
};

declare type NoShadowLights = THREE_TYPES.AmbientLight | THREE_TYPES.RectAreaLight;

declare type NoSky = {
    type: 'none';
};

declare type Object3D = Omit<Object3D_2, keyof Overrides> & Overrides;

/**
 * @description An object ID.
 */
declare type ObjectId = string;

export declare const OrbitControls: RootAttribute<BaseSchema<    {
speed: "f32";
maxAngularSpeed: "f32";
maxZoomSpeed: "f32";
distanceMin: "f32";
distanceMax: "f32";
pitchAngleMin: "f32";
pitchAngleMax: "f32";
constrainYaw: "boolean";
yawAngleMin: "f32";
yawAngleMax: "f32";
inertiaFactor: "f32";
focusEntity: "eid";
invertedX: "boolean";
invertedY: "boolean";
invertedZoom: "boolean";
controllerSupport: "boolean";
horizontalSensitivity: "f32";
verticalSensitivity: "f32";
}>>;

declare type OrderedSchema = Array<[string, Type, number]>;

declare type Overflow = 'visible' | 'hidden' | 'scroll';

declare type Overrides = {
    parent: Object3D | null;
    children: Object3D[];
};

declare type P<T> = Partial<T>;

export declare const ParticleEmitter: RootAttribute<BaseSchema<    {
stopped: "boolean";
emitterLife: "f32";
particlesPerShot: "ui32";
emitDelay: "f32";
minimumLifespan: "f32";
maximumLifespan: "f32";
mass: "f32";
gravity: "f32";
scale: "f32";
forceX: "f32";
forceY: "f32";
forceZ: "f32";
spread: "f32";
radialVelocity: "f32";
spawnAreaType: "string";
spawnAreaWidth: "f32";
spawnAreaHeight: "f32";
spawnAreaDepth: "f32";
spawnAreaRadius: "f32";
boundingZoneType: "string";
boundingZoneWidth: "f32";
boundingZoneHeight: "f32";
boundingZoneDepth: "f32";
boundingZoneRadius: "f32";
resourceType: "string";
resourceUrl: "string";
blendingMode: "string";
animateColor: "boolean";
colorStart: "string";
colorEnd: "string";
randomDrift: "boolean";
randomDriftRange: "f32";
collisions: "boolean";
}>>;

export declare interface ParticlesSchema {
    stopped: boolean;
    emitterLife: number;
    particlesPerShot: number;
    emitDelay: number;
    minimumLifespan: number;
    maximumLifespan: number;
    mass: number;
    gravity: number;
    scale: number;
    forceX: number;
    forceY: number;
    forceZ: number;
    spread: number;
    radialVelocity: number;
    spawnAreaType: string;
    spawnAreaWidth: number;
    spawnAreaHeight: number;
    spawnAreaDepth: number;
    spawnAreaRadius: number;
    boundingZoneType: string;
    boundingZoneWidth: number;
    boundingZoneHeight: number;
    boundingZoneDepth: number;
    boundingZoneRadius: number;
    resourceType: string;
    resourceUrl: string;
    blendingMode: string;
    animateColor: boolean;
    colorStart: string;
    colorEnd: string;
    randomDrift: boolean;
    randomDriftRange: number;
    collisions: boolean;
}

declare type Patch = {
    patch: number;
};

export declare const Persistent: RootAttribute<Schema>;

export declare const physics: {
    enable: (world: World) => void;
    disable: (world: World) => void;
    setWorldGravity: (world: World, gravity: number) => void;
    getWorldGravity: (world: World) => number;
    applyForce: (world: World, eid: Eid, forceX: number, forceY: number, forceZ: number) => void;
    applyImpulse: (world: World, eid: Eid, impulseX: number, impulseY: number, impulseZ: number) => void;
    applyTorque: (world: World, eid: Eid, torqueX: number, torqueY: number, torqueZ: number) => void;
    setLinearVelocity: (world: World, eid: Eid, velocityX: number, velocityY: number, velocityZ: number) => void;
    getLinearVelocity: (world: World, eid: Eid) => {
        x: number;
        y: number;
        z: number;
    };
    setAngularVelocity: (world: World, eid: Eid, velocityX: number, velocityY: number, velocityZ: number) => void;
    getAngularVelocity: (world: World, eid: Eid) => {
        x: number;
        y: number;
        z: number;
    };
    registerConvexShape: (world: World, vertices: Float32Array) => number;
    unregisterConvexShape: (world: World, id: number) => void;
    COLLISION_START_EVENT: "physics-collision-start";
    COLLISION_END_EVENT: "physics-collision-end";
    UPDATE_EVENT: "physics-update";
    ColliderShape: {
        readonly Box: 0;
        readonly Sphere: 1;
        readonly Plane: 2;
        readonly Capsule: 3;
        readonly Cone: 4;
        readonly Cylinder: 5;
        readonly Circle: 6;
    };
    ColliderType: {
        readonly Static: 0;
        readonly Dynamic: 1;
        readonly Kinematic: 2;
    };
};

declare namespace PhysicsEvents {
    export {
        dispatchPhysicsEvents,
        COLLISION_END_EVENT,
        COLLISION_START_EVENT,
        UPDATE_EVENT
    }
}

export declare const PlaneGeometry: RootAttribute<    {
width: "f32";
height: "f32";
}>;

declare type PlaneGeometry_2 = {
    type: 'plane';
    width: number;
    height: number;
};

declare interface PointerApi {
    attach: () => void;
    detach: () => void;
}

declare namespace PointerEvents {
    export {
        getProportionalPosition,
        createPointerListener,
        SCREEN_TOUCH_START,
        SCREEN_TOUCH_MOVE,
        SCREEN_TOUCH_END,
        GESTURE_START,
        GESTURE_MOVE,
        GESTURE_END,
        ScreenPosition,
        ScreenTouchStartEvent,
        ScreenTouchMoveEvent,
        ScreenTouchEndEvent,
        GestureStartEvent,
        GestureMoveEvent,
        GestureEndEvent,
        RaycastStage
    }
}

declare type PointerId = PointerEvent['pointerId'];

export declare const PolyhedronGeometry: RootAttribute<    {
faces: "ui8";
radius: "f32";
}>;

declare type PolyhedronGeometry_2 = {
    type: 'polyhedron';
    radius: number;
    faces: Faces;
};

export declare const Position: RootAttribute<    {
readonly x: "f32";
readonly y: "f32";
readonly z: "f32";
}>;

export declare const PositionAnimation: RootAttribute<BaseSchema<    {
autoFrom: "boolean";
fromX: "f32";
fromY: "f32";
fromZ: "f32";
toX: "f32";
toY: "f32";
toZ: "f32";
duration: "f32";
loop: "boolean";
reverse: "boolean";
easeIn: "boolean";
easeOut: "boolean";
easingFunction: "string";
target: "eid";
}>>;

declare type PositionMode = 'absolute' | 'relative' | 'static';

declare type PrefabInstanceChildren = Record<string, PrefabInstanceChildrenData>;

declare type PrefabInstanceChildrenData = Omit<Partial<BaseGraphObject>, 'id' | 'prefab'> & {
    id?: string;
    deletions?: PrefabInstanceDeletions;
    deleted?: true;
};

declare type PrefabInstanceDeletions = Partial<{
    [K in keyof Omit<BaseGraphObject, 'id' | 'name' | 'parentId' | 'prefab' | 'components'>]: true;
}> & Partial<{
    components: Record<GraphComponent['id'], true>;
}>;

declare type PrefabsHandle = {
    graphIdToPrefab: Map<string, Eid>;
    getPrefab: (name: string) => Eid | undefined;
};

declare interface ProgressInfo {
    progress: number;
    total: number;
}

/**
 * Interface representing a quaternion.  A quaternion is represented by (x, y, z, w) coordinates,
 * and represents a 3D rotation. Quaternions can be converted to and from 4x4 rotation matrices with
 * the interfaces in `Mat4`. `Quat` objects are created with the `ecs.math.quat` `QuatFactory`, or
 * through operations on other `Quat` objects.
 */
declare interface Quat extends QuatSource {
    /**
     * Convert the quaternion to an axis-angle representation.  The direction of the vector gives the
     * axis of rotation, and the magnitude of the vector gives the angle, in radians. If `target` is
     * supplied, the result will be stored in `target` and `target` will be returned. Otherwise, a new
     * Vec3 will be created and returned.
     *
     * API Type: Immutable API.
     *
     * @param target optional vector to store the result in.
     * @returns target if supplied, otherwise a new Vec3.
     */
    axisAngle: (target?: Vec3) => Vec3;
    /**
     * Create a new quaternion with the same components as this quaternion.
     *
     * API Type: Immutable API.
     *
     * @returns a new quaternion with the same components as this quaternion.
     */
    clone: () => Quat;
    /**
     * Return the rotational conjugate of this quaternion. The conjugate of a quaternion represents
     * the same rotation in the opposite direction about the rotational axis.
     *
     * API Type: Immutable API.
     *
     * @returns a new quaternion representing the rotational conjugate of this quaternion.
     */
    conjugate: () => Quat;
    /**
     * Access the quaternion as an array of [x, y, z, w].
     *
     * API Type: Immutable API.
     *
     * @returns an array of [x, y, z, w].
     */
    data: () => number[];
    /**
     * Angle between two quaternions, in degrees.
     *
     * API Type: Immutable API.
     *
     * @param target quaternion to compute the angle to.
     * @returns the angle between this quaternion and the target quaternion, in degrees.
     */
    degreesTo: (target: QuatSource) => number;
    /**
     * Compute the quaternion required to rotate this quaternion to the target quaternion.
     *
     * API Type: Immutable API.
     *
     * @param target quaternion to rotate towards.
     * @returns the quaternion required to rotate this quaternion to the target quaternion.
     */
    delta: (target: QuatSource) => Quat;
    /**
     * Compute the dot product of this quaternion with another quaternion.
     *
     * API Type: Immutable API.
     *
     * @param q quaternion to compute the dot product with.
     * @returns the dot product of this quaternion with the target quaternion.
     */
    dot: (q: QuatSource) => number;
    /**
     * Check whether two quaternions are equal, with a specified floating point tolerance.
     *
     * API Type: Immutable API.
     *
     * @param v quaternion to compare to.
     * @param tolerance used to judge near equality.
     * @returns true if quaternions components are each equal within the specified tolerance, false
     *   otherwise.
     */
    equals: (q: QuatSource, tolerance: number) => boolean;
    /**
     * Compute the quaternion which multiplies this quaternion to get a zero rotation quaternion.
     *
     * API Type: Immutable API.
     *
     * @returns the inverse of this quaternion.
     */
    inv: () => Quat;
    /**
     * Negate all components of this quaternion. The result is a quaternion representing the same
     * rotation as this quaternion.
     *
     * API Type: Immutable API.
     *
     * @returns the negated quaternion.
     */
    negate: () => Quat;
    /**
     * Get the normalized version of this quaternion with a length of 1.
     *
     * API Type: Immutable API.
     *
     * @returns the normalized quaternion.
     */
    normalize: () => Quat;
    /**
     * Convert the quaternion to pitch, yaw, and roll angles in radians.
     *
     * API Type: Immutable API.
     *
     * @param target optional vector to store the result in.
     * @returns target if supplied, otherwise a new Vec3.
     */
    pitchYawRollRadians: (target?: Vec3) => Vec3;
    /**
     * Convert the quaternion to pitch, yaw, and roll angles in degrees.
     *
     * API Type: Immutable API.
     *
     * @param target optional vector to store the result in.
     * @returns target if supplied, otherwise a new Vec3.
     */
    pitchYawRollDegrees: (target?: Vec3) => Vec3;
    /**
     * Add two quaternions together.
     *
     * API Type: Immutable API.
     *
     * @param q quaternion to add.
     * @returns the sum of this quaternion and the target quaternion.
     */
    plus: (q: QuatSource) => Quat;
    /**
     * Angle between two quaternions, in radians.
     *
     * API Type: Immutable API.
     *
     * @param target quaternion to compute the angle to.
     * @returns the angle between this quaternion and the target quaternion, in radians.
     */
    radiansTo: (target: QuatSource) => number;
    /**
     * Rotate this quaternion towards the target quaternion by a given number of radians, clamped to
     * the target.
     *
     * API Type: Immutable API.
     *
     * @param target quaternion to rotate towards.
     * @param radians number of radians to rotate.
     * @returns the rotated quaternion.
     */
    rotateToward: (target: QuatSource, radians: number) => Quat;
    /**
     * Spherical interpolation between two quaternions given a provided interpolation value. If the
     * interpolation is set to 0, then it will return this quaternion. If the interpolation is set to
     * 1, then it will return the target quaternion.
     *
     * API Type: Immutable API.
     *
     * @param target quaternion to interpolate towards.
     * @param t factor to interpolate; should be between in 0 to 1, inclusive.
     * @returns the interpolated quaternion.
     */
    slerp: (target: QuatSource, t: number) => Quat;
    /**
     * Multiply two quaternions together.
     *
     * API Type: Immutable API.
     *
     * @param q quaternion to multiply.
     * @returns the product of this quaternion and the target quaternion.
     */
    times: (q: QuatSource) => Quat;
    /**
     * Multiply the quaternion by a vector. This is equivalent to converting the quaternion to a
     * rotation matrix and multiplying the matrix by the vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to multiply.
     * @param target optional vector to store the result in.
     * @returns target if supplied, otherwise a new Vec3.
     */
    timesVec: (v: Vec3Source, target?: Vec3) => Vec3;
    /**
     * Set this quaternion to its rotational conjugate. The conjugate of a quaternion represents the
     * same rotation in the opposite direction about the rotational axis. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this quaternion for chaining.
     */
    setConjugate: () => Quat;
    /**
     * Compute the quaternion required to rotate this quaternion to the target quaternion. Store the
     * result in this Quat and return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @param target quaternion to rotate towards.
     * @returns this quaternion for chaining.
     */
    setDelta: (target: QuatSource) => Quat;
    /**
     * Set this quaternion to the value in another quaternion. Store the result in this Quat and
     * return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @param q quaternion to set from.
     * @returns this quaternion for chaining.
     */
    setFrom: (q: QuatSource) => Quat;
    /**
     * Set this to the quaternion which multiplies this quaternion to get a zero rotation quaternion.
     * Store the result in this Quat and return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this quaternion for chaining.
     */
    setInv: () => Quat;
    /**
     * Negate all components of this quaternion. The result is a quaternion representing the same
     * rotation as this quaternion. Store the result in this Quat and return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this quaternion for chaining.
     */
    setNegate: () => Quat;
    /**
     * Get the normalized version of this quaternion with a length of 1. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this quaternion for chaining.
     */
    setNormalize: () => Quat;
    /**
     * Add this quaternion to another quaternion. Store the result in this Quat and return this
     * Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @param q quaternion to add.
     * @returns this quaternion for chaining.
     */
    setPlus: (q: QuatSource) => Quat;
    /**
     * Set this quaternion to the result of q times this quaternion. Store the result in this Quat
     * and return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @param q quaternion to premultiply.
     * @returns this quaternion for chaining.
     */
    setPremultiply: (q: QuatSource) => Quat;
    /**
     * Rotate this quaternion towards the target quaternion by a given number of radians, clamped to
     * the target. Store the result in this Quat and return this Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @param target quaternion to rotate towards.
     * @param radians number of radians to rotate.
     * @returns this quaternion for chaining.
     */
    setRotateToward: (target: QuatSource, radians: number) => Quat;
    /**
     * Spherical interpolation between two quaternions given a provided interpolation value. If the
     * interpolation is set to 0, then it will return this quaternion. If the interpolation is set to
     * 1, then it will return the target quaternion. Store the result in this Quat and return this
     * Quat for chaining.
     *
     * API Type: Mutable API.
     *
     * @param target quaternion to interpolate towards.
     * @param t factor to interpolate; should be between in 0 to 1, inclusive.
     * @returns this quaternion for chaining.
     */
    setSlerp: (target: QuatSource, t: number) => Quat;
    /**
     * Multiply two quaternions together. Store the result in this Quat and return this Quat for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @param q quaternion to multiply.
     * @returns this quaternion for chaining.
     */
    setTimes: (q: QuatSource) => Quat;
    /**
     * Set the quaternion to the specified x, y, z and w values. Store the result in this Quat and
     * return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param x x component of the quaternion.
     * @param y y component of the quaternion.
     * @param z z component of the quaternion.
     * @param w w component of the quaternion.
     * @returns this quaternion for chaining.
     */
    setXyzw: (x: number, y: number, z: number, w: number) => Quat;
    /**
     * Set a Quat from an axis-angle representation. The direction of the vector gives the axis of
     * rotation, and the magnitude of the vector gives the angle, in radians. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param aa vector containing the axis-angle representation of the rotation.
     * @returns this quaternion for chaining.
     */
    makeAxisAngle: (aa: Vec3Source) => Quat;
    /**
     * Set the quaternion to a rotation specified by pitch, yaw, and roll angles in radians. Store the
     * result in this Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param v vector containing the pitch, yaw, and roll angles in radians.
     * @returns this quaternion for chaining.
     */
    makePitchYawRollRadians: (v: Vec3Source) => Quat;
    /**
     * Set the quaternion to a rotation that would cause the eye to look at the target with the given
     * up vector. Store the result in this Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param eye vector where the eye is located.
     * @param target vector where the target is located.
     * @param up vector representing the up direction from the eye's perspective.
     * @returns this quaternion for chaining.
     */
    makeLookAt: (eye: Vec3Source, target: Vec3Source, up: Vec3Source) => Quat;
    /**
     * Set the quaternion to a rotation specified by pitch, yaw, and roll angles in degrees. Store the
     * result in this Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param v vector containing the pitch, yaw, and roll angles in degrees.
     * @returns this quaternion for chaining.
     */
    makePitchYawRollDegrees: (v: Vec3Source) => Quat;
    /**
     * Set the quaternion to a rotation about the x-axis (pitch) in degrees. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param degrees the angle of rotation in degrees.
     * @returns this quaternion for chaining.
     */
    makeXDegrees: (degrees: number) => Quat;
    /**
     * Set the quaternion to a rotation about the x-axis (pitch) in radians. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param radians the angle of rotation in radians.
     * @returns this quaternion for chaining.
     */
    makeXRadians: (radians: number) => Quat;
    /**
     * Set the quaternion to a rotation about the y-axis (yaw) in degrees. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param degrees the angle of rotation in degrees.
     * @returns this quaternion for chaining.
     */
    makeYDegrees: (degrees: number) => Quat;
    /**
     * Set the quaternion to a rotation about the y-axis (yaw) in radians. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param radians the angle of rotation in radians.
     * @returns this quaternion for chaining.
     */
    makeYRadians: (radians: number) => Quat;
    /**
     * Set the quaternion to a rotation about the z-axis (roll) in degrees. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param degrees the angle of rotation in degrees.
     * @returns this quaternion for chaining.
     */
    makeZDegrees: (degrees: number) => Quat;
    /**
     * Set the quaternion to a rotation about the z-axis (roll) in radians. Store the result in this
     * Quat and return this Quat for chaining.
     *
     * API Type: Set API.
     *
     * @param radians the angle of rotation in radians.
     * @returns this quaternion for chaining.
     */
    makeZRadians: (radians: number) => Quat;
    /**
     * Set the quaternion to a zero rotation. Store the result in this Quat and return this Quat for
     * chaining.
     *
     * API Type: Set API.
     *
     * @returns this quaternion for chaining.
     */
    makeZero: () => Quat;
}

declare const quat: QuatFactory;

export declare const Quaternion: RootAttribute<    {
readonly x: "f32";
readonly y: "f32";
readonly z: "f32";
readonly w: "f32";
}>;

/**
 * Factory for Quat. Quat objects are created with the `ecs.math.quat` QuatFactory.
 */
declare interface QuatFactory {
    /**
     * Create a Quat from an axis-angle representation. The direction of the `aa` vector gives the
     * axis of rotation, and the magnitude of the vector gives the angle, in radians. For example,
     * `quat.axisAngle(vec3.up().scale(Math.PI / 2))` represents a 90-degree rotation about the
     * y-axis, and is equivalent to `quat.yDegrees(90)`. If `target` is supplied, the result will be
     * stored in `target` and `target` will be returned. Otherwise, a new Quat will be created and
     * returned.
     *
     * API Type: Factory API.
     *
     * @param aa vector containing the axis-angle representation of the rotation.
     * @param target optional quaternion to store the result in.
     * @returns target if supplied, otherwise a new Quat.
     */
    axisAngle: (aa: Vec3Source, target?: Quat) => Quat;
    /**
     * Create a Quat from an object with x, y, z, w properties.
     *
     * API Type: Factory API.
     *
     * @param source object with x, y, z, w properties.
     * @returns a new Quat with the same components as the source object.
     */
    from: (source: QuatSource) => Quat;
    /**
     * Create a Quat representing the rotation required for an object positioned at `eye` to look at
     * an object positioned at `target`, with the given `up` vector.
     *
     * API Type: Factory API.
     *
     * @param eye vector where the eye is located.
     * @param target vector where the target is located.
     * @param up vector representing the up direction from the eye's perspective.
     * @returns a new Quat representing the rotation required for an object positioned at `eye` to
     *  look at an object positioned at `target`, with the given `up` vector.
     */
    lookAt: (eye: Vec3Source, target: Vec3Source, up: Vec3Source) => Quat;
    /**
     * Construct a quaternion from a pitch / yaw / roll representation, also known as YXZ Euler
     * angles. Rotation is specified in degrees.
     *
     * API Type: Factory API.
     *
     * @param v vector containing the pitch, yaw, and roll angles in degrees.
     * @returns a new Quat representing the rotation specified by the pitch, yaw, and roll angles in
     *   degrees.
     */
    pitchYawRollDegrees: (v: Vec3Source) => Quat;
    /**
     * Construct a quaternion from a pitch / yaw / roll representation, also known as YXZ Euler
     * angles. Rotation is specified in radians.
     *
     * API Type: Factory API.
     *
     * @param v rotation specified in radians.
     * @returns a new Quat representing the rotation specified by the pitch, yaw, and roll angles in
     *   radians.
     */
    pitchYawRollRadians: (v: Vec3Source) => Quat;
    /**
     * Create a Quat which represents a rotation about the x-axis. Rotation is specified in degrees.
     *
     * API Type: Factory API.
     *
     * @param degrees to rotate.
     * @returns a new Quat representing the rotation.
     */
    xDegrees: (degrees: number) => Quat;
    /**
     * Create a Quat which represents a rotation about the x-axis. Rotation is specified in radians.
     *
     * API Type: Factory API.
     *
     * @param radians to rotate.
     * @returns a new Quat representing the rotation.
     */
    xRadians: (radians: number) => Quat;
    /**
     * Create a Quat from the specified x, y, z, and w values.
     *
     * API Type: Factory API.
     *
     * @param x component of the quaternion.
     * @param y component of the quaternion.
     * @param z component of the quaternion.
     * @param w component of the quaternion.
     * @returns a new Quat with the specified components.
     */
    xyzw: (x: number, y: number, z: number, w: number) => Quat;
    /**
     * Create a Quat which represents a rotation about the y-axis. Rotation is specified in degrees.
     *
     * API Type: Factory API.
     *
     * @param degrees to rotate.
     * @returns a new Quat representing the rotation.
     */
    yDegrees: (degrees: number) => Quat;
    /**
     * Create a Quat which represents a rotation about the y-axis. Rotation is specified in radians.
     *
     * API Type: Factory API.
     *
     * @param radians to rotate.
     * @returns a new Quat representing the rotation.
     */
    yRadians: (radians: number) => Quat;
    /**
     * Create a Quat which represents a rotation about the z-axis. Rotation is specified in degrees.
     *
     * API Type: Factory API.
     *
     * @param degrees to rotate.
     * @returns a new Quat representing the rotation.
     */
    zDegrees: (degrees: number) => Quat;
    /**
     * Create a Quat which represents a rotation about the z-axis. Rotation is specified in radians.
     *
     * API Type: Factory API.
     *
     * @param radians to rotate.
     * @returns a new Quat representing the rotation.
     */
    zRadians: (radians: number) => Quat;
    /**
     * Create a Quat which represents a zero rotation.
     *
     * API Type: Factory API.
     *
     * @returns a new Quat representing a zero rotation.
     */
    zero: () => Quat;
}

/**
 * Interface representing any object that has x, y, z, and w properties and hence can be used as a
 * data source to create a Quat. In addition, QuatSource can be used as an argument to Quat
 * algorithms, meaning that any object with `{x: number, y: number, z: number, w: number}`
 * properties can be used.
 */
declare interface QuatSource {
    /**
     * Access the x component of the quaternion.
     */
    readonly x: number;
    /**
     * Access the y component of the quaternion.
     */
    readonly y: number;
    /**
     * Access the z component of the quaternion.
     */
    readonly z: number;
    /**
     * Access the w component of the quaternion.
     */
    readonly w: number;
}

declare type Query = (world: World) => Eid[];

declare type QueuedEvent<D = unknown> = {
    target: Eid;
    currentTarget: Eid;
    name: string;
    data: D;
};

declare type RaycastStage = {
    scene: Scene;
    getCamera: () => CameraObject;
    includeWorldPosition: boolean;
};

export declare type ReadData<T extends Schema> = {
    readonly [key in keyof T]: ElementOf<T[key]>;
};

export declare const ready: () => Promise<void>;

export declare const registerBehavior: (callback: WorldBehavior) => void;

export declare const registerComponent: <ES extends ExtendedSchema<Schema>, ED extends ExtendedSchema<Schema>>(registration: ComponentRegistration<ES, ED>) => RootAttribute<BaseSchema<ES>>;

declare type RelaxedObject3D = Object3D_2 | Object3D;

declare type RemovedComponentCursor<S extends Schema, D extends Schema> = Omit<ComponentCursor<S, D>, 'schema' | 'data'>;

declare type Resource = Url | Asset;

export declare const RingGeometry: RootAttribute<    {
innerRadius: "f32";
outerRadius: "f32";
}>;

declare type RingGeometry_2 = {
    type: 'ring';
    innerRadius: number;
    outerRadius: number;
};

export declare type RootAttribute<T extends Schema> = {
    set(world: World, eid: Eid, data?: Partial<ReadData<T>>): void;
    get(world: World, eid: Eid): ReadData<T>;
    has(world: World, eid: Eid): boolean;
    cursor(world: World, eid: Eid): WriteData<T>;
    mutate: (world: World, eid: Eid, fn: (cursor: WriteData<T>) => void | boolean) => void;
    acquire(world: World, eid: Eid): WriteData<T>;
    commit(world: World, eid: Eid, modified?: boolean): void;
    reset(world: World, eid: Eid): void;
    remove(world: World, eid: Eid): void;
    dirty(world: World, eid: Eid): void;
    forWorld: (world: World) => WorldAttribute<T>;
    schema: T | undefined;
    orderedSchema: OrderedSchema;
    defaults: Partial<ReadData<T>> | undefined;
};

declare interface RootQuery extends Query {
    terms: RootAttribute<any>[];
}

export declare const RotateAnimation: RootAttribute<BaseSchema<    {
autoFrom: "boolean";
fromX: "f32";
fromY: "f32";
fromZ: "f32";
toX: "f32";
toY: "f32";
toZ: "f32";
shortestPath: "boolean";
duration: "f32";
loop: "boolean";
reverse: "boolean";
easeIn: "boolean";
easeOut: "boolean";
easingFunction: "string";
target: "eid";
}>>;

declare type RuntimeVersionTarget = {
    type: 'version';
    level: 'major';
} & Major & P<Minor> & P<Patch> | {
    type: 'version';
    level: 'minor';
} & Major & Minor & P<Patch> | {
    type: 'version';
    level: 'patch';
} & Major & Minor & Patch;

export declare const Scale: RootAttribute<    {
readonly x: "f32";
readonly y: "f32";
readonly z: "f32";
}>;

export declare const ScaleAnimation: RootAttribute<BaseSchema<    {
autoFrom: "boolean";
fromX: "f32";
fromY: "f32";
fromZ: "f32";
toX: "f32";
toY: "f32";
toZ: "f32";
duration: "f32";
loop: "boolean";
reverse: "boolean";
easeIn: "boolean";
easeOut: "boolean";
easingFunction: "string";
target: "eid";
}>>;

declare type SceneGraph = {
    activeCamera?: string;
    activeMap?: string;
    inputs?: InputMap;
    sky?: Sky;
    reflections?: string | Resource;
    entrySpaceId?: string;
    spaces?: Spaces;
    objects: Record<string, GraphObject>;
    runtimeVersion?: RuntimeVersionTarget;
};

declare type SceneHandle = SpacesHandle & PrefabsHandle & {
    remove: () => void;
    updateBaseObjects: (newObjects: DeepReadonly<Record<string, GraphObject>>) => void;
    updateDebug: (newGraph: DeepReadonly<SceneGraph>) => void;
    graphIdToEid: Map<string, Eid>;
    eidToObject: Map<Eid, DeepReadonly<GraphObject>>;
    graphIdToPrefab: Map<string, Eid>;
    _graphIdToEidOrPrefab: Map<string, Eid>;
};

export declare interface Schema {
    [key: string]: Type;
}

export declare type SchemaOf<T extends RootAttribute<Schema>> = T extends RootAttribute<infer P> ? P : never;

declare const SCREEN_TOUCH_END: "screen-touch-end";

declare const SCREEN_TOUCH_MOVE: "screen-touch-move";

declare const SCREEN_TOUCH_START: "screen-touch-start";

declare type ScreenPosition = {
    x: number;
    y: number;
};

export declare interface ScreenTouchEndEvent {
    pointerId: PointerId;
    position: ScreenPosition;
    start: ScreenPosition;
    target: Eid | undefined;
    endTarget: Eid | undefined;
    worldPosition: Vec3 | undefined;
}

export declare interface ScreenTouchMoveEvent {
    pointerId: PointerId;
    position: ScreenPosition;
    start: ScreenPosition;
    change: ScreenPosition;
    target: Eid | undefined;
}

export declare interface ScreenTouchStartEvent {
    pointerId: PointerId;
    position: ScreenPosition;
    target: Eid | undefined;
    worldPosition: Vec3 | undefined;
}

export declare const Shadow: RootAttribute<    {
castShadow: "boolean";
receiveShadow: "boolean";
}>;

declare type Shadow_2 = {
    castShadow?: boolean;
    receiveShadow?: boolean;
};

declare type ShadowLights = THREE_TYPES.DirectionalLight | THREE_TYPES.PointLight | THREE_TYPES.SpotLight;

export declare const ShadowMaterial: RootAttribute<    {
r: "ui8";
g: "ui8";
b: "ui8";
opacity: "f32";
side: "string";
depthTest: "boolean";
depthWrite: "boolean";
}>;

declare type ShadowMaterial_3 = {
    type: 'shadow';
    color: string;
    opacity?: number;
    side?: Side;
    depthTest?: boolean;
    depthWrite?: boolean;
};

declare type Side = 'front' | 'back' | 'double';

declare type SimplificationMode = 'convex' | 'concave';

declare type Sky<T = Resource> = Color_2 | Gradient | Image_2<T> | NoSky;

declare type Space = {
    id: string;
    name: string;
    sky?: Sky;
    activeCamera?: string;
    includedSpaces?: string[];
    reflections?: string | Resource | null;
    fog?: Fog;
};

declare type SpaceData = {
    id: string;
    name: string;
    spawned: boolean;
};

declare type Spaces = Record<string, Space>;

declare type SpacesHandle = {
    loadSpace: (idOrName: string) => void;
    listSpaces: () => SpaceData[] | undefined;
    getActiveSpace: () => SpaceData | undefined;
};

export declare const SphereGeometry: RootAttribute<    {
radius: "f32";
}>;

declare type SphereGeometry_2 = {
    type: 'sphere';
    radius: number;
};

export declare const Splat: RootAttribute<    {
url: "string";
skybox: "boolean";
}>;

declare type Splat_2 = {
    src: Resource;
    skybox?: boolean;
};

export declare interface State<CallbackArgument = void> {
    triggers: Record<string, Trigger[]>;
    onEnter?: TransitionCallback<CallbackArgument>;
    onTick?: TransitionCallback<CallbackArgument>;
    onExit?: () => void;
    listeners?: ListenerParams[];
}

export declare interface StateGroup<CallbackArgument = void> {
    substates?: StateId[];
    triggers: Record<string, Trigger[]>;
    onEnter?: TransitionCallback<CallbackArgument>;
    onTick?: TransitionCallback<CallbackArgument>;
    onExit?: () => void;
    listeners?: ListenerParams[];
}

declare type StateId = string | {
    name: string;
};

export declare type StateMachineDefiner = (props: BaseMachineDefProps) => void;

export declare interface StateMachineDefinition<CallbackArgument = void> {
    initialState: string;
    states: Record<string, State<CallbackArgument>>;
    groups?: StateGroup<CallbackArgument>[];
    prepareCallback?: CallbackArgument extends void ? never : () => CallbackArgument;
}

declare type StaticImageTargetOrientation = {
    rollAngle: number;
    pitchAngle: number;
};

declare type StoredAssetManifest = {
    assets: AssetManifestMappings;
};

export declare const string = "string";

declare type SystemCallback<T extends Attributes> = ((world: World, eid: Eid, cursors: WriteDataForTerms<T>) => void);

declare type SystemQuery<T extends Attributes> = (world: World) => Generator<TableMatch<T>>;

declare type TableMatch<T extends Attributes> = {
    eids: Generator<Eid>;
    ptrs: {
        [K in keyof T]: number;
    };
    count: number;
};

export declare const TetrahedronGeometry: RootAttribute<    {
radius: "f32";
}>;

declare type TetrahedronGeometry_2 = {
    type: 'tetrahedron';
    radius: number;
};

declare type TextAlignContent = 'left' | 'center' | 'right' | 'justify';

declare type TextureFiltering = 'smooth' | 'sharp';

declare type TextureWrap = 'clamp' | 'repeat' | 'mirroredRepeat';

declare namespace THREE_TYPES {
    export {
        Vector3,
        Vector4,
        Matrix4,
        Quaternion_2 as Quaternion,
        Scene,
        AnimationMixer,
        AnimationClip,
        Mesh,
        AmbientLight,
        DirectionalLight,
        MeshStandardMaterial,
        MeshPhysicalMaterial,
        Intersection,
        PositionalAudio,
        Audio_3 as Audio,
        Camera_2 as Camera,
        WebGLRenderer,
        PerspectiveCamera,
        OrthographicCamera,
        AudioListener_2 as AudioListener,
        Euler,
        PointLight,
        Group,
        ShadowMaterial_2 as ShadowMaterial,
        Color,
        ShaderMaterial,
        MeshBasicMaterial,
        Texture,
        ColorSpace,
        Event_2 as Event,
        Material_2 as Material,
        AnimationAction,
        SpotLight,
        BufferGeometry,
        WebGLRenderTarget,
        Raycaster,
        Object3DEventMap,
        TextureLoader,
        VideoTexture,
        RectAreaLight,
        WebGLRenderList,
        MinificationTextureFilter,
        MagnificationTextureFilter,
        DRACOLoader,
        GLTFLoader,
        RGBELoader,
        GLTF,
        Object3D,
        RelaxedObject3D,
        clone as skeletonClone
    }
}

export declare const ThreeObject: RootAttribute<    {
order: "f32";
}>;

declare interface ThreeState {
    renderer: WebGLRenderer;
    activeCamera: CameraObject;
    entityToObject: Map<Eid, Object3D>;
    scene: Scene;
    /**
     * By default, 'manual' uses more efficient matrix update logic, but requires you to call
     * `world.three.notifyChanged` after moving or reparenting raw three.js objects.
     * If it's preferred have all matrices recalculated on each frame, set to 'auto'.
     */
    setMatrixUpdateMode(mode: MatrixUpdateMode): void;
    /**
     * When in manual matrix update mode, call notifyChanged after moving or reparenting
     * raw three.js objects.
     */
    notifyChanged: (object: Object3D) => void;
}

declare type TickMode = 'partial' | 'full' | 'zero';

export declare const tickStateMachine: (world: World, machineId: MachineId) => void;

declare type Time = TimeState & TimeApi;

declare type TimeApi = {
    setTimeout: (cb: Callback, timeout: number) => TimeoutId;
    setInterval: (cb: Callback, timeout: number) => TimeoutId;
    clearTimeout: (id: TimeoutId) => void;
};

declare type TimeoutId = number;

/**
 * a trigger that transitions to the next state after a timeout
 * @param timeout number of ms before transitioning to the next state
 */
declare type TimeoutTrigger = {
    type: 'timeout';
    timeout: number;
};

declare type TimeState = {
    elapsed: number;
    delta: number;
    absolute: number;
    absoluteDelta: number;
};

export declare const TorusGeometry: RootAttribute<    {
radius: "f32";
tubeRadius: "f32";
}>;

declare type TorusGeometry_2 = {
    type: 'torus';
    radius: number;
    tubeRadius: number;
};

declare type TransformManager = {
    getLocalPosition(eid: Eid, out?: Vec3): Vec3;
    getLocalTransform(eid: Eid, out?: Mat4): Mat4;
    getWorldPosition(eid: Eid, out?: Vec3): Vec3;
    getWorldQuaternion(eid: Eid, out?: Quat): Quat;
    getWorldTransform(eid: Eid, out?: Mat4): Mat4;
    setLocalPosition(eid: Eid, position: Vec3Source): void;
    setLocalTransform(eid: Eid, mat4: Mat4): void;
    setWorldPosition(eid: Eid, position: Vec3Source): void;
    setWorldQuaternion(eid: Eid, rotation: QuatSource): void;
    setWorldTransform(eid: Eid, mat4: Mat4): void;
    translateSelf(eid: Eid, translation: Partial<Vec3Source>): void;
    translateLocal(eid: Eid, translation: Partial<Vec3Source>): void;
    translateWorld(eid: Eid, translation: Partial<Vec3Source>): void;
    rotateSelf(eid: Eid, rotation: QuatSource): void;
    rotateLocal(eid: Eid, rotation: QuatSource): void;
    lookAt(eid: Eid, other: Eid): void;
    lookAtLocal(eid: Eid, position: Vec3Source): void;
    lookAtWorld(eid: Eid, position: Vec3Source): void;
};

declare type TransitionCallback<CallbackArgument = void> = CallbackArgument extends void ? () => void : (arg: CallbackArgument) => void;

declare type Trigger = EventTrigger<any> | TimeoutTrigger | CustomTrigger;

declare type TriggerHandle = {
    trigger: () => void;
    listen: (cb: Callback_2) => void;
    unlisten: (cb: Callback_2) => void;
};

declare type Type = keyof TypeToValue;

declare type TypeToValue = {
    'eid': Eid;
    'f32': number;
    'f64': number;
    'i32': number;
    'ui8': number;
    'ui32': number;
    'string': string;
    'boolean': boolean;
};

export declare const Ui: RootAttribute<    {
type: "string";
font: "string";
fontSize: "f32";
position: "string";
opacity: "f32";
backgroundOpacity: "f32";
backgroundSize: "string";
nineSliceBorderTop: "string";
nineSliceBorderBottom: "string";
nineSliceBorderLeft: "string";
nineSliceBorderRight: "string";
nineSliceScaleFactor: "f32";
background: "string";
color: "string";
text: "string";
image: "string";
fixedSize: "boolean";
width: "string";
height: "string";
top: "string";
left: "string";
bottom: "string";
right: "string";
borderColor: "string";
borderRadius: "f32";
borderRadiusTopLeft: "string";
borderRadiusTopRight: "string";
borderRadiusBottomLeft: "string";
borderRadiusBottomRight: "string";
ignoreRaycast: "boolean";
alignContent: "string";
alignItems: "string";
alignSelf: "string";
borderWidth: "f32";
columnGap: "string";
direction: "string";
display: "string";
flex: "f32";
flexBasis: "string";
flexDirection: "string";
flexGrow: "f32";
flexShrink: "f32";
flexWrap: "string";
gap: "string";
justifyContent: "string";
margin: "string";
marginBottom: "string";
marginLeft: "string";
marginRight: "string";
marginTop: "string";
maxHeight: "string";
maxWidth: "string";
minHeight: "string";
minWidth: "string";
overflow: "string";
padding: "string";
paddingBottom: "string";
paddingLeft: "string";
paddingRight: "string";
paddingTop: "string";
rowGap: "string";
textAlign: "string";
verticalTextAlign: "string";
stackingOrder: "f32";
}>;

export declare const ui32 = "ui32";

export declare const ui8 = "ui8";

declare const UI_CLICK: "click";

declare const UI_HOVER_END: "ui-hover-end";

declare const UI_HOVER_START: "ui-hover-start";

declare const UI_PRESSED: "ui-pressed";

declare const UI_RELEASED: "ui-released";

export declare type UiClickEvent = {
    x: number;
    y: number;
};

declare namespace UiEvents {
    export {
        UI_CLICK,
        UI_PRESSED,
        UI_RELEASED,
        UI_HOVER_START,
        UI_HOVER_END,
        UiClickEvent,
        UiHoverEvent
    }
}

declare type UiGraphSettings = Partial<{
    top: string;
    left: string;
    right: string;
    bottom: string;
    width: number | string;
    height: number | string;
    type: UiRootType;
    ignoreRaycast: boolean;
    font: FontResource;
    background: string;
    backgroundOpacity: number;
    backgroundSize: BackgroundSize;
    nineSliceBorderTop: string;
    nineSliceBorderBottom: string;
    nineSliceBorderLeft: string;
    nineSliceBorderRight: string;
    nineSliceScaleFactor: number;
    opacity: number;
    color: string;
    text: string;
    textAlign: FlexStyles.TextAlignContent;
    verticalTextAlign: FlexStyles.VerticalTextAlignContent;
    image: Resource;
    fixedSize: boolean;
    borderColor: string;
    borderRadius: number;
    borderRadiusTopLeft: string;
    borderRadiusTopRight: string;
    borderRadiusBottomRight: string;
    borderRadiusBottomLeft: string;
    fontSize: number;
    alignContent: FlexStyles.AlignContent;
    alignItems: FlexStyles.AlignItems;
    alignSelf: FlexStyles.AlignItems;
    borderWidth: number;
    direction: FlexStyles.Direction;
    display: FlexStyles.Display;
    flex: number;
    flexBasis: string;
    flexDirection: FlexStyles.FlexDirection;
    rowGap: string;
    gap: string;
    columnGap: string;
    flexGrow: number;
    flexShrink: number;
    flexWrap: FlexStyles.FlexWrap;
    justifyContent: FlexStyles.JustifyContent;
    margin: string;
    marginBottom: string;
    marginLeft: string;
    marginRight: string;
    marginTop: string;
    maxHeight: string;
    maxWidth: string;
    minHeight: string;
    minWidth: string;
    overflow: FlexStyles.Overflow;
    padding: string;
    paddingBottom: string;
    paddingLeft: string;
    paddingRight: string;
    paddingTop: string;
    position: FlexStyles.PositionMode;
    stackingOrder: number;
}>;

export declare type UiHoverEvent = {
    x: number;
    y: number;
    targets: Eid[];
};

declare type UiRootType = 'overlay' | '3d';

export declare const UnlitMaterial: RootAttribute<    {
r: "ui8";
g: "ui8";
b: "ui8";
textureSrc: "string";
opacity: "f32";
side: "string";
opacityMap: "string";
blending: "string";
repeatX: "f32";
repeatY: "f32";
offsetX: "f32";
offsetY: "f32";
wrap: "string";
depthTest: "boolean";
depthWrite: "boolean";
wireframe: "boolean";
forceTransparent: "boolean";
textureFiltering: "string";
mipmaps: "boolean";
}>;

declare type UnlitMaterial_2 = {
    type: 'unlit';
    color: string;
    textureSrc?: string | Resource;
    opacity?: number;
    opacityMap?: string | Resource;
    side?: Side;
    blending?: MaterialBlending;
    repeatX?: number;
    repeatY?: number;
    offsetX?: number;
    offsetY?: number;
    wrap?: TextureWrap;
    depthTest?: boolean;
    depthWrite?: boolean;
    wireframe?: boolean;
    forceTransparent?: boolean;
    textureFiltering?: TextureFiltering;
    mipmaps?: boolean;
};

export declare const unregisterBehavior: (callback: WorldBehavior) => void;

declare const UPDATE_EVENT: "physics-update";

declare type Url = {
    type: 'url';
    url: string;
};



/**
 * Interface representing a 2D vector. A 2D vector is represented by (x, y) coordinates, and can
 * represent a point in space, a directional vector, or other types of data with three ordered
 * dimensions. `Vec2` objects are created with the `ecs.math.vec2` `Vec2Factory`, or through
 * operations on other `Vec2` objects.
 */
declare interface Vec2 extends Vec2Source {
    /**
     * Create a new vector with the same components as this vector.
     *
     * API Type: Immutable API.
     *
     * @returns a new vector with the same components as this vector.
     */
    clone: () => Vec2;
    /**
     * Compute the cross product of this vector and another vector. For 2D vectors, the cross product
     * is the magnitude of the z component of the 3D cross product of the two vectors with 0 as the z
     * component.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compute the cross product with.
     * @returns the cross product of this vector and another vector.
     */
    cross: (v: Vec2) => number;
    /**
     * Compute the euclidean distance between this vector and another vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compute the distance to.
     * @returns the euclidean distance between this vector and v.
     */
    distanceTo: (v: Vec2Source) => number;
    /**
     * Element-wise vector division.
     *
     * API Type: Immutable API.
     *
     * @param v vector to divide by.
     * @returns the result of dividing each element of this vector by each element of v.
     */
    divide: (v: Vec2Source) => Vec2;
    /**
     * Compute the dot product of this vector and another vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compute the dot product with.
     * @returns the dot product of this vector and v.
     */
    dot: (v: Vec2Source) => number;
    /**
     * Check whether two vectors are equal, with a specified floating point tolerance.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compare to.
     * @param tolerance used to judge near equality.
     * @returns true if vector components are each equal within the specified tolerance, false
     *   otherwise.
     */
    equals: (v: Vec2Source, tolerance: number) => boolean;
    /**
     * Compute the length of the vector.
     *
     * API Type: Immutable API.
     *
     * @returns the length of the vector.
     */
    length: () => number;
    /**
     * Subtract a vector from this vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to subtract.
     * @returns the result of subtracting v from this vector.
     */
    minus: (v: Vec2Source) => Vec2;
    /**
     * Compute a linear interpolation between this vector and another vector v with a factor t such
     * that the result is thisVec * (1 - t) + v * t.
     *
     * API Type: Immutable API.
     *
     * @param v vector to interpolate with.
     * @param t factor to interpolate; should be between in 0 to 1, inclusive.
     * @returns the result of the linear interpolation.
     */
    mix: (v: Vec2Source, t: number) => Vec2;
    /**
     * Return a new vector with the same direction as this vector, but with a length of 1.
     *
     * API Type: Immutable API.
     *
     * @returns a new vector with the same direction as this vector, but with a length of 1.
     */
    normalize: () => Vec2;
    /**
     * Add two vectors together.
     *
     * API Type: Immutable API.
     *
     * @param v vector to add.
     * @returns the result of adding v to this vector.
     */
    plus: (v: Vec2Source) => Vec2;
    /**
     * Multiply the vector by a scalar.
     *
     * API Type: Immutable API.
     *
     * @param s scalar to multiply by.
     * @returns the result of multiplying this vector by s.
     */
    scale: (s: number) => Vec2;
    /**
     * Element-wise vector multiplication.
     *
     * API Type: Immutable API.
     *
     * @param v vector to multiply by.
     * @returns the result of multiplying each element of this vector by each element of v.
     */
    times: (v: Vec2Source) => Vec2;
    /**
     * Element-wise vector division. Store the result in this Vec2 and return this Vec2 for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to divide by.
     * @returns this vector for chaining.
     */
    setDivide: (v: Vec2Source) => Vec2;
    /**
     * Subtract a vector from this vector. Store the result in this Vec2 and return this Vec2
     * for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to subtract.
     * @returns this vector for chaining.
     */
    setMinus: (v: Vec2Source) => Vec2;
    /**
     * Compute a linear interpolation between this vector and another vector v with a factor t such
     * that the result is thisVec * (1 - t) + v * t. The factor t should be between 0 and 1. Store the
     * result in this Vec2 and return this Vec2 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to interpolate with.
     * @param t factor to interpolate; should be between in 0 to 1, inclusive.
     * @returns this vector for chaining.
     */
    setMix: (v: Vec2Source, t: number) => Vec2;
    /**
     * Set the vector to be a version of itself with the same direction, but with length 1. Store the
     * result in this Vec2 and return this Vec2 for chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this vector for chaining.
     */
    setNormalize: () => Vec2;
    /**
     * Add two vectors together. Store the result in this Vec2 and return this Vec2 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to add.
     * @returns this vector for chaining.
     */
    setPlus: (v: Vec2Source) => Vec2;
    /**
     * Multiply the vector by a scalar. Store the result in this Vec2 and return this Vec2 for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @param s scalar to multiply by.
     * @returns this vector for chaining.
     */
    setScale: (s: number) => Vec2;
    /**
     * Element-wise vector multiplication. Store the result in this Vec2 and return this Vec2
     * for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to multiply by.
     * @returns this vector for chaining.
     */
    setTimes: (v: Vec2Source) => Vec2;
    /**
     * Set the Vec2's x component. Store the result in this Vec2 and return this for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v value to set this vector's x component to.
     * @returns this vector for chaining.
     */
    setX(v: number): Vec2;
    /**
     * Set the Vec2's y component. Store the result in this Vec2 and return this Vec2 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v value to set this vector's y component to.
     * @returns this vector for chaining.
     */
    setY(v: number): Vec2;
    /**
     * Set the Vec2 to be all ones. Store the result in this Vec2 and return this Vec2 for chaining.
     *
     * API Type: Set API.
     *
     * @returns this vector for chaining.
     */
    makeOne: () => Vec2;
    /**
     * Set the Vec2 to have all components set to the scale value s. Store the result in this Vec2
     * and return this Vec2 for chaining.
     *
     * API Type: Set API.
     *
     * @param s value to set all components to.
     * @returns this vector for chaining.
     */
    makeScale: (s: number) => Vec2;
    /**
     * Set the Vec2 to be all zeros. Store the result in this Vec2 and return this Vec2 for
     * chaining.
     *
     * API Type: Set API.
     *
     * @returns this vector for chaining.
     */
    makeZero: () => Vec2;
    /**
     * Set this Vec2 to have the same value as another Vec2. Store the result in this Vec2 and return
     * this Vec2 for chaining.
     *
     * API Type: Set API.
     *
     * @param v vector to copy from.
     * @returns this vector for chaining.
     */
    setFrom: (v: Vec2Source) => Vec2;
    /**
     * Set the Vec2's x and y components. Store the result in this Vec2 and return this for chaining.
     *
     * API Type: Set API.
     *
     * @param x value to set this vector's x component to.
     * @param y value to set this vector's y component to.
     * @returns this vector for chaining.
     */
    setXy: (x: number, y: number) => Vec2;
}

declare const vec2: Vec2Factory;

/**
 * Factory for Vec2. Vec2 objects are created with the `ecs.math.vec2` Vec2Factory.
 */
declare interface Vec2Factory {
    /**
     * Create a Vec2 from an object with x, y properties.
     *
     * API Type: Factory API.
     *
     * @param source to copy.
     * @returns a new Vec2 with the same components as the source.
     */
    from: (source: Vec2Source) => Vec2;
    /**
     * Create a Vec2 with all elements set to one.
     *
     * API Type: Factory API.
     *
     * @returns a new Vec2 with all elements set to one.
     */
    one: () => Vec2;
    /**
     * Create a Vec2 with all elements set to the scale value s.
     *
     * API Type: Factory API.
     *
     * @param s value to set all components to.
     * @returns a new Vec2 with all elements set to the scale value s.
     */
    scale: (s: number) => Vec2;
    /**
     * Create a Vec2 from x, y, z values.
     *
     * API Type: Factory API.
     *
     * @param x value to set the x component to.
     * @param y value to set the y component to.
     * @returns a new Vec2 with the x and y components set to the specified values.
     */
    xy: (x: number, y: number) => Vec2;
    /**
     * Create a Vec2 with all components set to zero.
     *
     * API Type: Factory API.
     *
     * @returns a new Vec2 with all components set to zero.
     */
    zero: () => Vec2;
}

/**
 * Interface representing any object that has x and y properties and hence can be used as a data
 * source to create a Vec2. In addition, Vec2Source can be used as an argument to Vec2 algorithms,
 * meaning that any object with `{x: number, y: number}` properties can be used.
 */
declare interface Vec2Source {
    /**
     * Access the x component of the vector.
     */
    readonly x: number;
    /**
     * Access the y component of the vector.
     */
    readonly y: number;
}

/**
 * @schema array(z.number()).length(2).describe('A 2D vector represented by [x, y] coordinates. Can represent a point in a plane, a directional vector, or other 2D quantities.')
 */
declare type Vec2Tuple = [number, number];

/**
 * Interface representing a 3D vector. A 3D vector is represented by (x, y, z) coordinates, and can
 * represent a point in space, a directional vector, or other types of data with three ordered
 * dimensions. 3D vectors can be multiplied by 4x4 matrices (`Mat4`) using homogeneous coordinate
 * math, enabling efficient 3D geometry computation. `Vec3` objects are created with the
 * `ecs.math.vec3` `Vec3Factory`, or through operations on other `Vec3` objects.
 */
declare interface Vec3 extends Vec3Source {
    /**
     * Create a new vector with the same components as this vector.
     *
     * API Type: Immutable API.
     *
     * @returns a new vector with the same components as this vector.
     */
    clone: () => Vec3;
    /**
     * Compute the cross product of this vector and another vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compute the cross product with.
     * @returns the cross product of this vector and another vector.
     */
    cross: (v: Vec3) => Vec3;
    /**
     * Access the vector as a homogeneous array (4 dimensions).
     *
     * API Type: Immutable API.
     *
     * @returns a homogeneous array (4 dimensions) representing the vector.
     */
    data: () => number[];
    /**
     * Compute the euclidean distance between this vector and another vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compute the distance to.
     * @returns the euclidean distance between this vector and v.
     */
    distanceTo: (v: Vec3Source) => number;
    /**
     * Element-wise vector division.
     *
     * API Type: Immutable API.
     *
     * @param v vector to divide by.
     * @returns the result of dividing each element of this vector by each element of v.
     */
    divide: (v: Vec3Source) => Vec3;
    /**
     * Compute the dot product of this vector and another vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compute the dot product with.
     * @returns the dot product of this vector and v.
     */
    dot: (v: Vec3Source) => number;
    /**
     * Check whether two vectors are equal, with a specified floating point tolerance.
     *
     * API Type: Immutable API.
     *
     * @param v vector to compare to.
     * @param tolerance used to judge near equality.
     * @returns true if vector components are each equal within the specified tolerance, false
     *   otherwise.
     */
    equals: (v: Vec3Source, tolerance: number) => boolean;
    /**
     * Compute the length of the vector.
     *
     * API Type: Immutable API.
     *
     * @returns the length of the vector.
     */
    length: () => number;
    /**
     * Subtract a vector from this vector.
     *
     * API Type: Immutable API.
     *
     * @param v vector to subtract.
     * @returns the result of subtracting v from this vector.
     */
    minus: (v: Vec3Source) => Vec3;
    /**
     * Compute a linear interpolation between this vector and another vector v with a factor t such
     * that the result is thisVec * (1 - t) + v * t.
     *
     * API Type: Immutable API.
     *
     * @param v vector to interpolate with.
     * @param t factor to interpolate; should be between in 0 to 1, inclusive.
     * @returns the result of the linear interpolation.
     */
    mix: (v: Vec3Source, t: number) => Vec3;
    /**
     * Return a new vector with the same direction as this vector, but with a length of 1.
     *
     * API Type: Immutable API.
     *
     * @returns a new vector with the same direction as this vector, but with a length of 1.
     */
    normalize: () => Vec3;
    /**
     * Add two vectors together.
     *
     * API Type: Immutable API.
     *
     * @param v vector to add.
     * @returns the result of adding v to this vector.
     */
    plus: (v: Vec3Source) => Vec3;
    /**
     * Multiply the vector by a scalar.
     *
     * API Type: Immutable API.
     *
     * @param s scalar to multiply by.
     * @returns the result of multiplying this vector by s.
     */
    scale: (s: number) => Vec3;
    /**
     * Element-wise vector multiplication.
     *
     * API Type: Immutable API.
     *
     * @param v vector to multiply by.
     * @returns the result of multiplying each element of this vector by each element of v.
     */
    times: (v: Vec3Source) => Vec3;
    /**
     * Compute the cross product of this vector and another vector. Store the result in this Vec3
     * and return this Vec3 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to compute cross product with.
     * @returns this vector for chaining.
     */
    setCross: (v: Vec3Source) => Vec3;
    /**
     * Element-wise vector division. Store the result in this Vec3 and return this Vec3 for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to divide by.
     * @returns this vector for chaining.
     */
    setDivide: (v: Vec3Source) => Vec3;
    /**
     * Subtract a vector from this vector. Store the result in this Vec3 and return this Vec3
     * for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to subtract.
     * @returns this vector for chaining.
     */
    setMinus: (v: Vec3Source) => Vec3;
    /**
     * Compute a linear interpolation between this vector and another vector v with a factor t such
     * that the result is thisVec * (1 - t) + v * t. The factor t should be between 0 and 1. Store the
     * result in this Vec3 and return this Vec3 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to interpolate with.
     * @param t factor to interpolate; should be between in 0 to 1, inclusive.
     * @returns this vector for chaining.
     */
    setMix: (v: Vec3Source, t: number) => Vec3;
    /**
     * Set the vector to be a version of itself with the same direction, but with length 1. Store the
     * result in this Vec3 and return this Vec3 for chaining.
     *
     * API Type: Mutable API.
     *
     * @returns this vector for chaining.
     */
    setNormalize: () => Vec3;
    /**
     * Add two vectors together. Store the result in this Vec3 and return this Vec3 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to add.
     * @returns this vector for chaining.
     */
    setPlus: (v: Vec3Source) => Vec3;
    /**
     * Multiply the vector by a scalar. Store the result in this Vec3 and return this Vec3 for
     * chaining.
     *
     * API Type: Mutable API.
     *
     * @param s scalar to multiply by.
     * @returns this vector for chaining.
     */
    setScale: (s: number) => Vec3;
    /**
     * Element-wise vector multiplication. Store the result in this Vec3 and return this Vec3
     * for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v vector to multiply by.
     * @returns this vector for chaining.
     */
    setTimes: (v: Vec3Source) => Vec3;
    /**
     * Set the Vec3's x component. Store the result in this Vec3 and return this for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v value to set this vector's x component to.
     * @returns this vector for chaining.
     */
    setX(v: number): Vec3;
    /**
     * Set the Vec3's y component. Store the result in this Vec3 and return this Vec3 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v value to set this vector's y component to.
     * @returns this vector for chaining.
     */
    setY(v: number): Vec3;
    /**
     * Set the Vec3's z component. Store the result in this Vec3 and return this Vec3 for chaining.
     *
     * API Type: Mutable API.
     *
     * @param v value to set this vector's z component to.
     * @returns this vector for chaining.
     */
    setZ(v: number): Vec3;
    /**
     * Set the Vec3 to be all ones. Store the result in this Vec3 and return this Vec3 for chaining.
     *
     * API Type: Set API.
     *
     * @returns this vector for chaining.
     */
    makeOne: () => Vec3;
    /**
     * Set the Vec3 to have all components set to the scale value s. Store the result in this Vec3
     * and return this Vec3 for chaining.
     *
     * API Type: Set API.
     *
     * @param s value to set all components to.
     * @returns this vector for chaining.
     */
    makeScale: (s: number) => Vec3;
    /**
     * Set the Vec3 to be pointed in the positive y direction. Store the result in this Vec3 and
     * return this Vec3 for chaining.
     *
     * API Type: Set API.
     *
     * @returns this vector for chaining.
     */
    makeUp: () => Vec3;
    /**
     * Set the Vec3 to be all zeros. Store the result in this Vec3 and return this Vec3 for
     * chaining.
     *
     * API Type: Set API.
     *
     * @returns this vector for chaining.
     */
    makeZero: () => Vec3;
    /**
     * Set this Vec3 to have the same value as another Vec3. Store the result in this Vec3 and return
     * this Vec3 for chaining.
     *
     * API Type: Set API.
     *
     * @param v vector to copy from.
     * @returns this vector for chaining.
     */
    setFrom: (v: Vec3Source) => Vec3;
    /**
     * Set the Vec3's x, y, and z components. Store the result in this Vec3 and return this for
     * chaining.
     *
     * API Type: Set API.
     *
     * @param x value to set this vector's x component to.
     * @param y value to set this vector's y component to.
     * @param z value to set this vector's z component to.
     * @returns this vector for chaining.
     */
    setXyz: (x: number, y: number, z: number) => Vec3;
}

declare const vec3: Vec3Factory;

/**
 * Factory for Vec3. Vec3 objects are created with the `ecs.math.vec3` Vec3Factory.
 */
declare interface Vec3Factory {
    /**
     * Create a Vec3 from an object with x, y, z properties.
     *
     * API Type: Factory API.
     *
     * @param source to copy.
     * @returns a new Vec3 with the same components as the source.
     */
    from: (source: Vec3Source) => Vec3;
    /**
     * Create a Vec3 with all elements set to one.
     *
     * API Type: Factory API.
     *
     * @returns a new Vec3 with all elements set to one.
     */
    one: () => Vec3;
    /**
     * Create a Vec3 with all elements set to the scale value s.
     *
     * API Type: Factory API.
     *
     * @param s value to set all components to.
     * @returns a new Vec3 with all elements set to the scale value s.
     */
    scale: (s: number) => Vec3;
    /**
     * Create a Vec3 pointing in the positive y direction.
     *
     * API Type: Factory API.
     *
     * @returns a new Vec3 pointing in the positive y direction.
     */
    up: () => Vec3;
    /**
     * Create a Vec3 from x, y, z values.
     *
     * API Type: Factory API.
     *
     * @param x value to set the x component to.
     * @param y value to set the y component to.
     * @param z value to set the z component to.
     * @returns a new Vec3 with the x, y, and z components set to the specified values.
     */
    xyz: (x: number, y: number, z: number) => Vec3;
    /**
     * Create a Vec3 with all components set to zero.
     *
     * API Type: Factory API.
     *
     * @returns a new Vec3 with all components set to zero.
     */
    zero: () => Vec3;
}

/**
 * Interface representing any object that has x, y, and z properties and hence can be used as a data
 * source to create a Vec3. In addition, Vec3Source can be used as an argument to Vec3 algorithms,
 * meaning that any object with `{x: number, y: number, z: number}` properties can be used.
 */
declare interface Vec3Source {
    /**
     * Access the x component of the vector.
     */
    readonly x: number;
    /**
     * Access the y component of the vector.
     */
    readonly y: number;
    /**
     * Access the z component of the vector.
     */
    readonly z: number;
}

/**
 * @schema array(z.number()).length(3).describe('A 3D vector represented by [x, y, z] coordinates. Can represent a point in space, a directional vector, or other 3D quantities.')
 */
declare type Vec3Tuple = [number, number, number];

/**
 * @schema array(z.number()).length(4).describe('A 4D vector represented by [x, y, z, w] coordinates. Often used for homogeneous coordinates in 3D transformations or for quaternions.')
 */
declare type Vec4Tuple = [number, number, number, number];

/**
 * @schema array(z.number()).length(6).describe('Used to represent shadow camera frustum bounds: left, right, top, bottom, near, far.')
 */
declare type Vec6Tuple = [number, number, number, number, number, number];

declare type VerticalTextAlignContent = 'start' | 'center' | 'end';

export declare const video: {
    getCurrentTime: (world: World, eid: Eid, query?: VideoQuery) => number;
    setCurrentTime: (world: World, eid: Eid, time: number, query?: VideoQuery) => void;
    getCurrentTimes: (world: World, eid: Eid, filter?: VideoQuery) => VideoTimeResult[];
    setCurrentTimes: (world: World, eid: Eid, time: number, filter?: VideoQuery) => void;
};

export declare const VideoControls: RootAttribute<    {
loop: "boolean";
paused: "boolean";
volume: "f32";
positional: "boolean";
speed: "f32";
refDistance: "f32";
rolloffFactor: "f32";
distanceModel: "string";
maxDistance: "f32";
}>;

declare type VideoControlsGraphSettings = {
    volume?: number;
    loop?: boolean;
    paused?: boolean;
    speed?: number;
    positional?: boolean;
    refDistance?: number;
    rolloffFactor?: number;
    distanceModel?: DistanceModel;
    maxDistance?: number;
};

export declare const VideoMaterial: RootAttribute<    {
r: "ui8";
g: "ui8";
b: "ui8";
textureSrc: "string";
opacity: "f32";
}>;

declare type VideoMaterial_2 = {
    type: 'video';
    color: string;
    textureSrc?: string | Resource;
    opacity?: number;
};

declare type VideoQuery = {
    src?: string;
    textureKey?: EcsTextureKey;
};

declare type VideoTimeResult = {
    src: string;
    textureKey: EcsTextureKey;
    time: number;
};

export declare interface World extends BaseWorld, LateWorld {
}

export declare type WorldAttribute<T extends Schema> = {
    id: number;
    set(eid: Eid, data?: Partial<ReadData<T>>): void;
    get(eid: Eid): ReadData<T>;
    has(eid: Eid): boolean;
    cursor(eid: Eid): WriteData<T>;
    mutate(eid: Eid, fn: (cursor: WriteData<T>) => void | boolean): void;
    acquire(eid: Eid): WriteData<T>;
    commit(eid: Eid, modified?: boolean): void;
    reset(eid: Eid): void;
    remove(eid: Eid): void;
    dirty(eid: Eid): void;
};

declare type WorldBehavior = (w: World) => void;

declare interface WorldEffectCameraSchema {
    disableWorldTracking: boolean;
    enableLighting: boolean;
    enableWorldPoints: boolean;
    leftHandedAxes: boolean;
    mirroredDisplay: boolean;
    scale: string;
    direction: string;
    allowedDevices: string;
    enableVps: boolean;
}

export declare type WriteData<T extends Schema> = {
    -readonly [key in keyof T]: ElementOf<T[key]>;
};

declare type WriteDataForTerms<T extends Attributes> = {
    [K in keyof T]: WriteData<SchemaOf<T[K]>>;
};

export declare const XR_FACE_FOUND: "facecontroller.facefound";

export declare const XR_FACE_LOST: "facecontroller.facelost";

export declare const XR_FACE_UPDATED: "facecontroller.faceupdated";

declare type XrCameraInfo = unknown;

declare type XrCameraType = 'world' | 'face' | 'hand' | 'layers' | 'worldLayers' | '3dOnly';

declare type XrConfig = {
    xrCameraType?: XrCameraType;
    phone?: DeviceSupportType;
    desktop?: DeviceSupportType;
    headset?: DeviceSupportType;
    leftHandedAxes?: boolean;
    uvType?: 'standard' | 'projected';
    direction?: CameraDirectionType;
    world?: XrWorldConfig;
    face?: XrFaceConfig;
};

declare type XrFaceConfig = {
    mirroredDisplay?: boolean;
    meshGeometryFace?: boolean;
    meshGeometryEyes?: boolean;
    meshGeometryIris?: boolean;
    meshGeometryMouth?: boolean;
    enableEars?: boolean;
    maxDetections?: number;
};

declare type XrManager = {
    createWorldEffect: (config: Partial<WorldEffectCameraSchema>, eid: Eid) => number;
    startCameraPipeline: (handle: number) => void;
    stopCameraPipeline: (handle: number) => void;
    createFaceEffect: (config: Partial<FaceEffectCameraSchema>, eid: Eid) => number;
    startMediaRecorder: () => void;
    stopMediaRecorder: () => void;
    takeScreenshot: () => Promise<Blob>;
    drawPausedBackground: () => void;
    setEcsRenderOverride: (renderOverride: EcsRenderOverride) => void;
    attach: () => void;
    detach: () => void;
    tick: () => void;
    tock: () => void;
};

declare type XrWorldConfig = {
    scale?: 'absolute' | 'responsive';
    disableWorldTracking?: boolean;
    enableLighting?: boolean;
    enableWorldPoints?: boolean;
    enableVps?: boolean;
};

export { }

